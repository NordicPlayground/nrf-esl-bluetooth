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

from Cryptodome.Cipher import AES
import binascii
import argparse
import array

aad = binascii.unhexlify(b'ea')
EAD_AD_TYPE = 0x31

def test_1():
    data = binascii.unhexlify(b'0F0953686F7274204D696E692D42757303190A8C')
    ''' Randomizer from data set should change to little endian'''
    randomizer = binascii.unhexlify(b'DECA57E118')
    randomizer = randomizer[::-1]
    iv = binascii.unhexlify(b'46E77AB1EF007A9E')
    key = binascii.unhexlify(b'57A9DA12D12E6E131E20612AD10A6A19')
    encrypt_and_decrypt(data, randomizer, iv, key)


def test_2():
    data = binascii.unhexlify(b'0F0953686F7274204D696E692D42757303190A8C')
    ''' Randomizer from data set should change to little endian'''
    randomizer = binascii.unhexlify(b'7A6E971C8D')
    randomizer = randomizer[::-1]
    iv = binascii.unhexlify(b'46E77AB1EF007A9E')
    key = binascii.unhexlify(b'57A9DA12D12E6E131E20612AD10A6A19')
    encrypt_and_decrypt(data, randomizer, iv, key)


def encrypt_and_print(data, randomizer, iv, session_key, log_from_esl=False):
    global aad

    ''' reverse endian'''
    if log_from_esl is True:
        iv = iv[::-1]

    nonce = randomizer + iv
    print("randomizer:", binascii.hexlify(randomizer))
    print("iv:", binascii.hexlify(iv))
    print("session_key:", binascii.hexlify(session_key))
    print("nonce:", binascii.hexlify(nonce))
    cipher = AES.new(session_key, AES.MODE_CCM,
                     nonce=nonce, mac_len=4, assoc_len=1)
    cipher.update(aad)
    ciphertext, tag = cipher.encrypt_and_digest(data)
    print("ciphertext :", binascii.hexlify(ciphertext))
    print("tag:", binascii.hexlify(tag))
    return ciphertext, tag


def decrypt_and_print(data, randomizer, iv, session_key, tag, log_from_esl=False):
    global aad

    ''' reverse endian'''
    if log_from_esl is True:
        iv = iv[::-1]

    nonce = randomizer + iv
    print("randomizer:", binascii.hexlify(randomizer))
    print("iv:", binascii.hexlify(iv))
    print("session_key:", binascii.hexlify(session_key))
    print("nonce:", binascii.hexlify(nonce))
    print("ciphertext:", binascii.hexlify(data))
    print("tag:", binascii.hexlify(tag))

    cipher = AES.new(session_key, AES.MODE_CCM,
                     nonce=nonce, mac_len=4, assoc_len=1)
    cipher.update(aad)
    try:
        decryptdata = cipher.decrypt_and_verify(data, tag)
    except KeyboardInterrupt:
        exit(1)
    except ValueError:
        return False

    print("decryptdata:", binascii.hexlify(decryptdata))
    return True

def encrypt_and_decrypt(data, orig_randomizer, orig_iv, session_key):
    ciphertext, tag = encrypt_and_print(
        data, orig_randomizer, orig_iv, session_key)
    decrypt_and_print(ciphertext, orig_randomizer, orig_iv, session_key, tag)


def main(options):
    global EAD_AD_TYPE

    if options.test == 1:
        test_1()
        exit(0)
    elif options.test == 2:
        test_2()
        exit(0)
    else:
        log_from_esl = options.log_from_esl

        if options.ad_data is not None:
            ad_data = binascii.unhexlify(options.ad_data.replace("0x", ""))
            if ad_data[1] == EAD_AD_TYPE:
                ad_data = ad_data[2:]
            # randomizer length = 5 bytes
            randomizer = ad_data[:5]
            data = ad_data[5:]
        else:
            ''' Randomizer from data set should change to little endian'''
            randomizer = binascii.unhexlify(options.randomizer.replace("0x", ""))
            randomizer = randomizer[::-1]
            data = binascii.unhexlify(options.data.replace("0x", ""))

        if options.key is None:
            key = binascii.unhexlify(options.session_key.replace("0x", ""))
            iv = binascii.unhexlify(options.iv.replace("0x", ""))
        else:
            key_data = options.key.replace("0x", "")
            # key materal length = 16 bytes * 2 (hexstring len)
            key = binascii.unhexlify(key_data[0:32])
            iv = binascii.unhexlify(key_data[32:])

        iv = iv[::-1]
        if options.decrypt is True:
            tag = data[-4:]
            if decrypt_and_print(data[:-4], randomizer, iv, key, tag, log_from_esl) == False:
                print("decrypt failed")
        else:
            encrypt_and_print(data, randomizer, iv, key, log_from_esl)


