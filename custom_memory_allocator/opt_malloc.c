//#include <stdlib.h>
#include <stdio.h>

#include "xmalloc.h"
#include <sys/mman.h>
#include <string.h>
#include <pthread.h>
#include <math.h>


static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static int buckets[10] = {2,3,5,9,17,33,65,129,257,513};

typedef long Align;

typedef unsigned long uint;

union header {
  struct {
    union header *ptr;
    uint size;
  } s;
  Align x;
};

typedef union header Header;

// TODO: This is shared global data.
// You're going to want a mutex to protect this.
static Header base[12];
static Header *freep[12];


void
xvfree(void *ap, int index)
{
  pthread_mutex_lock(&mutex);
  Header *bp, *p;

  bp = (Header*)ap - 1;
  
  for(p = freep[index]; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
  {
    if(p >= p->s.ptr && (bp > p || bp < p->s.ptr))
    {
       break;
    }
  }
  if(bp + bp->s.size == p->s.ptr)
  {
    bp->s.size += p->s.ptr->s.size;
    bp->s.ptr = p->s.ptr->s.ptr;
  } 
  else
  {
    bp->s.ptr = p->s.ptr;
  }
  if(p + p->s.size == bp)
  {
    p->s.size += bp->s.size;
    p->s.ptr = bp->s.ptr;
  } 
  else
  {
    p->s.ptr = bp;
  }
  freep[index] = p;
  pthread_mutex_unlock(&mutex);
}

static Header*
morecore(unsigned long nu, int index)
{
  char *p;
  Header *hp;

  if(nu < 4096)
  {
    nu = 4096;
  }
  p = mmap(NULL, nu*sizeof(Header), (PROT_READ|PROT_WRITE|PROT_EXEC), (MAP_PRIVATE|MAP_ANONYMOUS), -1, 0);
  if(p == (char*)-1)
  {
    return 0;
  }
  hp = (Header*)p;
  hp->s.size = nu;
  xvfree((void*)(hp + 1), index);
  return freep[index];
}

void*
xvmalloc(unsigned long nbytes, int index)
{
  Header *p, *prevp;
  unsigned long nunits;
  nunits = (nbytes + sizeof(Header) - 1)/sizeof(Header) + 1;
  //protect the global free list with a mutex
  pthread_mutex_lock(&mutex);
  if((prevp = freep[index]) == 0)
  {
    base[index].s.ptr = freep[index] = prevp = &base[index];
    base[index].s.size = 0;
  }
  for(p = prevp->s.ptr; ; prevp = p, p = p->s.ptr)
  {
    if(p->s.size >= nunits)
    {
      if(p->s.size == nunits)
        prevp->s.ptr = p->s.ptr;
      else 
      {
        p->s.size -= nunits;
        p += p->s.size;
        p->s.size = nunits;
      }
      //unlock mutex before returning
      freep[index] = prevp;
      pthread_mutex_unlock(&mutex);
      return (void*)(p + 1);
    }
    if(p == freep[index])
    {
      //unlock the mutex before morecore
      pthread_mutex_unlock(&mutex);
      if((p = morecore(nunits, index)) == 0)
      {
        return 0;
      }
      //lock mutex so that next iteration of loop
      //unlocks it 
      pthread_mutex_lock(&mutex);
    }
  }
}

size_t minimum (size_t a, size_t b)
{
  return a<b?a:b;
}

void*
xrealloc(void* prev, size_t nn)
{
  
  Header *prevh = (Header*)prev;
  prevh = prevh - 1;

  //create new page 
  void* new = xmalloc(nn);

  //copy contents to new page  
  size_t size = (prevh->s.size - 1)*sizeof(Header) - sizeof(Header) + 1;
  memcpy(new, prev, minimum(nn,size));
  //add the old block to the free list
  xfree(prevh+1);
  //return new address
  return new;
}

void*
xmalloc(unsigned long nbytes)
{
    unsigned long nunits = (nbytes + sizeof(Header) - 1)/sizeof(Header) + 1;
    int index;
    for(index=0;index<10;index++)
    {
        if(buckets[index]>= nunits)
        {
            break;
        }
    }
    return (xvmalloc(nbytes, index));
}

void
xfree(void *addr)
{
    Header *prev = (Header*) addr;
    prev = prev - 1;
    unsigned long nunits = prev->s.size;
    int i;
    for(i=0;i<10;i++)
    {
        if(buckets[i]>= nunits)
        {
            break;
        }
    }
    int index = i;
    xvfree(addr, index);
}