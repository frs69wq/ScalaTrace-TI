/* -*- Mode: C; -*- */
/* Creator: Jeffrey Vetter (vetter3@llnl.gov) Tue Feb 22 2000 */

/* queue.c -- simple queue 
   - ordering of elements is not enforced.
   - the data is the key, no separate entries in q struct
   - all comparison ops depend on user provide functions (i.e., they can change in same app).
   - comparison op should decide what fields matter and how they rank
   - hence, if comparison ops can change, ordering cannot be systematically enforced.
   - append,enqueue just add something to the tail.
   - use _walk to apply a function to each ele of the queue.
*/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "queue.h"

static const int _q_cookie = 2069096667;

#define ASSERT_COOKIE(q) {assert(q);\
 assert(_q_cookie == q->cookie);\
 assert(q->count >= 0);}

int
qCompare_ints (void *a, void *b)
{
  int *a_ptr = (int *) a;
  int *b_ptr = (int *) b;

  if (*a_ptr == *b_ptr)
    return 0;

  if (*a_ptr < *b_ptr)
    return -1;

  return 1;
}

int
qCompare_q_ele_data_addrs (void *a, void *b)
{
  if (a == b)
    return 0;

  if (a < b)
    return -1;

  return 1;
}

static Q_ele_t *
_q_create_ele (void *data, Q_ele_t * prev, Q_ele_t * next)
{
  Q_ele_t *e = NULL;
  assert (data);
  e = (Q_ele_t *) malloc (sizeof (Q_ele_t));
  e->data = data;
  e->prev = prev;
  e->next = next;
  return e;
}


/* this probably should do something with the apply return values... */
int
_q_walk (Q_t * q, _qApply apply)	/* internal walk */
{
  Q_ele_t *e;
  Q_ele_t *next = NULL;		/* in case the user frees e */
  ASSERT_COOKIE (q);
  for (e = q->head; e; e = next)
    {
      next = e->next;
      apply (e);
    }
  return 0;
}

Q_t *
q_open ()
{
  Q_t *q = NULL;
  q = (Q_t *) malloc (sizeof (Q_t));
  q->cookie = _q_cookie;
  q->count = 0;
  q->head = NULL;
  q->tail = NULL;
  ASSERT_COOKIE (q);
  return q;
}

static int
_q_close_empty_apply (Q_ele_t * e)
{
  assert (e);
  assert (e->data);
  free (e->data);
  free (e);
  return 0;
}

int
q_close (Q_t * q)
{
  ASSERT_COOKIE (q);

  /* walk the q and release any remaining memory incl data */
  _q_walk (q, _q_close_empty_apply);

  free (q);

  return 0;
}

#ifndef UMPI_HAND_INLINING

int
q_empty (Q_t * q)
{
  ASSERT_COOKIE (q);
  return (q->count == 0);
}

int
q_count (Q_t * q)
{
  ASSERT_COOKIE (q);
  return q->count;
}

int
q_append (Q_t * q, void *data)	/* add an element to the tail */
{
  ASSERT_COOKIE (q);
  return q_enqueue (q, data);
}

void *
q_head (Q_t * q)
{
  ASSERT_COOKIE (q);
  if (q->head)
    {
      return q->head->data;
    }
  else
    {
      return NULL;
    }
}

void *
q_tail (Q_t * q)
{
  ASSERT_COOKIE (q);
  if (q->tail)
    {
      return q->tail->data;
    }
  else
    {
      return NULL;
    }
}

#endif

int
q_replace (Q_t * q, qComparator comp, void *oldkey, void *newkey, void **data)
{
  Q_ele_t *e;
  ASSERT_COOKIE (q);
  for (e = q->head; e; e = e->next)
    {
      if (comp (oldkey, e->data) == 0)
	{
	  *data = e->data;
	  e->data = newkey;
	  return 0;
	}
    }
  ASSERT_COOKIE (q);
  return 1;
}

