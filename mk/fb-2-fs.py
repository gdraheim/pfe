#! /usr/bin/env python
import sys

# convert a block (forth) script into a serial (forth) script
# a block script has a fixed linelength of 64 chars and no
# line end character.

for arg in sys.argv[1:]:
    if arg.startswith("--"):
        print "no options available, sorry"
    else:
        try:
            F = open (arg, "r")
        except IOError, x:
            print "could not open",arg,":",str(x)
        else:
            line = F.read(64)
            while len(line):
                sys.stdout.write(line)
                sys.stdout.write("\n")
                line = F.read(64)
            ##
            F.close()
        #
    #
##
