import os
import os.path
from xml.etree import ElementTree as ET
from xml.etree.ElementTree import SubElement
from xml.etree.ElementTree import Element
from xml.etree.ElementTree import ElementTree
import os
import os.path
import zipfile
import re
import subprocess
import platform
from xml.dom import minidom
import codecs
import sys
from optparse import OptionParser
curDir = os.getcwd()

androidNS = 'http://schemas.android.com/apk/res/android'

def getCurrDir():
    global curDir
    retPath = curDir
    if platform.system() == 'Darwin':
        retPath = sys.path[0]
        lstPath = os.path.split(retPath)
        if lstPath[1]:
            retPath = lstPath[0]

    return retPath

def getFullPath(filename):
    if os.path.isabs(filename):
        return filename
    currdir = getCurrDir()
    filename = os.path.join(currdir, filename)
    filename = filename.replace('\\', '/')
    filename = re.sub('/+', '/', filename)
    return filename

if __name__ == '__main__':
    parser = OptionParser()
    parser.add_option("-s", "--sdk", dest="sdk_chId",help='the build sdk mode for java project')
    (opts, args) = parser.parse_args()

    print opts.sdk_chId
    MainChId = opts.sdk_chId

    manifestFile = getFullPath('.') + "/AndroidManifest.xml"
    ET.register_namespace('android', androidNS)
    key = '{' + androidNS + '}name'
    val = '{' + androidNS + '}value'

    tree = ET.parse(manifestFile)
    root = tree.getroot()
    appNode = root.find('application')
    metaDataList = appNode.findall('meta-data')

    if metaDataList != None:
        for metaDataNode in metaDataList:
            keyName = metaDataNode.attrib[key]
            if keyName == 'MainChId':
                metaDataNode.set(val, MainChId)
                break

    tree.write(manifestFile, 'UTF-8')
