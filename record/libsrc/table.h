/* -*- Mode: C; -*- */
/* Creator: Bronis de Supinski (bronis@llnl.gov) Mon Apr 15 2002 */
/* $Header: /home/cvs/cvs/xwu3/ScalaTraceV2/record/libsrc/table.h,v 1.1 2012/04/02 05:12:09 xwu3 Exp $ */
/* table.h -- queue header */

#ifndef _UMPI_TABLE_H
#define _UMPI_TABLE_H

#define UMPI_TABLE_SUCCESS    0
#define UMPI_TABLE_NO_SHRINK -1

typedef struct umpi_table_column
{
  int row_count;
  int *rows;
}
umpi_table_column_t;

typedef struct umpi_table
{
  int cookie;
  int column_count;
  int default_val;
  int default_row_count;
  umpi_table_column_t *columns;
}
umpi_table_t;

umpi_table_t *table_open (int rows, int columns, int default_val);
int table_close (umpi_table_t *t);
int table_adjust_columns (umpi_table_t *t, int new_column_count);
int table_adjust_rows (umpi_table_t *t, int column, int new_row_count);
int table_set_row_column (umpi_table_t *t, int row, int column, int new_val);
int table_copy_sub_column (umpi_table_t *t, int from_column, 
			   int to_column, int start_row, int end_row);


#ifndef UMPI_HAND_INLINING
int table_get_row_column (umpi_table_t *t, int row, int column);
#endif

#endif

/* EOF */
