import numpy
from ui import Ui_Dialog

from PyQt5 import QtCore, QtGui, QtWidgets
import time
import logging

class GUIController:
    def __init__(self,parent):
        self.ui=Ui_Dialog()
        self.parent = parent
        print()

    def setSignals(self):
        self.ui.search_esp32.clicked.connect(self.criticalError)
    
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