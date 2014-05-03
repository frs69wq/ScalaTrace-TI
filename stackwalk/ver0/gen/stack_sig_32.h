

#ifndef STACK_SIG_H
#define STACK_SIG_H

#include <stdio.h>
#include <setjmp.h>

#ifdef i386
/***
16(%ebp)         - third function parameter
12(%ebp)        - second function parameter
8(%ebp)         - first function parameter
4(%ebp)         - old %EIP (the function's "return address")
0(%ebp)         - old %EBP (previous function's base pointer)
-4(%ebp)        - first local variable
-8(%ebp)        - second local variable
-12(%ebp)       - third local variable
**/
#define ParentFP(jb) NextFP(((void*)jb[0].__jmpbuf[3 /*JB_BP*/]))
#define FramePC(jb) (*(((int*)jb)+1))
#define NextFP(jb)      (*((int*)jb))

#elif defined (AIX)

#define ParentFP(jb) NextFP(((void*)jb[3]))
#define FramePC(fp) ( *( ( (intptr_t*)fp ) + 2 ) )
#define NextFP(fp) ((intptr_t)(*((intptr_t*)fp)))

#elif defined (__blrts__)

#define ParentFP(jb) NextFP(((void*)jb[0].__jmpbuf[JB_GPR1]))
#define FramePC(jb) (*(((intptr_t*)jb)+1))
#define NextFP(jb)      (intptr_t)(*((intptr_t*)jb))

#endif

#endif
