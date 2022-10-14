import socket

UDP_IP = "192.168.4.1" # "localhost" 
UDP_PORT = 5010 # Port to listen on (non-privileged ports are > 1023)

def createUDPServer(IP: str, PORT: int):
    sock = socket.socket(socket.AF_INET, #internet
                      socket.SOCK_DGRAM) #UDP
    sock.bind((IP, PORT))
    return sock

def receiveUDPMessage(sock : socket.socket): return sock.recvfrom(1024)
def sendUDPMessage(sock : socket.socket, msg: str, addr: tuple): sock.sendto(msg.encode(), addr)

def main():
    s = createUDPServer(UDP_IP, UDP_PORT)
    print(f"Listening (UDP) on {UDP_IP}:{UDP_PORT}")
    try:
        while True:
            print("Waiting for message..", end=" ")
            msg, addr = receiveUDPMessage(s)
            print(f".OK\nReceived: {msg} from {addr}")
            print(f"Sending message ({msg}) to {addr} ..", end=" ")
            sendUDPMessage(s, msg, addr)
            print(".OK\n")
            if msg.decode() == "quit": break
    except KeyboardInterrupt:
        msg = "quit".encode()
        print(f"Sending message ({msg}) to {addr} ..", end=" ")
        sendUDPMessage(s, msg, addr)
        print(".OK\n")
    s.close()
    print("Server closed")

if __name__ == "__main__": main()
