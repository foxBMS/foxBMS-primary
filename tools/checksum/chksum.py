# @copyright &copy; 2010 - 2016, Fraunhofer-Gesellschaft zur Foerderung der angewandten Forschung e.V. All rights reserved.
#
# BSD 3-Clause License
# Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
# 1.  Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
# 2.  Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
# 3.  Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# We kindly request you to use one or more of the following phrases to refer to foxBMS in your hardware, software, documentation or advertising materials:
#
# &Prime;This product uses parts of foxBMS&reg;&Prime;
#
# &Prime;This product includes parts of foxBMS&reg;&Prime;
#
# &Prime;This product is derived from foxBMS&reg;&Prime;

'''
@file       chksum.py
@date       02.09.2015 (date of creation)
@author     foxBMS Team
@ingroup    tools
@prefix     none
@brief      Checksum tool

This script will be invoked in a post-build step to generate either a STM32 compatible CRC32
checksum or a simple Modulo32BitAddition of the provided binary.
For this purpose the generated .hex file will be loaded from disc and its content is being parsed.
In the next step a buffer file will be created, initialized with 0xFF to match the pattern during
flashand parsed content from the hex file is stored into this filebuffer. Each line gets its own
line-chksum and desired chksum type is calculated and written into a headerfile in the final step.
'''
import time
import sys
import os
import ConfigParser
import binascii
import struct
import argparse
import cStringIO
import bitstring
STARTS = '************************************************************************'
STARTS_LEN = len(STARTS)

def sprint(str):
    """pretty printing configuration and output"""
    output_string = '* ' + str + (STARTS_LEN - len(str) - 3) * ' ' + '*'
    return output_string

class IniFile(object):
    """ Read the project configuration of checksum project"""
    def __init__(self, file_path, hex_file=0):
        if not os.path.isfile(file_path):
            print 'Error: ini file not found.'
            sys.exit(1)

        run_cfg = ConfigParser.ConfigParser()
        run_cfg.read(file_path)

        self.project_type = self.get_key_by_section(run_cfg, 'project', 'projecttype')
        if hex_file:
            self.hex_file_path = hex_file
        else:
            self.hex_file_path = self.get_key_by_section(run_cfg, 'file', 'path')
        if not os.path.isfile(self.hex_file_path):
            print 'Error: hex file not found.'
            sys.exit(1)
        self.flashsize = long(self.get_key_by_section(run_cfg, 'flashsize', 'flashsize'), 16)
        self.flash_header_address = long(self.get_key_by_section(run_cfg, \
            'addresses', 'flashheader'), 16)
        self.flash_header_address &= 0x00ffffff
        self.begin_address = long(self.get_key_by_section(run_cfg, 'addresses', 'beginaddress'), 16)
        self.begin_address &= 0x00ffffff
        self.end_address = long(self.get_key_by_section(run_cfg, 'addresses', 'endaddress'), 16)
        self.end_address &= 0x00ffffff
        self.chksum_address = long(self.get_key_by_section(run_cfg, \
            'addresses', 'chksumaddress'), 16)
        self.chksum_address &= 0x00ffffff
        self.checksum_algo = self.get_key_by_section(run_cfg, \
            'checksum', 'checksumalgorithm')
        self.crc_polynomial = self.get_key_by_section(run_cfg, \
            'crc-polynomial', 'polynomial')

    def print_config(self):
        """helper function for pretty printing the configuration"""
        pretty_config = STARTS + '\n'
        pretty_config += sprint('Configuration:') + '\n'
        pretty_config += sprint('Project type:         ' + self.project_type) + '\n'
        pretty_config += sprint('Hex file path:        ' + self.hex_file_path) + '\n'
        pretty_config += sprint('Flashsize:            0x%0x (%d)' % \
            (self.flashsize, self.flashsize)) + '\n'
        pretty_config += sprint('Flash header address: 0x%0x (%d)' % \
            (self.flash_header_address, self.flash_header_address)) + '\n'
        pretty_config += sprint('Begin address:        0x%0x (%d)' % \
            (self.begin_address, self.begin_address)) + '\n'
        pretty_config += sprint('End address:          0x%0x (%d)' % \
            (self.end_address, self.end_address)) + '\n'
        pretty_config += sprint('Checksum address:     0x%0x (%d)' % \
            (self.chksum_address, self.chksum_address)) + '\n'
        pretty_config += sprint('Checksum algorithm:   ' + self.checksum_algo) + '\n'
        pretty_config += sprint('Crc polynomial:       ' + self.crc_polynomial)
        return pretty_config

    @staticmethod
    def get_key_by_section(project_cfg, section, key):
        """helper function for getting accuarte error messages on
        configuration parsing."""
        err_bit = False
        try:
            gkey = project_cfg.get(section, key)
        except ConfigParser.NoSectionError:
            print 'Config parsing Error: Could not find sectioon \"%s\"' %(section)
            err_bit = True
        except ConfigParser.NoOptionError:
            print 'Config parsing Error: Could not find key \"%s\" in section \"%s\"' \
                %(key, section)
            err_bit = True
        except ConfigParser.Error:
            print 'Config parsing Error: general error'
            err_bit = True
        if err_bit:
            sys.exit()
        return gkey




