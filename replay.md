# UDP







## Check your ip
```bash
$ ip route
default via 192.168.1.1 dev eno1 proto dhcp src 192.168.1.113 metric 100 
169.254.0.0/16 dev eno1 scope link metric 1000 
192.168.0.0/23 dev eno1 proto kernel scope link src 192.168.1.113 metric 100
```


## Rewrite net data
Change pcap ip dest to your ip address (192.168.1.113)
```bash
tcpprep --port --pcap=./a.pcap --cachefile=a.cache
sudo tcprewrite --enet-dmac=00:00:00:00:00:00,00:00:00:00:00:00 --endpoints=192.168.1.113:10.10.1.2 --cachefile=a.cache  -i a.pcap -o b.pcap
```
## Replay net data
```bash
sudo tcpreplay -v -o -i lo ./b.pcap

$ sudo tcpreplay -v --maxsleep=100 -i lo ./b.pcap 
Warning in sendpacket.c:sendpacket_open_pf() line 952:
Unsupported physical layer type 0x0304 on lo.  Maybe it works, maybe it won't.  See tickets #123/318
reading from file -, link-type EN10MB (Ethernet), snapshot length 65535
11:20:03.1693387203 IP 169.254.189.244.7503 > 192.168.1.113.7503: UDP, length 48
11:20:03.1693387203 IP 169.254.189.244.7503 > 192.168.1.113.7503: UDP, length 48
11:20:03.1693387203 IP 169.254.189.244.7503 > 192.168.1.113.7503: UDP, length 48
11:20:03.1693387203 IP 169.254.189.244.7502 > 192.168.1.113.7502: UDP, length 24832
11:20:03.1693387203 IP 169.254.189.244.7503 > 192.168.1.113.7503: UDP, length 48
11:20:03.1693387203 IP 169.254.189.244.7502 > 192.168.1.113.7502: UDP, length 24832
11:20:03.1693387203 IP 169.254.189.244.7502 > 192.168.1.113.7502: UDP, length 24832
11:20:03.1693387203 IP 169.254.189.244.7502 > 192.168.1.113.7502: UDP, length 24832
11:20:03.1693387203 IP 169.254.189.244.7502 > 192.168.1.113.7502: UDP, length 24832
11:20:03.1693387203 IP 169.254.189.244.7503 > 192.168.1.113.7503: UDP, length 48
11:20:03.1693387203 IP 169.254.189.244.7502 > 192.168.1.113.7502: UDP, length 24832
11:20:03.1693387203 IP 169.254.189.244.7502 > 192.168.1.113.7502: UDP, length 24832
11:20:03.1693387203 IP 169.254.189.244.7502 > 192.168.1.113.7502: UDP, length 24832
11:20:03.1693387203 IP 169.254.189.244.7502 > 192.168.1.113.7502: UDP, length 24832
11:20:03.1693387203 IP 169.254.189.244.7503 > 192.168.1.113.7503: UDP, length 48
11:20:03.1693387203 IP 169.254.189.244.7502 > 192.168.1.113.7502: UDP, length 24832
11:20:03.1693387203 IP 169.254.189.244.7502 > 192.168.1.113.7502: UDP, length 24832
11:20:03.1693387203 IP 169.254.189.244.7502 > 192.168.1.113.7502: UDP, length 24832
11:20:03.1693387203 IP 169.254.189.244.7502 > 192.168.1.113.7502: UDP, length 24832
11:20:03.1693387203 IP 169.254.189.244.7502 > 192.168.1.113.7502: UDP, length 24832
11:20:03.1693387203 IP 169.254.189.244.7503 > 192.168.1.113.7503: UDP, length 48
11:20:03.1693387203 IP 169.254.189.244.7502 > 192.168.1.113.7502: UDP, length 24832

```











## Capture and replay
* Dont filter by port. <br>
Filtering by port and then replay will not replay any UDP packets. Port information is not in every packet.
* Don't replay original MAC address. <br>
A simple C socket program will not receive any. Changing destination MAC to 00:00:00:00:00:00 will fix it.
```bash
tshark -nn -i eno1 -c 1000 -f 'host 192.168.1.137' -w capture.pcap
sudo tcpreplay-edit -v -o -i lo --enet-smac=00:00:00:00:00:00 --enet-dmac=00:00:00:00:00:00 capture.pcap

sudo tcpreplay-edit -v -o -i lo --enet-smac=00:00:00:00:00:00 --enet-dmac=00:00:00:00:00:00 --srcipmap=169.254.189.244:192.168.1.6 --dstipmap=169.254.189.246:127.0.0.1 a.pcap

```



### References
* https://www.kali.org/tools/tcpreplay/
* https://tcpreplay.appneta.com/
* https://superuser.com/questions/1638231/cant-receive-udp-data-unless-wireshark-is-running
* https://cse.sc.edu/~pokeefe/tutorials/wireshark/AppToolstcpdump.html
* https://explainshell.com/explain?cmd=tshark+--color+-V+-i+eth0+-d+udp.port%3D8472%2Cvxlan++-c+2+-f+%22port+8472%22
* https://wireshark-users.wireshark.narkive.com/YT5W0lCK/tshark-how-to-capture-snmp-traps-udp-port-162-that-might-be-fragmented
* https://www.youtube.com/watch?v=1gX_-DqGNxA<br>Don't filter by port number
* https://wireshark.marwan.ma/docs/wsug_html_chunked/AppToolseditcap.html
* https://wiki.wireshark.org/SLL.md