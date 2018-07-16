#ifndef BSORT_HEADER_H
#define BSORT_HEADER_H

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define SWITCH_TO_SHELL 20

namespace bsort {

struct sort {
  int fd;
  off_t size;
  void *buffer;
};

static inline void
shellsort(unsigned char *a,
          const int n,
          const int record_size,
          const int key_size);

int compare(int *length, unsigned char *a, unsigned char *b);

void
radixify(unsigned char *buffer,
         const long count,
         const long digit,
         const long char_start,
         const long char_stop,
         const long record_size,
         const long key_size,
         const long stack_size,
         const long cut_off);

int open_sort(char *path, struct sort *sort);

void close_sort(struct sort *sort);

}

#endif
