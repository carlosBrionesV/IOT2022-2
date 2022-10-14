import socket

TCP_IP = "192.168.4.1" # "localhost"
TCP_PORT = 5000  # Port to listen on (non-privileged ports are > 1023)

def createTCPServer(IP: str, PORT: int):
    sock = socket.socket(socket.AF_INET, #internet
                      socket.SOCK_STREAM) #TCP
    sock.bind((IP, PORT))
    sock.listen(5)
    return sock

def acceptTCPConnection(sock: socket.socket): return sock.accept()
def sendTCPMessage(conn: socket.socket, msg: str): conn.send(msg)
def receiveTCPMessage(conn: socket.socket): return conn.recv(1024)
def closeTCPConnection(conn: socket.socket): conn.close()

def main():
    s = createTCPServer(TCP_IP, TCP_PORT)
    print(f"Listening (TCP) on {TCP_IP}:{TCP_PORT}")
    conn, addr = acceptTCPConnection(s)
    print(f"Connection from {addr}")
    try:
        while True:
            print("Waiting for message..", end=" ")
            msg = receiveTCPMessage(conn)
            print(f".OK\nReceived: {msg}")
            print(f"Sending message ({msg}) ..", end=" ")
            sendTCPMessage(conn, msg)
            print(".OK\n")
            if msg.decode() == "quit": break
    except KeyboardInterrupt:
        msg = "quit".encode()
        print(f"Sending message ({msg}) to {addr} ..", end=" ")
        sendTCPMessage(conn, msg)
        print(".OK\n")
    
    closeTCPConnection(conn)
    print("Connection closed")

if __name__ == "__main__": main()
