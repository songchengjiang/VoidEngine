#!/usr/bin/python

import sys
import json
import os, os.path
import shutil

def replace_string(filepath, src_string, dst_string):
    """ From file's content replace specified string
        Arg:
        filepath: Specify a file contains the path
        src_string: old string
        dst_string: new string
        """
    if src_string is None or dst_string is None:
        raise TypeError
    
    content = ""
    f1 = open(filepath, "rb")
    for line in f1:
        strline = line.decode('utf8')
        if src_string in strline:
            content += strline.replace(src_string, dst_string)
        else:
            content += strline
    f1.close()
    f2 = open(filepath, "wb")
    f2.write(content.encode('utf8'))
    f2.close()

# -------------- main --------------
if __name__ == '__main__':
    qt_root = os.environ.get('QT_ROOT')
    replace_string('project.pbxproj', '$(VE_QT_ROOT)', qt_root)
