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

int verbosity;


struct sort {
  int fd;
  off_t size;
  void *buffer;
};


int compare(int *length, unsigned char *a, unsigned char *b) {
  return memcmp(a, b, *length);
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
main(int argc, char *argv[]) {
  int opt;
  int record_size=100;
  int key_size=10;
  verbosity = 0;
  while ((opt = getopt(argc, argv, "vr:k:")) != -1) {
    switch (opt) {
    case 'v':
      verbosity += 1;
      break;
    case 'r':
      record_size = atoi(optarg);
      break;
    case 'k':
      key_size = atoi(optarg);
      break;
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
    if (verbosity)
      printf("sorting %s\n", argv[optind]);
    struct sort sort;
    if (-1==open_sort(argv[optind], &sort))
      goto failure;


    qsort_r(sort.buffer,
            sort.size / record_size,
            record_size,
            &compare,
            &record_size);

    close_sort(&sort);
    optind++;
  }

  exit(0);
failure:
  fprintf(stderr,
          "Usage: %s [-v] [-a] [-r ###] [-k ###] [-s ###] file1, file2 ... \n",
          argv[0]);
  fprintf(stderr,
          "Individually sort binary files with libc's qsort\n"
          "\n"
          "Sorting Options:\n"
          "\n"
          "  -k ###   size of compariable section of record, in bytes (default 100)\n"
          "  -r ###   size of overall record, in bytes.  (default 100)\n"
          "\n"
          "Options:\n"
          "  -v  verbose output logging\n"
          "\n"
          "Report bsort bugs to adam@pelotoncycle.com\n"
         );
  exit(1);
}




