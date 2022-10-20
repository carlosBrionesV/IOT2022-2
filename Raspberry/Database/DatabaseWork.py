import json
import sqlite3 as sql

# Documentación https://docs.python.org/3/library/sqlite3.html

def dataSave(header: dict, data: dict):
    """
    Guarda los datos en la BDD \n
    header: diccionario con los datos del encabezado \n
    data: diccionario con los datos del paquete
    """
    # se busca el protocolo para ver cuantos datos se van a guardar
    if header["IDProtocol"] == 0: N = 3
    elif header["IDProtocol"] == 1: N = 7
    elif header["IDProtocol"] == 2: N = 8
    elif header["IDProtocol"] == 3: N = 14
    elif header["IDProtocol"] == 4: N = 10
    elif header["IDProtocol"] == 5: return

    query = '''INSERT INTO Datos'''
    columns = '''IDDevice, MAC, TransportLayer, IDProtocol'''
    values = '''?, ?, ?, ?'''
    for i in range(N):
        columns += f", Data{i+1}"
        values += ", ?"
    query += f"({columns}) VALUES ({values})"

    with sql.connect("DB.sqlite") as con:
        cur = con.cursor()
        cur.execute(query,(
            header["IDDevice"], 
            header["MAC"], 
            header["TransportLayer"], 
            header["IDProtocol"], 
            json.dumps(data)
            ))
        con.commit()
        print(f"(BDD DATA) Saved data with IDProtocol {header['IDProtocol']}")
        cur.close()

def logSave(header: dict):
    """
    Guarda las estadisticas de un mensaje en la BDD \n
    header: diccionario con los datos del encabezado
    """
    query = '''INSERT INTO Logs (IDDevice, MAC, TransportLayer, IDProtocol) VALUES (?, ?, ?, ?)'''
    with sql.connect("DB.sqlite") as con:
        cur = con.cursor()
        cur.execute(query,(
            header["IDDevice"], 
            header["MAC"], 
            header["TransportLayer"], 
            header["IDProtocol"]
            ))
        con.commit()
        print(f"(BDD LOG) Log saved for IDDevice {header['IDDevice']}")
        cur.close()

def getConfig(protocol: int, create = False) -> int:
    """
    Devuelve la TransportLayer de un protocolo dado en la BDD. \n
    Si el protocolo no existe y create es True lo crea con la transport layer por defecto \n
    (TransportLayer: 0 = TCP, 1 = UDP)
    """
    query = "SELECT * FROM Config WHERE IDProtocol = ?"
    with sql.connect("DB.sqlite") as con:
        cur = con.cursor()
        cur.execute(query, (protocol,))
        rows = cur.fetchall()
        if len(rows) == 0:
            if create:
                alternateConfig(protocol)
                return 0
            else: return -1
        return rows[0][1]

def alternateConfig(IDProtocol : int):
    """
    Alterna la TranportLayer de un protocolo dado en la BDD. \n
    Si el protocolo no existe lo crea con la transport layer por defecto \n
    (TransportLayer: 0 = TCP, 1 = UDP)
    """
    # se busca si existe el protocolo en la BDD
    query = "SELECT * FROM Config WHERE IDProtocol = ?"
    with sql.connect("DB.sqlite") as con:
        cur = con.cursor()
        cur.execute(query, (IDProtocol,))
        rows = cur.fetchall()
        # si no existe se crea
        if len(rows) == 0:
            insert_query = "INSERT INTO Config (IDProtocol) VALUES (?)"
            cur.execute(insert_query, (IDProtocol,))
            print(f"(BDD Config) INSERT: IDProtocol {IDProtocol} - TransportLayer 0 (TCP)")
        # si existe se alterna
        else:
            update_query = "UPDATE Config SET TransportLayer = ? WHERE IDProtocol = ?"
            if rows[0][1] == 0: val = 1
            else: val = 0
            cur.execute(update_query, (val, IDProtocol,))
            print(f"(BDD Config) UPDATE: IDProtocol {IDProtocol} => TransportLayer {val} ({'TCP' if val == 0 else 'UDP'})")
        con.commit()
        cur.close()