int
q_search (Q_t * q, qComparator comp, void *key, void **data)
{
  Q_ele_t *e;
  ASSERT_COOKIE (q);
  assert (data);
  for (e = q->head; e; e = e->next)
    {
      if (comp (key, e->data) == 0)
	{
	  *data = e->data;
	  return 0;
	}
    }
  ASSERT_COOKIE (q);
  return 1;
}

int
q_search_ele (Q_t * q, qComparator comp, void *key, Q_ele_t ** ele)
{
  Q_ele_t *e;
  ASSERT_COOKIE (q);
  assert (ele);
  for (e = q->head; e; e = e->next)
    {
      if (comp (key, e->data) == 0)
	{
	  *ele = e;
	  return 0;
	}
    }
  ASSERT_COOKIE (q);
  return 1;
}

int
_q_remove (Q_t * q, Q_ele_t *e) 
{
  /* patch up the structure */
  if (q->count == 1)	/* easy */
    {
      q->head = q->tail = NULL;
    }
  else if (q->head == e)
    {
      q->head = e->next;
      e->next->prev = NULL;
    }
  else if (q->tail == e)
    {
      q->tail = e->prev;
      e->prev->next = NULL;
    }
  else if (q->count < 0)	/* some error checking */
    {
      assert (0);
    }
  else
    {
      e->next->prev = e->prev;
      e->prev->next = e->next;
    }
  free (e);
  q->count--;

  return 0;
}

int
q_delete (Q_t * q, qComparator comp, void *key, void **data)
{
  Q_ele_t *e;
  ASSERT_COOKIE (q);
  assert (data);
  for (e = q->head; e; e = e->next)
    {
      if (comp (key, e->data) == 0)
	{
	  *data = e->data;

	  _q_remove (q, e);

	  return 0;
	}
    }
  ASSERT_COOKIE (q);
  return 1;
}

/* macro version has an unfixed bug... */
/*
#ifndef UMPI_HAND_INLINING
*/

int
q_enqueue (Q_t * q, void *data)	/* add an element to the tail */
{
  Q_ele_t *e;
  ASSERT_COOKIE (q);
  assert (data);
  e = _q_create_ele (data, q->tail, NULL);

  if (q->count == 0)
    {
      q->head = q->tail = e;
    }
  else if (q->count > 0)
    {
      q->tail->next = e;
      q->tail = e;
    }
  else
    {
      assert (0);
    }

  q->count++;
  ASSERT_COOKIE (q);
  return 0;
}

/*
#endif
*/

void
q_push (Q_t * q, void *data)	/* add an element to the head */
{
  Q_ele_t *e;
  ASSERT_COOKIE (q);
  assert (data);
  e = _q_create_ele (data, NULL, q->head);

  if (q->count == 0)
    {
      q->head = q->tail = e;
    }
  else if (q->count > 0)
    {
      q->head->prev = e;
      q->head = e;
    }
  else
    {
      assert (0);
    }

  q->count++;
  ASSERT_COOKIE (q);
}

/* macro version has an unfixed bug... */
/*
#ifndef UMPI_HAND_INLINING
*/

int
q_dequeue (Q_t * q, void **data)	/* take an element from the head */
{
  Q_ele_t *e = NULL;
  ASSERT_COOKIE (q);
  *data = NULL;
  if (q->count == 0)
    {
      return -1;
    }
  else if (q->count == 1)
    {
      e = q->head;
      *data = e->data;
      q->head = NULL;
      q->tail = NULL;
      q->count--;
      free (e);
    }
  else if (q->count > 1)
    {
      e = q->head;
      *data = e->data;
      q->head = e->next;
      q->head->prev = NULL;
      q->count--;
      free (e);
    }
  else
    {
      assert (0);
    }

  ASSERT_COOKIE (q);
  return 0;
}

/*
#endif
*/

