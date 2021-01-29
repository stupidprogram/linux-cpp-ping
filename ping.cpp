#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <netinet/ip_icmp.h>
#include <unistd.h>
#include <sys/time.h>

#include "ping.hpp"

Ping::Ping(const char* s) : ip(inet_addr(s)), id(getpid()), seq(1)
{
}

void Ping::run()
{
	int fd;
	struct sockaddr_in raddr;
	socklen_t rlen;
	char buff[BUFSIZ];

	fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_ICMP);
	if (fd < 0)
	{
		perror("socket()");
	}

	raddr.sin_family = AF_INET;
	raddr.sin_addr.s_addr = ip;

	struct icmp* my_icmp;
	my_icmp = reinterpret_cast<struct icmp*>(malloc(64));
	
	for (;;)
	{
		icmp_packet(my_icmp);

		int ret = sendto(fd, my_icmp, 64, 0, reinterpret_cast<struct sockaddr*>(&raddr), sizeof(raddr));
		if (ret < 0)
		{
			perror("sendto()");
		}
		int len = read(fd, buff, BUFSIZ);
		parse_packet(buff, len);
		sleep(1);
	}
}

int Ping::icmp_packet(struct icmp* my_icmp)
{
	my_icmp->icmp_type = ICMP_ECHO;
	my_icmp->icmp_cksum = 0;
	my_icmp->icmp_seq = static_cast<uint16_t>(seq++);
	my_icmp->icmp_id = static_cast<uint16_t>(id);
	gettimeofday(reinterpret_cast<struct timeval*>(my_icmp->icmp_data), NULL);
	my_icmp->icmp_cksum = static_cast<uint16_t>(chk_sum(my_icmp, 64));
	return 1;
}

unsigned int Ping::chk_sum(void* my_icmp, int len)
{
	uint16_t* cur = reinterpret_cast<uint16_t*>(my_icmp);
	uint16_t te = 0;
	uint32_t sum = 0;

	while (len > 1)
	{
		sum += *cur++;
		len -= 2;
	}

	if (len == 1)
	{
		te = *reinterpret_cast<uint8_t*>(cur);
		sum += te;
	}

	sum = (sum >> 16) + (sum & 0xffff);
	sum += sum >> 16;
	return static_cast<uint16_t>(~sum);
}

int Ping::parse_packet(void* my_ip, int len)
{
	if (chk_sum(my_ip, len) > 0)
	{
		std::cerr << "chk_sum not valid!" << std::endl;
		return 1;
	}

	struct icmp* my_icmp = reinterpret_cast<struct icmp*>(my_ip);
	struct timeval cur_time;
	gettimeofday(&cur_time, NULL);

	std::cout << "type = " << static_cast<int>(my_icmp->icmp_type) << "\t" << "seq = " << my_icmp->icmp_seq << "\t" << "time = " << cur_time.tv_usec - reinterpret_cast<struct timeval*>(my_icmp->icmp_data)->tv_usec << std::endl;
	return 0;
}































