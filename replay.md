# UDP







## Check your ip
```bash
$ ip route
default via 192.168.1.1 dev eno1 proto dhcp src 192.168.1.113 metric 100 
169.254.0.0/16 dev eno1 scope link metric 1000 
192.168.0.0/23 dev eno1 proto kernel scope link src 192.168.1.113 metric 100
```


## Rewrite net data
Change pcap ip dest to your ip address (default via 192.168.1.1)
```bash
tcpprep --port --pcap=./a.pcap --cachefile=a.cache
sudo tcprewrite --endpoints=192.168.1.113:169.254.189.244 --enet-smac=00:00:00:00:00:00 --enet-dmac=00:00:00:00:00:00 --cachefile=a.cache  --infile=a.pcap --outfile=b.pcap
```
## Replay net data
```bash
sudo tcpreplay -v -o -i lo ./b.pcap
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