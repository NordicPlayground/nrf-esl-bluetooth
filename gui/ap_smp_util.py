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
import subprocess
import argparse
import csv
from pathlib import Path
from serial.tools.list_ports import comports

CURRENT_FOLDER = Path(__file__).resolve().parent
TAG_LIST_FILE = CURRENT_FOLDER / 'tag_list.txt'
TAG_BLE_LIST_FILE = CURRENT_FOLDER / 'tag_ble_list.txt'
IMAGE_LIST_FILE = CURRENT_FOLDER / 'image_list.txt'
TAG_DATA_FOLDER = CURRENT_FOLDER / 'tag_data'
TAG_BLE_FOLDER = TAG_DATA_FOLDER / 'tag_ble'
COM_PORT = ''
SMP_ECHO_CMD = 'newtmgr --conntype="serial" --connstring="{},baud=115200" echo hello -t 2'

def find_com_port():
    global COM_PORT
    if sys.platform.startswith('win'):
        import serial.tools.list_ports
        ports = serial.tools.list_ports.comports()
        for port in ports:
            proc = subprocess.Popen(SMP_ECHO_CMD.format(port.name),
                stdout = subprocess.PIPE,
                stderr = subprocess.PIPE,
            )

            stdout, stderr = proc.communicate()
            if proc.returncode == 0:
                COM_PORT = port.name
                print(f'Found SMP COM port {COM_PORT}')
                return
    elif sys.platform.startswith('linux'):
        print('Linux not supported yet')
        exit(1)
    COM_PORT = ''
    print('No SMP COM port found')
    exit(1)

def get_tag_from_ap():
    print('Getting tags esl_addr from AP')
    tag_list = ()
    if not os.path.exists(TAG_DATA_FOLDER):
        os.mkdir(TAG_DATA_FOLDER)

    if not os.path.exists(TAG_BLE_FOLDER):
        os.mkdir(TAG_BLE_FOLDER)

    if os.path.exists(TAG_LIST_FILE):
        fh = open(TAG_LIST_FILE, 'r')
        tag_list = fh.read().split()

    if len(tag_list) > 0:
        print('Getting tags from tag list')
        for esl_addr_str in tag_list:
            print(f'dl tag 0x{esl_addr_str}')
            proc = subprocess.Popen(f'newtmgr --conntype="serial" --connstring="{COM_PORT},baud=115200" fs download /ots_image/esl/{esl_addr_str} {TAG_DATA_FOLDER}/{esl_addr_str}',
                stdout = subprocess.PIPE,
                stderr = subprocess.PIPE,
            )

            stdout, stderr = proc.communicate()
    else:
        print('No tag list file found, getting all tags')
        esl_addr = 0
        for i in range(0, 32768):
            esl_addr_str = f'{esl_addr:04x}'
            proc = subprocess.Popen(f'newtmgr --conntype="serial" --connstring="{COM_PORT},baud=115200" fs download /ots_image/esl/{esl_addr_str} {TAG_DATA_FOLDER}/{esl_addr_str}',
                stdout = subprocess.PIPE,
                stderr = subprocess.PIPE,
            )

            stdout, stderr = proc.communicate()
            if (os.path.getsize(CURRENT_FOLDER / f'tag_data/{esl_addr_str}')) == 0:
                os.remove(f'tag_data/{esl_addr_str}')
            else:
                print(f'Got data from {esl_addr_str}')
            if i % 100 == 0:
                print (f'{esl_addr} of 32768 done')
            esl_addr +=1

    print('Getting tags ble_addr from AP')
    tag_list = ()
    if os.path.exists(TAG_BLE_LIST_FILE):
        fh = open(TAG_BLE_LIST_FILE, 'r')
        tag_list = fh.read().split()

    if len(tag_list) > 0:
        print('Getting tags ble_addr from tag list')
        for ble_addr_str in tag_list:
            print(f'dl tag {ble_addr_str}')
            if os.name == 'nt':
                ble_addr_str_filename = ble_addr_str.replace(':', '_')
            else:
                ble_addr_str_filename = ble_addr_str_filename
            proc = subprocess.Popen(f'newtmgr --conntype="serial" --connstring="{COM_PORT},baud=115200" fs download /ots_image/ble/{ble_addr_str} {TAG_BLE_FOLDER}/{ble_addr_str_filename}',
                stdout = subprocess.PIPE,
                stderr = subprocess.PIPE,
            )

            stdout, stderr = proc.communicate()

def upload_image_to_ap():
    print('Uploading images to AP')
    with open(IMAGE_LIST_FILE, 'r') as file:
        image_list = csv.reader(file)
        for row in image_list:
            print(f'Uploading {row[0]} to {row[1]}')
            print(f'newtmgr --conntype="serial" --connstring="{COM_PORT},baud=115200" fs upload {row[0]} {row[1]}')
            proc = subprocess.Popen(f'newtmgr --conntype="serial" --connstring="{COM_PORT},baud=115200" fs upload {row[0]} {row[1]}',
                stdout = subprocess.PIPE,
                stderr = subprocess.PIPE,
            )

            stdout, stderr = proc.communicate()

def upload_tag_to_ap():
    print('Uploading tag data to AP')
    for filename in os.listdir(TAG_DATA_FOLDER):
        if os.path.isfile(os.path.join(TAG_DATA_FOLDER, filename)):
            cmd_str = f'newtmgr --conntype="serial" --connstring="{COM_PORT},baud=115200" fs upload {TAG_DATA_FOLDER}/{filename} /ots_image/esl/{filename}'
            print (cmd_str)
            proc = subprocess.Popen(cmd_str,
                stdout = subprocess.PIPE,
                stderr = subprocess.PIPE,
            )

            stdout, stderr = proc.communicate()

    for filename in os.listdir(TAG_BLE_FOLDER):
        if os.path.isfile(os.path.join(TAG_BLE_FOLDER, filename)):
            if os.name == 'nt':
                ble_addr_str = filename.replace('_', ':')
            else:
                ble_addr_str = filename
            cmd_str = f'newtmgr --conntype="serial" --connstring="{COM_PORT},baud=115200" fs upload {TAG_BLE_FOLDER}/{filename} /ots_image/ble/{ble_addr_str}'
            print (cmd_str)
            proc = subprocess.Popen(cmd_str,
                stdout = subprocess.PIPE,
                stderr = subprocess.PIPE,
            )

            stdout, stderr = proc.communicate()


def __main():
    parser = argparse.ArgumentParser(
        formatter_class=argparse.RawDescriptionHelpFormatter,
        description=(
            "This script upload image and download tag data from ESL Access Point "
            "on Windows and Linux"
        ),
    )

    parser.add_argument(
        "-A",
        "--action",
        choices=["get", "set" , "set_tag"],
        help="Get tag data or Upload image to Access Point",
    )

    if len(sys.argv) == 1:
        parser.print_help()
        sys.exit(1)

    options = parser.parse_args(args=sys.argv[1:])
    find_com_port()
    if options.action == 'get':
        get_tag_from_ap()
    elif options.action == 'set':
        upload_image_to_ap()
    elif options.action == 'set_tag':
        upload_tag_to_ap()

if __name__ == "__main__":
    __main()
