/* -*- Mode: C; -*- */
/* Creator: Bronis de Supinski (bronis@llnl.gov) Mon Apr 15 2002 */

/* table.c -- dynamic int table
   - number of columns determined initially but can grow
   - number of rows is initially same for all columns
   - rows added on per column basis (i.e. rows per column can diverge)
   - growing rows or columns is relatively expensive
   - the data is an int, can use as index or key into other data structure
*/

#ifndef lint
static char *rcsid = "$Header: /home/cvs/cvs/xwu3/ScalaTraceV2/record/libsrc/table.c,v 1.1 2012/04/02 05:12:09 xwu3 Exp $";
#endif

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "table.h"

static const int _table_cookie = 3170107778;

#define ASSERT_COOKIE(t) {assert(t);\
 assert(_table_cookie == t->cookie);\
 assert(t->columns);\
 assert(t->default_row_count >= 0);\
 assert(t->column_count >= 0);}

#define ASSERT_COLUMN(c) {assert(c);\
 assert(c->rows);\
 assert(c->row_count >= 0);}

umpi_table_t *
table_open (int rows, int columns, int default_val)
{
  umpi_table_t        *t = NULL;
  umpi_table_column_t *col = NULL;
  int                 i, j;

  assert ((rows > 0) && (columns > 0));

  t = (umpi_table_t *) malloc (sizeof (umpi_table_t));
  t->cookie = _table_cookie;

  t->columns = (umpi_table_column_t *) 
    malloc (columns * sizeof (umpi_table_column_t));
  t->column_count = columns;
  t->default_row_count = rows;
  t->default_val = default_val;
  ASSERT_COOKIE (t);

  if (default_val == 0) {
    for (i = 0; i < columns; i++) {
      col = &(t->columns[i]);
      col->rows = (int *) calloc (sizeof(int), rows);
      col->row_count = rows;
      ASSERT_COLUMN (col);
    }
  }
  else {
    for (i = 0; i < columns; i++) {
      col = &(t->columns[i]);
      col->rows = (int *) malloc (rows * sizeof(int));
      col->row_count = rows;
      ASSERT_COLUMN (col);
      for (j = 0; j < rows; j++)
	col->rows[j] = default_val;
    }
  }

  return t;
}


int
table_close (umpi_table_t *t)
{
  umpi_table_column_t *col = NULL;
  int                 i, j;

  ASSERT_COOKIE (t);

  for (i = 0; i < t->column_count; i++) {
    col = &(t->columns[i]);
    ASSERT_COLUMN (col);
    free (col->rows);
  }

  free (t->columns);
  free (t);

  return 0;
}


/* grow t to at least new_column_count columns... */
int
table_adjust_columns (umpi_table_t *t, int new_column_count)
{
  umpi_table_column_t *new_col = NULL;
  umpi_table_column_t *col = NULL;
  int                 i, j;

  ASSERT_COOKIE (t);
  
  assert (new_column_count > 0);

  if (new_column_count < t->column_count) {
    fprintf (stderr, "WARNING: Cannot shrink column count\n");
    return UMPI_TABLE_NO_SHRINK;
  }
 
  new_col = (umpi_table_column_t *) 
    malloc (sizeof (umpi_table_column_t) * new_column_count);

  bcopy (t->columns, new_col, t->column_count * sizeof (umpi_table_column_t));

  if (t->default_val == 0) {
    for (i = 0; i < new_column_count - t->column_count; i++) {
      col = &(new_col[i + t->column_count]);
      col->rows = (int *) calloc (sizeof(int), t->default_row_count);
      col->row_count = t->default_row_count;
      ASSERT_COLUMN (col);
    }
  }
  else {
    for (i = 0; i < new_column_count - t->column_count; i++) {
      col = &(new_col[i + t->column_count]);
      col->rows = (int *) malloc (t->default_row_count * sizeof(int));
      col->row_count = t->default_row_count;
      ASSERT_COLUMN (col);
      for (j = 0; j < t->default_row_count; j++)
	col->rows[j] = t->default_val;
    }
  }

  free (t->columns);

  t->columns = new_col;
  t->column_count = new_column_count;

  return UMPI_TABLE_SUCCESS;
}  


/* grow t->columns[column] to at least new_row_count rows... */
int
table_adjust_rows (umpi_table_t *t, int column, int new_row_count)
{
  umpi_table_column_t *col = NULL;
  int                 *new_rows, i, j, retval;

  ASSERT_COOKIE (t);
  
  assert ((column >= 0) && (new_row_count > 0));

  if (column >= t->column_count) {
    retval = table_adjust_columns (t, column + 1);
    if (retval != UMPI_TABLE_SUCCESS)
      return retval;
  }

  col = &(t->columns[column]);
  ASSERT_COLUMN (col);

  if (new_row_count == col->row_count)
    return UMPI_TABLE_SUCCESS;

  new_rows = (int *) calloc (sizeof (int), new_row_count);

  if (col->row_count > new_row_count) {
    bcopy (col->rows, new_rows, sizeof (int) * new_row_count);
  }
  else {
    bcopy (col->rows, new_rows, sizeof (int) * col->row_count);
    if (t->default_val != 0) {
      for (i = 0; i < new_row_count - col->row_count; i++)
	new_rows[i + col->row_count] = t->default_val;
    }
  }

  free (col->rows);

  col->rows = new_rows;
  col->row_count = new_row_count;

  return UMPI_TABLE_SUCCESS;
}  


int
table_set_row_column (umpi_table_t *t, int row, int column, int new_val)
{
  umpi_table_column_t *col = NULL;

  ASSERT_COOKIE (t);

  assert ((row >= 0) && (column >= 0));

  if (column >= t->column_count)
    table_adjust_columns (t, column + 1);

  col = &(t->columns[column]);
  ASSERT_COLUMN (col);

  if (row >= col->row_count)
    table_adjust_rows (t, column, row + 1);

  col->rows[row] = new_val;

  return new_val;
}


int
table_copy_sub_column (umpi_table_t *t, int from_column, 
		       int to_column, int start_row, int end_row)
{
  umpi_table_column_t *from_col = NULL;
  umpi_table_column_t *to_col = NULL;

  ASSERT_COOKIE (t);

  assert ((start_row >= 0) && (end_row >= start_row) && 
	  (from_column >= 0) && from_column < t->column_count);

  if (to_column >= t->column_count)
    table_adjust_columns (t, to_column + 1);

  from_col = &(t->columns[from_column]);
  to_col = &(t->columns[to_column]);
  ASSERT_COLUMN ((from_col) && (to_col));

  assert (end_row < from_col->row_count);

  if (end_row >= to_col->row_count)
    table_adjust_rows (t, to_column, end_row + 1);

  bcopy (&(from_col->rows[start_row]), &(to_col->rows[start_row]), 
	 (end_row - start_row + 1) * sizeof (int));

  return UMPI_TABLE_SUCCESS;
}


#ifndef UMPI_HAND_INLINING

int
table_get_row_column (umpi_table_t *t, int row, int column)
{
  umpi_table_column_t *col = NULL;

  ASSERT_COOKIE (t);

  if (column >= t->column_count)
    return t->default_val;

  col = &(t->columns[column]);
  ASSERT_COLUMN (col);

  if (row >= col->row_count)
    return t->default_val;

  return col->rows[row];
}

#endif

/* EOF */
