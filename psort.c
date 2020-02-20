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

//sem_t lock;

int comparator(const void *a, const void *b)
{
    if(*(float *)a < *(float *)b)
        return -1;
    else if(*(float *)a > *(float *)b)
        return 1;
    else
        return 0; 
}  

void
qsort_floats(floats* xs)
{
    // TODO: man 3 qsort ?
    qsort(xs->data,xs->size,sizeof(float),comparator);
}

floats*
sample(float* data, long size, int P)
{
    // TODO: Randomly sample 3*(P-1) items from the input data.

    int i, count=3*(P-1);
    struct floats *sample_space = make_floats(0);
    //float sample[P+1];
    for(i=0;i< count;i++){
        //sample_space[i]= data[rand()%size];
        floats_push(sample_space, data[rand()%size]);
    }



    return sample_space;
}

void
sort_worker(int pnum, float* data, long size, int P, floats* samps, int* offset, long* sizes, barrier* bb)
{
    floats* xs = make_floats(0);
    //= malloc(sizes[pnum]*sizeof(floats));
    //xs->size=0;
    //xs=(floats *)mmap(NULL, sizes[pnum]*sizeof(floats),PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANON,(-1),0);
   // xs=make_floats(0);
    // TODO: Copy the data for our partition into a locally allocated array.
    int i;
   // for(j=0;j<P;j++)
    for(i=0;i<size;i++){
        if(data[i]>=samps->data[pnum] && data[i]<samps->data[pnum+1])
            floats_push(xs,data[i]);
    }

    printf("%d: start %.04f, count %ld\n", pnum, samps->data[pnum], xs->size);
   // printf("----Offset[%d]= %d\n ",pnum,offset[pnum]);
    //printf("%d: start %.04f, count %ld\n", pnum, samps->data[pnum], sizes[pnum]);
    // TODO: Sort the local array.
    qsort_floats(xs);
    // TODO: Using the shared sizes array, determine where the local
    // output goes in the global data array.
    //printf("Process number: %d\n",pnum);
   /*barrier_wait(bb);
    for(i=0;i<sizes[pnum];i++){
        printf("----%f\n",xs->data[i]);
    }
    */
  //  sem_post(&lock);
    // TODO: Copy the local array to the appropriate place in the global array.
    barrier_wait(bb);
   
   // barrier_wait(bb);
   
    for(i=0;i<sizes[pnum];i++){
        *(data+(i+offset[pnum]))=xs->data[i];
        //data[i+offset[pnum]]=xs->data[i];
    }
    free_floats(xs);
    // TODO: Make sure this function doesn't have any data races.
   // free(xs);
}

void
run_sort_workers(float* data, long size, int P, floats* samps, long* sizes, barrier* bb)
{
    // TODO: Spawn P processes running sort_worker
    int i;
    int *offset=(int *)mmap(NULL, P*sizeof(int),PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANON,(-1),0);
    offset[0]=0;
    for(i=1;i<P;i++)
    {
        offset[i]=offset[i-1]+sizes[i-1];

        //printf("offset %d\n",offset[i]);
    }
    for(i=0;i<P;i++){
        if(fork()==0){
            sort_worker(i,data,size,P,samps,offset,sizes,bb);
            // TODO: Once all P processes have been started, wait for them all to finish.
            exit(0);
        }
    }
    for(i=0;i<P;i++){
        wait(NULL);
    }
    munmap(offset,P*sizeof(int));
}

void
sample_sort(float* data, long size, int P, long* sizes, barrier* bb)
{
    // TODO: Sequentially sample the input data.
    //
    struct floats *sample_space= sample(data, size, P);

    // TODO: Sort the input data using the sampled array to allocate work
    // between parallel processes.

    qsort_floats(sample_space);
    int i;
    /*
    for(i=0;i<sample_space->size;i++){
        printf("%f\n",sample_space->data[i]);
    }*/
    struct floats *sample= make_floats(0);

    floats_push(sample,0);
    int j;
    for(i=1;i<sample_space->size;i=i+3){
        floats_push(sample, sample_space->data[i]);
    }
    //sample[0]=0;
    //sample[P]=FLT_MAX;
    floats_push(sample,FLT_MAX);
    for(j=0;j<P;j++){
        for(i=0;i<size;i++){
            if(data[i]>=sample->data[j] && data[i]<sample->data[j+1])
                sizes[j]++;
        }
    }
    run_sort_workers(data,size,P,sample,sizes,bb);

    free_floats(sample_space);
    free_floats(sample);
    /*
    printf("Printing samples:\n");
    for(i=0;i<sample->size;i++){
        printf("%f\n",sample->data[i]);
    }
*/
}

int
main(int argc, char* argv[])
{
    if (argc != 3) {
        printf("Usage:\n");
        printf("\t%s P data.dat\n", argv[0]);
        return 1;
    }
    //sem_init(&lock,0,0);
    const int P = atoi(argv[1]);
    const char* fname = argv[2];
    int fd,rv,filesize;
    seed_rng();

    fd = open(fname, O_RDWR);
    check_rv(fd);

    void * file = 0; // TODO: Use mmap for I/O

    struct stat statbuf1;
    //memset( statbuf1, 0, sizeof(struct stat) );

    rv= fstat(fd, &statbuf1);
    check_rv(rv);

    filesize= statbuf1.st_size;

    file = mmap(NULL, filesize, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);

    long count=0;
    memcpy(&count, (long*)file, sizeof(long));

    float* data = (void *)file+8; // TODO: this is in the file


    //data= malloc(count*(sizeof(float)));

    //memcpy(data, (float *)file+2, count*(sizeof(float)));
/*
    int i;
   printf("Data array\n");
    for(i=0;i<count;i++)
    printf("%f\n",data[i]);

    printf("End of data\n");

*/
    long sizes_bytes = P * sizeof(long);

    //sample(data, count, P);

   // long* sizes = malloc(sizes_bytes); // TODO: This should be shared memory.
    long *sizes = (long *) mmap(NULL, sizes_bytes, PROT_READ|PROT_WRITE, MAP_ANON|MAP_SHARED, (-1), 0);
    barrier* bb = make_barrier(P);

    sample_sort(data, count, P, sizes, bb);

    free_barrier(bb);

    // TODO: Clean up resources.
  //  free(sizes);
    munmap(file,filesize);
    munmap(sizes,sizes_bytes);

    return 0;
}