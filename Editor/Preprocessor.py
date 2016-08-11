#!/usr/bin/python

import sys
import json
import os, os.path
import shutil

qt_root = os.environ.get('QT_ROOT')

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

# -------------- main --------------
if __name__ == '__main__':
    generated_files_dir = 'GeneratedFiles'
    if os.path.exists(generated_files_dir):
        shutil.rmtree(generated_files_dir)
    os.mkdir(generated_files_dir)

    GenerateFiles('.', generated_files_dir)
