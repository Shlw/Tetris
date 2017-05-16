#!/usr/bin/env python3
import os
import sys
if len(sys.argv) != 3:
    print("usage: compare.py player0 player1")
    sys.exit(1)
p0, p1 = sys.argv[1], sys.argv[2]
cnt0 = 0
cnt1 = 0
cnt2 = 0
N = 100
for i in range(N):
    os.system("./logic %s %s cmplog.txt > cmp.txt" % (p0, p1))
    with open("cmp.txt", "r") as f:
        s = f.readlines()[-1]
        win = int(s[7:-1])
    print(win)
    if win == 0: cnt0+=1
    if win == 1: cnt1+=1
    if win == -1: cnt2+=1
print("%s : %s = %.2f : %.2f" % (p0, p1, (cnt0+0.5*cnt2)/N, (cnt1+0.5*cnt2)/N))
