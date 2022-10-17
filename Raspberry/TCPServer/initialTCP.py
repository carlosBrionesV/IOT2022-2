from Raspberry.Database import Desempaquetamiento
import TCPRaspServer

def initial_protocol(headerD, dataD):
  # se consulta por el protocolo en "Val"
  q = Desempaquetamiento.queryConfig(dataD["Val"])
  srv_protocol = q[0]
  srv_tLayer = q[1]
  if headerD["TLayer"] == srv_tLayer: change = False
  else: change = True
  r = Desempaquetamiento.response(change, srv_tLayer, srv_protocol)
  return r


def main():
  s = TCPRaspServer.createTCPServer(TCPRaspServer.TCP_IP, TCPRaspServer.TCP_PORT)
  print(f"Listening (TCP) on {TCPRaspServer.TCP_IP}:{TCPRaspServer.TCP_PORT}")
  conn, addr = TCPRaspServer.acceptTCPConnection(s)
  print(f"Connection accepted from {addr}")
  try:
    while True:
      print("Waiting for message..", end=" ")
      msg = TCPRaspServer.receiveTCPMessage(conn)
      print(f".OK\nReceived: {msg} from {addr}")

      # Desempaquetamiento
      print("Desempaquetando mensaje")
      headerD, dataD = Desempaquetamiento.parseMsg(msg)
      if headerD is None:
        print("ERROR")
        continue
      elif headerD["protocol"] == 5:
        print("Protocolo 5: consulta de TLayer y protocolo")
        response = initial_protocol(headerD, dataD)
        print(f"Response: {response}")
        TCPRaspServer.sendTCPMessage(conn, response)
      else:
        print("Protocolo ")
        continue
      if msg == "quit": break
  except KeyboardInterrupt:
    msg = "quit"
    print(f"Sending message ({msg}) to {addr} ..", end=" ")
    TCPRaspServer.sendTCPMessage(conn, msg)
    print(".OK\n")
  conn.close()
  s.close()
  print("Server closed")
