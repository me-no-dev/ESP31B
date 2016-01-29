#!/usr/bin/python
#
# File	: gen_appbin.py
# This file is part of Espressif's generate bin script.
# Copyright (C) 2013 - 2016, Espressif Systems
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of version 3 of the GNU General Public License as
# published by the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program.  If not, see <http://www.gnu.org/licenses/>.

"""This file is part of Espressif's generate bin script.
    argv[1] is elf file name
    argv[2] is version num"""

import string
import sys
import os
import re
import binascii
import struct
import zlib


TEXT_ADDRESS = 0x40040000
# app_entry = 0
# data_address = 0x3ffb0000
# data_end  = 0x40000000
# text_end  = 0x40120000

CHECKSUM_INIT = 0xEF

chk_sum = CHECKSUM_INIT
blocks = 0
flash_blocks = 0

def write_file(file_name,data):
    if file_name is None:
        print 'file_name cannot be none\n'
        sys.exit(0)

    fp = open(file_name,'ab')

    if fp:
        fp.seek(0,os.SEEK_END)
        fp.write(data)
        fp.close()
    else:
        print '%s write fail\n'%(file_name)

def combine_bin(file_name,dest_file_name,start_offset_addr,need_chk):
    global chk_sum
    
    if dest_file_name is None:
        print 'dest_file_name cannot be none\n'
        sys.exit(0)

    if file_name:        
        fp = open(file_name,'rb')
        if fp:
            ########## write text ##########
            fp.seek(0,os.SEEK_END)
            data_len = fp.tell()
            if data_len:
                if need_chk:
                    tmp_len = (data_len + 3) & (~3)
                else:
                    tmp_len = (data_len + 15) & (~15)
                data_bin = struct.pack('<II',start_offset_addr,tmp_len)
                write_file(dest_file_name,data_bin)
                fp.seek(0,os.SEEK_SET)
                data_bin = fp.read(data_len)
                write_file(dest_file_name,data_bin)
                if need_chk:
                    for loop in range(len(data_bin)):
                        chk_sum ^= ord(data_bin[loop])
                    # print '%s size is %d(0x%x),align 4 bytes,\nultimate size is %d(0x%x)'%(file_name,data_len,data_len,tmp_len,tmp_len)
                tmp_len = tmp_len - data_len
                if tmp_len:
                    data_str = ['00']*(tmp_len)
                    data_bin = binascii.a2b_hex(''.join(data_str))
                    write_file(dest_file_name,data_bin)
                    if need_chk:
                        for loop in range(len(data_bin)):
                            chk_sum ^= ord(data_bin[loop])
                
            fp.close()
        else:
            print '!!!Open %s fail!!!'%(file_name)


def getFileCRC(_path): 
    try: 
        blocksize = 1024 * 64 
        f = open(_path,"rb") 
        str = f.read(blocksize) 
        crc = 0 
        while(len(str) != 0): 
            crc = binascii.crc32(str, crc) 
            str = f.read(blocksize) 
        f.close() 
    except: 
        print 'get file crc error!' 
        return 0 
    return crc

