#!/usr/bin/python

import sys
import json
import os, os.path
import shutil
import xml.etree.ElementTree as ET
from xml.dom import minidom

qt_root = os.environ.get('QT_ROOT')

def fomatTree(elem):
    root_str = ET.tostring(elem, 'utf-8')
    reparse = minidom.parseString(root_str)
    return reparse.toprettyxml(indent="    ")

def GenerateFiles(rootDir, generated_files_dir):
    for lists in os.listdir(rootDir):
        path = os.path.join(rootDir, lists)
        if os.path.isdir(path):
            if lists != generated_files_dir:
                GenerateFiles(path, generated_files_dir)
        else:
            split_file_name = os.path.splitext(lists)
            if split_file_name[1] == '.qrc':
                rcc_command = qt_root + '/bin/rcc -name %s %s/%s.qrc -o %s/qrc_%s.cpp' % (split_file_name[0], rootDir, split_file_name[0], generated_files_dir, split_file_name[0])
                os.system(rcc_command)
                print rcc_command

def GenerateQRC(qresource, rootDir, generated_files_dir):
    for lists in os.listdir(rootDir):
        path = os.path.join(rootDir, lists)
        if os.path.isdir(path):
            if lists != generated_files_dir:
                GenerateQRC(qresource, path, generated_files_dir)
        else:
            split_file_name = os.path.splitext(lists)
            if split_file_name[1] == '.qml':
                file = ET.Element('file')
                file.text = path
                qresource.append(file)


# -------------- main --------------
if __name__ == '__main__':
    generated_files_dir = 'GeneratedFiles'
    if os.path.exists(generated_files_dir):
        shutil.rmtree(generated_files_dir)
    os.mkdir(generated_files_dir)

    rcc = ET.Element('RCC')
    qresource = ET.Element('qresource')
    qresource.attrib['prefix'] = '/'
    rcc.append(qresource)
    GenerateQRC(qresource, '.', generated_files_dir)
    ET.ElementTree(ET.fromstring(fomatTree(rcc))).write('qml.qrc')

    GenerateFiles('.', generated_files_dir)
