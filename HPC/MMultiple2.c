/*
 * =====================================================================================
 *
 *       Filename:  MMmultiple.c
 *
 *    Description:  Do Matrix Multiplication C = A x B with A and B blocks generated by
 *		    mkmatrices.c.
 *
 *        Version:  1.0
 *        Created:  09/21/2016 22:53:31
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Xiukun Hu 
 *   Organization:  University of Wyoming, Department of Mathematics
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include "matrices.h"


int main(){
    /* Local declarations */
    int         acols = 0;      /* Block columns in A */
    int         arows = 0;      /* Block rows    in A */

    int		bcols = 0;      /* Block columns in B */
    int		brows = 0;      /* Block rows	 in B */

    int		ccols = 0;      /* Block columns in C */
    int		crows = 0;      /* Block rows	 in C */

    int         blk_cols = 0;   /* Columns in a block */
    int         blk_rows = 0;   /* Rows    in a block */

    double**    ablock;         /* Pointer to one block of A */
    double**    bblock;         /* Pointer to one block of B */
    double**	cblock;         /* Pointer to one block of C */

    int         mopt_a = 0;     /* How to allocate space in A blocks */
    int         mopt_b = 1;     /* How to allocate space in B blocks */
    int		mopt_c = 1;     /* How to allocate space in C blocks */

    char        c = ' ';        /* Input character */
    int         opt_print = 0;  /* Print block flag */
    int		opt_gen = 0;    /* Generate Matrix flag */

    int         i;              /* Loop index */
    int         j;              /* Loop index */
    int		k;              /* Loop index */
    int		ir;             /* Loop index */
    int		ic;             /* Loop index */

    double      t1;             /* Time keeper */
    double      t2;             /* Time keeper */
    double	tc1;            /* Compute time keeper */
    double	tc2 = 0;        /* Compute time keeper */
    double      mrun();         /* Get timing information */



    /* Get matrix information from disk */
    matrix_info_read( &blk_rows, &blk_cols, 
	    &arows, &acols, 
	    &brows, &bcols,
	    &crows, &ccols );


    /* Allocate 3 block matrices (one each for A, B and C) */
    ablock = block_allocate( blk_rows, blk_cols, mopt_a );
    bblock = block_allocate( blk_rows, blk_cols, mopt_b );
    cblock = block_allocate( blk_rows, blk_cols, mopt_c );


    /* Loop over every block */
    t1 = mrun();
    for ( i= 0 ; i < crows ; i++ )
	for ( j = 0 ; j < ccols ; j++ ) {

	    /* Initialize cblock */
	    for ( ir = 0 ; ir < blk_rows ; ir++ )
		for ( ic = 0 ; ic < blk_cols ; ic++ )
		    cblock[ir][ic] = 0;

	    /* Do multiplication for block A(i,k) x B(k,j) */
	    for ( k = 0 ; k < acols ; k++ ) {
		block_readdisk( blk_rows, blk_cols, "A", i, k, ablock, mopt_a, opt_print );
		block_readdisk( blk_rows, blk_cols, "B", k, j, bblock, mopt_b, 0 );
		tc1 = mrun();
		block_multiply( blk_rows, blk_cols, ablock, bblock, cblock, 1 );
		tc2 += mrun() - tc1;
	    }

	    /* Write cblock to disk and print it */
	    block_write2disk( blk_rows, blk_cols, "C", i, j, cblock[0] );
//	    block_print( blk_rows, blk_cols, "C", i, j, cblock[0] );
	}

    /* Time */
    t2 = mrun() - t1;
    printf("\n|step2 code|\tblk:%5dx%5d, matrix:%2dx%2d, %2dThreads\n",blk_rows,blk_cols,arows,acols,1);
    printf( "Total time:%les\n", t2);
    printf("--------------------------------------------------------\n");

    /* End */
    return 0;
}
