
from os import listdir
from os.path import isfile, join
import sys
onlyfiles = [ f for f in listdir(".") if isfile(join(".",f)) ]
for f in onlyfiles:
    string = open(f,"r")
    if sys.argv[1] in string.read():
        print f

