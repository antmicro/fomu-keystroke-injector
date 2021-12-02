#!/usr/bin/env python3
# Based on: https://github.com/im-tomu/foboot/blob/master/examples/add_autoboot_timeout.py
import os

COMMENT_START = 0x00FF
COMMENT_END = 0xFF00
MAGIC_WORD = 0x4A6DE3AC
BITSTREAM_SYNC_HEADER1 = 2123999870
BITSTREAM_SYNC_HEADER2 = 2125109630
ENDIANNESS = "little"


def to_bytes(v, n=4):
    return v.to_bytes(n, byteorder=ENDIANNESS, signed=False)


def from_bytes(v):
    return int.from_bytes(v, byteorder=ENDIANNESS)




def autoboot_timeout(bitstream, timeout):
    if timeout not in range(1, 256):
        print("[autoboot] Timeout out of range (between 1 and 255)")
        return False

    temp = "{}.tmp".format(bitstream)
    try:
        os.rename(bitstream, temp)
    except OSError as err:
        print("[autoboot] Couldn't move file {}".format(bitstream))
        print("[autoboot] OS error: {0}".format(err))
        return False

    infile = open(temp, "rb")
    outfile = open(bitstream, "wb")

    first_word = from_bytes(infile.read(4))
    if (
        first_word != BITSTREAM_SYNC_HEADER1
        and first_word != BITSTREAM_SYNC_HEADER2
        and first_word & 0xFFFF != COMMENT_START
    ):
        print("[autoboot] Error: File does not appear to be a bitstream")
        return False
    
    outfile.write(to_bytes(COMMENT_START))
    outfile.write(to_bytes(MAGIC_WORD))
    outfile.write(to_bytes(timeout))
    if first_word & 0xFFFF == COMMENT_START:  # there already is a comments section
        outfile.write(to_bytes(0, n=2))  # make sure alignment is correct
        outfile.write(to_bytes(first_word >> 16, n=2))
    else:
        outfile.write(to_bytes(COMMENT_END))
    
    outfile.write(infile.read())
    
    infile.close()
    outfile.close()

    try:
        os.remove(temp)
    except OSError as err:
        print("[autoboot] Couldn't delete old bitstream file")
        print("[autoboot] OS error: {0}".format(err))
    
    return True
