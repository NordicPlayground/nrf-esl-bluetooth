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

import numpy as np
import argparse
import struct
import argparse
import matplotlib.pyplot as plt
import sys
import os
import zlib
from PIL import Image

EPD_HEADER_SIZE = 6
MONOCHROME_PIXEL_PER_BYTE = 8
GRAY_BLACK = 1
GRAY_RED = 3


class ImageHeader:
    def __init__(self, scan, gray, width, height):
        self.scan = scan
        self.gray = gray
        self.width = width
        self.height = height

    def to_bytes(self):
        # Convert the fields of the ImageHeader to bytes and return them.
        # This is just an example; you'll need to replace it with code that
        # correctly converts your specific fields.
        return struct.pack('BBHH', self.scan, self.gray, self.width, self.height)


def read_image_binary_file(file_path):
    with open(file_path, 'rb') as file:
        # initial header and data for second frame
        header_2 = None
        data_2 = None

        # Read the first 6 bytes and unpack them according to the header structure
        temp = file.read(EPD_HEADER_SIZE)
        header_1 = ImageHeader(*struct.unpack('BBHH', temp))
        frame_size_1 = int(header_1.width * header_1.height /
                           MONOCHROME_PIXEL_PER_BYTE)
        # Read the rest of the data
        data_1 = np.frombuffer(file.read(frame_size_1), dtype=np.uint8)

        '''check second frame'''
        '''file size > header size*2 + data size'''
        if os.path.getsize(file_path) > EPD_HEADER_SIZE * 2 + frame_size_1:
            # Read the second header
            temp = file.read(EPD_HEADER_SIZE)
            header_2 = ImageHeader(*struct.unpack('BBHH', temp))
            frame_size_2 = int(
                header_2.width * header_2.height / MONOCHROME_PIXEL_PER_BYTE)
            # Read the second data
            data_2 = np.frombuffer(file.read(frame_size_2), dtype=np.uint8)
            print('second frame exists')
        else:
            print('second frame does not exist')

    return header_1, data_1, header_2, data_2


