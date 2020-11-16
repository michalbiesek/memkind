# **README**

This is utils/qemu/README.

Scripts in this directory are useful for HMAT emulation.

## Dependencies

- [QEMU](https://www.qemu.org/download/#source) (v5.0.0 or later)

# Create QEMU Image/Install OS
To set up your own guest OS image, please follow [instructions](https://en.wikibooks.org/wiki/QEMU/Images#Creating_an_image)

Example:
qemu-img create -f qcow2 linux-ubuntu.img 30g
# check if server clouding could be used
# wget https://cloud-images.ubuntu.com/releases/focal/release-20201111/ubuntu-20.04-server-cloudimg-amd64.img
wget https://releases.ubuntu.com/20.04.1/ubuntu-20.04.1-desktop-amd64.iso
sudo qemu-system-x86_64 -cdrom ubuntu-20.04.1-desktop-amd64.iso -cpu host -enable-kvm -m 4G -vnc :5 -drive file=linux-ubuntu.img,format=qcow2 -boot d
# connect via vnc (port 5905)
# TODO
- automatically download OS and install (https://linux.die.net/man/1/virt-install + Kickstart file)
- setup PROXY (apt/dnf/git)
- setup necessary packages as memkind mandatory dependencies and hwloc
- setup GRUB parameter from Setup no-graphics mode:

# Run example configuration OS (current value in script)

+-------+    +-------+
| node0 |    | node1 |
| CPU   |----| CPU   |
| DRAM  |    | DRAM  |
+-------+    +-------+
    |            |
    |            |
+-------+    +-------+
| node2 |    | node3 |
|  HBW  |    |  HBW  |
+-------+    +-------+

sudo ./setup-hmat.sh qemu-linux.img /path/to/memkind_host_workdir


# Prerequisites
# To mount in guest directory from host please run following command:

# where /tmp/memkind is path to mounted directory in guest and memkind_host is tag created for directory in host
mkdir $HOME/memkind
sudo mount memkind_host $HOME/memkind -t 9p -o trans=virtio

# Setup no-graphics mode:
Append to etc/default/GRUB in guest:

GRUB_CMDLINE_LINUX="console=tty0 console=ttyS0"

# To connect via ssh
Install openssh-server on guest
# Generate SSH keys
ssh-keygen -b 2048 -t rsa -q -N "" -f ./qemukey
# setup root password in guest
sudo passwd -l root

# copy generated public key from host to guest
ssh-copy-id -p 10022 -i ~/.ssh/qemukey root@localhost

# edit /etc/ssh/sshd_config in guest
PermitRootLogin yes

# Connect from host using
ssh localhost -p 10022
# Make grub load without waiting:

GRUB_HIDDEN_TIMEOUT=0
GRUB_HIDDEN_TIMEOUT_QUIET=true

# TODO
- easy adapt different HMAT arch (virt-qemu-run + domain xml format)



# TODO:
- docker support