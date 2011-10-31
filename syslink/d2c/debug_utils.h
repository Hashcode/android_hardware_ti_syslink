/*
 *  debug_utils.h
 *
 *  Debug Utility definitions.
 *
 *  Copyright (C) 2009-2011 Texas Instruments, Inc.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *  *  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *
 *  *  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 *  *  Neither the name of Texas Instruments Incorporated nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 *  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 *  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 *  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 *  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 *  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 *  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _DEBUG_UTILS_H_
#define _DEBUG_UTILS_H_

/*#define __DEBUG__*/
/*#define __DEBUG_ENTRY__*/
/*#define __DEBUG_ASSERT__*/

/* ---------- Generic Debug Print Macros ---------- */

/**
 * Use as:
 *    P("val is %d", 5);
 *    ==> val is 5
 *    DP("val is %d", 15);
 *    ==> val is 5 at test.c:56:main()
 */
/* debug print (fmt must be a literal); adds new-line. */
#ifdef __DEBUG__
#define P(fmt, ...) S_ { fprintf(stdout, fmt "\n", ##__VA_ARGS__); fflush(stdout); } _S
#else
#define P(fmt, ...)
#endif

/* debug print with context information (fmt must be a literal) */
#define DP(fmt, ...) P(fmt " at %s(" __FILE__ ":%d)", ##__VA_ARGS__, __FUNCTION__, __LINE__)

/* ---------- Program Flow Debug Macros ---------- */

/**
 * Use as:
 *    int get5() {
 *      IN;
 *      return R_I(5);
 *    }
 *    void check(int i) {
 *      IN;
 *      if (i == 5) { RET; return; }
 *      OUT;
 *    }
 *    void main() {
 *      IN;
 *      check(get5());
 *      check(2);
 *      OUT;
 *    }
 *    ==>
 *    in main(test.c:11)
 *    in get5(test.c:2)
 *    out(5) at get5(test.c:3)
 *    in check(test.c:6)
 *    out() at check(test.c:7)
 *    in check(test.c:6)
 *    out check(test.c:8)
 *    out main(test.c:14)
 */

#ifdef __DEBUG_ENTRY__
/* function entry */
#define IN P("in %s(" __FILE__ ":%d)", __FUNCTION__, __LINE__)
/* function exit */
#define OUT P("out %s(" __FILE__ ":%d)", __FUNCTION__, __LINE__)
/* function abort (return;)  Use as { RET; return; } */
#define RET DP("out() ")
/* generic function return */
#define R(val,type,fmt) E_ { type __val__ = (type) val; DP("out(" fmt ")", __val__); __val__; } _E
#else
#define IN
#define OUT
#define RET
#define R(val,type,fmt) (val)
#endif

/* integer return */
#define R_I(val) R(val,int,"%d")
/* pointer return */
#define R_P(val) R(val,void *,"%p")
/* long return */
#define R_UP(val) R(val,long,"0x%lx")

/* ---------- Assertion Debug Macros ---------- */

/**
 * Use as:
 *     int i = 5;
 *     // int j = i * 5;
 *     int j = A_I(i,==,3) * 5;
 *     // if (i > 3) P("bad")
 *     if (NOT_I(i,<=,3)) P("bad")
 *     P("j is %d", j);
 *     ==> assert: i (=5) !== 3 at test.c:56:main()
 *     assert: i (=5) !<= 3 at test.c:58:main()
 *     j is 25
 *
 */
/* generic assertion check, A returns the value of exp, CHK return void */
#ifdef __DEBUG_ASSERT__
#define A(exp,cmp,val,type,fmt) E_ { \
    type __exp__ = (type) (exp); type __val__ = (type) (val); \
    if (!(__exp__ cmp __val__)) DP("assert: %s (=" fmt ") !" #cmp " " fmt, #exp, __exp__, __val__); \
    __exp__; \
} _E
#define CHK(exp,cmp,val,type,fmt) S_ { \
    type __exp__ = (type) (exp); type __val__ = (type) (val); \
    if (!(__exp__ cmp __val__)) DP("assert: %s (=" fmt ") !" #cmp " " fmt, #exp, __exp__, __val__); \
} _S
#else
#define A(exp,cmp,val,type,fmt) (exp)
#define CHK(exp,cmp,val,type,fmt)
#endif

/* typed assertions */
#define A_I(exp,cmp,val) A(exp,cmp,val,int,"%d")
#define A_L(exp,cmp,val) A(exp,cmp,val,long,"%ld")
#define A_P(exp,cmp,val) A(exp,cmp,val,void *,"%p")
#define CHK_I(exp,cmp,val) CHK(exp,cmp,val,int,"%d")
#define CHK_L(exp,cmp,val) CHK(exp,cmp,val,long,"%ld")
#define CHK_P(exp,cmp,val) CHK(exp,cmp,val,void *,"%p")

/* generic assertion check, returns true iff assertion fails */
#ifdef __DEBUG_ASSERT__
#define NOT(exp,cmp,val,type,fmt) E_ { \
    type __exp__ = (type) (exp); type __val__ = (type) (val); \
    if (!(__exp__ cmp __val__)) DP("assert: %s (=" fmt ") !" #cmp " " fmt, #exp, __exp__, __val__); \
    !(__exp__ cmp __val__); \
} _E
#else
#define NOT(exp,cmp,val,type,fmt) (!((exp) cmp (val)))
#endif

/* typed assertion checks */
#define NOT_I(exp,cmp,val) NOT(exp,cmp,val,int,"%d")
#define NOT_L(exp,cmp,val) NOT(exp,cmp,val,long,"%ld")
#define NOT_P(exp,cmp,val) NOT(exp,cmp,val,void *,"%p")

/* error propagation macros - these macros ensure evaluation of the expression
   even if there was a prior error */

/* new error is accumulated into error */
#define ERR_ADD(err, exp) S_ { int __error__ = A_I(exp,==,0); err = err ? err : __error__; } _S
/* new error overwrites old error */
#define ERR_OVW(err, exp) S_ { int __error__ = A_I(exp,==,0); err = __error__ ? __error__ : err; } _S

#endif
