/* Hi, everybody!
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
#ifdef _OPENMP
    #include <omp.h>
#else
    #define omp_get_num_threads() 1;
    #define omp_get_thread_num() 0;
    #define omp_get_max_threads() 1;
#endif

/* Cache Info */
#ifndef mc
    #define mc 8
#endif

#ifndef kc
    #define kc 8
#endif

#ifndef mr
    #define mr 4
#endif

#ifndef nr
    #define nr 4
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "matrices.h"

#ifndef WIDTH
#define WIDTH 30
#endif

void ClearMatrix( double** matrix, int nrows, int ncols ) {
    int i, j;
    for ( i = 0 ; i < nrows ; i++ ) 
	for ( j = 0 ; j < ncols ; j++ ) 
	    matrix[i][j] = 0;
}

void GEPB_OPT1(const double ** A_block, const int Acol, const double * B_rpanel_packed, double ** C_rpanel, const int blk_cols);
void GEPP_BLK_VAR1(const double ** A_cpanel, const int Acol, const double ** B_rpanel, double ** C, const int blk_cols);

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

    double      mrun();         /* Get timing information */

    double	**ablock[2];    /* Pointer to one block of A */
    double	**bblock[2];    /* Pointer to one block of B */
    double	***C_local;

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

    int		TID;            /* Thread ID */

    int		rc;
    int		nI;

    int		nThreads;


    
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
    ablock[1] = block_allocate( blk_rows, blk_cols, mopt_a );
    bblock[1] = block_allocate( blk_rows, blk_cols, mopt_b );


    /* Enter parallel region */
    #pragma omp parallel default(none)  \
    shared(blk_cols, blk_rows,	    \
	    ablock, bblock, C_local,\
	    mopt_a, mopt_b, mopt_c, \
	    acols, crows, ccols,    \
	    colleft, nI, nThreads,  \
	    rc, t1, t2, tsc, tsc1)  \
    firstprivate( tog, i, j, k, tio, tc, tw ) \
    private( TID, I, J, K, iplus, jplus, kplus, tio1, tc1, tw1 )
    {

	#pragma omp single
	{
	nThreads = omp_get_num_threads();
	if ( (C_local = (double ***) malloc( nThreads * sizeof(double **) )) == NULL ) {
	    perror("memory allocation for C_local");
	    free(C_local);
	}
	t1  = mrun();
	}


	tc1 = t1; 

	TID = omp_get_thread_num();

	C_local[TID] = block_allocate( blk_rows, blk_cols, mopt_c);
	ClearMatrix( C_local[TID], blk_rows, blk_cols );

	/* Single thread reading the A00 B00 for calculating */
	#pragma omp single
	{
	    tio1 = mrun();
	    tc += tio1 - tc1;
	    block_readdisk( blk_rows, blk_cols, "A", 0, 0, ablock[0], mopt_a, 0 );
	    block_readdisk( blk_rows, blk_cols, "B", 0, 0, bblock[0], mopt_b, 0 );
	    tc1 = mrun();
	    tio += tc1 - tio1;
	    printf("Thread %d reading A00 and B00 in %les\n", TID, tio);
	} // single thread reading A00 B00 

	/* Reading and calculating at the same time */
	while ( i < crows ){
	    
	    /* Get next loop's index i+, j+ and k+ */
	    kplus = (k+1) % acols;
	    jplus = (kplus==0)? ((j+1)%ccols) : j;
	    iplus = (jplus==0 && kplus==0)? i+1 : i;

	    /* Single thread reading A_i+k+ & B_k+j+ */
	    #pragma omp single nowait
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


	    #pragma omp single nowait
	    if ( i == 0 && j == 0 && k == 0 ) 
		tsc1 = mrun();

	    /* Multithreads calculating A_ik x B_kj */
	    #pragma omp for nowait schedule(dynamic)
	    for ( I = 0 ; I < blk_cols ; I += kc ) {
		GEPP_BLK_VAR1( (const double**)ablock[tog], I, (const double**)(bblock[tog]+I), C_local[TID], blk_cols );
	    }

	    tw1 = mrun();
	    tc += tw1 - tc1;

	    if ( i == 0 && j == 0 && k == 0 ) 
		tsc[TID] = mrun();

	    /* Barrier for reading A_i+k+ B_k+j+ and calculating A_ik x B_kj */
	    #pragma omp barrier

	    tc1 = mrun();
	    tw += tc1 - tw1;

	    /* Every thread check but single thread write to disk */
	    if ( kplus==0 ) {
		for ( I = 2 ; I <= nThreads ; I<<=1 ) {
		    if ( ! (TID % I) ) 
			if ( TID+I <= nThreads ) 
			    for ( J = 0 ; J < blk_rows ; J++ ) 
				for ( K = 0 ; K < blk_cols ; K++ ) 
				    C_local[TID][J][K] += C_local[TID+I/2][J][K];
		    #pragma omp barrier
		}

		#pragma omp master
		{
		    tio1 = mrun();
		    tc += tio1 - tc1;
		    block_write2disk( blk_rows, blk_cols, "C", i, j, C_local[TID][0] );
		    tc1 = mrun();
		    tio += tc1 - tio1;
		} // Write C: OMP master

		ClearMatrix( C_local[TID], blk_rows, blk_cols );
	    }

	    /* Every thread change to another ablock and bblock and update index */
	    tog = 1 - tog;
	    i = iplus;
	    j = jplus;
	    k = kplus;
	} /* While loop for blocks */
	printf("Thread %d, compute for %les, io for %les, wait for %le\n", TID, tc, tio, tw);

	#pragma omp master
	{
	    t2 = mrun() - t1;
	}

    }// End of parallel region

    printf("Time in parallel region: %les\n", t2);

    for ( i = 1 ; i < nThreads ; i++ ) 
	tsc[0] = (tsc[0] < tsc[i])? tsc[i] : tsc[0];

    tt = mrun() - tt1;

    /* Print time */
    printf("Total time: %les\n", tt);
    printf("Time for multiplying A00 x B00 in parallel: %le\n", tsc[0]-tsc1);

    /* End */
    return 0;
} 

