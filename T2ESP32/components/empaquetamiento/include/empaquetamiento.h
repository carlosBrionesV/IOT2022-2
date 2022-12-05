
#define HEADER_SIZE 12

int dataLen(int protocol);
int messageLen(int protocol);
char *header(int protocol, int status);
char *mensaje(int protocol, int status, uint8_t val);
