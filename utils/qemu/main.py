
#!/usr/bin/python3

import argparse
import collections
import contextlib
import fabric
import libvirt
import pathlib
import os
import shlex
import subprocess
import sys
import time
import typing
import xml.etree.ElementTree as ET

TCP_PORT=10022
MEMKIND_GUEST_PATH='/mnt/memkind/'
MEMKIND_MOUNT_TAG='memkind_host'
MEMKIND_QEMU_PREFIX='utils/qemu'
KEY_NAME='qemukey'
TOPOLOGY_ENV_VAR='MEMKIND_TEST_TOPOLOGY'
TOPOLOGY_DIR='topology'
#TODO document function/classes
#TODO handling fsdax??

QemuCfg = collections.namedtuple('QemuCfg', ['workdir', 'image', 'daemonize', 'force_reinstall', 'run_test', 'topology_l'])


class NUMANode():

    def __init__(self):
        self.id = None
        self.initiator = None
        self.cpu_list = []
        self.socket_id = None
        self.memory = None
        self.bandwith = dict() #DESTINATION NUMA NODE and value
        self.latency = dict() #DESTINATION NUMA NODE and value
        self.distance = dict() #DESTINATION NUMA NODE and value


class GuestConnection():


    @property
    def _connection_params(self) -> dict:
        return {'host': 'localhost',
                'user': 'root',
                'connect_kwargs': {"key_filename": {self.quemu_key_path}},
                'port':f'{TCP_PORT}'
                }

    def __init__(self, cfg: QemuCfg, topology_name:str) -> None:
        self.run_test = cfg.run_test
        self.force_reinstall = cfg.force_reinstall
        self.quemu_key_path =  os.path.join(cfg.workdir, MEMKIND_QEMU_PREFIX, KEY_NAME)
        self.topology_name = topology_name

    def _build_and_reinstall_memkind(self, c : fabric.Connection) -> None:
        """
        Rebuild and reinstall memkind library
        """
        # handle correct incorrect status - this could fail
        # c.run('make distclean', echo=True)
        c.run('./autogen.sh', echo=True)
        c.run('./configure --prefix=/usr', echo=True)
        c.run('make' , echo=True)
        c.run('make checkprogs' , echo=True)
        c.run('make uninstall', echo=True)
        c.run('make install', echo=True)
        c.run('ldconfig', echo=True)
        self.force_reinstall = False

    def _mount_workdir(self, c : fabric.Connection) -> None:
        """
        Mount working directory from host to guest
        """
        c.run(f'mkdir -p {MEMKIND_GUEST_PATH}', echo=True)
        c.run(f'mount {MEMKIND_MOUNT_TAG} {MEMKIND_GUEST_PATH} -t 9p -o trans=virtio', echo=True)


    def _setTestEnvname(self, c: fabric.Connection):
        if self.topology_name:
            c.config.run.env = {f'{TOPOLOGY_ENV_VAR}':f'{self.topology_name}'}

    def _run_test(self, c : fabric.Connection) -> None:
        """
        Run test
        """
        self._setTestEnvname(c)
        c.run('ls')
        c.run('echo $TOPOLOGY_ENV_VAR')
        c.run('make unit_tests_hmat')


    def run_connection(self):
        with fabric.Connection(**self._connection_params, inline_ssh_env=True) as c:
            try:
                print('Start QEMU Connection')
                self._mount_workdir(c)
                with c.cd(MEMKIND_GUEST_PATH):
                    if self.force_reinstall:
                        #TODO DECORATOR FOR THIS
                        print('Start Build and Install')
                        self._build_and_reinstall_memkind(c)
                        print('End Build and Install')
                    if self.run_test:
                        print('Start Test')
                        self._run_test(c)
                        print('End Test')
            finally:
                if not self.run_test:
                    c.run('shutdown now')
                    print('Shutdown QEMU Connection')



