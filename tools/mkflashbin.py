#!/usr/bin/env python3

import sys
import os

LEN_ADDR = (0x100 - 0x04)
APP_ADDR = 0x100

def make_flash_bin(bootrombinfile, appbinfile, outbinfile):
    bootrom = open(bootrombinfile, 'rb')
    bootrom_content = bootrom.read()

    app = open(appbinfile, 'rb')
    app_content = app.read()

    out = open(outbinfile, 'wb+')

    # wirte bootrom
    for b in bootrom_content:
        array = []
        array.append(b)
        out.write(bytearray(array))

    # write pad
    for i in range(252 - len(bootrom_content)):
        array = []
        array.append(0x00)
        out.write(bytearray(array))

    # write len
    array = []
    app_len = len(app_content)
    array.append(app_len & 0xff)
    array.append((app_len >> 8) & 0xff)
    array.append((app_len >> 16) & 0xff)
    array.append((app_len >> 24) & 0xff)
    out.write(bytearray(array))

    # write app
    for b in app_content:
        array = []
        array.append(b)
        out.write(bytearray(array))

    bootrom.close()
    app.close()
    out.close()

if __name__ == '__main__':
    if len(sys.argv) == 4:
        make_flash_bin(sys.argv[1], sys.argv[2], sys.argv[3])
    else:
        print('Usage: %s bootrombinfile appbinfile outbinfile' % sys.argv[0])
