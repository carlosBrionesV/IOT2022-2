
#define UDP_MAX_SEND_TRIES 3
#define UDP_MAX_RECV_TRIES 1
#define UDP_TIMEOUT_SEC 2

int socketUdp();
int timeoutUdp(int sock, int timeout_sec);
int sendUdp(int sock, char *ip, int port, char *message, int message_len);
int recvUdp(int sock, char *rx_buffer, int rx_buffer_len);
int manageUdp();
