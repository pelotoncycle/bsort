This repository is not maintained and will not be updated.

------

bsort
-----

A high performance radix sort for fixed width binary records.


Usage
=====

To sort a blob with fixed sized records of _X_ bytes, just do this:

```
    # bsort -k X -r X myfile.bin
```

If only the first _Y_ byte of each record are orderable, you can speed things up a little bit by telling bsort:

```
   # bsort -k Y -r x myfile.bin
```


Installation
============

This is an automake managed tool.  You can grab the tarball and do this:

```
# curl https://media.githubusercontent.com/media/pelotoncycle/bsort/master/downloads/bsort-0.0.1.tar.gz  | tar -xvzf
# cd bsort
# ./configure
# make
# make install
```

or you can grab a clone of this repo's git repository like this:

```
# git clone ....
# autoreconf --install
# ./configure
# make
# make install
```


Performance
===========

bsort can sort file with `n` byte keys in `n+1` passes; it easily
outperforms every other sort implemementation the author has
encountered.

Compared to the builtin qsort on an Ubuntu 14.10 i7, bsort is able to
sort 10,000,000 records in 1.7 seconds vs 4.2 for qsort running on an
"Intel(R) Core(TM) i7-6560U CPU @ 2.20GHz".


```
    $ time  ../src/bsort -k 16 -r 16 <160,000,000 random bytes>

    real    0m1.547s
    user    0m1.436s
    sys     0m0.104s

    $ time  ../src/qsort -k 16 -r 16 <160,000,000 random bytes>

    real    0m4.311s
    user    0m4.188s
    sys     0m0.120s

    $ pypy pybsort.py 16 < randomdata.txt
    loaded 10000000 records
    10000000 16 bytes records sorted in  16.8088638783 seconds
```

Unlike qsort, bsort's use of memory reasonally well preserves locality.   It
suitable for sorting binary datasets substantially larger than memory.

On a mid 2014 era macbook pro 15" with an I7 and 16Gb RAM runnin an
early prototype of bsort in `src/j18e.c` was ... well ... you read the memo


```
You've probably heard of http://sortbenchmark.org; one of their
competitions is "energy efficient sorts."  The current world record
for 10^8 100 byte records is 889 Joules.

A Macbook Pro running one core flat our consumes 16 watts, with the
screen turned on.  My custom msb inplace radix sort with a custom
gapped shell sort for cleanup runs in 21.7 second.  That's ~350
Joules.

There's still more work to be done.  I think its possible to beat 20
seconds on this hardware, and maybe find something more efficient to
run it on.  There's also the 10^9, 10^10 record efficiency
competitions.


$ gcc -O3 sort.c && ./gensort-1.5/gensort -a 100000000 jouleinput; echo Starting; time ./a.out; gensort-1.5/valsort jouleinput
Starting

real	0m21.646s
user	0m16.822s
sys	0m4.824s
Records: 100000000
Checksum: 2faf0ab746e89a8
Duplicate keys: 0
SUCCESS - all records are in order
```
