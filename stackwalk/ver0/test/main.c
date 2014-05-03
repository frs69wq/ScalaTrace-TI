/*
 * Copyright (c) 2008-May 2, 2014: Prasun Ratn <prasun@ncsu.edu>
 * Copyright (c) of subsequent modifications of this code:
 * Frederic Suter, CNRS / IN2P3 Computing Center <fsuter@cc.in2p3.fr>
 * Henri Casanova, ICS Dept., University of Hawai`i at Manoa <henric@hawaii.edu>

 * The license of the original code is unknown. Modifications are made
 * under the terms of the GNU LGPL license.
 */

#include <stdint.h>
#include <stdio.h>

void a(int b)
{
  if (b) 
  {
    a (b - 1); 
  }
  else
  {
    intptr_t * tmp = do_stack_unwind (); 
    int i = 0;
    while (tmp[i]) {
      printf ("%x ", tmp[i]);
      i++;
    }   
    printf ("\n");
  }
}
void one ()
{
  a(2);
}
void two ()
{
  a(2);
}
void three ()
{
  a(2);
}
int main ()
{
  one(); //two(); three();
  return 0;
}

/* vim: set tw=80 ts=2 sw=2 expandtab: */
