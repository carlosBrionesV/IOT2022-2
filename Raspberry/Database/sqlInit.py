import mysql.connector as sql
# from mysql import Error

from pathlib import Path

folderPath = Path(__file__).parent

# def init_database():
#     create_data_table = \
# '''CREATE TABLE Datos(\
# IDDevice TEXT, \
# MAC TEXT, \
# TransportLayer INTEGER, \
# IDProtocol INTEGER, \
# Timestamp DATETIME DEFAULT CURRENT_TIMESTAMP, \
# Data1, Data2, Data3, \
# Data4, Data5, Data6, \
# Data7, Data8, Data9, \
# Data10, Data11, Data12, \
# Data13, Data14\
# );'''

#     create_logs_table = \
# '''CREATE TABLE Logs(\
# IDDevice TEXT, \
# MAC TEXT, \
# TransportLayer INTEGER, \
# IDProtocol INTEGER, \
# Timestamp DATETIME DEFAULT CURRENT_TIMESTAMP\
# );'''

#     create_config_table = \
# '''CREATE TABLE Config(\
# IDProtocol INTEGER PRIMARY KEY, \
# TransportLayer INTEGER DEFAULT 0\
# );'''

#     conn = sql.connect('DB.sqlite')
#     c = conn.cursor()
#     try:
#         c.execute(create_data_table)
#     except sql.OperationalError:
#         print("La tabla Datos ya existe")
#     try:
#         c.execute(create_logs_table)
#     except sql.OperationalError:
#         print("La tabla Logs ya existe")
#     try:
#         c.execute(create_config_table)
#     except sql.OperationalError:
#         print("La tabla Config ya existe")
    
#     conn.commit()
#     print("Tablas en la base de datos:")
#     print(c.execute("select name from sqlite_master;").fetchall())

#     conn.close()

print("Starting...")
cnn = None
filename = folderPath / 'DB.mysql'


with open((folderPath / "dbFile.sql").resolve(),"r") as sql_file:
    sqlite = sql_file.read()

cnn = sql.connect(
    host="localhost",
    user="rasppi",
    password="tupungatitogaming"
)
print("database connected..")
cs = cnn.cursor()
cs.execute(sqlite)
cnn.commit()

if cnn:
    cnn.close()
print("done..")
