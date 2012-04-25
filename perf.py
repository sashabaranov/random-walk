from commands import getstatusoutput
from math import sqrt


for N in range(40, 100, 10):
    print "# %dx%dx%d :" % (N, N, N)
    
    p = 1

    for cops in range(1, N**3):
        cmd = './random-walk -d 3 -N %d -c %d -K %d -M 100000' % (N, cops, 50 * sqrt(cops))
        out = getstatusoutput(cmd)[1]

        if "Abort" in out:
            continue
        else:
            print out

            v = out.split(' ')
            if float(v[1]) < 0.01:
                break
