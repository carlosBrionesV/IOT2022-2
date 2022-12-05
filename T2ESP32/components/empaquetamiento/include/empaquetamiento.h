
#define HEADER_SIZE 12

int dataLen(int protocol);
int messageLen(int protocol);
char *header(int protocol, int status);
char *mensaje(int protocol, int status, uint8_t val);

char *dataProtocol0(uint8_t val);
char *dataProtocol1(uint8_t val);
char *dataProtocol2(uint8_t val);
char *dataProtocol3(uint8_t val);
char *dataProtocol4(uint8_t val);
char *dataProtocol5(uint8_t val);