if __name__ == '__main__':
    HELP_TEXT = """ Post-Build Program chksum.py for checksum generation.
                In the first build the elf-file will contain a wrong checksum value,
                only *.hex-file will be modified with a valid checksum.
                """
    # get parameters
    parser = argparse.ArgumentParser(description=HELP_TEXT, add_help=True)
    parser.add_argument('iniFile', help='ini-file')
    parser.add_argument('-bd', '--builddir', help='specify build directory')
    parser.add_argument('-hf', '--hexfile', help='specify hexfile in command \
        line instead of *.ini file')
    CMD_LINE_ARGS = parser.parse_args()

    # set build dir and build file
    if CMD_LINE_ARGS.builddir:
        TEMP_DIR = [os.path.normpath(CMD_LINE_ARGS.builddir), 'chk5um']
    else:
        TEMP_DIR = ['build', 'chk5um']
    CHK_SUM_DIR = os.path.join(*TEMP_DIR)

    # make checksum dir in build directory
    try:
        os.makedirs(CHK_SUM_DIR)
    except OSError:
        if not os.path.isdir(CHK_SUM_DIR):
            raise

    # get project configuration
    if CMD_LINE_ARGS.hexfile:
        chksum_cfg = IniFile(CMD_LINE_ARGS.iniFile, CMD_LINE_ARGS.hexfile)
    else:
        chksum_cfg = IniFile(CMD_LINE_ARGS.iniFile)

    CONFIG_STRING = chksum_cfg.print_config()
    print CONFIG_STRING

    output = ''

    p_buffer_file_ptr_dst_add = 0x00000000
    dst_pos_finalchksum_err = False

    src_line_data_length_cpy = 0
    src_line_address_cpy = 0
    src_line_record_type_cpy = 0
    src_line_data_cpy = []

    src_file_curr_line = 0
    with open(chksum_cfg.hex_file_path, 'rb+') as p_src_file:
        p_buffer_file = cStringIO.StringIO()
        buffer_input = ['%c' % 0xFF for i in xrange(chksum_cfg.flashsize)]
        p_buffer_file.writelines(buffer_input)

        p_buffer_file.seek(0)  # go to beginning of the file for further operations
        final_data_chksum = 0x00000000

        while True:  # iterate whole file content and parse it line by line
            src_file_curr_line += 1
            src_file_line_buffer = p_src_file.readline()
            if not src_file_line_buffer:
                break # EOF

            src_file_line_buffer_stripped = src_file_line_buffer[1:].rstrip()
            src_file_line_decoded = struct.unpack('>BH' + str(len(src_file_line_buffer_stripped)/2 - 3) + \
                                               'B', binascii.unhexlify(src_file_line_buffer_stripped))
            src_line_data_length = src_file_line_decoded[0]
            src_line_address = src_file_line_decoded[1]  # Convert the offset address
            src_line_record_type = src_file_line_decoded[2]  # Convert the record type
            src_line_data = src_file_line_decoded[3:]

            src_line_chksum_new_calc = 0

            for i in range(src_line_data_length):
                src_line_chksum_new_calc += src_line_data[i]  # pre calc for checksum

            src_line_chksum = (src_file_line_decoded[src_line_data_length+3])  # Convert line checksum

            src_line_chksum_new_calc += (int(src_line_data_length) + int(src_line_address / 256) + \
                                     int(src_line_address % 256) + int(src_line_record_type) + \
                                     int(src_line_chksum))  # calculated value must be 0

            # sum of all bytes inclusive the checksum must be 0x00, print error when != 0
            if (src_line_chksum_new_calc % 256) != 0:
                output += '**************************** E R R O R *********************************' + '\n'
                output += 'Error! Checksum of line %d not correct.\n' %(src_file_curr_line)
            else:
                if src_line_record_type == 0x04:  # changing the two higher byte of src_line_address if record-typ is 0x04
                    p_buffer_file_ptr_dst_add = 0x00000000
                    p_buffer_file_ptr_dst_add = src_line_data[1] * 65536

                p_buffer_file_ptr_dst_add &= 0xFFFF0000  # mask the lower 2 bytes
                p_buffer_file_ptr_dst_add = p_buffer_file_ptr_dst_add + src_line_address

                # if record-typ declared as Data (record type == 0x00),
                # then write into buffer file at the position (from src_line_address)
                if src_line_record_type == 0x00:
                    p_buffer_file.seek(p_buffer_file_ptr_dst_add)
                    for i in range(src_line_data_length):
                        p_buffer_file.write('%c' % src_line_data[i]) # write src_line_data into binary buffer file

                # pre-calc for destination of final checksum if data record type and adress within
                # checksum position
                if p_buffer_file_ptr_dst_add <= chksum_cfg.chksum_address and \
                   (p_buffer_file_ptr_dst_add + int(src_line_data_length) - 4) >= chksum_cfg.chksum_address and \
                   (src_line_record_type == 0x00):
                    dst_pos_final_chksum = p_src_file.tell() - int(src_line_data_length) * 2 - 4
                    dst_pos_new_line_chksum = p_src_file.tell() - 4
                    # define dst_pos_final_chksum as start of the src_line_data bytes
                    # tell => position at end of line
                    # - src_line_data_length*2 => minus the src_line_data_length in ascii
                    # -4 => minus line-src_line_chksum and 0xA and 0xD (ascii: newline)

                    # calc the destination of final src_line_chksum position
                    dst_pos_final_chksum = dst_pos_final_chksum + \
                        (chksum_cfg.chksum_address - p_buffer_file_ptr_dst_add) * 2
                    dst_pos_finalchksum_err = True  # position for final data-checksum is found, no error

                    # copy the destination line of the final data checksum (the line checksum has to recalculate!)
                    src_line_data_length_cpy = src_line_data_length
                    src_line_address_cpy = src_line_address
                    src_line_record_type_cpy = src_line_record_type
                    src_line_data_cpy = list(src_line_data)

                if chksum_cfg.flash_header_address == p_buffer_file_ptr_dst_add:
                    flash_header_hex_start = p_src_file.tell() - 4 - src_line_data_length * 2


        if dst_pos_finalchksum_err is False:
            output += '**************************** E R R O R *********************************' + '\n'
            output += 'Error! Position for final checksum not found.' + '\n'
        else:
            p_buffer_file.seek(chksum_cfg.begin_address)  # get the start address for the calculating src_line_chksum
            src_file_4byte_buff = struct.unpack('I', p_buffer_file.read(4))[0]  # read 4 bytes
            chksum_start_address = src_file_4byte_buff & 0x00ffffff # convert to long and mask the highest byte

            p_buffer_file.seek(chksum_cfg.end_address) # get the end address for the calculating src_line_chksum
            src_file_4byte_buff = struct.unpack('I', p_buffer_file.read(4))[0]  # read 4 bytes
            chksum_end_address = (src_file_4byte_buff - 4) & 0x00ffffff  # mask the highest byte
            chksum_end_address = (src_file_4byte_buff) & 0x00ffffff
            p_buffer_file.seek(chksum_start_address)

            chksum_calc_length = chksum_end_address - chksum_start_address + 1

            # calculate Modulo-32-Bit-Addition
            if chksum_cfg.checksum_algo == 'modulo32bitaddition':
                chksum_calc_length /= 4  # calc the count for 4 byte steps
                while chksum_calc_length > 0:
                    src_file_4byte_buff = struct.unpack('I', p_buffer_file.read(4))[0]  # read 4 bytes
                    final_data_chksum += src_file_4byte_buff  # calculate the addition-checksum
                    final_data_chksum &= 0xFFFFFFFF  # mask 32bit to prevent python from using auto-types
                    chksum_calc_length -= 1

            # calc crc32, using python binascii module, use mask to ensure compatible output
            if chksum_cfg.checksum_algo == 'crc32':
                final_data_chksum = (binascii.crc32(p_buffer_file.read(chksum_calc_length), 0) \
                    & 0xFFFFFFFF)

            chksum_pack_temp = struct.pack('>L', final_data_chksum)
            final_data_chksum_written = struct.unpack('<L', chksum_pack_temp)
            

            p_src_file.seek(dst_pos_final_chksum)
            p_src_file.write('%08X' % final_data_chksum_written)  # write chcksum

            src_line_address_cpy_temp = 0
            src_line_address_cpy_temp &= 0x0000FFFF
            src_line_address_cpy_temp |= src_line_address_cpy

            # re-calc new line checksum for changed line and overwrite the new data checksum
            idx = (chksum_cfg.chksum_address & 0x0000ffff) - src_line_address_cpy_temp
            
            src_line_data_cpy[idx:idx+4] = struct.unpack('>BBBB', chksum_pack_temp)

            for i in range(src_line_data_length_cpy):
                src_line_chksum_new_calc -= src_line_data_cpy[i]

            #  calc new line chcksum
            src_line_chksum_new_calc -= (int(src_line_data_length_cpy) + \
                int(src_line_address_cpy / 256) + int(src_line_address_cpy % 256) + \
                int(src_line_record_type_cpy))

            p_src_file.seek(dst_pos_new_line_chksum)
            p_src_file.write('%02X' % (src_line_chksum_new_calc % 256))  # write new line chcksum
            chksum_output_string = '32-bit SW-Chksum:     0x%08X' % (final_data_chksum)
            output += sprint(chksum_output_string) +  '\n'
            output += STARTS + '\n'

            p_buffer_file.seek(chksum_cfg.flash_header_address + 24)
    output += '* Checksum tool successful                                             *' + '\n'
    output += STARTS
    print output
    with open(os.path.join(CHK_SUM_DIR, 'chksum.log'), 'w+') as f:
        f.write(CONFIG_STRING + '\n' + output)
