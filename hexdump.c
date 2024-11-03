#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef size_t usize;
typedef uint8_t u8;
typedef uint32_t u32;

#define MAX_BUFF_SIZE 1024 * 500 // 500 kilobytes
#define IS_PRINTABLE_ASCII(c) (((c) > 31) && ((c) < 127))
#define NON_PRINTABLE_ASCCI '.'

void hexdump(void *buffer, usize size) {
  u8 *data = buffer;
  usize i, j;

  for (i = 0; i < size; i++) {
    u8 byte = data[i];

    if((i % 16) == 0){
      printf("%08lx ", i);
    }

    //space between 8 bytes
    if (i % 8 == 0) { printf(" ");}
    
    printf("%02x ", byte);

    // 0-15th byte in a row   row also ends if you reach last index
    if (((i % 16) == 15) || (i == size - 1)) {

      // extra spaces
      for (j = 0; j < 15 - (i % 16); j++) {
        printf("   ");
      }

      printf(" |");
      for (j = (i - (i % 16)); j <= i; j++) {
        printf("%c", IS_PRINTABLE_ASCII(data[j]) ? data[j] : NON_PRINTABLE_ASCCI);
      }
      printf("|\n");
    }
  }

  printf("%08lx\n", i);
}

usize file_open_and_read(char *filepath, char *buffer, usize size) {

  // file exists
  if (access(filepath, F_OK) != 0) {
    fprintf(stderr, "[ERROR]: FILE '%s' does not exist!\n", filepath);
    exit(1);
  }

  // open file
  FILE *f = fopen(filepath, "r");
  if (f == NULL) {
    fprintf(stderr, "[ERROR]: Could not open the file '%s'!\n", filepath);
    exit(1);
  }

  // read file size
  usize file_size;
  fseek(f, 0L, SEEK_END);
  file_size = ftell(f);
  fseek(f, 0L, SEEK_SET);

  if (file_size <= 0) {
    fprintf(stderr, "[ERROR]: File has '%zu' <= 0 bytes!\n", file_size);
    exit(1);
  } else if (file_size > size) {
    fprintf(stderr,
            "[ERROR]: File size(%zu) is too big, max size allowed is %zu\n",
            file_size, size);
    exit(1);
  }

  // read the file in memory
  fread(buffer, sizeof(char), file_size, f);
  fclose(f);

  return file_size;
}

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <file>\n", argv[0]);
    exit(1);
  }

  char *filepath = argv[1];
  char buffer[MAX_BUFF_SIZE] = {0};
  usize file_size = file_open_and_read(filepath, buffer, MAX_BUFF_SIZE);
  hexdump(buffer, file_size);

  return 0;
}
