#include<stdio.h>
#include<omp.h>
#include<stdlib.h>

#define DIM 512

void mat_init(long long int **mat,long long int *vect)
{
    int i,j;
    for(i=0;i<DIM;i++)
    {
       vect[i]= 99 + (rand()%900); 
       //vect[i]=2;
       for(j=0;j<DIM;j++)
         mat[i][j]=999+(rand()%9000);
         //mat[i][j]=3;
         
    }
}

void mat_vect_product(long long int **mat, long long int *vect,long long int *result)
{
    int i,j;
    int sum;
#pragma omp parallel shared(mat,result, vect) private(i,j) 
    //private(i,j)
{
    printf("In thread id: %d\n",omp_get_thread_num());

    for(i=0;i<DIM;i++){
        sum=0;
#pragma omp for schedule(static) reduction(+:sum)
        for(j=0;j<DIM;j++)
        {
            sum=sum + mat[i][j]*vect[j];
        }
        result[i]=sum;
    }
}

}

void mat_print(long long int *result)
{
    int i;
    printf("Printing result matrix:\n");
    for(i=0;i<DIM;i++)
        printf("%d\n",result[i]);
}


int main()
{
    int i;
    
    long long int **mat=(long long int **)malloc(DIM * sizeof(long long int *));
    for(i=0;i<DIM;i++)
        mat[i]=(long long int *)malloc(DIM * sizeof(long long int));

    long long int vect[DIM];
    long long int result[DIM];

    mat_init(mat,vect);
    
    mat_vect_product(mat, vect, result);

    mat_print(result);

    printf("Total number of available threads: %d\n",omp_get_max_threads());
    return 0;
}


