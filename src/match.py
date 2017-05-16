#!/usr/bin/env python3
import os
import sys
if len(sys.argv) == 2:
    N = int(sys.argv[1])
else:
    N = 300
p0 = "./old"
p1 = "./current"
cnt0 = 0
cnt1 = 0
for i in range(N):
    os.system("./logic %s %s cmplog.txt > cmp.txt" % (p0, p1))
    with open("cmp.txt", "r") as f:
        s = f.readlines()[-1]
        win = int(s[7:-1])
    print(win)
    if win == 0: cnt0+=1
    if win == 1: cnt1+=1
print("%s : %s = %d : %d" % (p0, p1, cnt0, cnt1))
