import sys
import time

_, size = sys.argv

data = []
count = 0
size = int(size)
chunk = sys.stdin.read(size)
while chunk:
    count += 1
    data.append(chunk)
    chunk = sys.stdin.read(size)
print "loaded", len(data), "records"
t = time.time()
data.sort()
print count, size, "bytes records sorted in ", time.time() - t, "seconds"
    