def draw_plot(header, data, transform=None):
    """
    Draw a plot of the image data.

    Args:
        header (ImageHeader): The header of the image.
        data (ndarray): The image data.
        transform (list, optional): A list of transformations to apply to the image data. Defaults to None.
    """
    # Unpack the bits
    data = np.unpackbits(data)
    scan = header.scan
    h = header.height
    w = header.width
    # Reshape the data array based on the scan mode
    if (scan & 0x0f) == 0:
        data = data.reshape((h, w))
    elif (scan & 0x0f) == 1:
        data = data.reshape((w, h)).T
        data = np.flip(data, axis=1)
    elif (scan & 0x0f) == 2:
        data = data.reshape((h//8, w, 8))
        data = np.transpose(data, (0, 2, 1)).reshape(h, w)
    elif (scan & 0x0f) == 3:
        data = data.reshape((w, h))
        data = np.flip(data, axis=1)
    elif (scan & 0x0f) == 4:
        data = data.reshape((h, w))
        data = np.flip(data, axis=0)
    elif (scan & 0x0f) == 5:
        data = data.reshape((w, h)).T
        data = np.flip(data, axis=0)
    elif (scan & 0x0f) == 6:
        data = data.reshape((h, w)).T
        data = np.flip(data, axis=1)
    elif (scan & 0x0f) == 7:
        data = data.reshape((w, h))
        data = np.flip(data, axis=(0, 1))
    else:
        raise ValueError("Invalid scan mode")

    # Apply the transformation
    if transform:
        for t in transform:
            if t == 'lr':
                # Flip left to right
                print('flipping left to right')
                data = np.fliplr(data)
            elif t == 'ud':
                # Flip upside down
                print('flipping upside down')
                data = np.flipud(data)
            elif t == 'udlr':
                # Flip upside down
                print('flipping upside down')
                data = np.flipud(np.fliplr(data))

    # Plot the data
    if header.gray == 1:
        plt.imshow(data, cmap='gray_r')
    elif header.gray == 3:
        print('plotting red')
        plt.imshow(data, cmap='Reds_r')
    plt.title('Pixel Data')
    plt.show()
    input()

def convert_to_monochrome(input_file, output_file):
    # Open the image file
    with Image.open(input_file) as img:
        # Convert the image to grayscale
        grayscale = img.convert('L')
        # Convert the grayscale image to monochrome
        monochrome = grayscale.point(lambda x: 0 if x < 254 else 255, '1')
        # Save the monochrome image
        monochrome.save(output_file, 'BMP')

def generate_binary(input_file_path, out_file_path, input_file2=None, not_inverted=False, red_only=False):
    """
    Generate a binary file from an image file.

    Args:
        input_file_path (str): The path to the input image file.
        out_file_path (str): The path to the output binary file.
        input_file2 (str, optional): The path to the second input image file. Defaults to None.
        not_inverted (bool, optional): Whether to invert the colors of the first frame. Defaults to False.
    """
    img1 = None
    img2 = None
    imgs = []
    header1 = None
    header2 = None
    headers = []
    # convert input bmp to monochrome temp file
    convert_to_monochrome(input_file_path, 'tmp')

    # Read the image
    img1 = plt.imread('tmp')

    # generate header scan mode, width, height from img
    # scan mode hardcode to 0
    # color for first frame = 1 (gray), color for second frame = 3 (red)
    gray = GRAY_BLACK if not red_only else GRAY_RED
    header1 = ImageHeader(0, gray, img1.shape[1], img1.shape[0])
    print(
        f'header1: scan: {header1.scan}, gray: {header1.gray}, width: {header1.width}, height: {header1.height}')
    headers.append(header1)
    imgs.append(img1)

    if not input_file2 == None:
        print('second frame exists')
        convert_to_monochrome(input_file2, 'tmp2')
        img2 = plt.imread('tmp2')
        imgs.append(img2)
        header2 = ImageHeader(0, GRAY_RED, img2.shape[1], img2.shape[0])
        print(
            f'header2: scan: {header2.scan}, gray: {header2.gray}, width: {header2.width}, height: {header2.height}')
        headers.append(header2)

    # item is not none in list
    frame_count = len([item for item in headers if item is not None])

    with open(out_file_path, 'wb+') as file:
        for i in range(frame_count):
            header = headers[i]
            # Convert the image to monochrome 2 color, bit on = white, bit off = black
            img = np.where(imgs[i] > 0.5, 0, 1)
            # Convert to grayscale image
            img = np.packbits(img, axis=2)
            imgs[i] = img
            '''write to file'''
            if i == 1:
                offset = int(header1.width * header1.height /
                             MONOCHROME_PIXEL_PER_BYTE)
                offset += EPD_HEADER_SIZE
                print(f'offset: {offset}')
                file.seek(offset)
                print(f'second frame file position {file.tell()}')
            file.write(header.to_bytes())
            img = np.packbits(img).tobytes()
            # First frame bit on = white, bit off = black, need to invert
            if i == 0 and not (not_inverted or red_only):
                print('inverting first frame')
                for img_byte in img:
                    inverted_byte = ~img_byte & 0xff
                    file.write(inverted_byte.to_bytes(1, 'big'))
            else:
                file.write(img)

    if os.path.exists('tmp'):
        os.remove('tmp')

    if os.path.exists('tmp2'):
        os.remove('tmp2')


def process_c_array_to_binary(input_file_path, output_file_path):
    """
    Process a C array and convert it to a binary file.

    Args:
        input_file_path (str): The path to the input C array file.
        output_file_path (str): The path to the output binary file.
    """
    # Read the content from the input file
    with open(input_file_path, 'r') as file:
        lines = file.readlines()

    print(f"Content: {lines}")  # Print the content

    # Remove lines that start with "//" or contain comments
    cleaned_lines = []
    for line in lines:
        # Check if "//" is in the line and split the line at the first occurrence
        comment_start = line.find("//")
        if comment_start != -1:
            # If there's code before the comment, keep the code part
            if comment_start > 0:
                cleaned_lines.append(line[:comment_start])
        else:
            cleaned_lines.append(line)

    # Join the cleaned lines back into a single string
    content = "".join(cleaned_lines)

    # Extract content within curly braces
    start = content.find('{') + 1
    end = content.find('}', start)
    array_content = content[start:end]
    print(f"Array Content: {array_content}")  # Print the array content

    # Split the string into a list of hex values
    hex_values = array_content.split(',')
    print(f"Hex Values: {hex_values}")  # Print the hex values

    # Remove any potential whitespace and convert to bytes
    byte_array = bytes([int(value.strip(), 16)
                       for value in hex_values if value.strip()])

    # Write the byte array to the output binary file
    with open(output_file_path, 'wb+') as bin_file:
        bin_file.write(byte_array)

    print(f"Array written to {output_file_path} successfully.")


def process_binary_to_array(input_file_path, output_file_path):
    # Read the binary data from the input file
    with open(input_file_path, 'rb') as bin_file:
        byte_array = bin_file.read()

    # Convert the byte array to a list of hex strings
    hex_values = [f"0x{byte:02x}" for byte in byte_array]

    # Format the hex strings into a C array
    c_array = "const unsigned char gImage_1[" + \
        str(len(hex_values)) + "] = {\n"

    # Add hex values to the C array, with a new line every 16 values
    for i in range(0, len(hex_values), 16):
        c_array += ", ".join(hex_values[i:i+16]) + ",\n"

    c_array += "};"

    # Write the C array to the output file
    with open(output_file_path, 'w') as file:
        file.write(c_array)

    print(f"C array written to {output_file_path} successfully.")


def calculate_crc32(file_path):
    """
    Calculate the CRC32 checksum of a file.

    Args:
        file_path (str): The path to the file.

    Returns:
        int: The CRC32 checksum.
    """
    # Open the file in binary mode
    with open(file_path, 'rb') as file:
        # Read the entire file content
        file_content = file.read()
        # Calculate CRC32 checksum
        # & 0xffffffff for a consistent unsigned CRC32 value
        crc32_value = zlib.crc32(file_content) & 0xffffffff

    return crc32_value


def main():
    parser = argparse.ArgumentParser(
        description='Plot data from a binary file or generate binary from bmp file')
    parser.add_argument('-i', "--input_file", type=str,
                        help='Path to the input file')
    parser.add_argument('-i2', "--input_file2", type=str,
                        help='Path to the input file for second frame')
    parser.add_argument('-n', "--not_inverted", action='store_true',
                        help='Not to invert pixel of first frame', default=False)
    parser.add_argument('-o', "--output_file", type=str,
                        help='Path to the output file')
    parser.add_argument('-a', "--action", type=str, choices=['d', 'g', 'b', 'a', 'c'],
                        help='Action to perform, d: draw, g: generate binary file from bmp b: generate binary file from c array, a: generate c array, c: generate checksum of input file', default='d')
    parser.add_argument('-s', "--scan", type=int,
                        choices=range(8), help='Scan mode (0-7)', default=0)
    parser.add_argument('-t', "--transform", type=str, nargs='+',
                        choices=['no', 'lr', 'ud', 'udlr'], help='Transformations to apply to the data (no, lr, ud, udlr)', default=['no'])
    parser.add_argument('-r', "--red_only", action='store_true',
                        help='generate red frame only')
    args = parser.parse_args()
    if not any(vars(args).values()):
        parser.print_help()
        sys.exit()

    if args.action.lower() == 'd':
        plt.figure()
        header_1, data_1, header_2, data_2, = read_image_binary_file(
            args.input_file)
        '''print header_1, data_1, header_2, data_2'''
        print(
            f'header_1: {header_1.scan}, {header_1.gray}, {header_1.width}, {header_1.height}, data size: {data_1.size} bytes')
        draw_plot(header_1, data_1, transform=args.transform)
        if not header_2 == None:
            print(
                f'header_2: {header_2.scan}, {header_2.gray}, {header_2.width}, {header_2.height}, data size: {data_2.size} bytes')
            draw_plot(header_2, data_2, transform=args.transform)

    if args.action.lower() == 'g':
        generate_binary(args.input_file, args.output_file, input_file2=args.input_file2,
                        not_inverted=args.not_inverted, red_only=args.red_only)

    if args.action.lower() == 'b':
        process_c_array_to_binary(args.input_file, args.output_file)

    if args.action.lower() == 'c':
        # Calculate and print the CRC32 value
        crc32_value = calculate_crc32(args.input_file)
        # Prints the checksum in hex format
        print(f"CRC32 Checksum: {crc32_value:#010x}")

    if args.action.lower() == 'a':
        process_binary_to_array(args.input_file, args.output_file)


if __name__ == "__main__":
    main()
