# UDP

https://www.kali.org/tools/tcpreplay/
https://tcpreplay.appneta.com/
https://superuser.com/questions/1638231/cant-receive-udp-data-unless-wireshark-is-running
https://cse.sc.edu/~pokeefe/tutorials/wireshark/AppToolstcpdump.html
https://explainshell.com/explain?cmd=tshark+--color+-V+-i+eth0+-d+udp.port%3D8472%2Cvxlan++-c+2+-f+%22port+8472%22
https://wireshark-users.wireshark.narkive.com/YT5W0lCK/tshark-how-to-capture-snmp-traps-udp-port-162-that-might-be-fragmented
## Change IP src and dst in a pcap file
```bash
$ sudo tcprewrite --cachefile=in.cache --endpoints=192.168.1.113:192.168.1.137  --infile=in.pcap --outfile=out.pcap
$ tshark -r out.pcap
15190  56.824000 192.168.1.137 → 192.168.1.113 UDP 24874 7502 → 7502 Len=24832
15191  56.827000 192.168.1.137 → 192.168.1.113 UDP 90 7503 → 7503 Len=48
15192  56.827000 192.168.1.137 → 192.168.1.113 UDP 24874 7502 → 7502 Len=24832
15193  56.828000 192.168.1.137 → 192.168.1.113 UDP 24874 7502 → 7502 Len=24832
15194  56.829000 192.168.1.137 → 192.168.1.113 UDP 24874 7502 → 7502 Len=24832
15195  56.832000 192.168.1.137 → 192.168.1.113 UDP 24874 7502 → 7502 Len=24832
15196  56.832000 192.168.1.137 → 192.168.1.113 UDP 90 7503 → 7503 Len=48
15197  56.833000 192.168.1.137 → 192.168.1.113 UDP 24874 7502 → 7502 Len=24832
15198  56.833000 192.168.1.137 → 192.168.1.113 UDP 24874 7502 → 7502 Len=24832
15199  56.835000 192.168.1.137 → 192.168.1.113 UDP 24874 7502 → 7502 Len=24832
15200  56.836000 192.168.1.137 → 192.168.1.113 UDP 24874 7502 → 7502 Len=24832
15201  56.838000 192.168.1.137 → 192.168.1.113 UDP 24874 7502 → 7502 Len=24832
15202  56.839000 192.168.1.137 → 192.168.1.113 UDP 24874 7502 → 7502 Len=24832
15203  56.840000 192.168.1.137 → 192.168.1.113 UDP 90 7503 → 7503 Len=48
15204  56.841000 192.168.1.137 → 192.168.1.113 UDP 24874 7502 → 7502 Len=24832
15205  56.842000 192.168.1.137 → 192.168.1.113 UDP 24874 7502 → 7502 Len=24832
15206  56.844000 192.168.1.137 → 192.168.1.113 UDP 24874 7502 → 7502 Len=24832
15207  56.845000 192.168.1.137 → 192.168.1.113 UDP 24874 7502 → 7502 Len=24832
15208  56.848000 192.168.1.137 → 192.168.1.113 UDP 24874 7502 → 7502 Len=24832
15209  56.848000 192.168.1.137 → 192.168.1.113 UDP 90 7503 → 7503 Len=48
15210  56.849000 192.168.1.137 → 192.168.1.113 UDP 24874 7502 → 7502 Len=24832
15211  56.856000 192.168.1.137 → 192.168.1.113 UDP 90 7503 → 7503 Len=48
15212  56.864000 192.168.1.137 → 192.168.1.113 UDP 90 7503 → 7503 Len=48
15213  56.872000 192.168.1.137 → 192.168.1.113 UDP 90 7503 → 7503 Len=48
15214  56.880000 192.168.1.137 → 192.168.1.113 UDP 90 7503 → 7503 Len=48
15215  56.888000 192.168.1.137 → 192.168.1.113 UDP 90 7503 → 7503 Len=48
```

## Replay UDP packets from pcap file

```bash
sudo tcpreplay -v -tK -i lo out.pcap
sudo tcpreplay -v -o -i lo out.pcap

sudo tcpreplay-edit -v -o -i lo --enet-dmac=00:00:00:00:00:00 hello.pcap
sudo tcpreplay-edit -v -o -i lo --enet-smac=00:00:00:00:00:00 --enet-dmac=00:00:00:00:00:00 OS1-128.pcap
```


## Save UDP packets in pcap file

```bash
sudo tcpdump -i any port 7502 or port 7503 -c 100 -w out.pcap
sudo tcpdump -s0 -T snmp -i eno1 port 7502 or port 7503 -c 1000 -w OS1-16_1.pcap
sudo tcpdump -nnXSs0 -T snmp -i eno1 port 7502 or port 7503 -c 1000 -w OS1-16_1.pcap
tcpdump -i <interface> -s 1500 -w <some-file>
```


## View packet
```bash
sudo tshark -f "udp port 7502" -i lo -T fields -e eth.dst
sudo tshark -i lo -T fields -e ip.proto -e frame.len -e ip.src -e udp.srcport -e udp.dstport
```


## Fix tshark
```bash
sudo dpkg-reconfigure wireshark-common
sudo chmod +x /usr/bin/dumpcap
```