"""
Supplement to the Bluetooth Core Specification CSS 11 2.3 ENCRYPTED ADVERTISING DATA SAMPLE DATA
# encrypt test data set 1
python ccm.py -d 0F0953686F7274204D696E692D42757303190A8C -r 0xDECA57E118 -i 0x46E77AB1EF007A9E -s 0x57A9DA12D12E6E131E20612AD10A6A19
python ccm.py -d 0F0953686F7274204D696E692D42757303190A8C -r 0xDECA57E118 -k 0x57A9DA12D12E6E131E20612AD10A6A1946E77AB1EF007A9E
# decrypt test data set 1
python ccm.py -d 74e4dcafdc51c7282810c2217f0e4cef4343181fba0069cc -r DECA57E118 -k 57A9DA12D12E6E131E20612AD10A6A1946E77AB1EF007A9E -D
# decrypt test data set 1
python ccm.py -a 1E3118E157CADE74E4DCAFDC51C7282810C2217F0E4CEF4343181FBA0069CC -k 57A9DA12D12E6E131E20612AD10A6A1946E77AB1EF007A9E -D
# decrypt test data set 2
python ccm.py -a 1E318D1C976E7A35444076125788C238A58E8BD9CFF0DEFE251A8E7275454C -k 57A9DA12D12E6E131E20612AD10A6A1946E77AB1EF007A9E -D
# PTS write key material in little endian.
- AP Sync SessionKey: 57A9DA12D12E6E131E20612AD10A6A19, IV: 46E77AB1EF007A9E
- ESL Response SessionKey: 57A9DA12D12E6E131E20612AD10A6A19, IV: 46E77AB1EF007A9E
- Write: AP Sync Key:
    Session Key: [0x196A0AD12A61201E136E2ED112DAA957]
    IV: [0x9E7A00EFB17AE746]
- Write Long Characteristic Value completed successfully.
- Write: ESL Response Key:
    Session Key: [0x196A0AD12A61201E136E2ED112DAA957]
    IV: [0x9E7A00EFB17AE746]

To use key material from PTS log
To use key material from TAG log, iv should be changed to big endian manually.
# decrypt PTS data, key and iv are from PTS log
python ccm.py -a 1431396a41fe204b31a50bf2ff506afbe3599ccdb9 -s 0x57a9da12d12e6e131e20612ad10a6a19 -i 0x46e77ab1ef007a9e -D
# decrypt PTS data, key and iv are from ESL console log
python ccm.py -a 1431396a41fe204b31a50bf2ff506afbe3599ccdb9 -s 0x57a9da12d12e6e131e20612ad10a6a19 -i 0x9e7a00efb17ae746 -D -l
"""

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('-t', '--test', help='use test data', type=int)
    parser.add_argument(
        '-d', '--data', help='data to encrypt / decrypt', type=str)
    parser.add_argument('-a', '--ad_data', help='adevertising data', type=str)
    parser.add_argument('-r', '--randomizer', help='randomizer', type=str)
    parser.add_argument('-i', '--iv', help='iv', type=str)
    parser.add_argument('-s', '--session_key', help='session key', type=str)
    parser.add_argument(
        '-k', '--key', help='key material, 16 bytes session key + 8 bytes IV', type=str)
    parser.add_argument('-l', '--log_from_esl',
                        help='key material from ESL console, session key has change to big endian', action='store_true')
    parser.add_argument('-D', '--decrypt',
                        help='print decrypt data', action='store_true')
    parser.add_argument('-E', '--encrypt',
                        help='print encrypt data and tag', action='store_true')

    options = parser.parse_args()
    if options.test == None and ((options.data == None and options.randomizer) and (options.ad_data == None)) and ((options.iv == None and options.session_key == None) or (options.key == None)):

        print('You have to specify either -t or (-d and -r) or (-a and -r) or (-k and -i) or (-k and -s)')
        parser.print_help()
        exit(1)
    main(options)
