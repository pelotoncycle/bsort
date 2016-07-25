import sys
import random

_, size, seed = sys.argv

size = int(size)
seed = int(seed) 

characters = map(chr, range(256))
random.seed(seed)

write = sys.stdout.write
for _ in xrange(size):
    write(random.choice(characters))
