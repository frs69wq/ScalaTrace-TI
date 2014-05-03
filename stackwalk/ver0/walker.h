/*
 * Filename: walker.h
 * Author:   prasun
 * Created:  Tue Jan 29 16:59:46 2008
 * $Id: walker.h,v 1.1 2012/04/02 05:12:11 xwu3 Exp $
 */
#ifndef __WALKER_H__
#define __WALKER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
extern intptr_t * do_stack_unwind ();

#define SKIP_DEPTH     4 /* not used */
#define TRACE_DEPTH 1000 /* maximum length of stack sig */

intptr_t pc_array[TRACE_DEPTH];

#ifdef __cplusplus
}
#endif

#endif
/* vim: set tw=80 sw=2 expandtab: */