class QEMU():

    VNC_PORT=5

    def __init__(self, cfg:QemuCfg) -> None:
        self.cfg = cfg

    @property
    def _qemu_exec(self) -> str:
        """
        QEMU binary
        """
        return 'qemu-system-x86_64'

    @property
    def _hda_option(self) -> str:
        """
        Hard drive as a file
        """
        return f'-hda {self.cfg.image}'

    @property
    def _optim_option(self) -> str:
        """
        Optimization options:
        - Use kernel based virtual machine
        """
        return '-enable-kvm -cpu host'

    @property
    def _connect_option(self) -> str:
        """
        Connect options:
        - VNC on port 5
        - tcp on port 10022
        """
        return f'-vnc :{self.VNC_PORT} -netdev user,id=net0,hostfwd=tcp::{TCP_PORT}-:22 -device virtio-net,netdev=net0'

    @property
    def _boot_option(self) -> str:
        """
        Boot order
        - Start from first virtual hard drive
        """
        return '-boot c'

    def _memory_option(self, topology_name:str) -> str:
        """
        Memory option
        - Start from first virtual hard drive
        """
        # if topology_name:
        #     #TODO CALCULATE MAX MEMORY FROM XML
        #     return None
        # else:
        return '-m 2G'

    @property
    def _daemonize_option(self) ->str:
        """
        Daemonize option
        - run process as daemon or in terminal
        """
        return '-daemonize' if self.cfg.daemonize else '-nographic'

    def _hmat_options(self, topology_name: str) -> str:
        """
        HMAT option
        """
        # return '-machine hmat=on' if topology_name else ''

        return ''

    def _memory_topology(self, topology_name: str) -> str:
        """
        real memory topology string
        """
        return ''

    @property
    def _mount_option(self) -> str:
        """
        mount option
        - Start from first virtual hard drive
        """
        return f'-fsdev local,security_model=passthrough,id=fsdev0,path={self.cfg.workdir} -device virtio-9p-pci,id=fs0,fsdev=fsdev0,mount_tag={MEMKIND_MOUNT_TAG}'


    def qemu_cmd(self, topology_name:str) -> typing.List[str]:
        """
        QEMU command concat
        """
        cmd_str = ' '.join([self._qemu_exec,
                            self._hda_option,
                            self._optim_option,
                            self._connect_option,
                            self._boot_option,
                            self._memory_option(topology_name),
                            self._hmat_options(topology_name),
                            self._mount_option,
                            self._daemonize_option])
        return shlex.split(cmd_str)

    def start(self) -> None:
        """
        Start QEMU
        """
        for top_name in self.cfg.topology_l:
            qemu_cmd = self.qemu_cmd(top_name)
            subprocess.run(qemu_cmd)
            time.sleep(10)
            con = GuestConnection(self.cfg, top_name)
            con.run_connection()

def find_root_memkind_dir():
    cmd = ['git', 'rev-parse', '--show-toplevel']
    result = subprocess.run(cmd, stdout=subprocess.PIPE)
    return result.stdout.decode('utf-8').strip()

##TODO
def real_parse(topologies:list) -> None:
    for t in topologies:
        print({t})

def parse_topology(cfg:dict) -> list:
    tpg_list = []
    if cfg['test_all']:
        for filename in os.listdir(os.path.join(find_root_memkind_dir(),MEMKIND_QEMU_PREFIX,TOPOLOGY_DIR)):
            if filename.endswith(".xml"):
                tpg_list.append(filename)
    else:
        tpg_file = cfg.get('topology', None)
        ## USE LIBVIRT add requirements txt Docker whatever else :)
        # tree = ET.parse(tpg_file)
        # root = tree.getroot()
        # xmlstr = ET.tostring(root, encoding='unicode', method='xml')
        # test = libvirt.open(None).domainXMLToNative('qemu-argv', xmlstr)
        # conn = libvirt.open(None)

        # test = libvirt.open(None).domainXMLToNative('qemu-argv', xmlstr)

        # conn.defineXML(xmlstr)
        # guest = conn.lookupByName(guestname)
        # xmlstr = guest.XMLDesc()
        # test = conn.domainXMLToNative('qemu-argv', xmlstr)
        # print({test})
        # root = tree.getroot()
        # ff = root.findall('numa')
        # print(f'{ff}')
        # sys.exit("KONIEC2")


    if tpg_file is not None:
        if pathlib.Path(tpg_file).is_file:
            tpg_list.append(tpg_file)
        else:
            sys.exit(f"Topology file {tpg_file} doesn't exists.")
    return tpg_list


def parse_arguments() -> QemuCfg:
    ## TODO inform user if this is not git repository
    qemu_cfg = {'workdir': find_root_memkind_dir()}
    parser = argparse.ArgumentParser()
    ##TODO HANDLE THIS LOGIC BETTER
    group = parser.add_mutually_exclusive_group()
    group_2 = parser.add_mutually_exclusive_group()
    group.add_argument('--test', action="store_true", help='single_test', default=False)
    copy_test_all = group.add_argument('--test_all', action="store_true", help='run all_tests', default=False)
    group_2.add_argument('--topology', help='memory topology XML file', default=os.path.join(qemu_cfg['workdir'], MEMKIND_QEMU_PREFIX, TOPOLOGY_DIR, 'bar.xml'))
    group_2._group_actions.append(copy_test_all)

    parser.add_argument('--image', help='QEMU image', default=os.path.join(qemu_cfg['workdir'], MEMKIND_QEMU_PREFIX, 'linux-ubuntu.img'))
    # parser.add_argument('--image', help='QEMU image', required=True)
    parser.add_argument('--daemonize', action="store_true", help='run qemu in background', default=True)
    parser.add_argument('--force_reinstall', action="store_true", help='force rebuild and install memkind', default=False)

    cli_cfg = vars(parser.parse_args())

    ##TODOD HANDLE LOGIC
    ff = parse_topology(cli_cfg)

    qemu_cfg['image'] = cli_cfg['image']
    qemu_cfg['daemonize'] = cli_cfg['daemonize']
    qemu_cfg['force_reinstall'] = cli_cfg['force_reinstall']
    qemu_cfg['topology_l'] = parse_topology(cli_cfg)
    qemu_cfg['run_test'] = True if cli_cfg['test'] or cli_cfg['test_all'] else False
    return QemuCfg(**qemu_cfg)

def main():
    print("Script started")
    qemu_cfg = parse_arguments()
    qemu = QEMU(qemu_cfg)
    qemu.start()
    print("Script ended")

if __name__ == "__main__":
    main()