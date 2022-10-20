# ESP32

```
ESP32
|
|--- main  
|--- conexiones  
|--- empaquetamiento  
|--- sensores  
```
### Main:
 Usara a los demás para realizar el flow de envio de datos, siendo el programa main del proyecto. <br>
 (Queriamos realizar la compilacion por componentes, pero no sabiamos como hacerlo, asi que terminamos importando 
 directamente con #include los demas archivos)

### Conexiones:
 - wifi_client: Contiene los codigos para conectarse a un WiFi, Importa de la configuracion (menuconfig) la SSID y contraseña de la red a la que conectarse.
 - tcp_socket: Contiene las funciones relacionadas a administrar el uso de sockets tcp. Importa de la configuracion la direccion del servidor TCP al que conectarse.
 - udp_socket: Contiene las funciones relacionadas a administrar el uso de sockets udp. Importa de la configuracion la direccion del servidor UDP al que enviar mensajes.

### Empaquetamiento:
 Tiene funciones para generar mensajes de envio de todos los protocolos. Llama a todo lo necesario desde sensores.
 
### Sensores:
 Genera todos los números necesarios para los datos, simulando ser sensores.
