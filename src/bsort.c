#include "bsort.h"

int
main(int argc, char *argv[]) {
  int opt;
  int char_start = 0;
  int char_stop = 255;
  int record_size=100;
  int key_size=10;
  int stack_size=5;
  int cut_off = 4;
  int verbosity = 0;
  unsigned long TickStart;

  while ((opt = getopt(argc, argv, "var:k:s:c:")) != -1) {
    switch (opt) {
    case 'v':
      verbosity += 1;
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

  TickStart = bsort::getTick();

  while(optind < argc) {
    if (verbosity)
      printf("sorting %s\n", argv[optind]);
    struct bsort::sort sort;
    if (-1==bsort::open_sort(argv[optind], &sort))
      goto failure;

    bsort::radixify((unsigned char*)sort.buffer,
             sort.size / record_size,
             0,
             char_start,
             char_stop,
             record_size,
             key_size,
             stack_size,
             cut_off);
    close_sort(&sort);
    optind++;
  }

  printf("Processing time: %.3f s\n", (float)(bsort::getTick() - TickStart) / 1000);

  exit(0);
failure:
  fprintf(stderr,
          "Usage: %s [-v] [-a] [-r ###] [-k ###] [-s ###] file1, file2 ... \n",
          argv[0]);
  fprintf(stderr,
          "Individually sort binary files inplace with a radix sort\n"
          "\n"
          "Sorting Options:\n"
          "\n"
          "  -a       assume files are printable 7-bit ascii instead of binary\n"
          "  -k ###   size of compariable section of record, in bytes (default 100)\n"
          "  -r ###   size of overall record, in bytes.  (default 100)\n"
          "\n"
          "Options:\n"
          "  -v  verbose output logging\n"
          "\n"
          "Tuning Options:\n"
          "\n"
          "  -s ###   pushahead stack size.  (default 12)\n"
          "  -c ###   recursion limit after which to use shell sort (defaults to 4)\n"
          "\n"
          "Report bsort bugs to adam@pelotoncycle.com\n"
         );
  exit(1);
}


