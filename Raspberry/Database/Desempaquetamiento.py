from struct import unpack, pack
import traceback
from DatabaseWork import * 

# Documentación struct unpack,pack :https://docs.python.org/3/library/struct.html#
'''
Estas funciones se encargan de parsear y guardar los datos recibidos.
Usamos struct para pasar de un array de bytes a una lista de numeros/strings. (https://docs.python.org/3/library/struct.html)
(La ESP32 manda los bytes en formato little-endian, por lo que los format strings deben empezar con <)

-dataSave: Guarda los datos en la BDD
-response: genera un OK para mandar de vuelta cuando se recibe un mensaje, con posibilidad de pedir que se cambie el status/protocol
-protUnpack: desempaca un byte array con los datos de un mensaje (sin el header)
-headerDict: Transforma el byte array de header (los primeros 10 bytes de cada mensaje) en un diccionario con la info del header
-dataDict: Transforma el byta array de datos (los bytes luego de los primeros 10) en un diccionario con los datos edl mensaje

'''

    
def response(change:bool=False, status:int=255, protocol:int=255):
    OK = 1
    CHANGE = 1 if change else 0
    return pack("<BBBB", OK, CHANGE, status, protocol)

def parseData(packet):
    header = packet[:12]
    data = packet[12:]
    header = headerDict(header)
    dataD = dataDict(header["protocol"], data)
    if dataD is not None:
        dataSave(header, dataD)
        
    return None if dataD is None else {**header, **dataD}

def protUnpack(protocol:int, data):
    # Valores
    # Val 1B: 1 unsigned char = 1
    # Batt_level 1B: 1 unsigned char = 1 ~ 100 (por tamaño en la tabla de protocolos)
    # Timestamp 4B: 1 long
    # Temp 1B: 1 unsigned char = 5.0 ~ 30.0 (por tamaño en tabla de protocolos)
    # Pres 4B: 1 unsigned int = 1000 ~ 1200
    # Hum 1B: 1 unsigned char = 30 ~ 80 (por tamaño en tabla de protocolos)
    # Co 4B: 1 float = 30.0 ~ 200.0
    # RMS 4B: 1 float
    # Amp 4B: 1 float = 0.004 ~ 0.15
    # Frec 4B: 1 float = 29.0 ~ 91.0
    # Acc 8000B: 2000 float
    protocol_unpack = ["<BBl", "<BBlBIBf", "<BBlBIBff", "<BBlBIBffffffff", "<BBlBIBf2000f2000f2000f", "<B"]
    return unpack(protocol_unpack[protocol], data)

def headerDict(data):
#   H,  B,  B,  B,  B,  B,  B,  B,      B,        H
    ID, M1, M2, M3, M4, M5, M6, TLayer, protocol, leng_msg = unpack("<H6B2BH", data)
    MAC = ".".join([hex(x)[2:] for x in [M1, M2, M3, M4, M5, M6]])
    return {"ID":ID, "MAC":MAC, "TLayer":TLayer, "protocol":protocol, "length":leng_msg}

def dataDict(protocol:int, data):
    if protocol not in [0, 1, 2, 3, 4, 5]:
        print("Error: protocol doesnt exist")
        return None
    def protFunc(protocol, keys):
        def p(data):
            unp = protUnpack(protocol, data)
            return {key:val for (key,val) in zip(keys, unp)}
        return p
    p_base = ["OK"]
    p0 = ["Val", "Batt_level", "Timestamp"]
    p1 = ["Val", "Batt_level", "Timestamp", "Temp", "Pres", "Hum", "Co"]
    p2 = ["Val", "Batt_level", "Timestamp", "Temp", "Pres", "Hum", "Co", "RMS"]
    p3 = ["Val", "Batt_level", "Timestamp", "Temp", "Pres", "Hum", "Co", "RMS", "Amp_x", "Frec_x", "Amp_y", "Frec_y", "Amp_z", "Frec_z"]
    p4 = ["Val", "Batt_level", "Timestamp", "Temp", "Pres", "Hum", "Co", "Acc_x", "Acc_y", "Acc_z"]
    p = [p0, p1, p2, p3, p4, p_base]

    try:
        return protFunc(protocol, p[protocol])(data)
    except Exception:
        print("Data unpacking Error:", traceback.format_exc())
        return None