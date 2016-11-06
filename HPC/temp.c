/*
 * =====================================================================================
 *
 *       Filename:  temp.c
 *
 *    Description:  for test
 *
 *        Version:  1.0
 *        Created:  11/05/2016 19:58:51
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Xiukun Hu (xhu), xiukun.hu@outlook.com
 *   Organization:  University of Wyoming, Math Dept.
 *
 * =====================================================================================
 */

#ifndef mc
    #define mc 256
#endif

#ifndef kc
    #define kc 128
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

void GEPB_OPT1(const double ** A_block, const int Acol, const double * B_rpanel_packed, double ** C_rpanel, const int blk_cols) {
    /* Local declarations */
    double  A_block_packed[mc*kc];              /* packed A_block stored in cache (hopefully) */
    double  C_aux[mr*nr];
    const double  *ap, *bp;
    double  *cp,*cpa;
    int	    i,j,k,M,N;                          /* Loop index */
    int	    count=0;

    /* Pack A_block into A_block_packed */
    for ( i = 0 ; i < mc ; i++ ) {
	memcpy( (void *)(A_block_packed+i*kc), (void *)(A_block[i]+Acol), kc * sizeof(double) );
    }

    /* For each column */
    for ( N = 0 ; N < blk_cols ; N+=nr ) {
	ap = A_block_packed;
	bp = B_rpanel_packed + count;
	/* For each mr rows */
	for ( M = 0 ; M < mc ; M+=mr ) {
	    cpa = C_aux;
	    /* Multiply */
	    for ( i = 0 ; i < mr ; i++ ) {
		count = 0;
		cp = C_rpanel[M+i]+N;
		for ( j = 0 ; j < nr ; j++ ) {
		    *cpa = *ap * bp[count++];
		    for ( k = 1 ; k < kc ; k++ ) 
			*cpa += *(ap+k) * bp[count++];
		    /* Write to C */
		    *(cp++) += *(cpa++);
		}
		ap += kc;
	    }
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
		*(bp++) = B_rpanel[k][i*nr+j];
	    }
	}
    }

    for ( i = 0 ; i < blk_cols ; i+=mc ) {
	GEPB_OPT1( A_cpanel+i, Acol, B_rpanel_packed, C+i, blk_cols );
    }
}
#define m 512
#define n 256

int main(){
    double **a;
    double **b;
    double **c;
    int i,j;

    if ( (a = (double **) malloc( m * sizeof(double *) )) == NULL ) {
	perror("memory allocation for a");
	free(a);
	return -1;
    }
    if ( (b = (double **) malloc( n * sizeof(double *) )) == NULL ) {
	perror("memory allocation for b");
	free(b);
	return -1;
    }
    if ( (c = (double **) malloc( m * sizeof(double *) )) == NULL ) {
	perror("memory allocation for c");
	return -1;
    }
    for ( i = 0 ; i < m ; i++ ) {
	if ( (a[i] = (double *) malloc( n * sizeof(double) )) == NULL ) {
	    perror("memory allocation for a[i]");
	    return -1;
	}
	if ( (c[i] = (double *) calloc( m, sizeof(double) )) == NULL ) {
	    perror("memory allocation for c[i]");
	    return -1;
	}
    }
    for ( j=0 ; j < n ; j++ ) 
	if ( (b[j] = (double *) malloc( m * sizeof(double) )) == NULL ) {
	    perror("memory allocation for b[j]");
	    return -1;
	}

    for ( i = 0 ; i < m ; i++ )
	for ( j = 0 ; j < n ; j++ )
	    a[i][j]=(b[j][i]=i);
    GEPP_BLK_VAR1((const double**)a,0,(const double**)b,(double**)c,1024);
    for ( i = 0 ; i < m ; i++ ) {
	for ( j = 0 ; j < m ; j++ )
	    printf("%8.2e ",c[i][j]);
	printf("\n");
    }
}
