create_data_table = '''CREATE TABLE Datos(
    IDDevice,
    MAC TEXT,
    TransportLayer,
    IDProtocol,
    Timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,
    Data1,
    Data2,
    Data3,
    Data4,
    Data5,
    Data6,
    Data7,
    Data8,
    Data9,
    Data10,
    Data11,
    Data12,
    Data13,
    Data14
);'''

create_logs_table = '''CREATE TABLE Logs(
    IDDevice,
    MAC TEXT,
    TransportLayer,
    IDProtocol,
    Timestamp DATETIME DEFAULT CURRENT_TIMESTAMP
);'''

create_config_table = '''CREATE TABLE Config(
    IDProtocol,
    TransportLayer,
);'''

import sqlite3 as sql

def init_database():
    conn = sql.connect('DB.sqlite')
    c = conn.cursor()
    try:
        c.execute(create_data_table)
    except sql.OperationalError:
        print("La tabla Datos ya existe")
    try:
        c.execute(create_logs_table)
    except sql.OperationalError:
        print("La tabla Logs ya existe")
    try:
        c.execute(create_config_table)
    except sql.OperationalError:
        print("La tabla Config ya existe")
    conn.commit()
    conn.close()

init_database()
