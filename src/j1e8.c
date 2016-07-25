#include <stdlib.h>
#include  <stdio.h>
#include <sys/mman.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

static inline void shellsort(char (*a)[100], int n) {
  int i, j;
  char temp[100];

  for (i=3; i < n; i++) {
    memcpy(&temp, &a[i], 100);
    for(j=i; j>=3 && memcmp(a+j-3, &temp, 10) >0; j -= 3) {
      memcpy(a+j, a+j-3, 100);
    }
    memcpy(a+j, &temp, 100);
  }

  for (i=1; i < n; i++) {
    memcpy(&temp, &a[i], 100);
    for(j=i; j>=1 && memcmp(a+j-1, &temp, 10) >0; j -= 1) {
      memcpy(a+j, a+j-1, 100);
    }
    memcpy(a+j, &temp, 100);
  }

}

#define SWITCH_TO_SHELL 10
#define STACK_SIZE 12
#define CHAR_START 32
#define CHAR_STOP 128

void radixify(char (*buffer)[100], const int count, const int digit) {
  int counts[CHAR_STOP];
  int offsets[CHAR_STOP];
  int starts[CHAR_STOP];
  int ends[CHAR_STOP];
  int offset=0;
  char temp[100];
  int target, x, a, b;
  int stack[STACK_SIZE];
  int stack_pointer;
  int last_position, last_value, next_value;
  for (x=CHAR_START; x<CHAR_STOP; x++) {
    counts[x] = 0;
    offsets[x] = 0;
  }
  // Compute starting positions
  for (x=0; x<count; x++) {
    counts[buffer[x][digit]] += 1;
  }

  // Compute offsets
  offset = 0;
  for(x=CHAR_START; x<CHAR_STOP; x++) {
    offsets[x] = offset;
    starts[x] = offsets[x];
    offset += counts[x];
  }

  for(x=CHAR_START; x<CHAR_STOP; x++) {
    ends[x] = offsets[x+1];
  }
  ends[CHAR_STOP-1] = count;

  for(x=CHAR_START; x<CHAR_STOP; x++) {
    while (offsets[x] < ends[x]) {

      if (buffer[offsets[x]][digit] == x) {
        offsets[x] += 1;
      } else {
        int p=0;
        stack_pointer=0;
        stack[stack_pointer] = offsets[x];
        stack_pointer += 1;
        target = buffer[offsets[x]][digit];
        while( target != x && stack_pointer < STACK_SIZE ) {
          stack[stack_pointer] = offsets[target];
          offsets[target] += 1;
          target = buffer[stack[stack_pointer]][digit];
          stack_pointer++;
        };
        if (stack_pointer != STACK_SIZE) {
          offsets[x] += 1;
        }
        stack_pointer--;
        memcpy(&temp, &buffer[stack[stack_pointer]], 100);
        while (stack_pointer) {
          memcpy(&buffer[stack[stack_pointer]], &buffer[stack[stack_pointer-1]], 100);
          stack_pointer--;
        }
        memcpy(&buffer[stack[0]], &temp, 100);
      }
    }
  }

  if (digit < 4) {
    for(x=CHAR_START; x<CHAR_STOP; x++) {
      if ( ends[x] - starts[x] > SWITCH_TO_SHELL) {
        radixify(&buffer[starts[x]], ends[x] - starts[x], digit+1);
      } else {
        if (ends[x] - starts[x] <= 1) continue;
        shellsort(&buffer[starts[x]], ends[x] - starts[x]);
      }
    }
  } else {
    for(x=CHAR_START; x<CHAR_STOP; x++)
      if (ends[x] - starts[x] > 1) {
        shellsort(&buffer[starts[x]], ends[x] - starts[x]);
      }
  }
}


int main(int argc, char *argv[]) {
  int f = open("jouleinput", O_RDWR);
  char (*buffer)[100] = mmap(NULL, 10000000000, PROT_READ|PROT_WRITE, MAP_SHARED, f, 0);
  madvise(buffer, 10000000000, POSIX_MADV_WILLNEED | POSIX_MADV_SEQUENTIAL);
  radixify(buffer, 100000000, 0);
  sync();
  return 0;
}





