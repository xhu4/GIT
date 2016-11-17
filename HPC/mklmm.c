/*
 * =====================================================================================
 *
 *       Filename:  mklmm.c
 *
 *    Description:  using mkl to do matrix multiplication
 *
 *        Version:  1.0
 *        Created:  11/16/2016 03:42:06 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Xiukun Hu (), xhu4@uwyo.edu
 *   Organization:  UWyo Math Dept
 *
 * =====================================================================================
 */


#ifdef _OPENMP
    #include <omp.h>
#else
    #define omp_get_num_threads() 1;
    #define omp_get_thread_num() 0;
    #define omp_get_max_threads() 1;
#endif

#include <stdio.h>
#include <stdlib.h>
#include "matrices.h"
#include "mkl.h"

#ifndef WIDTH
#define WIDTH 30
#endif

void ClearMatrix( double** matrix, int nrows, int ncols ) {
    int i, j;
    for ( i = 0 ; i < nrows ; i++ ) 
	for ( j = 0 ; j < ncols ; j++ ) 
	    matrix[i][j] = 0;
}

int main(){

    /* Local declarations */
    const int	NTH = omp_get_max_threads();

    double	tsc[NTH];
    double	tsc1;
    double      t1;             /* Time keeper */
    double      t2;             /* Time keeper */
    double	tt1;
    double	tt;
    double	tio1;           /* Private I/O time keeper */
    double	tio  = 0;       /* Private I/O time keeper */
    double	tc1;            /* Compute time */
    double	tc   = 0;       /* Compute time */
    double	tw1;            /* Wate time */
    double	tw = 0;         /* Wate time */

    double	temp;        /* Private pointer for saving results */

    double      mrun();         /* Get timing information */

    double	**ablock[2];    /* Pointer to one block of A */
    double	**bblock[2];    /* Pointer to one block of B */
    double	**cblock[2];    /* Pointer to one block of C */

    int         acols = 0;      /* Block columns in A */
    int         arows = 0;      /* Block rows    in A */

    int		bcols = 0;      /* Block columns in B */
    int		brows = 0;      /* Block rows	 in B */

    int		ccols = 0;      /* Block columns in C */
    int		crows = 0;      /* Block rows	 in C */

    int         blk_cols = 0;   /* Columns in a block */
    int         blk_rows = 0;   /* Rows    in a block */

    int         mopt_a = 0;     /* How to allocate space in A blocks */
    int         mopt_b = 1;     /* How to allocate space in B blocks */
    int		mopt_c = 1;     /* How to allocate space in C blocks */

    int		colleft;        /* Block columns residue by WIDTH */

    int         i = 0;          /* Loop index */
    int         j = 0;          /* Loop index */
    int		k = 0;          /* Loop index */
    int		I,J,K;          /* Loop index */
    int		iplus;          /* Loop index */
    int		jplus;          /* Loop index */
    int		kplus;          /* Loop index */

    int		tog  = 0;       /* Toggle for a&bblock */
    int		ctog = 0;       /* Toggle for cblock   */

    int		TID;            /* Thread ID */

    int		ar;             /* ablock row index */
    int		ac;             /* ablock col index */
    int		rc;             
    int		nI;

    int		nThreads;

    char        c = ' ';        /* Input character */

    
    tt1 = mrun();
    
    /* Get matrix information from disk */
    matrix_info_read( &blk_rows, &blk_cols, 
	    &arows, &acols, 
	    &brows, &bcols,
	    &crows, &ccols );


    /* Preprocess message */
    colleft = blk_cols % WIDTH;                 /* Colunms left for each block over WIDTH */
    nI = blk_rows * (blk_cols / WIDTH);         /* Number of iterations for each block */
    rc = blk_cols - colleft;                    /* The starting index of the residue column */


    /* Allocate 6 block matrices (two each for A, B and C) */
    ablock[0] = block_allocate( blk_rows, blk_cols, mopt_a );
    bblock[0] = block_allocate( blk_rows, blk_cols, mopt_b );
    cblock[0] = block_allocate( blk_rows, blk_cols, mopt_c );
    ablock[1] = block_allocate( blk_rows, blk_cols, mopt_a );
    bblock[1] = block_allocate( blk_rows, blk_cols, mopt_b );
    cblock[1] = block_allocate( blk_rows, blk_cols, mopt_c );

    ClearMatrix( cblock[0], blk_rows, blk_cols );
    ClearMatrix( cblock[1], blk_rows, blk_cols );



    /* Enter parallel region */
    {

	{
	nThreads = omp_get_num_threads();
	t1  = mrun();
	}

	tc1 = t1; 

	TID = omp_get_thread_num();

	/* Single thread reading the A00 B00 for calculating */
	{
	    tio1 = mrun();
	    tc += tio1 - tc1;
	    block_readdisk( blk_rows, blk_cols, "A", 0, 0, ablock[0], mopt_a, 0 );
	    block_readdisk( blk_rows, blk_cols, "B", 0, 0, bblock[0], mopt_a, 0 );
	    tc1 = mrun();
	    tio += tc1 - tio1;
	    //printf("Thread %d reading A00 and B00 in %les\n", TID, tio);
	} // single thread reading A00 B00 

	/* Reading and calculating at the same time */
	while ( i < crows ){
	    
	    /* Get next loop's index i+, j+ and k+ */
	    kplus = (k+1) % acols;
	    jplus = (kplus==0)? ((j+1)%ccols) : j;
	    iplus = (jplus==0 && kplus==0)? i+1 : i;

	    /* Single thread reading A_i+k+ & B_k+j+ */
	    {
		if ( iplus < crows ) {
		    tio1 = mrun();
		    tc += tio1 - tc1;
		    block_readdisk( blk_rows, blk_cols, "A", iplus, kplus, ablock[1-tog], mopt_a, 0 );
		    block_readdisk( blk_rows, blk_cols, "B", kplus, jplus, bblock[1-tog], mopt_b, 0 );
		    tc1 = mrun();
		    tio += tc1 - tio1;
		}
	    }


	    if ( i == 0 && j == 0 && k == 0 ) 
		tsc1 = mrun();

	    /* Multithreads calculating A_ik x B_kj */
	    cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans,
		    blk_cols,blk_rows,blk_cols, 1, ablock[tog][0], blk_rows, bblock[tog][0], blk_cols,
		    1, cblock[ctog][0], blk_rows);

	    tw1 = mrun();
	    tc += tw1 - tc1;

	    if ( i == 0 && j == 0 && k == 0 ) 
		tsc[TID] = mrun();
	    
	    /* Barrier for reading A_i+k+ B_k+j+ and calculating A_ik x B_kj */

	    tc1 = mrun();
	    tw += tc1 - tw1;

	    /* Every thread check but single thread write to disk */
	    if ( kplus==0 ) {
		{
		    tio1 = mrun();
		    tc += tio1 - tc1;
		    block_write2disk( blk_rows, blk_cols, "C", i, j, cblock[ctog][0] );
		    ClearMatrix( cblock[ctog], blk_rows, blk_cols );
		    tc1 = mrun();
		    tio += tc1 - tio1;
		} // Write cblock: OMP single nowait

		ctog = 1-ctog; // Every thread change ctog if k+ = 0.
	    }

	    /* Every thread change to another ablock and bblock and update index */
	    tog = 1 - tog;
	    i = iplus;
	    j = jplus;
	    k = kplus;
	} /* While loop for blocks */
	//printf("Thread %d, compute for %les, io for %les, wait for %le\n", TID, tc, tio, tw);

	{
	    t2 = mrun() - t1;
	}

    }// End of parallel region

    //printf("Time in parallel region: %les\n", t2);

    for ( i = 1 ; i < nThreads ; i++ ) 
	tsc[0] = (tsc[0] < tsc[i])? tsc[i] : tsc[0];

    tt = mrun() - tt1;

    /* Print matrix info */
    printf("\n|mkl serial|\tblk:%5dx%5d, matrix:%2dx%2d, %2dThreads\n",blk_rows,blk_cols,arows,acols,nThreads);
    /* Print time */
    printf("Total time: %les\n", tt);
    printf("--------------------------------------------------------\n");

    /* End */
    return 0;
} 
