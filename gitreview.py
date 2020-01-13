#!/usr/bin/python
"""
for icode code-review
just type `./gitreview.py` in shell
"""

import os
import sys

if __name__ == "__main__":
    pipe_out = os.popen("git status | grep 'On branch' | awk '{print $3}'")
    branchName = pipe_out.readline().strip()
    print "Current branch: " + branchName

    if len(sys.argv) == 1:
        os.system("git push origin %s:refs/for/%s" % (branchName, branchName))
    else:
        os.system("git push origin %s:refs/for/%s" % (branchName, sys.argv[1]))
