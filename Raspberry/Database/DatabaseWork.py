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
        con.commit()
        print("Datos guardados")
        cur.close()
        
def queryConfig(protocol):
    with sql.connect("DB.sqlite") as con:
        cur = con.cursor()
        cur.execute("SELECT * FROM Config WHERE IDProtocol = ?", (protocol))
        rows = cur.fetchall()
        if len(rows) == 0:
            alternateConfig(protocol)
            res : list = queryConfig(protocol)
            return res
        else:
            return rows[0]

# alterna la tranport layer de un protocolo, si el protocolo no existe lo crea con la transport layer por defecto (0 = UDP)
def alternateConfig(protocol):
    with sql.connect("DB.sqlite") as con:
        cur = con.cursor()
        cur.execute("SELECT * FROM Config WHERE IDProtocol = ?", (protocol))
        rows = cur.fetchall()
        if len(rows) == 0:
            cur.execute("INSERT INTO Config (IDProtocol, TransportLayer) values (?)", (protocol, 0))
            print(f"Configuración alternativa creada: protocolo {protocol}, Tlayer 0")
        else:
            if rows[0][1] == 0:
                cur.execute("UPDATE Config SET TransportLayer = 1 WHERE IDProtocol = ?", (protocol))
                print(f"Configuración alternativa actualizada: protocolo {protocol}, Tlayer 1")
            else:
                cur.execute("UPDATE Config SET TransportLayer = 0 WHERE IDProtocol = ?", (protocol))
                print(f"Configuración alternativa actualizada: protocolo {protocol}, Tlayer 0")
        con.commit()
        cur.close()
