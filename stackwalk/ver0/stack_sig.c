/**
 * Platform-aware stack-sig file by Todd Gamblin, 5/23/2007
 * This attempts to determine what architecture we're on and which stack
 * signature to use.  
 * 
 * Currently thinks that everything that is not x86_64 should use the 
 * regular 32-bit stack_sig file.
 */
#include "walker.h"

#ifdef __x86_64__
#include "x86_64/stack_sig_x86-64.c"
#else 
#include "gen/stack_sig_32.c"
#endif
