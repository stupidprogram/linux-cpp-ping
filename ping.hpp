#ifndef PING_H__
#define PING_H__
class Ping
{
public:
    Ping(const char* s);
    ~Ping();
    void run();

private:
    unsigned int ip;
    int icmp_packet(struct icmp* my_icmp);
	int id;
	int seq;
	unsigned int chk_sum(void* my_icmp, int len);
	int parse_packet(void* my_icmp, int len);
};



#endif
