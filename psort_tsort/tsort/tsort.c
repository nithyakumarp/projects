#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>
#include <string.h>
#include <float.h>

#include "float_vec.h"
#include "barrier.h"
#include "utils.h"

float* data;
long size;
int P;
floats* samps;
int* offset;
long* sizes;
barrier* bb;

//comparator function to pass to qsort()
int comparator(const void *a, const void *b)
{
    if(*(float *)a < *(float *)b)
        return -1;
    else if(*(float *)a > *(float *)b)
        return 1;
    else
        return 0; 
}  

//calls qsort to sort the floats* struct passed to the function
void
qsort_floats(floats* xs)
{
    qsort(xs->data,xs->size,sizeof(float),comparator);
}

//Function to return 3*(P-1) samples from float* data
floats*
sample()
{
    int i, count=3*(P-1);
    struct floats *sample_space = make_floats(0);   //creates a floats structure
    for(i=0;i< count;i++)
        floats_push(sample_space, data[rand()%size]);   //copies random 3*(P-1) data to the newly created floats structure
    return sample_space;    //returns sampled floats structure
}

//This is the worker function that runs parallely in different threads
void *sort_worker(void* ii)
{
    int pnum = *((int *)ii);
    floats* xs = make_floats(0);
    // Done: Copy the data for our partition into a locally allocated array.
    int i;
   //Uses samps structure to find the range to copy data
    for(i=0;i<size;i++){
        if(data[i]>=samps->data[pnum] && data[i]<samps->data[pnum+1])
            floats_push(xs,data[i]);
    }

    //Print the stuff required to pass the test script
    printf("%d: start %.04f, count %ld\n", pnum, samps->data[pnum], xs->size);
   // Done: Sort the local array.
    qsort_floats(xs);
    // Done: Using the shared sizes array, determine where the local
    // output goes in the global data array.
    barrier_wait(bb);
    // Done: Copy the local array to the appropriate place in the global array.
    for(i=0;i<sizes[pnum];i++)
        *(data+(i+offset[pnum]))=xs->data[i];
    free_floats(xs);
    // Done: Made sure this function doesn't have any data races by using barriers
   return NULL;
}

//Function that creates parallel processes to run the sort_workers() parallely
void
run_sort_workers()
{
    // Done: Spawn P processes running sort_worker
    pthread_t *tid = malloc(P*sizeof(pthread_t));
    int i;
    //Offset is used to copy the sorted data back to the array so it is shared
    offset=(int *)mmap(NULL, P*sizeof(int),PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANON,(-1),0);
    offset[0]=0;
    //copying offset values from sizes
    for(i=1;i<P;i++)
        offset[i]=offset[i-1]+sizes[i-1];
    //phil[] array has the thread number to pass to the thread
    int *phil=malloc(P*sizeof(int));
    for(i=0;i<P;i++){
        phil[i]=i;
    }

    //creates P number of threads
    for(i=0;i<P;i++)
       pthread_create(&tid[i],NULL,sort_worker,&phil[i]);
    //Joins all the threads created
    for(i=0;i<P;i++)
        pthread_join(tid[i],NULL);

    munmap(offset,P*sizeof(int));   //Unmapping the created map
    free(tid);
    free(phil);
}

//This function is called from main and does all the work for sorting
void
sample_sort()
{
    // Done: Sequentially sample the input data.
    struct floats *sample_space= sample();

    //Done: Sort the input data using the sampled array to allocate work
    // between parallel processes.

    qsort_floats(sample_space);
    int i;
    samps= make_floats(0);
    //First sample is 0;

    floats_push(samps,0);
    int j;
    //data in an interval of 3 is pushed to the sample array;
    for(i=1;i<sample_space->size;i=i+3){
        floats_push(samps, sample_space->data[i]);
    }
    //FLT_MAX value is inserted for infinity
    floats_push(samps,FLT_MAX);
    //Finding the sizes of every group each process is going to sort
    for(j=0;j<P;j++){
        for(i=0;i<size;i++){
            if(data[i]>=samps->data[j] && data[i]<samps->data[j+1])
                sizes[j]++;
        }
    }
    //CAlling run_sort workers to sort
    run_sort_workers();

    //Freeing the created floats structure
    free_floats(sample_space);
    free_floats(samps);
}

int
main(int argc, char* argv[])
{
    if (argc != 3) {
        printf("Usage:\n");
        printf("\t%s P data.dat\n", argv[0]);
        return 1;
    }
    //P is the number of threads
    P = atoi(argv[1]);
    const char* fname = argv[2];
    int fd,rv,filesize;
    seed_rng();
    //opening the file.
    fd = open(fname, O_RDWR);
    check_rv(fd);

    void * file = 0; 
    struct stat statbuf1;
    rv= fstat(fd, &statbuf1);   //fstat to find the file size
    check_rv(rv);

    filesize= statbuf1.st_size;
    // Done: Used mmap for I/O and mapped the file to void *file
    file = mmap(NULL, filesize, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);

    memcpy(&size, (long*)file, sizeof(long));   //count is the first 8 bytes of the file

    data = (void *)file+8; // mapped the data that is at 8bytes from the start to float *data
    long sizes_bytes = P * sizeof(long);
    // Done: sizes is shared memory created with mmap with MAP_SHARED
    sizes = (long *) mmap(NULL, sizes_bytes, PROT_READ|PROT_WRITE, MAP_ANON|MAP_SHARED, (-1), 0);
    //Barrier is created
    bb = make_barrier(P);
    // running sample_sort function to sort the data
    sample_sort();
    //Barrier is destroyed
    free_barrier(bb);

    // Done: Clean up resources.
    munmap(file,filesize);
    munmap(sizes,sizes_bytes);

    return 0;
}