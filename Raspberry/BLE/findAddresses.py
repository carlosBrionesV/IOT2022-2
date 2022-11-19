import asyncio
from uuid import UUID
from bleak import BleakScanner
from binascii import hexlify, unhexlify

def findAddresses():
    '''
    Funciones para escanear y encontrar los dispositivos Bluetooth disponibles
    '''
    names = []
    macs = []
    UUIDs = []
    async def main():
        devices = await BleakScanner.discover()
        for d in devices:
            #print(d)
            names.append(d.name)
            macs.append(d.address)
            UUIDs.append(d.details['props']['UUIDs'])
        return devices

    devices = asyncio.run(main())
    return [names,macs,UUIDs]

def handle_data(handle, value):
    """
    handle -- integer, characteristic read handle the data was received on\n
    value -- bytearray, the data returned in the notification
    """
    print("Received data: %s" % value.decode())
    if value.decode() == "OK":
        print("Stop")
        handle.stop()