/* THIS IS WRONG SEMANTICALLY */
/* SEMANTICALLY DEQUEUE AND POP ARE THE SAME OPERATION */
/* SHOULD BE TAKING AN ELEMENT FROM THE HEAD */
int
q_pop (Q_t * q, void **data)	/* take an element from the tail */
{
  Q_ele_t *e = NULL;
  ASSERT_COOKIE (q);
  *data = NULL;
  if (q->count == 0)
    {
      return -1;
    }
  else if (q->count == 1)
    {
      e = q->head;
      *data = e->data;
      q->head = NULL;
      q->tail = NULL;
      q->count--;
      free (e);
    }
  else if (q->count > 1)
    {
      e = q->tail;
      *data = e->data;
      q->tail = e->prev;
      q->tail->next = NULL;
      q->count--;
      free (e);
    }
  else
    {
      assert (0);
    }

  ASSERT_COOKIE (q);
  return 0;
}

/* this needs to maintain order defined by comp... */
/* assumes comp returns < 0 if data < e->data; */
/* == 0 if data == e->data and > 0 if data > e->data */
int
q_insert (Q_t * q, qComparator comp, void *data)
{
  Q_ele_t *e;
  Q_ele_t *eprev;
  Q_ele_t *enew;

  ASSERT_COOKIE (q);
  assert (data);

  /* data is later in list if data > e->data */
  for (e = q->head; ((e) && (comp (data, e->data) > 0)); e = e->next)
    {
      /* VOID */ ;
    }

  if (e)
    {
      eprev = e->prev;
      if (eprev) {
	  /* data is earlier than e but later than eprev */
	  /* need to patch in a new element for data */
	  enew = _q_create_ele (data, eprev, e);

	  e->prev = enew;
	  eprev->next = enew; 
	  q->count++;
	}
      else 
	{
	  /* it's earlier than anything else there so */
	  /* just push it in at the tail of the queue... */
	  assert (e == q->head);
	  q_push (q, data);
	}
    }
  else
    {
      /* reached the end (even if it is empty) so */
      /* just append it to the tail of the queue... */
      q_append (q, data);
    }

  ASSERT_COOKIE (q);
  return 0;
}

void *
q_next (Q_t * q, Q_ele_t * e)
{
  ASSERT_COOKIE (q);
  if (e)
    {
      return e->next;
    }
  else
    {
      return NULL;
    }
}

void *
q_prev (Q_t * q, Q_ele_t * e)
{
  ASSERT_COOKIE (q);
  if (e)
    {
      return e->prev;
    }
  else
    {
      return NULL;
    }
}

int
q_walk (Q_t * q, qApply apply)
{
  Q_ele_t *e;
  int res = 0;
  ASSERT_COOKIE (q);
  for (e = q->head; e; e = e->next)
    {
      res |= apply (e->data);
    }
  ASSERT_COOKIE (q);

  return res;
}

/* like q_walk but it supports passing a pthreads-style void *arg */
/* and allows the user to break the iteration when appropriate */
int
q_iterate (Q_t * q, qApplyWithArg apply, void *arg, int base_res)
{
  Q_ele_t *e;
  ASSERT_COOKIE (q);
  for (e = q->head; e; e = e->next)
    {
      if (apply (e->data, arg, &base_res) == Q_BREAK)
	break;
    }
  ASSERT_COOKIE (q);

  return base_res;
}

/* like q_iterate but limited to range defined by start, stop and step */
int
q_range_iterate (Q_t * q, qApplyWithArg apply, void *arg, 
		 int base_res, int start, int stop, int step)
{
  Q_ele_t *e;
  int     cur, next;
  ASSERT_COOKIE (q);
  for (e = q->head, cur = 0, next = start; 
       ((e) && (next < stop)); 
       e = e->next, cur++)
    {
      if (cur == next) {
	if (apply (e->data, arg, &base_res) == Q_BREAK)
	  break;
	next += step;
      }
    }
  ASSERT_COOKIE (q);

  return base_res;
}


/* EOF */
