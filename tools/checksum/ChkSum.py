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
@file       ChkSum.py
@date       02.09.2015 (date of creation)
@author     foxBMS Team
@ingroup    tools
@prefix     none
@brief      Checksum tool

This script will be invoked in a post-build step to generate either a STM32 compatible CRC32 checksum 
or a simple Modulo32BitAddition of the provided binary.
For this purpose the generated .hex file will be loaded from disc and its content is being parsed.
In the next step a buffer file will be created, initialized with 0xFF to match the pattern during flash
and parsed content from the hex file is stored into this filebuffer. Each line gets its own line-chksum
and desired chksum type is calculated and written into a headerfile in the final step.
'''

import sys
import os
import ConfigParser
import bitstring
import binascii
import numpy as np
import struct
import argparse

stars = '************************************************************************'
stars_len = len(stars)

def sprint(str):
    o = '* ' + str + (stars_len - len(str) - 3) * ' ' + '*'
    return o

# ini file class, used to store needed information about the input binary
class IniFile:
    def __init__(self, ProjectType, HexFilePath, ChecksumHeaderFilePath, FlashHeaderAdress, BeginAddress, EndAddress, ChksumAddress, ChecksumAlgo, CrcPolynomial):
        self.ProjectType = ProjectType
        self.HexFilePath = HexFilePath
        self.ChecksumHeaderFilePath = ChecksumHeaderFilePath
        self.FlashHeaderAdress = FlashHeaderAdress
        self.BeginAddress = BeginAddress
        self.EndAddress = EndAddress
        self.ChksumAddress = ChksumAddress
        self.ChecksumAlgo = ChecksumAlgo
        self.CrcPolynomial = CrcPolynomial

    # function to parse alle needed informations from the config file, using Python ConfigParser module
    def ParseConfig(self, FilePath):
        configOK = False

        Config = ConfigParser.ConfigParser()

        try:
            Config.read(FilePath)
        except:
            print('\nError! Ini-File not found in path \n' + os.getcwd())

        try:
            print stars
            print sprint('Configuration:')

            #############################################################
            # [project]
            self.ProjectType = Config.get('project', 'projecttype')
            o = 'Project type:         ' + self.ProjectType
            print sprint(o)

            #############################################################
            # [file]
            self.HexFilePath = Config.get('file', 'path')
            self.HexFilePath = os.path.normpath(self.HexFilePath)
            o = 'Hex file path:        ' + self.HexFilePath
            print sprint(o)

            ##############################################################
            # [flashsize]
            self.flashsize = Config.get('flashsize', 'flashsize')
            fs = long(Cfg.flashsize, 16)
            self.flashsize = fs
            o = 'Flashsize:            0x%0x (%d)' % (self.flashsize, self.flashsize)
            print sprint(o)

            # # [addresses]
            self.FlashHeaderAdress = Config.get('addresses', 'flashheader')
            a = long(Cfg.FlashHeaderAdress, 16)
            a &= 0x00ffffff
            self.FlashHeaderAdress = a
            o = 'Flash header address: 0x%0x (%d)' % (self.FlashHeaderAdress, self.FlashHeaderAdress)
            print sprint(o)

            self.BeginAddress = Config.get('addresses', 'beginaddress')
            a = long(self.BeginAddress, 16)
            a &= 0x00ffffff
            self.BeginAddress = a
            o = 'Begin address:        0x%0x (%d)' % (self.BeginAddress, self.BeginAddress)
            print sprint(o)

            self.EndAddress = Config.get('addresses', 'endaddress')
            a = long(self.EndAddress, 16)
            a &= 0x00ffffff
            self.EndAddress = a
            o = 'End address:          0x%0x (%d)' % (self.EndAddress, self.EndAddress)
            print sprint(o)

            self.ChksumAddress = Config.get('addresses', 'chksumaddress')
            a = long(self.ChksumAddress, 16)
            a &= 0x00ffffff
            self.ChksumAddress = a
            o = 'Chksum address:       0x%0x (%d)' % (self.ChksumAddress, self.ChksumAddress)
            print sprint(o)

            #############################################################
            # [checksum]
            self.ChecksumAlgo = Config.get('checksum', 'checksumalgorithm')
            o = 'Checksum algorithm:   ' + self.ChecksumAlgo
            print sprint(o)

            #############################################################
            # [crc-polynomial]
            self.CrcPolynomial = Config.get('crc-polynomial', 'polynomial')
            o = 'CrcPolynomial:        ' + self.CrcPolynomial
            print sprint(o)

            configOK = True

        except:
            print('\nConfig parsing Error\n' + os.getcwd())
            configOK = False

        return configOK

class HexaToUx:
    def __init__(self, LineString, Position):
        self.LineString = LineString
        self.Position = Position

    # convert from hex to U8
    def HexasciiToU8(self):
        a = bitstring.Bits(hex=self.LineString[self.Position:self.Position + 2])
        return np.uint8(a.int)

    # convert from hex to U16
    def HexasciiToU16(self):
        a = bitstring.Bits(hex=self.LineString[self.Position:self.Position + 4])
        return np.uint16(a.int)

    # convert from hex to U32
    def HexasciiToU32(self):
        a = bitstring.Bits(hex=self.LineString[self.Position:self.Position + 8])
        return np.uint32(a.int)

if __name__ == '__main__':
    helpText = ''' Post-Build Program chksum.py for checksum generation.
                In the first build the elf-file will contain a wrong checksum value,
                only *.hex-file will be modified with a valid checksum.
                '''
    # get parameters
    parser = argparse.ArgumentParser(description=helpText, add_help=True)
    parser.add_argument('iniFile', help='ini-file')
    args = parser.parse_args()

    # set build dir and build file
    tempD = ['build', 'chk5um']
    tempF = ['build', 'chk5um', 'binbuffer_without_chksum.bin']
    chkSumDir = os.path.join(*tempD)
    binBufferWithoutChkSum = os.path.join(*tempF)
    try:
        os.makedirs(chkSumDir)
    except OSError:
        if not os.path.isdir(chkSumDir):
            raise

    pBufferFilePtrDstAdd = 0x00000000
    DstPosFinalChksumErr = False

    SrcLineDataLengthCpy = 0
    SrcLineAddressCpy = []
    SrcLineRecordTypeCpy = 0
    SrcLineDataCpy = []

    # Get initial configuration
    Cfg = IniFile('', '', '', 0x00000000, 0x00000000, 0x00000000, 0x00000000, '', 0x00000000)
    Cfg.ParseConfig(args.iniFile)

    SrcFileCurrLine = 0
    with open(Cfg.HexFilePath, 'rb+') as pSrcFile, open(binBufferWithoutChkSum, 'wb+') as pBufferFile:  # open source hex-file, path from the ini-file
        bufferInput = ['%c' % 0xFF for i in xrange(Cfg.flashsize)]
        pBufferFile.writelines(bufferInput)

        pBufferFile.seek(0)  # go to beginning of the file for further operations
        FinalDataChksum = 0x00000000

        while True:  # iterate whole file content and parse it line by line
            SrcFileCurrLine += 1
            SrcFileLineBuffer = pSrcFile.readline()
            if not SrcFileLineBuffer:
                break # EOF

            SrcLineAddress = []
            SrcLineDataLength = (HexaToUx(SrcFileLineBuffer, 1)).HexasciiToU8()  # Convert the SrcLineData length
            SrcLineAddress.append((HexaToUx(SrcFileLineBuffer, 3)).HexasciiToU8())  # Convert the offset address
            SrcLineAddress.append((HexaToUx(SrcFileLineBuffer, 5)).HexasciiToU8())  # Convert the offset address
            SrcLineRecordType = (HexaToUx(SrcFileLineBuffer, 7)).HexasciiToU8()  # Convert the record type

            SrcLineChksumNewCalc = 0
            SrcLineData = []

            for i in range(SrcLineDataLength):
                SrcLineData.append((HexaToUx(SrcFileLineBuffer, i * 2 + 9)).HexasciiToU8())  # Convert the data
                SrcLineChksumNewCalc += SrcLineData[i]  # pre calc for checksum

            SrcLineChksum = (HexaToUx(SrcFileLineBuffer, SrcLineDataLength * 2 + 9)).HexasciiToU8()  # Convert line checksum

            SrcLineChksumNewCalc += (int(SrcLineDataLength) + int(SrcLineAddress[0]) + int(SrcLineAddress[1]) + int(SrcLineRecordType) + int(SrcLineChksum))  # calculated value must be 0

            if np.uint8(SrcLineChksumNewCalc) != 0x00:  # sum of all bytes inclusive the checksum must be 0x00, print error when != 0
                print('**************************** E R R O R *********************************')
                print('Error! Checksum of line %d not correct.' % SrcFileCurrLine)
            else:
                if SrcLineRecordType == 0x04:  # changing the two higher byte of SrcLineAddress if record-typ is 0x04
                    pBufferFilePtrDstAdd = 0x00000000
                    pBufferFilePtrDstAdd = np.uint32(SrcLineData[1] << 16)  # shifting 2 bytes

                pBufferFilePtrDstAdd &= 0xFFFF0000  # mask the lower 2 bytes
                pBufferFilePtrDstAdd |= np.uint8(SrcLineAddress[0]) << 8  # add the lower byte
                pBufferFilePtrDstAdd |= np.uint8(SrcLineAddress[1])  # add the lowest byte

                if SrcLineRecordType == 0x00:  # if record-typ declared as Data (record type == 0x00), then write into buffer file at the position (from SrcLineAddress)
                    pBufferFile.seek(pBufferFilePtrDstAdd)
                    for i in range(np.int32(SrcLineDataLength)):
                        pBufferFile.write('%c' % np.uint8(SrcLineData[i]))  # write SrcLineData into binary buffer file

                # pre-calc for destination of final checksum if data record type and adress within checksum position
                if pBufferFilePtrDstAdd <= Cfg.ChksumAddress and (pBufferFilePtrDstAdd + int(SrcLineDataLength) - 4) >= Cfg.ChksumAddress and (SrcLineRecordType == 0x00):
                    DstPosFinalChksum = pSrcFile.tell() - int(SrcLineDataLength) * 2 - 4
                    DstPosNewLineChksum = pSrcFile.tell() - 4
                    # define DstPosFinalChksum as start of the SrcLineData bytes
                    # tell => position at end of line
                    # - SrcLineDataLength*2 => minus the SrcLineDataLength in ascii
                    # -4 => minus line-SrcLineChksum and 0xA and 0xD (ascii: newline)
                    DstPosFinalChksum = DstPosFinalChksum + (Cfg.ChksumAddress - pBufferFilePtrDstAdd) * 2  # calc the destination of final SrcLineChksum position
                    DstPosFinalChksumErr = True  # position for final data-checksum is found, no error

                    # copy the destination line of the final data checksum (the line checksum has to recalculate!)
                    SrcLineDataLengthCpy = SrcLineDataLength;
                    SrcLineAddressCpy.append(SrcLineAddress[0])
                    SrcLineAddressCpy.append(SrcLineAddress[1])
                    SrcLineRecordTypeCpy = SrcLineRecordType
                    for i in range(np.int32(SrcLineDataLength)):
                        SrcLineDataCpy.append(SrcLineData[i])

                if Cfg.FlashHeaderAdress == pBufferFilePtrDstAdd:
                    FlashHeaderHexStart = pSrcFile.tell() - 4 - np.int32(SrcLineDataLength) * 2


        if DstPosFinalChksumErr == False:
            print('**************************** E R R O R *********************************')
            print('Error! Position for final checksum not found.')
        else:
            pBufferFile.seek(Cfg.BeginAddress)  # get the start address for the calculating SrcLineChksum
            SrcFile4BytBuff = struct.unpack('I', pBufferFile.read(4))[0]  # read 4 bytes
            ChksumStartAddress = SrcFile4BytBuff & 0x00ffffff # convert to long and mask the highest byte

            pBufferFile.seek(Cfg.EndAddress) # get the end address for the calculating SrcLineChksum
            SrcFile4BytBuff = struct.unpack('I', pBufferFile.read(4))[0]  # read 4 bytes
            ChksumEndAddress = (SrcFile4BytBuff - 4) & 0x00ffffff  # SrcFile4BytBuff & 0x00ffffff         # convert to long and mask the highest byte
            ChksumEndAddress = (SrcFile4BytBuff) & 0x00ffffff
            pBufferFile.seek(ChksumStartAddress)

            ChksumCalcLength = ChksumEndAddress - ChksumStartAddress + 1

            # calculate Modulo-32-Bit-Addition
            if Cfg.ChecksumAlgo == 'modulo32bitaddition':
                ChksumCalcLength /= 4  # calc the count for 4 byte steps
                while ChksumCalcLength > 0:
                    SrcFile4BytBuff = struct.unpack('I', pBufferFile.read(4))[0]  # read 4 bytes
                    FinalDataChksum += SrcFile4BytBuff  # calculate the addition-checksum
                    FinalDataChksum &= 0xFFFFFFFF  # mask 32bit to prevent python from using auto-types
                    ChksumCalcLength -= 1

            if Cfg.ChecksumAlgo == 'crc32':  # calc crc32, using python binascii module, use mask to ensure compatible output
                FinalDataChksum = (binascii.crc32(pBufferFile.read(ChksumCalcLength), 0) & 0xFFFFFFFF)

            FinalDataChksumWritten = FinalDataChksum


            pSrcFile.seek(DstPosFinalChksum)
            pSrcFile.write('%08X' % FinalDataChksumWritten)  # write chcksum

            SrcLineAddressCpyTemp = 0
            SrcLineAddressCpyTemp &= 0x0000FFFF
            SrcLineAddressCpyTemp |= np.uint8(SrcLineAddressCpy[0]) << 8
            SrcLineAddressCpyTemp |= np.uint8(SrcLineAddressCpy[1])

            # re-calc new line checksum for changed line and overwrite the new data checksum
            idx = (Cfg.ChksumAddress & 0x0000ffff) - np.uint16(SrcLineAddressCpyTemp)
            SrcLineDataCpy[0 + idx] = np.int8((FinalDataChksum & 0x000000ff) >> 0)
            SrcLineDataCpy[1 + idx] = np.int8((FinalDataChksum & 0x0000ff00) >> 8)
            SrcLineDataCpy[2 + idx] = np.int8((FinalDataChksum & 0x00ff0000) >> 16)
            SrcLineDataCpy[3 + idx] = np.int8((FinalDataChksum & 0xff000000) >> 24)

            for i in range(np.int32(SrcLineDataLengthCpy)):
                SrcLineChksumNewCalc -= SrcLineDataCpy[i]

            #  calc new line chcksum
            SrcLineChksumNewCalc -= (int(SrcLineDataLengthCpy) + int(SrcLineAddressCpy[0]) + int(SrcLineAddressCpy[1]) + int(SrcLineRecordTypeCpy))

            pSrcFile.seek(DstPosNewLineChksum)
            pSrcFile.write('%02X' % np.uint8(SrcLineChksumNewCalc))  # write new line chcksum
            o = '32-bit SW-Chksum:     0x%08X' % (FinalDataChksumWritten)
            print sprint(o)
            print stars

            pBufferFile.seek(Cfg.FlashHeaderAdress + 24)
    print '* Checksum tool successful                                             *'
    print stars
