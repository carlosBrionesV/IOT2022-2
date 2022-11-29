import numpy
from ui import Ui_Dialog

from PyQt5 import QtCore, QtGui, QtWidgets
import time
import logging

VIA_BLUETHOOTH = 0
VIA_TCP = 1

class GUIController:
    via = VIA_BLUETHOOTH
    
    def __init__(self,parent):
        self.ui=Ui_Dialog()
        self.parent = parent
        print()

    def setSignals(self):
        self.ui.search_esp32.clicked.connect(self.criticalError)
        self.ui.config_mode_select.currentIndexChanged.connect(self.configModeChanged)
    
    def configModeChanged(self):
        self.via = self.ui.config_mode_select.currentIndex()
        self.ui.port_tcp_field.setText(str(self.via))
    
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
    Dialog.show()
    cont.setSignals()
    # on close, ui should run ui.terminate_all_servers()
    sys.exit(app.exec_())