def gen_appbin():
    global chk_sum
    global crc_sum
    global blocks 
    global irom0_addr
    global irom1_addr

    if len(sys.argv) != 5:
        print 'Usage: gen_appbin.py app.elf eagle.ld build_path toolchain_path'
        sys.exit(0)

    elf_file = sys.argv[1]
    
    ld_fd = open(sys.argv[2])
    ld_data = ld_fd.read()
    
    build_path = sys.argv[3]
    toolchain_path = sys.argv[4]
    
    match = re.search("irom0_0_seg.+?org = (0x[0-9a-fA-F]+),",ld_data)
    if match is not None:
        irom0_addr = match.group(1)
        #irom0_addr = int(addr,base=16)
        #print irom0_addr
    match = re.search("irom0_0_seg.+?len = (0x[0-9a-fA-F]+)",ld_data)
    if match is not None:
        irom0_len = match.group(1)
        irom0_len = int(irom0_len,base=16)
        #print irom0_len
    match = re.search("irom0_1_seg.+?org = (0x[0-9a-fA-F]+),",ld_data)
    if match is not None:
        irom1_addr = match.group(1)
        #irom1_addr = int(addr,base=16)
        #print irom1_addr
    match = re.search("irom0_1_seg.+?len = (0x[0-9a-fA-F]+)",ld_data)
    if match is not None:
        irom1_len = match.group(1)
        irom1_len = int(irom1_len,base=16)
        #print irom1_len
    flash_data_line  = 16
    data_line_bits = 0xf

    flash_bin_name = elf_file + '.ota.bin'
    Dcache_bin_name = elf_file + '.0x04000.bin'
    Icache_flash_bin_name = elf_file + '.0x40000.bin'
    text_bin_name = build_path + '/eagle.app.v7.text.bin'
    data_bin_name = build_path + '/eagle.app.v7.data.bin'
    rodata_bin_name = build_path + '/eagle.app.v7.rodata.bin'
    irom0text_bin_name = build_path + '/eagle.app.v7.irom0text.bin'
    irom1text_bin_name = build_path + '/eagle.app.v7.irom1text.bin'
    
    os.system('rm -rf "' + flash_bin_name + '"')
    os.system('rm -rf "' + Dcache_bin_name + '"')
    os.system('rm -rf "' + Icache_flash_bin_name + '"')
    os.system('rm -rf "' + text_bin_name + '"')
    os.system('rm -rf "' + data_bin_name + '"')
    os.system('rm -rf "' + rodata_bin_name + '"')
    os.system('rm -rf "' + irom0text_bin_name + '"')
    os.system('rm -rf "' + irom1text_bin_name + '"')
    
    os.system('"' + toolchain_path + '/xtensa-esp108-elf-objcopy" --only-section .text -O binary ' + elf_file + ' ' + text_bin_name)
    os.system('"' + toolchain_path + '/xtensa-esp108-elf-objcopy" --only-section .data -O binary ' + elf_file + ' ' + data_bin_name)
    os.system('"' + toolchain_path + '/xtensa-esp108-elf-objcopy" --only-section .rodata -O binary ' + elf_file + ' ' + rodata_bin_name)
    os.system('"' + toolchain_path + '/xtensa-esp108-elf-objcopy" --only-section .irom0.text -O binary ' + elf_file + ' ' + irom0text_bin_name)
    os.system('"' + toolchain_path + '/xtensa-esp108-elf-objcopy" --only-section .irom1.text -O binary ' + elf_file + ' ' + irom1text_bin_name)
    

    BIN_MAGIC_FLASH  = 0xE9
    BIN_MAGIC_IROM0  = 0xEA
    BIN_MAGIC_IROM1  = 0xEC
    data_str = ''
    sum_size = 0

    os.system('"' + toolchain_path + '/xtensa-esp108-elf-nm" -g ' + elf_file + ' > ' + elf_file + '.sym')

    fp = file(elf_file + '.sym')
    if fp is None:
        print "open sym file error\n"
        sys.exit(0)

    lines = fp.readlines()
    fp.close()

    entry_addr = None
    p = re.compile('(\w*)(\sT\s)(call_user_start)$')
    for line in lines:
        m = p.search(line)
        if m != None:
            entry_addr = m.group(1)
            #print 'entry point: %s' % entry_addr
                        

    if entry_addr is None:
        print 'no entry point!!'
        sys.exit(0)

    data_start_addr = '0'
    p = re.compile('(\w*)(\sA\s)(_data_start)$')
    for line in lines:
        m = p.search(line)
        if m != None:
            data_start_addr = m.group(1)
            #print 'data start: %s' % data_start_addr
                        

    rodata_start_addr = '0'
    p = re.compile('(\w*)(\sA\s)(_rodata_start)$')
    for line in lines:
        m = p.search(line)
        if m != None:
            rodata_start_addr = m.group(1)
            #print 'rodata start: %s' % rodata_start_addr
                        
    if os.path.getsize(text_bin_name):
        blocks = blocks + 1
    if os.path.getsize(data_bin_name):
        blocks = blocks + 1
    if os.path.getsize(rodata_bin_name):
        blocks = blocks + 1
            
    fp1 = open(irom1text_bin_name,'rb')
    fp1.seek(0,os.SEEK_END)
    Dcache_data_len = fp1.tell()
    # print data_len
    if Dcache_data_len :       
        Dcache_data_len = struct.pack('<BBBBI',BIN_MAGIC_IROM1,1,0,0,long(irom1_addr,16))
        sum_size = len(Dcache_data_len)
        write_file(flash_bin_name,Dcache_data_len)
        #write_file(Dcache_bin_name,data_bin)
        # irom1.text.bin
        combine_bin(irom1text_bin_name,flash_bin_name,irom1_len,0)
        open(Dcache_bin_name, "wb").write(open(flash_bin_name, "rb").read())
        
    # write irom0 bin head
    fp2 = open(irom0text_bin_name,'rb')
    fp2.seek(0,os.SEEK_END)
    data_len = fp2.tell()
    # print data_len
    if data_len :  
        data_bin = struct.pack('<BBBBI',BIN_MAGIC_IROM0,blocks + 1,0,0,long(irom0_addr,16)) 
        sum_size = len(data_bin)
        write_file(flash_bin_name,data_bin)
        #write_file(Icache_flash_bin_name,data_bin)
        # irom0.text.bin
        icache_flash_size = data_len +  os.path.getsize(text_bin_name) + os.path.getsize(data_bin_name) + os.path.getsize(rodata_bin_name) + 36
        combine_bin(irom0text_bin_name,flash_bin_name,icache_flash_size,0)
        #combine_bin(irom0text_bin_name,Icache_flash_bin_name,0x0,0)     

    # text.bin
    
    
    data_bin = struct.pack('<BBBBI',BIN_MAGIC_FLASH,blocks,0,0,long(entry_addr,16))
    sum_size = len(data_bin)
    write_file(flash_bin_name,data_bin)
    #write_file(Icache_flash_bin_name,data_bin)
    
    combine_bin(text_bin_name,flash_bin_name,TEXT_ADDRESS,1)
    #combine_bin(text_bin_name,Icache_flash_bin_name,TEXT_ADDRESS,0)

    # data.bin
    if data_start_addr:
        combine_bin(data_bin_name,flash_bin_name,long(data_start_addr,16),1)
        #combine_bin(data_bin_name,Icache_flash_bin_name,long(data_start_addr,16),0)

    # rodata.bin
    combine_bin(rodata_bin_name,flash_bin_name,long(rodata_start_addr,16),1)
    #combine_bin(rodata_bin_name,Icache_flash_bin_name,long(rodata_start_addr,16),0)

    # write checksum header
    sum_size = os.path.getsize(flash_bin_name) + 1
    sum_size = flash_data_line - (data_line_bits&sum_size)
    if sum_size:
        data_str = ['00']*(sum_size)
        data_bin = binascii.a2b_hex(''.join(data_str))
        write_file(flash_bin_name,data_bin)
        #write_file(Icache_flash_bin_name,data_bin)
    write_file(flash_bin_name,chr(chk_sum & 0xFF)) 
    #write_file(Icache_flash_bin_name,chr(chk_sum & 0xFF))   	  	
  
    all_bin_crc = getFileCRC(flash_bin_name)
    # print all_bin_crc
    if all_bin_crc < 0:
        all_bin_crc = abs(all_bin_crc) - 1
    else :
        all_bin_crc = abs(all_bin_crc) + 1
    # print all_bin_crc
    write_file(flash_bin_name,chr((all_bin_crc & 0x000000FF))+chr((all_bin_crc & 0x0000FF00) >> 8)+chr((all_bin_crc & 0x00FF0000) >> 16)+chr((all_bin_crc & 0xFF000000) >> 24))
    icache_file = open(flash_bin_name, "rb")
    if int(os.path.getsize(irom1text_bin_name)):
        Dcache_bin_len = (int(os.path.getsize(irom1text_bin_name)) + 16 +15 ) &(~15)
        #print Dcache_bin_len
        icache_file.seek(Dcache_bin_len ,0)
    open(Icache_flash_bin_name, "wb").write(icache_file.read())

if __name__=='__main__':
    gen_appbin()
