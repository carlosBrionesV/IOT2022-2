# Raspberry

```
Raspberry
|
|--- Database 
|    |--- DatabaseWork.py
|    |--- Desempaquetamiento.py
|    |--- sqlInit.py
|
|--- TCPServer
|--- UDPServer
|--- main.py
```

## Main
Contiene el programa que correra el servidor, inicializacion de la BDD, la creacion de sockets TCP y UDP, la configuracion inicial y los ciclos de recepcion de mensajes, ademas de varios inputs de usuario para alterar la configuracion durante la ejecucion.

## Database
Contiene las funciones de python relacionadas a la base de datos y el desempaquetamiento de mensajes.

- ### DatabaseWork
  Contiene las funciones para guardar datos, logs y modificar la configuracion de recepcion (TransportLayer).

- ### Desempaquetamiento
  Contiene las funciones para desempaquetar los mensajes recibidos por la ESP. Ademas, tiene una funcion para empaquetar una respuesta simple a mandar.

- ### sqlInit
  Contiene la funcion para generar la BDD.

## TCPServer
Contiene las funciones para administrar sockets TCP. Ademas, tiene una funcion para manejar el caso inicial de conexion, donde se pregunta con IDProtocol 5 la configuracion actual.

## UDPServer
Continene las funciones para administrar sockets UDP.
