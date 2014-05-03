/*
 * Filename: main.c
 * Author:   prasun
 * Created:  Sun Feb 10 18:23:53 2008
 * $Id: main.c,v 1.1 2012/04/02 05:12:11 xwu3 Exp $
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
