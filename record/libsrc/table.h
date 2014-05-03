/*
 * Copyright (c) 2002-May 2, 2014: Bronis de Supinski (bronis@llnl.gov)
 * Copyright (c) of subsequent modifications of this code:
 * Frederic Suter, CNRS / IN2P3 Computing Center <fsuter@cc.in2p3.fr>
 * Henri Casanova, ICS Dept., University of Hawai`i at Manoa <henric@hawaii.edu>

 * The license of the original code is unknown. Modifications are made
 * under the terms of the GNU LGPL license.
 */

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
