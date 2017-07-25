#!/usr/bin/python2.7
# -*- python -*-
#
import os, sys, imp, json, io

configuration = "../RepeatManagerKeyAction.h"
protoconfig = "../PB/RepeatManagerKeyAction.proto"

string = '/**************************************************************************\r\n'
string += '* This file is auto generated, DO NOT EDIT!\r\n'
string += '* To generate this file execute creat_keyaction_header.sh file\r\n'
string += '**************************************************************************/\r\n\r\n'
string += '#pragma once\r\n#include "KeyEventAction.pb.h"\r\n\r\n'
string += 'static const std::map<std::string, int> keyEventActionMap = {\r\n'

protostring = '/**************************************************************************\r\n'
protostring += '* ABSOLUTELY UNDER NO CONDITION SHOULD YOU INCLUDE THIS FILE DIRECTLY\r\n'
protostring += '* INSTEAD; YOU SHOULD INCLUDE KeyEventAction.pb.h, WHICH IS ITS\r\n*\r\n'
protostring += '* This file is auto generated, DO NOT EDIT!\r\n'
protostring += '**************************************************************************/\r\n\r\n'
protostring += 'enum KEY_EVENT_ACTION\r\n{\r\n    INVALID_BUTTON_EVENT_ACTION = -1;'

index = 0
lenght = 0

with open('KeyConfiguration.json') as data_file:
    data = json.load(data_file)


for item in data["keyTable"]:
    lenght = len(item["EventAction"])
    string += '            {"' + item["EventAction"] + '",' + (75-lenght)*' ' + item["EventAction"] + '},\r\n'
    protostring += '\r\n    ' + item["EventAction"] + ' = ' + str(index) + ';'
    index += 1

string += '};\r\n'
protostring += '\r\n};\r\n'

# Creating proto file with enum values after reading from json config file
with io.FileIO(protoconfig, "w") as file:
    file.write(protostring)

# Command to execute protoc for proto file to generate pb.cc and pb.h file
cmd = './../../builds/SoundTouch-SDK-qc8017_32/prebuilt/Release/native/bin/protoc --proto_path=./../PB/ --cpp_out=../ ./../PB/RepeatManagerKeyAction.proto'
os.system(cmd)

# Creating Header file to map the string and the enum value for EventAction of Json file
with io.FileIO(configuration, "w") as file:
    file.write(string)
