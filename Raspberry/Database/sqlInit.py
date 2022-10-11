create_data_table = '''CREATE TABLE Datos (
    IDDevice INTEGER,
    MAC TEXT,
    TransportLayer INTEGER,
    IDProtocol INTEGER,
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
    Data14,
);'''

create_logs_table = '''CREATE TABLE Logs (
    IDDevice INTEGER,
    MAC TEXT,
    TransportLayer INTEGER,
    IDProtocol INTEGER,
    Timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,
);'''

create_config_table = '''CREATE TABLE Config (
    IdProtocol INTEGER,
    TransportLayer INTEGER,
);'''

import sqlite3 as sql

# inicializa la BDD
conn = sql.connect("DB.sqlite")
cur = conn.cursor()
cur.execute(create_data_table)
cur.execute(create_logs_table)
cur.execute(create_config_table)
conn.close()
