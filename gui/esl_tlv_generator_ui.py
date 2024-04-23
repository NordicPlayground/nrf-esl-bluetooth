# Copyright (c) 2024 Nordic Semiconductor ASA
#
# All rights reserved.
#
# SPDX-License-Identifier: Nordic-5-Clause
#
# Redistribution and use in source and binary forms, with or without modification,
# are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice, this
#    list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form, except as embedded into a Nordic
#    Semiconductor ASA integrated circuit in a product or a software update for
#    such product, must reproduce the above copyright notice, this list of
#    conditions and the following disclaimer in the documentation and/or other
#    materials provided with the distribution.
#
# 3. Neither the name of Nordic Semiconductor ASA nor the names of its
#    contributors may be used to endorse or promote products derived from this
#    software without specific prior written permission.
#
# 4. This software, with or without modification, must only be used with a
#    Nordic Semiconductor ASA integrated circuit.
#
# 5. Any software provided in binary form under this license must not be reverse
#    engineered, decompiled, modified and/or disassembled.
#
# THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
# OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
# OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
# GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
# OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'esl_tlv_generator.ui'
##
## Created by: Qt User Interface Compiler version 6.5.0
##
## WARNING! All changes made in this file will be lost when recompiling UI file!
################################################################################

from PySide6.QtCore import (QCoreApplication, QDate, QDateTime, QLocale,
    QMetaObject, QObject, QPoint, QRect,
    QSize, QTime, QUrl, Qt)
from PySide6.QtGui import (QBrush, QColor, QConicalGradient, QCursor,
    QFont, QFontDatabase, QGradient, QIcon,
    QImage, QKeySequence, QLinearGradient, QPainter,
    QPalette, QPixmap, QRadialGradient, QTransform)
from PySide6.QtWidgets import (QApplication, QCheckBox, QComboBox, QHBoxLayout,
    QLabel, QLineEdit, QMainWindow, QMenuBar,
    QPushButton, QSizePolicy, QSpinBox, QStatusBar,
    QTextBrowser, QTextEdit, QWidget)

