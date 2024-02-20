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

import sys
import os
import struct
import binascii

from pathlib import Path
from PySide6.QtCore import QRect
from PySide6 import QtWidgets
from PySide6.QtCore import Qt
from PySide6.QtWidgets import *

from esl_tlv_generator_ui import Ui_MainWindow

TLV_COUNT = 0
TLV_TOTAL_LENGTH = 0
TLV_MAX_LENGTH = 48
ESL_AP_PUSH_BUF_CMD = 'esl_c pawr push_sync_buf'
TLV_BYTES = bytearray()
RSP_KEY_BYTES = list()
DEFAULT_ESL_ID =0x00
DEFAULT_GROUP_ID =0x00
ESL_BROADCAST_ID = 0xff
PATTERN_LENGTH = 40
PATTERN_BIT_PER_ROW = 10
RSP_KEY_LIST = dict()
GUI_FOLDER = Path(__file__).resolve().parent
TAG_DB_FOLDER = GUI_FOLDER / 'tag_data'
DEBUG_MSG = True

class MainWindow(QMainWindow):
    def __init__(self, parent = None) :
        super().__init__(parent)
        self.ui = Ui_MainWindow()
        self.ui.setupUi(self)
        ''' Initial UI event '''
        self.ui.btnAdd_TLV.clicked.connect(self.TLV_ADD)
        self.ui.btnReset.clicked.connect(self.reset_tlv)
        self.ui.btnBroadcast.clicked.connect(self.esl_id_broadcast)
        self.ui.btnReset_LED.clicked.connect(self.reset_led)
        self.ui.btnGen_Command.clicked.connect(self.gen_command)
        self.ui.comboCommand.currentIndexChanged.connect(self.combo_command_changed)
        self.ui.txtGroup_ID.textChanged.connect(self.esl_addr_changed)
        self.ui.txtESL_ID.textChanged.connect(self.esl_addr_changed)

        ''' Initial Combo BOX command list '''
        self.ui.comboCommand.addItem("Ping", 0x00)
        self.ui.comboCommand.addItem("Unassociated", 0x01)
        self.ui.comboCommand.addItem("Service reset", 0x02)
        self.ui.comboCommand.addItem("Factory reset", 0x03)
        self.ui.comboCommand.addItem("Read Sensor", 0x10)
        self.ui.comboCommand.addItem("Refresh Display", 0x11)
        self.ui.comboCommand.addItem("Display image", 0x20)
        self.ui.comboCommand.addItem("Display timed image", 0x60)
        self.ui.comboCommand.addItem("LED Control", 0xB0)
        self.ui.comboCommand.addItem("LED Timed Control", 0xF0)
        self.ui.comboCommand.addItem("Vendor-specific Tag", 0xFF)
        self.hide_all_widget()

        ''' Initial Check Box LED flashing Pattern '''
        self.ui.led_flashing_pattern_box_list = list()
        for i in range(PATTERN_LENGTH):
            width = 15
            height = 30
            leftoffset = (i % PATTERN_BIT_PER_ROW) * width
            topoffset = ( i // PATTERN_BIT_PER_ROW) * height

            newcheckbox = QCheckBox(self.ui.widgetLED)
            newcheckbox.setObjectName(f'led_flashing_pattern{i}')
            newlabel = QLabel(self.ui.widgetLED)
            newlabel.setText(f'{i}')
            newlabel.setGeometry(QRect(leftoffset, 160 + topoffset, width, height))
            newlabel.lower()
            newcheckbox.setGeometry(QRect(leftoffset, 175 + topoffset, width, height))
            self.ui.led_flashing_pattern_box_list.append(newcheckbox)

        ''' Load RSP key from tag_data '''
        if os.path.exists(TAG_DB_FOLDER) == True:
            for filename in os.listdir(TAG_DB_FOLDER):
                f = os.path.join(TAG_DB_FOLDER, filename)
                # checking if it is a file
                if os.path.isfile(f):
                    esl_addr = int(filename, 16)
                    print(f)
                    with open(f, mode='rb') as file:
                        data = file.read()
                        data= data[7:31]
                        RSP_KEY_LIST[esl_addr] = data
        self.esl_addr_changed()

    ''' Initial and reset TLV UI '''
    def reset_tlv(self):
        global TLV_COUNT
        global TLV_TOTAL_LENGTH
        global TLV_BYTES
        global RSP_KEY_BYTES

        TLV_COUNT = 0
        TLV_TOTAL_LENGTH = 0
        TLV_BYTES = bytearray()
        RSP_KEY_BYTES = list()

        self.ui.txtESL_ID.setText(f'{DEFAULT_ESL_ID:02x}')
        self.ui.txtGroup_ID.setText(f'{DEFAULT_GROUP_ID:02x}')
        self.ui.txtTLV_Count.setText(f'{TLV_COUNT:02x}')
        self.ui.txtTLV.setText('')
        self.ui.txtCmd.setText('')

    ''' combo box command changed '''
    def hide_all_widget(self):
        self.ui.widgetDisplay.hide()
        self.ui.widgetLED.hide()
        self.ui.widgetSensor.hide()

    def combo_command_changed(self):
        self.debug_print(f'{hex(self.ui.comboCommand.currentData())}')
        self.hide_all_widget()

        # Read sensor
        if self.ui.comboCommand.currentData() == 0x10:
            self.ui.widgetSensor.setGeometry(500, 206, 321, 351)
            self.ui.widgetSensor.show()

        # Refresh display
        elif self.ui.comboCommand.currentData() == 0x11:
            self.ui.widgetDisplay.setGeometry(500, 206, 321, 351)
            self.ui.widgetDisplay.show()
            self.ui.lblImgidx.hide()
            self.ui.txtImgidx.hide()
            self.ui.lblDispABSTime.hide()
            self.ui.txtDispABSTime.hide()

        # Display image
        elif self.ui.comboCommand.currentData() == 0x20:
            self.ui.widgetDisplay.setGeometry(500, 206, 321, 351)
            self.ui.widgetDisplay.show()
            self.ui.lblImgidx.show()
            self.ui.txtImgidx.show()
            self.ui.lblDispABSTime.hide()
            self.ui.txtDispABSTime.hide()

        # Display timed image
        elif self.ui.comboCommand.currentData() == 0x60:
            self.ui.widgetDisplay.setGeometry(500, 206, 321, 351)
            self.ui.widgetDisplay.show()
            self.ui.lblImgidx.show()
            self.ui.txtImgidx.show()
            self.ui.lblDispABSTime.show()
            self.ui.txtDispABSTime.show()

        # LED Control
        elif self.ui.comboCommand.currentData() == 0xb0:
            self.ui.widgetLED.setGeometry(500, 206, 321, 420)
            self.ui.widgetLED.show()
            self.ui.lblABSTime.hide()
            self.ui.txtABSTime.hide()
        elif self.ui.comboCommand.currentData() == 0xf0:
            self.ui.widgetLED.setGeometry(500, 206, 321, 460)
            self.ui.widgetLED.show()
            self.ui.lblABSTime.show()
            self.ui.txtABSTime.show()

    def debug_print(self, msg):
        if DEBUG_MSG == True:
            self.ui.txtDebug.append(msg)

    def TLV_ADD(self):
        global TLV_COUNT
        global TLV_TOTAL_LENGTH
        global TLV_BYTES

        esl_id = int(self.ui.txtESL_ID.text(), 16) & 0xff
        op_code = int(self.ui.comboCommand.currentData())
        current_tlv_length = ((op_code & 0xf0 ) >> 4) + 2
        group_id = int(self.ui.txtGroup_ID.text(), 16) & 0xff
        self.debug_print(f'op_code 0x{op_code:02x} TLV_COUNT: {TLV_COUNT} esl_id: 0x{esl_id:02x} current_tlv_length: {current_tlv_length}')
        if (current_tlv_length + TLV_TOTAL_LENGTH) > TLV_MAX_LENGTH:
            self.ui.txtDebug.setText('TLV total length is over 48 bytes')
            return

        TLV_BYTES.append(op_code)
        TLV_BYTES.append(esl_id)
        TLV_TOTAL_LENGTH += current_tlv_length
        TLV_COUNT += 1

        # Read sensor
        if op_code == 0x10:
            TLV_BYTES.append(int(self.ui.txtSensorIdx.text()))
        # Refresh display
        elif op_code == 0x11:
            TLV_BYTES.append(int(self.ui.txtDispidx.text()))
        # Display image or Display timed image
        elif op_code == 0x20 or op_code == 0x60:
            TLV_BYTES.append(int(self.ui.txtDispidx.text(), 16) & 0xff)
            TLV_BYTES.append(int(self.ui.txtImgidx.text(), 16) & 0xff)
        # LED Control or LED Timed Control
        elif op_code == 0xb0 or op_code == 0xf0:
            color_brightness = ((int(self.ui.boxBrightness.text()) & 0x3) << 6) | ((int(self.ui.boxBlue.text()) & 0x3) << 4) | ((int(self.ui.boxGreen.text()) & 0x3) << 2) | (int(self.ui.boxRed.text()) & 0x3)
            repeat_data = int(self.ui.boxRepeat_type.text()) & 0x01 | ((int(self.ui.txtRepeat_Duration.text(), 16) & 0x7fff) << 1)
            print(f'color_brightness = 0x{color_brightness:02x}')
            print(f'repeat_data = 0x{repeat_data:04x}')

            TLV_BYTES.append(int(self.ui.txtLEDIdx.text(), 16) & 0xff)
            TLV_BYTES.append(color_brightness)
            # flash pattern
            pattern = 0
            for i in range(PATTERN_LENGTH):
                if (self.ui.led_flashing_pattern_box_list[i].isChecked()):
                    print(f'led_flashing_pattern_box_list[{i}] is checked')
                    pattern |= 1 << i
            print(f'pattern = {hex(pattern)}')
            TLV_BYTES.extend(pattern.to_bytes(PATTERN_LENGTH // 8, 'little'))
            print(f'pattern = {pattern}')
            TLV_BYTES.append(int(self.ui.txtOffperiod.text(), 16) & 0xff)
            TLV_BYTES.append(int(self.ui.txtOnperiod.text(), 16) & 0xff)
            # Repeat type and duration
            TLV_BYTES.extend(repeat_data.to_bytes(2, 'little'))

        # Display timed image add absolute time
        if op_code == 0x60:
            abstime = int(self.ui.txtDispABSTime.text(), 16)
            TLV_BYTES.extend(abstime.to_bytes(4, 'little'))
        # LED Timed Control add absolute time
        if op_code == 0xf0:
            abstime = int(self.ui.txtABSTime.text(), 16)
            TLV_BYTES.extend(abstime.to_bytes(4, 'little'))
        # Add response key
        if self.ui.boxRSP_Key.isChecked() and esl_id != ESL_BROADCAST_ID:
            RSP_KEY_BYTES.append(self.ui.txtResp_key.text())

        self.ui.txtTLV_Count.setText(str(TLV_COUNT))
        self.ui.txtTLV.setText(f'0x{TLV_BYTES.hex()}')
        self.debug_print(f'TLV: {TLV_BYTES} TLV_TOTAL_LENGTH: {TLV_TOTAL_LENGTH}')

    def esl_id_broadcast(self):
        self.ui.txtESL_ID.setText(f'{ESL_BROADCAST_ID:2x}')

    def reset_led(self):
        self.ui.boxRed.setValue(0)
        self.ui.boxGreen.setValue(0)
        self.ui.boxBlue.setValue(0)
        self.ui.boxBrightness.setValue(0)
        self.ui.boxRepeat_type.setValue(0)
        self.ui.txtRepeat_Duration.setText('0')
        self.ui.txtLEDIdx.setText('0')
        self.ui.txtOnperiod.setText('0')
        self.ui.txtRepeat_Duration.setText('0')
        self.ui.txtABSTime.setText('0')
        for i in range(PATTERN_LENGTH):
            self.ui.led_flashing_pattern_box_list[i].setChecked(False)

    def gen_command(self):
        global TLV_BYTES

        group_id = int(self.ui.txtGroup_ID.text(), 16) & 0xff
        cmd_str =  f'{ESL_AP_PUSH_BUF_CMD} {group_id} {TLV_BYTES.hex()} '
        for i in range(len(RSP_KEY_BYTES)):
            cmd_str += f'{RSP_KEY_BYTES[i]} '

        print(cmd_str)
        self.ui.txtCmd.setText(cmd_str)
        cb = QApplication.clipboard()
        cb.clear(mode=cb.Mode.Clipboard)
        cb.setText(cmd_str, mode=cb.Mode.Clipboard)

    def esl_addr_changed(self):
        esl_addr = ((int(self.ui.txtGroup_ID.text(), 16) & 0xff) << 4 ) | (int(self.ui.txtESL_ID.text(), 16) &0xff)
        if esl_addr in RSP_KEY_LIST:
            self.ui.txtResp_key.setText(binascii.hexlify(bytearray(RSP_KEY_LIST[esl_addr])).decode('ascii'))
        else:
            self.ui.txtResp_key.setText('0')

if __name__ == '__main__':
    app = QtWidgets.QApplication(sys.argv)
    win = MainWindow()
    win.move(100, 50)
    win.setWindowTitle("ESL AP TLV Generator")
    win.show()
    app.exit(app.exec())
