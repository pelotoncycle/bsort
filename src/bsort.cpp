#include "bsort.hpp"

namespace bsort {


static inline void
shellsort(unsigned char *a,
          const int n,
          const int record_size,
          const int key_size) {
  int i, j;
  char temp[record_size];

  for (i=3; i < n; i++) {
    memcpy(&temp, &a[i * record_size], record_size);
    for(j=i; j>=3 && memcmp(a+(j-3)*record_size, &temp, key_size) >0; j -= 3) {
      memcpy(a+j*record_size, a+(j-3)*record_size, record_size);
    }
    memcpy(a+j*record_size, &temp, record_size);
  }

  for (i=1; i < n; i++) {
    memcpy(&temp, &a[i*record_size], record_size);
    for(j=i; j>=1 && memcmp(a+(j-1)*record_size, &temp, key_size) >0; j -= 1) {
      memcpy(a+j*record_size, a+(j-1)*record_size, record_size);
    }
    memcpy(a+j*record_size, &temp, record_size);
  }
}


int compare(int *length, unsigned char *a, unsigned char *b) {
  return memcmp(a, b, *length);
}


void
radixify(unsigned char *buffer,
         const long count,
         const long digit,
         const long char_start,
         const long char_stop,
         const long record_size,
         const long key_size,
         const long stack_size,
         const long cut_off,
         const long switch_to_shell) {
  long counts[char_stop+1];
  long offsets[char_stop+1];
  long starts[char_stop+1];
  long ends[char_stop+1];
  long offset=0;
  unsigned char temp[record_size];
  long target, x, a, b;
  long stack[stack_size];
  long stack_pointer;
  long last_position, last_value, next_value;

  for (x=char_start; x<=char_stop; x++) {
    counts[x] = 0;
    offsets[x] = 0;
  }

  // Compute starting positions
  for (x=0; x<count; x++) {
    long c = buffer[x*record_size + digit];
    counts[c] += 1;
  }

  // Compute offsets
  offset = 0;
  for(x=char_start; x<=char_stop; x++) {
    offsets[x] = offset;
    starts[x] = offsets[x];
    offset += counts[x];
  }

  for(x=char_start; x<char_stop; x++) {
    ends[x] = offsets[x+1];
  }
  ends[char_stop] = count;

  for(x=char_start; x<=char_stop; x++) {
    while (offsets[x] < ends[x]) {

      if (buffer[offsets[x] * record_size + digit] == x) {
        offsets[x] += 1;
      } else {
        stack_pointer=0;
        stack[stack_pointer] = offsets[x];
        stack_pointer += 1;
        target = buffer[offsets[x] * record_size + digit];
        while( target != x && stack_pointer < stack_size ) {
          stack[stack_pointer] = offsets[target];
          offsets[target] += 1;
          target = buffer[stack[stack_pointer] * record_size + digit];
          stack_pointer++;
        };
        if (stack_pointer != stack_size) {
          offsets[x] += 1;
        }
        stack_pointer--;
        memcpy(&temp, &buffer[stack[stack_pointer] * record_size], record_size);
        while (stack_pointer) {
          memcpy(&buffer[stack[stack_pointer] * record_size], &buffer[stack[stack_pointer-1] * record_size], record_size);
          stack_pointer--;
        }
        memcpy(&buffer[stack[0] * record_size], &temp, record_size);
      }
    }
  }

#pragma omp parallel
#pragma omp single nowait
  {
    if (digit < cut_off) {
      for(x=char_start; x<=char_stop; x++) {
        if ( ends[x] - starts[x] > switch_to_shell) {
#pragma omp task shared(buffer)
          radixify(&buffer[starts[x] * record_size],
                   ends[x] - starts[x],
                   digit+1,
                   char_start,
                   char_stop,
                   record_size,
                   key_size,
                   stack_size,
                   cut_off,
                   switch_to_shell);
        } else {
          if (ends[x] - starts[x] <= 1) continue;
#pragma omp task shared(buffer)
          shellsort(&buffer[starts[x] * record_size], ends[x] - starts[x], record_size, key_size);
        }
      }
    } else {
      for(x=char_start; x<=char_stop; x++) {
        if (ends[x] - starts[x] > 1) {
#pragma omp task shared(buffer)
          shellsort(&buffer[starts[x] * record_size], ends[x] - starts[x], record_size, key_size);
        }
      }
    }
  }
}


int open_sort(char *path, struct sort *sort) {
  void *buffer = NULL;

  int fd = open(path, O_RDWR);
  if (fd == -1)
    goto error;

  struct stat stats;
  if (-1 == fstat(fd, &stats))
    goto error;
  if (!(buffer = mmap(NULL,
                      stats.st_size,
                      PROT_READ | PROT_WRITE,
                      MAP_SHARED,
                      fd,
                      0
                     )))
    goto error;


  madvise(buffer, stats.st_size, POSIX_MADV_WILLNEED | POSIX_MADV_SEQUENTIAL);

  sort->buffer = buffer;
  sort->size = stats.st_size;
  sort->fd = fd;
  return 0;

error:
  perror(path);
  if (buffer)
    munmap(buffer, stats.st_size);
  if (fd != -1)
    close(fd);
  sort->buffer = 0;
  sort->fd = 0;
  return -1;
}


void close_sort(struct sort *sort) {
  if (sort->buffer) {
    munmap(sort->buffer, sort->size);
    sort->buffer = 0;
    sort->size = 0;
  }

  if (sort->fd) {
    close(sort->fd);
    sort->fd = 0;
  }
}

}
