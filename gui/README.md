# Python Utilities for Working with nrf-esl-bluetooth

This folder contains python scripts for working with ESL access point.

## Scripts
`ap_smp_util.py` - A python script to perform SMP operations on an ESL access point. It could be used to for the following feature.
Upload EPD images to the storage of an ESL access point.
Download ESL Tag data including ESL address / BLE address / response key from an ESL access point.

`ead_ccm.py` - A python script to perform EAD (encrypted advertising data) encryption and decryption offline.

`esl_tlv_generator.py` - A GUI python script to compose ESL payload TLVs. This script demonstrates how one can use this script to create multiple ECP commands and supply response key for each response slot.

`epd_simulator.py` - A python script to generate pixel mapped imagefiles for EPD display.

# ap_smp_util.py Usage

This script is used to interact with an ESL Access Point. It provides functionalities to download tag data from the Access Point, upload images to the Access Point, and upload tag data to the Access Point.

## Prerequisites
Newt Manager (newtmgr)

https://dlcdn.apache.org/mynewt/

https://dlcdn.apache.org/mynewt/apache-mynewt-1.12.0/

## Usage

Run the script with the `-A` or `--action` argument followed by the action you want to perform. The possible actions are:

- `get`: This action will download the tag data from the Access Point and save it in the `tag_data` folder in the current directory. If a `tag_list.txt` and `tag_list_ble.txt` file exists in the current directory, the script will only download data for the tags listed in this file.

    #### To get the `tag_list.txt` file, follow these steps:

    1. Open the terminal.
    2. Run the following shell command:

        ```bash
        esl_c list_tags_storage
    3. Save terminal output tag list to `tag_list.txt`

    #### To get the `tag_list_ble.txt` file, follow these steps:

    1. Open the terminal.
    2. Run the following shell command:

        ```bash
        fs ls ots_image/ble
    3. Save terminal output tag list to `tag_list_ble.txt`

- `set`: This action will upload images to the Access Point. The script expects a `image_list.txt` file in the current directory, which should be a CSV file where each row contains the path to an image file and the destination path on the Access Point.

- `set_tag`: This action will upload tag data to the Access Point. The script will upload all files in the `tag_data` folder to the Access Point.

Example:

```bash
python ap_smp_util.py --action get
```

# `ead_ccm.py`Usage

This script is used to encrypt and decrypt payload data according to the Bluetooth Core Specification for Encrypted Advertising Data.

## Prerequisites

- Python 3.x
- Cryptodome


## Instructions

The script takes several command-line arguments:

- `-d` or `--data`: The raw data to encrypt or decrypt.
- `-a` or `--ad_data`: The advertising data with ad header.
- `-r` or `--randomizer`: The randomizer value.
- `-i` or `--iv`: The Initialization Vector (IV).
- `-s` or `--session_key`: The session key.
- `-k` or `--key`: The key material, which includes an 16-byte session key and an 8-byte IV.
- `-l` or `--log_from_esl`: If this flag is set, the key material is from the ESL console. The session key will be changed to big endian.
- `-D` or `--decrypt`: If this flag is set, the script will print the decrypted data.
- `-E` or `--encrypt`: If this flag is set, the script will print the encrypted data and tag.

1. **Encrypt Payload**: To encrypt payload data, run the script with the `-E` command followed by the payload you want to encrypt.

    ```bash
        python ead_ccm.py -a 1431396a41fe204b31a50bf2ff506afbe3599ccdb9 -s 0x57a9da12d12e6e131e20612ad10a6a19 -i 0x46e77ab1ef007a9e -E
    ```

2. **Decrypt Payload**: To decrypt payload data, run the script with the `-D` command followed by the payload you want to decrypt.

    ```bash
        python ead_ccm.py -a 1431396a41fe204b31a50bf2ff506afbe3599ccdb9 -s 0x57a9da12d12e6e131e20612ad10a6a19 -i 0x46e77ab1ef007a9e -D
    ```

# `esl_tlv_generator_ui` GUI Tool Usage

The `esl_tlv_generator_ui` is a graphical user interface (GUI) tool for generating TLV (Type-Length-Value) data.

## Prerequisites

- Python 3.x
- PySide6

# `epd_simulator.py` Image file generator Usage

The `epd_simulator.py` script is used to convert monochrome bmp to pixel-mapped image files for EPD displays.

Prepare monochrome bmp files for EPD displays.

## Instructions
For most of EPD, the pixel of first frame(black frame) is inverted which means bit on in BMP will be pixel off on EPD. So this script will invert pixel automatically.

The script takes several command-line arguments:

- `-i` or `--input_file`: Path to the input file.
- `-i2` or `--input_file2`: Path to the input file for the second frame.
- `-n` or `--not_inverted`: If present, the pixel of the first frame will not be inverted.
- `-o` or `--output_file`: Path to the output file.
- `-a` or `--action`: Action to perform. Options are:
  - `d`: Draw
  - `g`: Generate binary file from bmp
  - `b`: Generate binary file from c array
  - `a`: Generate c array from binary file
  - `c`: Generate checksum of input file
- `-s` or `--scan`: Scan mode (0-7).
- `-c` or `--color`: Color mode. Options are:
  - `1`: Monochrome black white
  - `3`: Monochrome red white
- `-t` or `--transform`: Transformations to apply to the data. Options are `no`, `lr`, `ud`, `udlr`.
- `-r` or `--red`: If present, the image will be generated with a red frame.


    #### To generate single black frame image, follow these steps:

    1. python epd_simulator.py -i `BMP file name` -a g -o `output file name`

    #### To generate single black frame image without inverted pixel follow these steps:

    1. python epd_simulator.py -i `BMP file name` -a g -o `output file name` -n

    #### To generate single red frame image, , follow these steps:

    1. python epd_simulator.py -i `BMP file name` -a g -o `output file name` -r

    #### To generate two frame image, follow these steps:

    1. python epd_simulator.py -i `black frame BMP file name` -i2 `red frame BMP file name` -a g -o `output file name`

    #### To generate c array from binary file, follow these steps:

    1. Generate binary file from bmp file with above steps.
    2. python epd_simulator.py -i `binary file name` -a a -o `output header file name`