void GEPB_OPT1(const double ** A_block, const int Acol, const double * B_rpanel_packed, double ** C_rpanel, const int blk_cols) {
    /* Local declarations */
    double  A_block_packed[mc*kc];              /* packed A_block stored in cache (hopefully) */
    double  C_aux[mr*nr];
    const double  *ap, *bp = B_rpanel_packed;
    double  *cpa;
    int	    i,j,k,M,N;                          /* Loop index */
    int	    count=0;

    /* Pack A_block into A_block_packed */
    for ( i = 0 ; i < mc ; i++ ) {
	memcpy( (void *)(A_block_packed+i*kc), (void *)(A_block[i]+Acol), kc * sizeof(double) );
    }

    cpa = C_aux;
    /* For each column */
    for ( N = 0 ; N < blk_cols ; N+=nr ) {
	ap = A_block_packed;
	bp = bp + count;
	/* For each mr rows */
	for ( M = 0 ; M < mc ; M+=mr ) {
	    /* Multiply */
	    for ( i = 0 ; i < mr ; i++ ) {
		count = 0;
		for ( j = 0 ; j < nr ; j++ ) {
		    *cpa = *ap * bp[count++];
		    for ( k = 1 ; k < kc ; k++ ) 
			*cpa += *(ap+k) * *(bp+(count++));
		    cpa++;
		}
		ap += kc;
	    }
	    for ( i = mr-1 ; i >= 0 ; i-- ) 
		for ( j = nr-1 ; j >= 0 ; j-- ) 
		    C_rpanel[M+i][N+j] += *(--cpa);
	}
    }
}

void GEPP_BLK_VAR1(const double ** A_cpanel, const int Acol, const double ** B_rpanel, double ** C, const int blk_cols) {
    /* Local declarations */
    double  *B_rpanel_packed, *bp;
    int	    i,j,k;                              /* loop index */

    if ( (B_rpanel_packed = (double *) malloc( blk_cols*kc * sizeof(double) )) == NULL ) {
	perror("memory allocation for bp");
	free(B_rpanel_packed);
    }

    bp = B_rpanel_packed;

    /* Pack B_rpanel into B_rpanel_packed */
    for ( i = 0 ; i < blk_cols ; i += nr ) {
	for ( j = 0 ; j < nr ; j++ ) {
	    for ( k = 0 ; k < kc ; k++ ) {
		*(bp++) = B_rpanel[k][i+j];
	    }
	}
    }

    for ( i = 0 ; i < blk_cols ; i+=mc ) {
	GEPB_OPT1( A_cpanel+i, Acol, B_rpanel_packed, C+i, blk_cols );
    }
    free(B_rpanel_packed);
}
