import sys

_, size = sys.stdin.argv

size = int(size)

read = sys.stdin.read

last = read(size)
c = 1
while last:
    current = read(size)
    if last > current:
        print "Not sorted row #: ", c
        sys.exit(1)
    last = current
