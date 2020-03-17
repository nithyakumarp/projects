#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include <fcntl.h>

int
length(char* text)
{
    char* z;
    for (z = text; *z; ++z);
    return z - text;
}

/*void print_arr(int *arr,int size)
{
  for(int i=0;i<size/4;i++)
  {
    printf("%d\t",arr[i]);
  }
  printf("\n");
}
*/
/*
int myAtoi(char* str)
{
    int res = 0; // Initialize result

    // Iterate through all characters of input string and
    // update result
    for (int i = 0; str[i] != '\0'; ++i)
        res = res * 10 + str[i] - '0';

    // return result.
    return res;
}
*/
int filesize(const char *file)
{
  int size;
  struct stat st;
  size = stat(file,&st);
  if(size<0)
  {
    perror("File name error");
    _exit(1);
  }
  size=st.st_size;
  //printf("Size : %d\n",size);
  return size;
}

void insertionSort(int *arr, int size)
{
  size=size/4;
  int i, key, j;
  for (i = 1; i < size; i++) {
      key = arr[i];
      j = i - 1;

        /* Move elements of arr[0..i-1], that are
          greater than key, to one position ahead
          of their current position */
      while (j >= 0 && arr[j] >= key) {
          arr[j + 1] = arr[j];
          j = j - 1;
      }
      arr[j + 1] = key;
  }
  //printf("out of insertionSort");
}

int main(int argc, char* argv[])
{
  int rv;
  if(argc!=3)
  {
    char* usage = "Usage: ./unpack file_to_unpack\n";
	  rv = write(1, usage, length(usage));
	  if (rv < 0) {
        // Checking your syscall return values is a
        // really good idea.
        perror("write in main usage");
        _exit(1);
    }
    _exit(1);
  }

  int input_fd=open(argv[1],O_RDONLY);
  if(input_fd== -1)
  {
    char *error = "Input file doesn't exist.\n";
    perror(error);
    _exit(1);
  }

  int size= filesize(argv[1]);
  int read_buf[size/4];
  int x= read(input_fd,read_buf,size);

  //printf("Before sort:\n");
  //print_arr(read_buf,size);
  //int count= myAtoi(argv[2]);
  //rv= write(rand_fd, argv[2], length(argv[2]));
  insertionSort(read_buf,size);
  //printf("After sort:\n");
  //print_arr(read_buf,size);
  close(input_fd);
  int output_fd=open(argv[2],O_CREAT|O_RDWR,0644);
  if(input_fd== -1)
  {
    char *error = "Cannot create new file\n";
    perror(error);
    _exit(1);
  }

  //printf("Writing data in the file\n");
  rv=write(output_fd,read_buf,size);
  if (rv < 0) {
      // Checking your syscall return values is a
      // really good idea.
      perror("write in main usage");
      _exit(1);
  }
  //printf("Finished writing\n");
  return 0;
}
