import numpy
from ui import Ui_Dialog
from enum import Enum

from PyQt5 import QtCore, QtGui, QtWidgets
import time
import logging

class Status(Enum):
    BLUETHOOTH_CONFIG = 0
    TCP_BD = 20
    TCP_CONTINUA = 21
    TCP_DISCONTINUA = 22
    UDP = 23
    BLE_CONTINUA = 30
    BLE_DISCONTINUA = 31

defaultValues = {
    "modoConfig" : Status.BLUETHOOTH_CONFIG,
    "modoOper" : Status.TCP_CONTINUA,
    "idProtocol" : 1,
    "ACC_SAMPLING" : 0,
    "ACC_SENSIBILITY" : 0,
    "GYRO_SENSIBILITY": 0,
    "BM3688" : 0,
    "DISCONTINUOS_TIME" : 0,
    "PORT_TCP" : 0,
    "PORT_UDP" : 0,
    "HOST_IP_ADDRESS" : 0,
    "SSID_WIFI" : 0,
    "PASS_WIFI" : 0,
}

class GUIController:
    modoConfig = defaultValues["modoConfig"]
    modoOper = defaultValues["modoOper"]
    idProtocol = defaultValues["idProtocol"]

    ACC_SAMPLING = defaultValues["ACC_SAMPLING"]
    ACC_SENSIBILITY = defaultValues["ACC_SENSIBILITY"]
    GYRO_SENSIBILITY= defaultValues["GYRO_SENSIBILITY"]
    BM3688 = defaultValues["BM3688"]
    DISCONTINUOS_TIME = defaultValues["DISCONTINUOS_TIME"]
    PORT_TCP = defaultValues["PORT_TCP"]
    PORT_UDP = defaultValues["PORT_UDP"]
    HOST_IP_ADDRESS = defaultValues["HOST_IP_ADDRESS"]
    SSID_WIFI = defaultValues["SSID_WIFI"]
    PASS_WIFI = defaultValues["PASS_WIFI"]
    
    def __init__(self,parent):
        self.ui=Ui_Dialog()
        self.parent = parent
        print()

    def setSignals(self):
        self.ui.search_esp32.clicked.connect(self.criticalError)
        self.ui.reset_btn.clicked.connect(self.resetConfig)
        self.ui.config_btn.clicked.connect(self.getConfig)
    
    def resetConfig(self):
        self.ui.acc_samp_field.setText(str(defaultValues["ACC_SAMPLING"]))
        self.ui.acc_sens_field.setText(str(defaultValues["ACC_SENSIBILITY"]))
        self.ui.gyro_sens_field.setText(str(defaultValues["GYRO_SENSIBILITY"]))
        self.ui.bme_field.setText(str(defaultValues["BM3688"]))
        self.ui.disc_time_field.setText(str(defaultValues["DISCONTINUOS_TIME"]))
        self.ui.port_tcp_field.setText(str(defaultValues["PORT_TCP"]))
        self.ui.port_udp_field.setText(str(defaultValues["PORT_UDP"]))
        self.ui.host_ip_addr_field.setText(str(defaultValues["HOST_IP_ADDRESS"]))
        self.ui.ssid_field.setText(str(defaultValues["SSID_WIFI"]))
        self.ui.pass_field.setText(str(defaultValues["PASS_WIFI"]))

    def getConfig(self):
        self.modoConfig = self.ui.config_mode_select.currentText()
        self.modoOper = self.ui.config_mode_select.currentText()
        self.idProtocol= self.ui.id_protocol_select.currentText()

        self.ACC_SAMPLING = self.ui.acc_samp_field.toPlainText()
        self.ACC_SENSIBILITY = self.ui.acc_sens_field.toPlainText()
        self.GYRO_SENSIBILITY = self.ui.gyro_sens_field.toPlainText()
        self.BM3688 = self.ui.bme_field.toPlainText()
        self.DISCONTINUOS_TIME = self.ui.disc_time_field.toPlainText()
        self.PORT_TCP = self.ui.port_tcp_field.toPlainText()
        self.PORT_UDP = self.ui.port_udp_field.toPlainText()
        self.HOST_IP_ADDRESS = self.ui.host_ip_addr_field.toPlainText()
        self.SSID_WIFI = self.ui.ssid_field.toPlainText()
        self.PASS_WIFI = self.ui.pass_field.toPlainText()

    def criticalError(self):
        popup = QtWidgets.QMessageBox(parent = self.parent)
        popup.setWindowTitle("ERROR MASIVO")
        popup.setText("QUE HAS APRETADO, NOS HAS CONDENADO A TODOS")
        popup.setIcon(QtWidgets.QMessageBox.Icon.Critical)
        popup.exec()
        return 

if __name__ == "__main__":
    import sys
    app = QtWidgets.QApplication(sys.argv)
    Dialog = QtWidgets.QDialog()
    cont = GUIController(Dialog)
    ui = cont.ui
    ui.setupUi(Dialog)
    cont.resetConfig()
    Dialog.show()
    cont.setSignals()
    # on close, ui should run ui.terminate_all_servers()
    sys.exit(app.exec_())