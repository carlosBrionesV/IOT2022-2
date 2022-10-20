
from enum import Enum
from TCPServer.TCPRaspServer import *
from UDPServer.UDPRaspServer import *
from Database.Desempaquetamiento import *
from Database.DatabaseWork import *
from Database.sqlInit import *

# # 
# En este archivo se encuentra el main del programa que correra en la Raspberry.
# 
# Inicialmente se crea la base de datos y dos servidores, uno TCP y otro UDP.
# Se espera una conexion TCP para recibir un mensaje de consulta por la configuracion inicial.
# (Este mensaje tiene header con IDProtocol = 5 y data con Val = 0-4)
# Se consulta la configuracion para un IDProtocol en dataD["Val"].
# Una vez realizada la consulta, se envia una respuesta y cierra la conexion TCP.
# 
# Luego, dependiendo de la configuracion del IDProtocol consultado, espera mensajes de ese protocolo
# en un ciclo de recepcion.
# Para cada mensaje recibido, se desempaqueta y se guarda en la base de datos un log y los datos.
# 
# Para terminar el ciclo, se puede utilizar Ctrl+C, lo cual sera interceptado para ofrecer la opcion
# de cambiar de IDProtocol de datos o cambiar de TransportLayer de recepcion.
# Utilizar Ctrl+C nuevamente terminara el main.
# #


class SOCKTYPE(Enum):
  TCP = 0
  UDP = 1

def initialSetup():
  # start database
  print("Starting database..")
  init_database()
  print("Database created\n")

  # create a TCP server
  print("Creating TCP server..", end="")
  TCPs = createTCPServer(TCP_IP, TCP_PORT)
  print(f".OK\nListening (TCP) on {TCP_IP}:{TCP_PORT}")

  # create a UDP server
  print("Creating UDP server..", end="")
  UDPs = createUDPServer(UDP_IP, UDP_PORT)
  print(f".OK\nListening (UDP) on {UDP_IP}:{UDP_PORT}")
  
  return TCPs, UDPs

def initialConfig(sock:socket.socket):
  """
  Usa el socket para recibir un mensaje de configuracion inicial. \n
  Retorna si se debe cambiar a UDP y el IDProtocol a usar
  """
  while True:
    print("Waiting for message..", end="")
    pkt = receiveTCPMessage(sock)
    print(f".OK\nReceived: {pkt}")
    (headerD, dataD) = parseMsg(pkt)
    print(f"Header: {headerD}\nData: {dataD}")
    logSave(headerD)

    if headerD["IDProtocol"] == 5:
      (change, TLayer, IDProt) = handleProt5(sock, headerD, dataD)
      break
    else: print("Not initial conection")
  return change, TLayer, IDProt

def recieveWithTCP(sock: socket.socket, TLayer = 0, IDProt = 0):
  """
  Recibe mensajes y responde con TCP en ciclo. \n
  Al terminar, retorna si se debe cambiar configuracion, TLayer y IDProtocol
  """
  change = False
  while True:
    try:
      print("Waiting for message..", end="")
      pkt = receiveTCPMessage(sock)
      
      print(f".OK\nReceived: {pkt}")
      (headerD, dataD) = parseMsg(pkt)
      print(f"Header: {headerD}\nData: {dataD}")

      if headerD["IDProtocol"] == 5:
        (change, TLayer, IDProt) = handleProt5(sock, headerD, dataD)
      else:
        logSave(headerD)
        dataSave(headerD, dataD)
        res = response(change, TLayer, IDProt)
        print(f"Sending: {res}")
        sendTCPMessage(sock, res)
        print("OK")
      # si se debe cambiar algo acaba el ciclo de recepcion
      if change: break
      # continuamos recibiendo mensajes
      continue

    # si interrumpimos con ctrl+c el ciclo de recepcion
    except KeyboardInterrupt:
      print("Interrupted by user, offering to change Config :)")
      print("To exit the whole program, use ctrl+c again during this setup")
      try:
        while True:
          i = input(f"Do you want to change IDProtocol?, current: {IDProt} (y/n): ").lower().strip()
          if i == "y" or i == "n": break
          else: print("Invalid input")
        # change IDProtocol
        if i == "y":
          while True:
            i = input("Which IDProtocol? (0-4): ").strip()
            if i.isdigit() and int(i) >= 0 and int(i) <= 4: break
            else: print("Invalid IDProtocol")
          IDProt = int(i)
          change = True
        
        # query TransportLayer for the current (maybe changed) IDProtocol
        currentTL = 'TCP' if getConfig(IDProt, True) == SOCKTYPE.TCP.value else 'UDP'
        TLayer = SOCKTYPE.TCP.value if currentTL == 'TCP' else SOCKTYPE.UDP.value
        
        while True:
          i = input(f"Do you want to alternate TransportLayer for the IDProtocol {IDProt}, current: {currentTL}? (y/n): ")
          if i == "y" or i == "n": break
        # change TransportLayer
        if i == "y":
          alternateConfig(IDProt)
          # set alternated TransportLayer
          TLayer = SOCKTYPE.UDP.value if currentTL == "TCP" else SOCKTYPE.TCP.value
          currentTL = 'TCP' if TLayer == SOCKTYPE.TCP.value else 'UDP'
          change = True

        # Se recibe un ultimo mensaje TCP
        # La siguiente respuesta informara al ESP si se debe cambiar IDProtocol y/o TransportLayer
        # Finalmente, se terminara el ciclo de recepcion
        print(f"Current config: TLayer {TLayer}, IDProt {IDProt}")
        continue

      except KeyboardInterrupt:
        print("Interrupted by user x2, ending reception loop")
        break

    # error no identificado en el ciclo de recepcion
    except Exception as e:
      print(f"Error: {e}")
      # se termina el ciclo de recepcion
      break

  # se returna change para que el main sepa si debe cambiar a config o fue un error
  return change, TLayer, IDProt

