import json
import sqlite3 as sql

# Documentación https://docs.python.org/3/library/sqlite3.html

def dataSave(header, data):
    if header["protocol"] == 0: N = 3
    elif header["protocol"] == 1: N = 7
    elif header["protocol"] == 2: N = 8
    elif header["protocol"] == 3: N = 14
    elif header["protocol"] == 4: N = 10
    query = '''INSERT INTO Datos (IDDevice, MAC, TransportLayer, IDProtocol'''
    values = ''') values (?, ?, ?, ?'''
    for i in range(N):
        query += f", Data{i}"
        values += ", ?"
    values += ")"
    query += values
    
    with sql.connect("DB.sqlite") as con:
        cur = con.cursor()
        cur.execute(query, 
            (header["ID"], header["MAC"], header["TLayer"], header["protocol"], json.dumps(data)))
        
def dataGet():
    with sql.connect("DB.sqlite") as con:
        cur = con.cursor()
        cur.execute("SELECT * FROM Config")
        rows = cur.fetchall()
        return rows

def modifyConfig(protocol, TLayer):
    with sql.connect("DB.sqlite") as con:
        cur = con.cursor()
        cur.execute("UPDATE Config SET IdProtocol = ?, TransportLayer = ?", 
            (protocol, TLayer))
        con.commit()
        print(f"Configuración actualizada: protocolo {protocol}, Tlayer {TLayer}")
