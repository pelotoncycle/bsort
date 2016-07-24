#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>


static inline void shellsort(char (**a), int n, int record_size, int key_size) {
    int i, j;
    char temp = alloca(record_size);

    for (i=3; i < n; i++) {
        memcpy(&temp, &a[i], record_size);
        for(j=i; j>=3 && memcmp(a+j-3, &temp, key_size) >0; j -= 3) {
            memcpy(a+j, a+j-3, record_size);
        }
        memcpy(a+j, &temp, record_size);
    }

    for (i=1; i < n; i++) {
        memcpy(&temp, &a[i], record_size);
        for(j=i; j>=1 && memcmp(a+j-1, &temp, key_size) >0; j -= 1) {
            memcpy(a+j, a+j-1, record_size);
        }
        memcpy(a+j, &temp, record_size);
    }

}

#define SWITCH_TO_SHELL 20


void radixify(char (**buffer), const int count, const int digit, int char_start, int char_stop, int record_size, int key_size, int stack_size) {
    int counts[char_stop];
    int offsets[char_stop];
    int starts[char_stop];
    int ends[char_stop];
    int offset=0;
    char *temp = alloca(record_size);
    int target, x, a, b;
    int *stack = alloca(sizeof(int) * stack_size);
    int stack_pointer;
    int last_position, last_value, next_value;
    for (x=char_start; x<char_stop; x++) {
        counts[x] = 0;
        offsets[x] = 0;
    }
    // Compute starting positions
    for (x=0; x<count; x++) {
        counts[buffer[x][digit]] += 1;
    }

    // Compute offsets
    offset = 0;
    for(x=char_start; x<char_stop; x++) {
        offsets[x] = offset;
        starts[x] = offsets[x];
        offset += counts[x];
    }

    for(x=char_start; x<char_stop; x++) {
        ends[x] = offsets[x+1];
    }
    ends[char_stop-1] = count;

    for(x=char_start; x<char_stop; x++) {
        while (offsets[x] < ends[x]) {

            if (buffer[offsets[x]][digit] == x) {
                offsets[x] += 1;
            } else {
                int p=0;
                stack_pointer=0;
                stack[stack_pointer] = offsets[x];
                stack_pointer += 1;
                target = buffer[offsets[x]][digit];
                while( target != x && stack_pointer < stack_size )
                {
                    stack[stack_pointer] = offsets[target];
                    offsets[target] += 1;
                    target = buffer[stack[stack_pointer]][digit];
                    stack_pointer++;
                };
                if (stack_pointer != stack_size) {
                    offsets[x] += 1;
                }
                stack_pointer--;
                memcpy(&temp, &buffer[stack[stack_pointer]], record_size);
                while (stack_pointer) {
                    memcpy(&buffer[stack[stack_pointer]], &buffer[stack[stack_pointer-1]], record_size);
                    stack_pointer--;
                }
                memcpy(&buffer[stack[0]], &temp, record_size);
            }
        }
    }

    if (digit < 4) {
        for(x=char_start; x<char_stop; x++) {
            if ( ends[x] - starts[x] > SWITCH_TO_SHELL) {
                radixify(&buffer[starts[x]], 
			 ends[x] - starts[x], 
			 digit+1,
			 char_start,
			 char_stop,
			 record_size,
			 key_size, 
			 stack_size);
            } else {
                if (ends[x] - starts[x] <= 1) continue;
                shellsort(&buffer[starts[x]], ends[x] - starts[x], record_size, key_size);
            }
        }
    } else {
        for(x=char_start; x<char_stop; x++)
            if (ends[x] - starts[x] > 1)
            {
	      shellsort(&buffer[starts[x]], ends[x] - starts[x], record_size, key_size);
            }
    }
}


int bsort(char *buffer,
	  int char_start,
	  int char_stop,
	  int key_size,
	  int record_size,
	  int stack_size,
	  int cut_off) {
  


}	  


struct sort {
  int fd;
  off_t size;
  void *buffer;
};



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
  

  if (-1==madvise(buffer, 
		  stats.st_size,
		  POSIX_MADV_WILLNEED | POSIX_MADV_SEQUENTIAL))

    goto error;
  
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


int
main(int argc, char *argv[])
{
  int flags, opt;
  int nsecs, tfnd;
  int char_start = 0;
  int char_stop = 255;
  int record_size=100;
  int key_size=10;
  int stack_size=12;
  int verbose = 0;
  int cut_off = 4;

  nsecs = 0;
  tfnd = 0;
  flags = 0;
  while ((opt = getopt(argc, argv, "var:k:s:c:")) != -1) {
    switch (opt) {
    case 'v':
      verbose += 1;
      break;
    case 'a':
      char_start = 32;
      char_stop = 128;
      break;
    case 'r':
      record_size = atoi(optarg);
      break;
    case 'k':
      key_size = atoi(optarg);
      break;
    case 's':
      stack_size = atoi(optarg);
      break;
    case 'c':
      cut_off = atoi(optarg);
    default:
      fprintf(stderr, "Invalid parameter: -%c\n", opt);
      goto failure;
    }
  }

  if (optind >= argc) {    
    fprintf(stderr, "Expected argument after options\n");
    goto failure;
  }

  while(optind < argc) {
    struct sort sort; 
    if (-1==open_sort(argv[optind], &sort))
      goto failure;
    radixify(sort.buffer, 
	     sort.size / char_start, 
	     0,
	     char_stop, 
	     key_size, 
	     record_size, 
	     stack_size, 
	     cut_off);
    close_sort(&sort);
    optind++;
  }

  exit(0);
 failure:
  fprintf(stderr, 
	  "Usage: %s [-v] [-a] [-r ###] [-k ###] [-s ###] file1, file2 ... \n",
	  argv[0]);
  exit(1);
}




