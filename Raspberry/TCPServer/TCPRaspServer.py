import socket

from Database.Desempaquetamiento import *

TCP_IP = "192.168.4.1" # "localhost"
TCP_PORT = 5000  # Port to listen on (non-privileged ports are > 1023)

def createTCPServer(IP: str, PORT: int):
    sock = socket.socket(socket.AF_INET, #internet
                      socket.SOCK_STREAM) #TCP
    sock.bind((IP, PORT))
    sock.listen(5)
    return sock

def acceptTCPConnection(sock: socket.socket): return sock.accept()
def sendTCPMessage(conn: socket.socket, msg): conn.send(msg)
def receiveTCPMessage(conn: socket.socket): return conn.recv(1024)
def closeTCPConnection(conn: socket.socket): conn.close()
def closeTCPServer(sock: socket.socket): sock.close()

def handleProt5(conn: socket.socket, headerD: dict, dataD: dict):
    """
    Consulta la BDD por la configuracion de TransportLayer de un IDProtocol,
    si no existe la crea como un nuevo registro (0 TCP).\n
    Luego envia la respuesta y retorna un booleano que indica si se debe
    cambiar el protocolo de transporte.
    """
    print("Protocol 5: Query config")
    TLayer = getConfig(dataD["Val"], True)
    change = True
    print(f"Queried Config for protocol {dataD['Val']}: {TLayer} ({'TCP' if TLayer == 0 else 'UDP'})")
    resp = response(change, TLayer, dataD["Val"])
    print(f"Sending message ({resp}) ..", end="")
    sendTCPMessage(conn, resp)
    print(".Ok\nResponse sent")
    return change, TLayer, dataD["Val"]
