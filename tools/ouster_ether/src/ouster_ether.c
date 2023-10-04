// answer to https://quant.stackexchange.com/q/55088/35
// compile with c++ pcap_udp.cpp -lpcap
// https://gist.github.com/chrisaycock/f0e064314cdfe48d5d662fd188332b74
// https://stackoverflow.com/questions/15393905/c-pcap-library-unknown-types-error
// https://gist.github.com/Taehun/2502332

#include <arpa/inet.h>
#include <errno.h>
#include <net/ethernet.h>
#include <netinet/ether.h>
#include <netinet/if_ether.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <pcap/pcap.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>

void print_ether_host(uint8_t const ether_host[ETH_ALEN])
{
	for (int i = 0; i < ETH_ALEN; ++i) {
		printf("%.2X ", ether_host[i]);
	}
}

void print_eth(struct ether_header const *eptr)
{
	uint16_t type = ntohs(eptr->ether_type);
	switch (type) {
	case ETHERTYPE_IP:
		printf("ether_shost = ");
		print_ether_host(eptr->ether_shost);
		printf("\n");
		printf("ether_dhost = ");
		print_ether_host(eptr->ether_dhost);
		printf("\n");
		break;
	case ETHERTYPE_ARP:
		break;
	default:
		printf("Ethernet type %x not IP", ntohs(eptr->ether_type));
		exit(1);
		break;
	}
}

void print_ip(struct ip const *iph)
{
	printf("IP Ver = %d\n", iph->ip_v);
	printf("IP Header len = %d\n", iph->ip_hl << 2);
	printf("IP Source Address = %s\n", inet_ntoa(iph->ip_src));
	printf("IP Dest Address = %s\n", inet_ntoa(iph->ip_dst));
}

void print_udp(struct udphdr const *udp)
{
	u_int16_t source_port = ntohs(udp->uh_sport);
	u_int16_t destination_port = ntohs(udp->uh_dport);
	u_int16_t len = ntohs(udp->uh_ulen);
	u_int16_t checksum = ntohs(udp->uh_sum);
	printf("UDP source_port = %d\n", source_port);
	printf("UDP destination_port = %d\n", destination_port);
	printf("UDP len = %d\n", len);
	printf("UDP checksum = %d\n", checksum);
}

void get_payload(const u_char *packet)
{
	printf("====================\n");
	struct ether_header const *eptr = (struct ether_header const *)packet;
	print_eth(eptr);
	packet += ETHER_HDR_LEN;
	struct ip const *iph = (struct ip const *)packet;
	print_ip(iph);
	if (iph->ip_p != IPPROTO_UDP) {
		return;
	}
	packet += iph->ip_hl << 2;
	struct udphdr const *udp = (struct udphdr const *)packet;
	print_udp(udp);
	return;
}

int main(int argc, char *argv[])
{
	// verify parameters
	if (argc != 2) {
		printf("Usage: %s file.pcap\n", argv[0]);
		return 1;
	}

	// open the pcap file
	char errbuf[PCAP_ERRBUF_SIZE];
	pcap_t *p = pcap_open_offline(argv[1], errbuf);
	if (p == NULL) {
		printf("pcap_open_offline error: %s\n", errbuf);
		return 1;
	}

	if (pcap_datalink(p) != DLT_EN10MB) {
		fprintf(stderr, "%s is not an Ethernet capture\n", argv[1]);
		exit(2);
	}

	struct bpf_program fp; /* compiled filter program (expression) */
	char filter_exp[] = "host 169.254.189.244";
	/* compile the filter expression */
	if (pcap_compile(p, &fp, filter_exp, 0, 0) == -1) {
		fprintf(stderr, "Couldn't parse filter %s: %s\n",
		        filter_exp, pcap_geterr(p));
		exit(EXIT_FAILURE);
	}

	/* apply the compiled filter */
	/*
	if (pcap_setfilter(p, &fp) == -1)
	{
	    fprintf(stderr, "Couldn't install filter %s: %s\n",
	            filter_exp, pcap_geterr(p));
	    exit(EXIT_FAILURE);
	}
	*/

	int done = 0;
	while (!done) {
		// read a packet
		struct pcap_pkthdr *header;
		const u_char *packet;
		int ret = pcap_next_ex(p, &header, &packet);

		// handle packet
		switch (ret) {
		case 1:
			// legitimate packet
			get_payload(packet);
			break;
		case 0:
			// timeout
			break;
		case -1:
			// error
			printf("pcap_geterr %s\n", pcap_geterr(p));
			return 1;
		case -2:
			// end of file
			done = 1;
			break;
		}
	}

	return 0;
}
