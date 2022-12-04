
#define TCP_MAX_SEND_TRIES 3
#define TCP_MAX_RECV_TRIES 3
#define TCP_TIMEOUT_SEC 45

const char *TCP_TAG = "tcp client";

int socketTcp();
int timeoutTcp(int sock, int timeout_sec);
int connectTcp(int sock, char *ip, int port);
int sendTcp(int sock, char *message, int message_len);
int recvTcp(int sock, char *rx_buffer, int rx_buffer_len);
int manageTcp();

