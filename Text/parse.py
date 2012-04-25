"""
i = 2

f = open("data", "r")
w = None

for l in f:
    if l[0] == "#":
        if w:
            w.close()
        i += 1
        w = open("points/%s" % l[1:-3].strip(), "w")
        continue

    w.write(l)
"""

import os

d = os.listdir("points")

print "plot ",
for f in d:
    print '    "points/%s" using 1:2:3 t "%s" with yerrorbars, \\' % (f,f)
    print '    "points/%s" using 1:2 t "%s" smooth bezier, \\' % (f,f)