class Ui_MainWindow(object):
    def setupUi(self, MainWindow):
        if not MainWindow.objectName():
            MainWindow.setObjectName(u"MainWindow")
        MainWindow.resize(1024, 768)
        self.centralwidget = QWidget(MainWindow)
        self.centralwidget.setObjectName(u"centralwidget")
        self.txtDebug = QTextBrowser(self.centralwidget)
        self.txtDebug.setObjectName(u"txtDebug")
        self.txtDebug.setGeometry(QRect(500, 20, 371, 181))
        self.widgetLED = QWidget(self.centralwidget)
        self.widgetLED.setObjectName(u"widgetLED")
        self.widgetLED.setGeometry(QRect(480, 210, 391, 461))
        self.layoutWidget = QWidget(self.widgetLED)
        self.layoutWidget.setObjectName(u"layoutWidget")
        self.layoutWidget.setGeometry(QRect(0, 100, 59, 24))
        self.horizontalLayout_18 = QHBoxLayout(self.layoutWidget)
        self.horizontalLayout_18.setObjectName(u"horizontalLayout_18")
        self.horizontalLayout_18.setContentsMargins(0, 0, 0, 0)
        self.label_10 = QLabel(self.layoutWidget)
        self.label_10.setObjectName(u"label_10")

        self.horizontalLayout_18.addWidget(self.label_10)

        self.boxBlue = QSpinBox(self.layoutWidget)
        self.boxBlue.setObjectName(u"boxBlue")
        self.boxBlue.setMaximum(3)

        self.horizontalLayout_18.addWidget(self.boxBlue)

        self.layoutWidget_2 = QWidget(self.widgetLED)
        self.layoutWidget_2.setObjectName(u"layoutWidget_2")
        self.layoutWidget_2.setGeometry(QRect(0, 130, 92, 24))
        self.horizontalLayout_7 = QHBoxLayout(self.layoutWidget_2)
        self.horizontalLayout_7.setObjectName(u"horizontalLayout_7")
        self.horizontalLayout_7.setContentsMargins(0, 0, 0, 0)
        self.label_13 = QLabel(self.layoutWidget_2)
        self.label_13.setObjectName(u"label_13")

        self.horizontalLayout_7.addWidget(self.label_13)

        self.boxBrightness = QSpinBox(self.layoutWidget_2)
        self.boxBrightness.setObjectName(u"boxBrightness")
        self.boxBrightness.setMaximum(3)

        self.horizontalLayout_7.addWidget(self.boxBrightness)

        self.layoutWidget_3 = QWidget(self.widgetLED)
        self.layoutWidget_3.setObjectName(u"layoutWidget_3")
        self.layoutWidget_3.setGeometry(QRect(0, 330, 141, 24))
        self.horizontalLayout_8 = QHBoxLayout(self.layoutWidget_3)
        self.horizontalLayout_8.setObjectName(u"horizontalLayout_8")
        self.horizontalLayout_8.setContentsMargins(0, 0, 0, 0)
        self.label_15 = QLabel(self.layoutWidget_3)
        self.label_15.setObjectName(u"label_15")

        self.horizontalLayout_8.addWidget(self.label_15)

        self.txtOnperiod = QLineEdit(self.layoutWidget_3)
        self.txtOnperiod.setObjectName(u"txtOnperiod")
        self.txtOnperiod.setMaxLength(2)

        self.horizontalLayout_8.addWidget(self.txtOnperiod)

        self.layoutWidget_5 = QWidget(self.widgetLED)
        self.layoutWidget_5.setObjectName(u"layoutWidget_5")
        self.layoutWidget_5.setGeometry(QRect(0, 390, 173, 24))
        self.horizontalLayout_19 = QHBoxLayout(self.layoutWidget_5)
        self.horizontalLayout_19.setObjectName(u"horizontalLayout_19")
        self.horizontalLayout_19.setContentsMargins(0, 0, 0, 0)
        self.label_16 = QLabel(self.layoutWidget_5)
        self.label_16.setObjectName(u"label_16")

        self.horizontalLayout_19.addWidget(self.label_16)

        self.txtRepeat_Duration = QLineEdit(self.layoutWidget_5)
        self.txtRepeat_Duration.setObjectName(u"txtRepeat_Duration")
        self.txtRepeat_Duration.setMaxLength(4)

        self.horizontalLayout_19.addWidget(self.txtRepeat_Duration)

        self.layoutWidget_4 = QWidget(self.widgetLED)
        self.layoutWidget_4.setObjectName(u"layoutWidget_4")
        self.layoutWidget_4.setGeometry(QRect(0, 300, 142, 24))
        self.horizontalLayout_20 = QHBoxLayout(self.layoutWidget_4)
        self.horizontalLayout_20.setObjectName(u"horizontalLayout_20")
        self.horizontalLayout_20.setContentsMargins(0, 0, 0, 0)
        self.label_17 = QLabel(self.layoutWidget_4)
        self.label_17.setObjectName(u"label_17")

        self.horizontalLayout_20.addWidget(self.label_17)

        self.txtOffperiod = QLineEdit(self.layoutWidget_4)
        self.txtOffperiod.setObjectName(u"txtOffperiod")
        self.txtOffperiod.setMaxLength(2)

        self.horizontalLayout_20.addWidget(self.txtOffperiod)

        self.layoutWidget_6 = QWidget(self.widgetLED)
        self.layoutWidget_6.setObjectName(u"layoutWidget_6")
        self.layoutWidget_6.setGeometry(QRect(0, 40, 58, 24))
        self.horizontalLayout_21 = QHBoxLayout(self.layoutWidget_6)
        self.horizontalLayout_21.setObjectName(u"horizontalLayout_21")
        self.horizontalLayout_21.setContentsMargins(0, 0, 0, 0)
        self.label_2 = QLabel(self.layoutWidget_6)
        self.label_2.setObjectName(u"label_2")

        self.horizontalLayout_21.addWidget(self.label_2)

        self.boxRed = QSpinBox(self.layoutWidget_6)
        self.boxRed.setObjectName(u"boxRed")
        self.boxRed.setMaximum(3)

        self.horizontalLayout_21.addWidget(self.boxRed)

        self.layoutWidget_7 = QWidget(self.widgetLED)
        self.layoutWidget_7.setObjectName(u"layoutWidget_7")
        self.layoutWidget_7.setGeometry(QRect(0, 10, 129, 24))
        self.horizontalLayout_22 = QHBoxLayout(self.layoutWidget_7)
        self.horizontalLayout_22.setObjectName(u"horizontalLayout_22")
        self.horizontalLayout_22.setContentsMargins(0, 0, 0, 0)
        self.label = QLabel(self.layoutWidget_7)
        self.label.setObjectName(u"label")

        self.horizontalLayout_22.addWidget(self.label)

        self.txtLEDIdx = QLineEdit(self.layoutWidget_7)
        self.txtLEDIdx.setObjectName(u"txtLEDIdx")
        self.txtLEDIdx.setMaxLength(2)

        self.horizontalLayout_22.addWidget(self.txtLEDIdx)

        self.layoutWidget_8 = QWidget(self.widgetLED)
        self.layoutWidget_8.setObjectName(u"layoutWidget_8")
        self.layoutWidget_8.setGeometry(QRect(0, 70, 68, 24))
        self.horizontalLayout_23 = QHBoxLayout(self.layoutWidget_8)
        self.horizontalLayout_23.setObjectName(u"horizontalLayout_23")
        self.horizontalLayout_23.setContentsMargins(0, 0, 0, 0)
        self.label_18 = QLabel(self.layoutWidget_8)
        self.label_18.setObjectName(u"label_18")

        self.horizontalLayout_23.addWidget(self.label_18)

        self.boxGreen = QSpinBox(self.layoutWidget_8)
        self.boxGreen.setObjectName(u"boxGreen")
        self.boxGreen.setMaximum(3)

        self.horizontalLayout_23.addWidget(self.boxGreen)

        self.layoutWidget_9 = QWidget(self.widgetLED)
        self.layoutWidget_9.setObjectName(u"layoutWidget_9")
        self.layoutWidget_9.setGeometry(QRect(0, 360, 100, 24))
        self.horizontalLayout_24 = QHBoxLayout(self.layoutWidget_9)
        self.horizontalLayout_24.setObjectName(u"horizontalLayout_24")
        self.horizontalLayout_24.setContentsMargins(0, 0, 0, 0)
        self.label_19 = QLabel(self.layoutWidget_9)
        self.label_19.setObjectName(u"label_19")

        self.horizontalLayout_24.addWidget(self.label_19)

        self.boxRepeat_type = QSpinBox(self.layoutWidget_9)
        self.boxRepeat_type.setObjectName(u"boxRepeat_type")
        self.boxRepeat_type.setMaximum(1)

        self.horizontalLayout_24.addWidget(self.boxRepeat_type)

        self.layoutWidget1 = QWidget(self.widgetLED)
        self.layoutWidget1.setObjectName(u"layoutWidget1")
        self.layoutWidget1.setGeometry(QRect(0, 420, 161, 24))
        self.horizontalLayout_25 = QHBoxLayout(self.layoutWidget1)
        self.horizontalLayout_25.setObjectName(u"horizontalLayout_25")
        self.horizontalLayout_25.setContentsMargins(0, 0, 0, 0)
        self.lblABSTime = QLabel(self.layoutWidget1)
        self.lblABSTime.setObjectName(u"lblABSTime")

        self.horizontalLayout_25.addWidget(self.lblABSTime)

        self.txtABSTime = QLineEdit(self.layoutWidget1)
        self.txtABSTime.setObjectName(u"txtABSTime")

        self.horizontalLayout_25.addWidget(self.txtABSTime)

        self.btnReset_LED = QPushButton(self.widgetLED)
        self.btnReset_LED.setObjectName(u"btnReset_LED")
        self.btnReset_LED.setGeometry(QRect(140, 10, 75, 24))
        self.widgetSensor = QWidget(self.centralwidget)
        self.widgetSensor.setObjectName(u"widgetSensor")
        self.widgetSensor.setGeometry(QRect(40, 500, 141, 51))
        self.layoutWidget_10 = QWidget(self.widgetSensor)
        self.layoutWidget_10.setObjectName(u"layoutWidget_10")
        self.layoutWidget_10.setGeometry(QRect(0, 10, 144, 24))
        self.horizontalLayout_26 = QHBoxLayout(self.layoutWidget_10)
        self.horizontalLayout_26.setObjectName(u"horizontalLayout_26")
        self.horizontalLayout_26.setContentsMargins(0, 0, 0, 0)
        self.label_21 = QLabel(self.layoutWidget_10)
        self.label_21.setObjectName(u"label_21")

        self.horizontalLayout_26.addWidget(self.label_21)

        self.txtSensorIdx = QLineEdit(self.layoutWidget_10)
        self.txtSensorIdx.setObjectName(u"txtSensorIdx")
        self.txtSensorIdx.setMaxLength(2)

        self.horizontalLayout_26.addWidget(self.txtSensorIdx)

        self.layoutWidget2 = QWidget(self.centralwidget)
        self.layoutWidget2.setObjectName(u"layoutWidget2")
        self.layoutWidget2.setGeometry(QRect(10, 20, 481, 26))
        self.horizontalLayout = QHBoxLayout(self.layoutWidget2)
        self.horizontalLayout.setObjectName(u"horizontalLayout")
        self.horizontalLayout.setContentsMargins(0, 0, 0, 0)
        self.btnAdd_TLV = QPushButton(self.layoutWidget2)
        self.btnAdd_TLV.setObjectName(u"btnAdd_TLV")

        self.horizontalLayout.addWidget(self.btnAdd_TLV)

        self.btnGen_Command = QPushButton(self.layoutWidget2)
        self.btnGen_Command.setObjectName(u"btnGen_Command")

        self.horizontalLayout.addWidget(self.btnGen_Command)

        self.btnReset = QPushButton(self.layoutWidget2)
        self.btnReset.setObjectName(u"btnReset")

        self.horizontalLayout.addWidget(self.btnReset)

        self.boxRSP_Key = QCheckBox(self.layoutWidget2)
        self.boxRSP_Key.setObjectName(u"boxRSP_Key")

        self.horizontalLayout.addWidget(self.boxRSP_Key)

        self.comboCommand = QComboBox(self.layoutWidget2)
        self.comboCommand.setObjectName(u"comboCommand")

        self.horizontalLayout.addWidget(self.comboCommand)

        self.layoutWidget3 = QWidget(self.centralwidget)
        self.layoutWidget3.setObjectName(u"layoutWidget3")
        self.layoutWidget3.setGeometry(QRect(10, 80, 174, 24))
        self.horizontalLayout_3 = QHBoxLayout(self.layoutWidget3)
        self.horizontalLayout_3.setObjectName(u"horizontalLayout_3")
        self.horizontalLayout_3.setContentsMargins(0, 0, 0, 0)
        self.lblGroup_ID = QLabel(self.layoutWidget3)
        self.lblGroup_ID.setObjectName(u"lblGroup_ID")

        self.horizontalLayout_3.addWidget(self.lblGroup_ID)

        self.txtGroup_ID = QLineEdit(self.layoutWidget3)
        self.txtGroup_ID.setObjectName(u"txtGroup_ID")
        self.txtGroup_ID.setMaxLength(2)

        self.horizontalLayout_3.addWidget(self.txtGroup_ID)

        self.layoutWidget4 = QWidget(self.centralwidget)
        self.layoutWidget4.setObjectName(u"layoutWidget4")
        self.layoutWidget4.setGeometry(QRect(10, 150, 401, 24))
        self.horizontalLayout_4 = QHBoxLayout(self.layoutWidget4)
        self.horizontalLayout_4.setObjectName(u"horizontalLayout_4")
        self.horizontalLayout_4.setContentsMargins(0, 0, 0, 0)
        self.lblRESP_KEY = QLabel(self.layoutWidget4)
        self.lblRESP_KEY.setObjectName(u"lblRESP_KEY")

        self.horizontalLayout_4.addWidget(self.lblRESP_KEY)

        self.txtResp_key = QLineEdit(self.layoutWidget4)
        self.txtResp_key.setObjectName(u"txtResp_key")

        self.horizontalLayout_4.addWidget(self.txtResp_key)

        self.layoutWidget5 = QWidget(self.centralwidget)
        self.layoutWidget5.setObjectName(u"layoutWidget5")
        self.layoutWidget5.setGeometry(QRect(10, 50, 171, 24))
        self.horizontalLayout_5 = QHBoxLayout(self.layoutWidget5)
        self.horizontalLayout_5.setObjectName(u"horizontalLayout_5")
        self.horizontalLayout_5.setContentsMargins(0, 0, 0, 0)
        self.lblTLV_Count = QLabel(self.layoutWidget5)
        self.lblTLV_Count.setObjectName(u"lblTLV_Count")

        self.horizontalLayout_5.addWidget(self.lblTLV_Count)

        self.txtTLV_Count = QLineEdit(self.layoutWidget5)
        self.txtTLV_Count.setObjectName(u"txtTLV_Count")
        self.txtTLV_Count.setReadOnly(True)

        self.horizontalLayout_5.addWidget(self.txtTLV_Count)

        self.layoutWidget6 = QWidget(self.centralwidget)
        self.layoutWidget6.setObjectName(u"layoutWidget6")
        self.layoutWidget6.setGeometry(QRect(20, 230, 401, 81))
        self.horizontalLayout_6 = QHBoxLayout(self.layoutWidget6)
        self.horizontalLayout_6.setObjectName(u"horizontalLayout_6")
        self.horizontalLayout_6.setContentsMargins(0, 0, 0, 0)
        self.lblTLV = QLabel(self.layoutWidget6)
        self.lblTLV.setObjectName(u"lblTLV")

        self.horizontalLayout_6.addWidget(self.lblTLV)

        self.txtTLV = QTextEdit(self.layoutWidget6)
        self.txtTLV.setObjectName(u"txtTLV")

        self.horizontalLayout_6.addWidget(self.txtTLV)

        self.widgetDisplay = QWidget(self.centralwidget)
        self.widgetDisplay.setObjectName(u"widgetDisplay")
        self.widgetDisplay.setGeometry(QRect(190, 520, 281, 121))
        self.layoutWidget_11 = QWidget(self.widgetDisplay)
        self.layoutWidget_11.setObjectName(u"layoutWidget_11")
        self.layoutWidget_11.setGeometry(QRect(0, 80, 161, 24))
        self.horizontalLayout_27 = QHBoxLayout(self.layoutWidget_11)
        self.horizontalLayout_27.setObjectName(u"horizontalLayout_27")
        self.horizontalLayout_27.setContentsMargins(0, 0, 0, 0)
        self.lblDispABSTime = QLabel(self.layoutWidget_11)
        self.lblDispABSTime.setObjectName(u"lblDispABSTime")

        self.horizontalLayout_27.addWidget(self.lblDispABSTime)

        self.txtDispABSTime = QLineEdit(self.layoutWidget_11)
        self.txtDispABSTime.setObjectName(u"txtDispABSTime")

        self.horizontalLayout_27.addWidget(self.txtDispABSTime)

        self.layoutWidget_12 = QWidget(self.widgetDisplay)
        self.layoutWidget_12.setObjectName(u"layoutWidget_12")
        self.layoutWidget_12.setGeometry(QRect(0, 10, 151, 24))
        self.horizontalLayout_28 = QHBoxLayout(self.layoutWidget_12)
        self.horizontalLayout_28.setObjectName(u"horizontalLayout_28")
        self.horizontalLayout_28.setContentsMargins(0, 0, 0, 0)
        self.lblDispIdx = QLabel(self.layoutWidget_12)
        self.lblDispIdx.setObjectName(u"lblDispIdx")

        self.horizontalLayout_28.addWidget(self.lblDispIdx)

        self.txtDispidx = QLineEdit(self.layoutWidget_12)
        self.txtDispidx.setObjectName(u"txtDispidx")

        self.horizontalLayout_28.addWidget(self.txtDispidx)

        self.layoutWidget_13 = QWidget(self.widgetDisplay)
        self.layoutWidget_13.setObjectName(u"layoutWidget_13")
        self.layoutWidget_13.setGeometry(QRect(0, 40, 151, 24))
        self.horizontalLayout_30 = QHBoxLayout(self.layoutWidget_13)
        self.horizontalLayout_30.setObjectName(u"horizontalLayout_30")
        self.horizontalLayout_30.setContentsMargins(0, 0, 0, 0)
        self.lblImgidx = QLabel(self.layoutWidget_13)
        self.lblImgidx.setObjectName(u"lblImgidx")

        self.horizontalLayout_30.addWidget(self.lblImgidx)

        self.txtImgidx = QLineEdit(self.layoutWidget_13)
        self.txtImgidx.setObjectName(u"txtImgidx")

        self.horizontalLayout_30.addWidget(self.txtImgidx)

        self.layoutWidget7 = QWidget(self.centralwidget)
        self.layoutWidget7.setObjectName(u"layoutWidget7")
        self.layoutWidget7.setGeometry(QRect(10, 110, 203, 26))
        self.horizontalLayout_2 = QHBoxLayout(self.layoutWidget7)
        self.horizontalLayout_2.setObjectName(u"horizontalLayout_2")
        self.horizontalLayout_2.setContentsMargins(0, 0, 0, 0)
        self.btnBroadcast = QPushButton(self.layoutWidget7)
        self.btnBroadcast.setObjectName(u"btnBroadcast")

        self.horizontalLayout_2.addWidget(self.btnBroadcast)

        self.lblESL_ID = QLabel(self.layoutWidget7)
        self.lblESL_ID.setObjectName(u"lblESL_ID")

        self.horizontalLayout_2.addWidget(self.lblESL_ID)

        self.txtESL_ID = QLineEdit(self.layoutWidget7)
        self.txtESL_ID.setObjectName(u"txtESL_ID")
        self.txtESL_ID.setMaxLength(2)

        self.horizontalLayout_2.addWidget(self.txtESL_ID)

        self.layoutWidget_14 = QWidget(self.centralwidget)
        self.layoutWidget_14.setObjectName(u"layoutWidget_14")
        self.layoutWidget_14.setGeometry(QRect(20, 320, 401, 131))
        self.horizontalLayout_9 = QHBoxLayout(self.layoutWidget_14)
        self.horizontalLayout_9.setObjectName(u"horizontalLayout_9")
        self.horizontalLayout_9.setContentsMargins(0, 0, 0, 0)
        self.lblCmd = QLabel(self.layoutWidget_14)
        self.lblCmd.setObjectName(u"lblCmd")

        self.horizontalLayout_9.addWidget(self.lblCmd)

        self.txtCmd = QTextEdit(self.layoutWidget_14)
        self.txtCmd.setObjectName(u"txtCmd")

        self.horizontalLayout_9.addWidget(self.txtCmd)

        MainWindow.setCentralWidget(self.centralwidget)
        self.menubar = QMenuBar(MainWindow)
        self.menubar.setObjectName(u"menubar")
        self.menubar.setGeometry(QRect(0, 0, 1024, 22))
        MainWindow.setMenuBar(self.menubar)
        self.statusbar = QStatusBar(MainWindow)
        self.statusbar.setObjectName(u"statusbar")
        MainWindow.setStatusBar(self.statusbar)

        self.retranslateUi(MainWindow)

        self.comboCommand.setCurrentIndex(-1)


        QMetaObject.connectSlotsByName(MainWindow)
    # setupUi

    def retranslateUi(self, MainWindow):
        MainWindow.setWindowTitle(QCoreApplication.translate("MainWindow", u"ESL AP TLV generator", None))
        self.label_10.setText(QCoreApplication.translate("MainWindow", u"Blue", None))
        self.label_13.setText(QCoreApplication.translate("MainWindow", u"Brightness", None))
        self.label_15.setText(QCoreApplication.translate("MainWindow", u"On Period: 0x", None))
        self.txtOnperiod.setInputMask(QCoreApplication.translate("MainWindow", u"HH", None))
        self.txtOnperiod.setText(QCoreApplication.translate("MainWindow", u"0", None))
        self.label_16.setText(QCoreApplication.translate("MainWindow", u"Repeat Duration: 0x", None))
        self.txtRepeat_Duration.setInputMask(QCoreApplication.translate("MainWindow", u"HHHH", None))
        self.txtRepeat_Duration.setText(QCoreApplication.translate("MainWindow", u"0", None))
        self.label_17.setText(QCoreApplication.translate("MainWindow", u"Off Period: 0x", None))
        self.txtOffperiod.setInputMask(QCoreApplication.translate("MainWindow", u"HH", None))
        self.txtOffperiod.setText(QCoreApplication.translate("MainWindow", u"0", None))
        self.label_2.setText(QCoreApplication.translate("MainWindow", u"RED", None))
        self.label.setText(QCoreApplication.translate("MainWindow", u"LED IDX: 0x", None))
        self.txtLEDIdx.setInputMask(QCoreApplication.translate("MainWindow", u"HH", None))
        self.txtLEDIdx.setText(QCoreApplication.translate("MainWindow", u"0", None))
        self.label_18.setText(QCoreApplication.translate("MainWindow", u"Green", None))
        self.label_19.setText(QCoreApplication.translate("MainWindow", u"Repeat type", None))
        self.lblABSTime.setText(QCoreApplication.translate("MainWindow", u"Absolute time: 0x", None))
        self.txtABSTime.setInputMask(QCoreApplication.translate("MainWindow", u"HHHHHHHH", None))
        self.txtABSTime.setText(QCoreApplication.translate("MainWindow", u"0", None))
        self.btnReset_LED.setText(QCoreApplication.translate("MainWindow", u"Reset LED", None))
        self.label_21.setText(QCoreApplication.translate("MainWindow", u"Sensor IDX: 0x", None))
        self.txtSensorIdx.setInputMask(QCoreApplication.translate("MainWindow", u"HH", None))
        self.txtSensorIdx.setText(QCoreApplication.translate("MainWindow", u"0", None))
        self.btnAdd_TLV.setText(QCoreApplication.translate("MainWindow", u"Add TLV", None))
        self.btnGen_Command.setText(QCoreApplication.translate("MainWindow", u"Command", None))
        self.btnReset.setText(QCoreApplication.translate("MainWindow", u"ResetTLV", None))
        self.boxRSP_Key.setText(QCoreApplication.translate("MainWindow", u"Incl RSP Key", None))
        self.lblGroup_ID.setText(QCoreApplication.translate("MainWindow", u"Group_ID", None))
        self.txtGroup_ID.setInputMask(QCoreApplication.translate("MainWindow", u"HH", None))
        self.txtGroup_ID.setText(QCoreApplication.translate("MainWindow", u"0", None))
        self.lblRESP_KEY.setText(QCoreApplication.translate("MainWindow", u"Resp key: 0x", None))
        self.txtResp_key.setInputMask(QCoreApplication.translate("MainWindow", u"HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH", None))
        self.txtResp_key.setText(QCoreApplication.translate("MainWindow", u"0", None))
        self.lblTLV_Count.setText(QCoreApplication.translate("MainWindow", u"TLV Count", None))
        self.txtTLV_Count.setText(QCoreApplication.translate("MainWindow", u"0", None))
        self.lblTLV.setText(QCoreApplication.translate("MainWindow", u"TLV", None))
        self.lblDispABSTime.setText(QCoreApplication.translate("MainWindow", u"Absolute time: 0x", None))
        self.txtDispABSTime.setInputMask(QCoreApplication.translate("MainWindow", u"HHHHHHHH", None))
        self.txtDispABSTime.setText(QCoreApplication.translate("MainWindow", u"0", None))
        self.lblDispIdx.setText(QCoreApplication.translate("MainWindow", u"Display Idx: 0x", None))
        self.txtDispidx.setInputMask(QCoreApplication.translate("MainWindow", u"99", None))
        self.txtDispidx.setText(QCoreApplication.translate("MainWindow", u"0", None))
        self.lblImgidx.setText(QCoreApplication.translate("MainWindow", u"Image Idx: 0x", None))
        self.txtImgidx.setInputMask(QCoreApplication.translate("MainWindow", u"999", None))
        self.txtImgidx.setText(QCoreApplication.translate("MainWindow", u"0", None))
        self.btnBroadcast.setText(QCoreApplication.translate("MainWindow", u"Broadcast", None))
        self.lblESL_ID.setText(QCoreApplication.translate("MainWindow", u"ESL_ID: 0x", None))
        self.txtESL_ID.setInputMask(QCoreApplication.translate("MainWindow", u"HH", None))
        self.txtESL_ID.setText(QCoreApplication.translate("MainWindow", u"0", None))
        self.lblCmd.setText(QCoreApplication.translate("MainWindow", u"CMD", None))
    # retranslateUi

