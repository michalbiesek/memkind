# README

This is utils/qemu/topology/README.

XML files in this directory simulates different memory architectures.

- knm_all2all.xml - Knights Mill, cluster mode: All2All - memory: DRAM+MCDRAM
- knm_snc2.xml - Knights Mill, cluster mode: SNC-2 - memory: DRAM+MCDRAM
- knm_snc4.xml - Knights Mill, cluster mode: SNC-4 - memory: DRAM+MCDRAM
- clx_2_var1.xml - Cascade Lake, 2 sockets - memory DRAM+PMEM - PMEM on socket 0
- clx_2_var2.xml - Cascade Lake, 2 sockets - memory DRAM+PMEM - PMEM on socket 1
- clx_2_var3.xml - Cascade Lake, 2 sockets - memory DRAM+PMEM - PMEM on both sockets
- clx_4_var1.xml - Cascade Lake, 4 sockets - memory DRAM+PMEM - PMEM on all 4 sockets
- clx_4_var2.xml - Cascade Lake, 4 sockets - memory DRAM+PMEM - PMEM on socket 1
- clx_4_var3.xml - Cascade Lake, 4 sockets - memory DRAM+PMEM - PMEM on sockets 0, 1 and 3
- clx_4_var4.xml - Cascade Lake, 4 sockets - memory DRAM+PMEM - PMEM on sockets 0 and 3




sudo qemu-system-x86_64 -hda qemu-image/ubuntu-20.04.img -enable-kvm -cpu Cascadelake-Server -vnc :5 -machine hmat=on -m 2G -object memory-backend-ram,size=512M,id=m0 -object memory-backend-ram,size=512M,id=m1 -object memory-backend-ram,size=512M,id=m2 -object memory-backend-ram,size=512M,id=m3 -smp 16,sockets=2,dies=1,cores=8,threads=1 -numa node,nodeid=0,memdev=m0,initiator=0 -numa node,nodeid=1,memdev=m1,initiator=1 -numa node,nodeid=2,memdev=m2,initiator=0 -numa node,nodeid=3,memdev=m3,initiator=1 -numa cpu,node-id=0,socket-id=0 -numa cpu,node-id=1,socket-id=1 -numa dist,src=0,dst=0,val=10 -numa dist,src=0,dst=1,val=20 -numa dist,src=0,dst=2,val=17 -numa dist,src=0,dst=3,val=28 -numa dist,src=1,dst=0,val=20 -numa dist,src=1,dst=1,val=10 -numa dist,src=1,dst=2,val=28 -numa dist,src=1,dst=3,val=17 -numa dist,src=2,dst=0,val=17 -numa dist,src=2,dst=1,val=28 -numa dist,src=2,dst=2,val=10 -numa dist,src=2,dst=3,val=28 -numa dist,src=3,dst=0,val=28 -numa dist,src=3,dst=1,val=17 -numa dist,src=3,dst=2,val=28 -numa dist,src=3,dst=3,val=10 -numa hmat-lb,initiator=0,target=0,hierarchy=memory,data-type=access-latency,latency=5 -numa hmat-lb,initiator=0,target=0,hierarchy=memory,data-type=access-bandwidth,bandwidth=20G -numa hmat-lb,initiator=0,target=1,hierarchy=memory,data-type=access-latency,latency=10 -numa hmat-lb,initiator=0,target=1,hierarchy=memory,data-type=access-bandwidth,bandwidth=10G -numa hmat-lb,initiator=0,target=2,hierarchy=memory,data-type=access-latency,latency=10 -numa hmat-lb,initiator=0,target=2,hierarchy=memory,data-type=access-bandwidth,bandwidth=200G -numa hmat-lb,initiator=0,target=3,hierarchy=memory,data-type=access-latency,latency=10 -numa hmat-lb,initiator=0,target=3,hierarchy=memory,data-type=access-bandwidth,bandwidth=180G -numa hmat-lb,initiator=1,target=0,hierarchy=memory,data-type=access-latency,latency=10 -numa hmat-lb,initiator=1,target=0,hierarchy=memory,data-type=access-bandwidth,bandwidth=10G -numa hmat-lb,initiator=1,target=1,hierarchy=memory,data-type=access-latency,latency=5 -numa hmat-lb,initiator=1,target=1,hierarchy=memory,data-type=access-bandwidth,bandwidth=20G -numa hmat-lb,initiator=1,target=2,hierarchy=memory,data-type=access-latency,latency=10 -numa hmat-lb,initiator=1,target=2,hierarchy=memory,data-type=access-bandwidth,bandwidth=180G -numa hmat-lb,initiator=1,target=3,hierarchy=memory,data-type=access-latency,latency=10 -numa hmat-lb,initiator=1,target=3,hierarchy=memory,data-type=access-bandwidth,bandwidth=200G