def recieveWithUDP(sock: socket.socket, TLayer = 0, IDProt = 0):
  """
  Recibe mensajes y responde con UDP en ciclo. \n
  Al terminar, retorna si se debe cambiar la config, TLayer y IDProtocol
  """
  change = False
  while True:
    try:
      print("Waiting for message..", end="")
      pkt, address = receiveUDPMessage(sock)
      print(f".OK\nReceived: {pkt}")
      (headerD, dataD) = parseMsg(pkt)
      print(f"Header: {headerD}\nData: {dataD}")

      logSave(headerD)
      dataSave(headerD, dataD)
      res = response(change, TLayer, IDProt)
      print(f"Sending: {res}")
      sendUDPMessage(sock, res, address)
      print("OK")
      # si se debe cambiar a TCP acaba el ciclo de recepcion
      if change: break
      # continuamos recibiendo mensajes
      continue

    # si interrumpimos con ctrl+c el ciclo de recepcion
    except KeyboardInterrupt:
      print("Interrupted by user, offering to change Config :)")
      print("To exit the whole program, use ctrl+c again during this setup")
      try:
        while True:
          i = input(f"Do you want to change IDProtocol?, current: {IDProt} (y/n): ").lower().strip()
          if i == "y" or i == "n": break
          else: print("Invalid input")
        # change IDProtocol
        if i == "y":
          while True:
            i = input("Which IDProtocol? (0-4): ").strip()
            if i.isdigit() and int(i) >= 0 and int(i) <= 4: break
            else: print("Invalid IDProtocol")
          IDProt = int(i)
          change = True
        
        # query TransportLayer for the current (maybe changed) IDProtocol, insert it if it doesn't exist
        currentTL = 'TCP' if getConfig(IDProt, True) == SOCKTYPE.TCP.value else 'UDP'
        TLayer = SOCKTYPE.TCP.value if currentTL == "TCP" else SOCKTYPE.UDP.value
        
        while True:
          i = input(f"Do you want to alternate TransportLayer for the IDProtocol {IDProt}, current: {currentTL}? (y/n): ")
          if i == "y" or i == "n": break
        # change TransportLayer
        if i == "y":
          alternateConfig(IDProt)
          # set alternated TransportLayer
          TLayer = SOCKTYPE.UDP.value if currentTL == "TCP" else SOCKTYPE.TCP.value
          currentTL = 'TCP' if TLayer == SOCKTYPE.TCP.value else 'UDP'
          change = True

        # Se recibe un ultimo mensaje UDP
        # La siguiente respuesta informara al ESP si se debe cambiar IDProtocol y/o TransportLayer
        # Finalmente, se terminara el ciclo de recepcion
        print(f"Current config: TLayer {TLayer}, IDProt {IDProt}")
        continue

      except KeyboardInterrupt:
        print("Interrupted by user x2, ending reception loop")
        break
  
    # error no identificado en el ciclo de recepcion
    except Exception as e:
      print(f"Error: {e}")
      # se termina el ciclo de recepcion
      break

  # se returna change para que el main sepa si debe cambiar config
  return change, TLayer, IDProt


def main():
  (TCPs0, UDPs) = initialSetup()
  
  # initial conection
  print("Waiting for conection..", end="")
  TCPs, addr = acceptTCPConnection(TCPs0)
  print(f"OK\nConnection from {addr}")
  (change, TLayer, IDProt) = initialConfig(TCPs)
  currentType = SOCKTYPE.TCP if TLayer == 0 else SOCKTYPE.UDP
  currentSock = TCPs if currentType == SOCKTYPE.TCP else UDPs
  print(f"Current config: TLayer {currentType}, IDProt {IDProt}")
  change = False

  # main loop
  while True:
    if currentType == SOCKTYPE.TCP:
      (change, TLayer, IDProt) = recieveWithTCP(currentSock, currentType.value, IDProt)
    elif currentType == SOCKTYPE.UDP:
      (change, TLayer, IDProt) = recieveWithUDP(currentSock, currentType.value, IDProt)
    
    if change:
      currentType = SOCKTYPE.TCP if TLayer == 0 else SOCKTYPE.UDP
      currentSock = UDPs if currentType == SOCKTYPE.UDP else TCPs
      print(f"Current config: TLayer {currentType}, IDProt {IDProt}")
      change = False
    else:
      print("Ending program")
      break
    
    # si se debe cambiar a TCP, empieza un ciclo de recepcion TCP
    # si se debe cambiar a UDP, empieza un ciclo de recepcion UDP
    # si no se debe cambiar, se termina el programa
    continue
  
  # cerrar conexiones
  closeTCPServer(TCPs)
  closeUDPServer(UDPs)
  print("Bye!")

if __name__ == "__main__": main()