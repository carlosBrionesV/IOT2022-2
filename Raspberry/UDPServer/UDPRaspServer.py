import socket

UDP_IP = "192.168.4.1" # "localhost" 
UDP_PORT = 5010 # Port to listen on (non-privileged ports are > 1023)

def createUDPServer(IP: str, PORT: int):
    sock = socket.socket(socket.AF_INET, #internet
                      socket.SOCK_DGRAM) #UDP
    sock.bind((IP, PORT))
    return sock

def receiveUDPMessage(sock : socket.socket): return sock.recvfrom(1024)
def sendUDPMessage(sock : socket.socket, msg: bytes, addr: tuple): sock.sendto(msg, addr)
def closeUDPServer(sock : socket.socket): sock.close()

