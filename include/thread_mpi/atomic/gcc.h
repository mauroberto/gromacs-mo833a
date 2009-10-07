/*
This source code file is part of thread_mpi.  
Written by Sander Pronk, Erik Lindahl, and possibly others. 

Copyright (c) 2009, Sander Pronk, Erik Lindahl.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
1) Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2) Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3) Neither the name of the copyright holders nor the
   names of its contributors may be used to endorse or promote products
   derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY US ''AS IS'' AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL WE BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

If you want to redistribute modifications, please consider that
scientific software is very special. Version control is crucial -
bugs must be traceable. We will be happy to consider code for
inclusion in the official distribution, but derived work should not
be called official thread_mpi. Details are found in the README & COPYING
files.

To help us fund development, we humbly ask that you cite
any papers on the package - you can find them in the top README file.

*/


/* this is for newer versions of gcc that have built-in intrinsics,
   on platforms not explicitly supported with inline assembly. */

#define tMPI_Atomic_memory_barrier()  __sync_synchronize()

/* Only gcc and Intel support this check, otherwise set it to true (skip doc) */
#if (!defined(__GNUC__) && !defined(__INTEL_COMPILER) && !defined DOXYGEN)
#define __builtin_constant_p(i) (1)
#endif


typedef struct tMPI_Atomic
{
    volatile int value;   
}
tMPI_Atomic_t;

typedef struct tMPI_Atomic_ptr
{
    volatile void* value;   
}
tMPI_Atomic_ptr_t;



#define TMPI_SPINLOCK_INITIALIZER   { 0 }


/* for now we simply assume that int and void* assignments are atomic */
#define tMPI_Atomic_get(a)  ((int)( (a)->value) )
#define tMPI_Atomic_set(a,i)  (((a)->value) = (i))


#define tMPI_Atomic_ptr_get(a)  ((void*)((a)->value) )
#define tMPI_Atomic_ptr_set(a,i)  (((a)->value) = (void*)(i))


#include "gcc_intrinsics.h"

#include "gcc_spinlock.h"

#if 0
/* our generic spinlocks: */
typedef struct tMPI_Spinlock
{
    volatile unsigned int  lock;
}
tMPI_Spinlock_t;



static inline void tMPI_Spinlock_init(tMPI_Spinlock_t *x)
{
    x->lock = 0;
}


static inline void tMPI_Spinlock_lock(tMPI_Spinlock_t *x)
{
#if 1
    while (__sync_lock_test_and_set(&(x->lock), 1)==1)
    {
        /* this is nicer on the system bus: */
        while (x->lock == 1)
        {
        }
    }
#else
    do
    {
    } while ( __sync_lock_test_and_set(&(x->lock), 1) == 1);
#endif
}


static inline int tMPI_Spinlock_trylock(tMPI_Spinlock_t *x)
{
    return (__sync_lock_test_and_set(&(x->lock), 1) == 1);
}


static inline void tMPI_Spinlock_unlock(tMPI_Spinlock_t *  x)
{
    __sync_lock_release(&(x->lock));
}
 
static inline int tMPI_Spinlock_islocked(tMPI_Spinlock_t *  x)
{
    __sync_synchronize();
    return ( x->lock == 1 );
}

static inline void tMPI_Spinlock_wait(tMPI_Spinlock_t *   x)
{
    do
    {
        __sync_synchronize();
    } while (x->lock == 1);
}

#endif
