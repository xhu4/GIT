#include <stdlib.h>
#include <stdio.h>

#include "matrices.h"


int mkmatrices () {

    /* Local declarations */
    int         acols = 0;      /* Block columns in A */
    int         arows = 0;      /* Block rows    in A */

    int         bcols = 0;      /* Block columns in B */

    int         blk_cols = 0;   /* Columns in a block */
    int         blk_rows = 0;   /* Rows    in a block */

    double**    ablock;         /* Pointer to one block of A */
    double**    bblock;         /* Pointer to one block of B */

    int         mopt_a = 0;     /* How to allocate space in A blocks */
    int         mopt_b = 1;     /* How to allocate space in B blocks */

    char        c = ' ';        /* Input character */
    int         opt_ran = 0;    /* Random block flag */
    int         opt_print = 0;  /* Print block flag */

    int         i;              /* Loop index */
    int         j;              /* Loop index */

    double      t1;             /* Time keeper */
    double      t2;             /* Time keeper */
    double      mrun();         /* Get timing information */


#ifdef GOOD_SEED
    /* Get a good random number seed */
    struct timeval t1;  /* holder for time of day (seconds, microseconds) */
    gettimeofday( &t1, NULL );
    srand( t1.tv_usec * t1.tv_sec );
#endif


    /* Get dimensions for blocks, A, and B */
    printf( "Rows in a block: " );
    scanf( "%d", &blk_rows );
    printf( "Columns in a block: " );
    scanf( "%d", &blk_cols );
    printf( "Number of block rows in A: " );
    scanf( "%d", &arows );
    printf( "Number of block columns in A: " );
    scanf( "%d", &acols );
    printf( "Number of block columns in B: " );
    scanf( "%d", &bcols );
    c = getchar();  /* remove return from stdin stream */

    /* Save the block and matrix dimensions */
    matrix_info_write( blk_rows, blk_cols, arows, acols, bcols );

    /* Allocate 2 block matrices (one each for A and B) */
    ablock = block_allocate( blk_rows, blk_cols, mopt_a );
    bblock = block_allocate( blk_rows, blk_cols, mopt_b );
    if ( ablock == NULL || bblock == NULL ) {
        fprintf( stderr, "Could not allocate blocks for A or B\n" );
        return 1;
        }

    /* Determine if blocks will be random or predictable */
    printf( "Do you want random numbers in your matrices (yY or other)? " );
    fflush( stdout );
    c = getchar();
    opt_ran = c == 'y' || c == 'Y';
    printf( "opt_ran = %d\n", opt_ran );
    c = getchar();  /* remove return from stdin stream */

    /* Determine if blocks will be printed or not */
    printf( "\n***** Generating A and B blocks *****\n" );
    printf( "Print blocks (yY or other)? " );
    c = getchar();
    opt_print = c == 'y' || c == 'Y';
    printf( "opt_print = %d\n", opt_print );
    c = getchar();  /* remove return from stdin stream */

    /* Generate the A and B blocks, one at a time */
    t1 = mrun();
    for ( i = 0 ; i < arows ; i++ )
        for ( j = 0 ; j < acols ; j++ )
            if ( block_generate( blk_rows, blk_cols,
                                 "A", i, j,
                                 ablock, mopt_a,
                                 opt_ran, opt_print ) ) {
                fprintf( stderr,
                         "main: Could not generate A block (%d,%d)\n", 
                         i, j );
                return 2;
                }
    for ( i = 0 ; i < acols ; i++ )
        for ( j = 0 ; j < bcols ; j++ )
            if ( block_generate( blk_rows, blk_cols, 
                                 "B", i, j,
                                 bblock, mopt_b,
                                 opt_ran, opt_print ) ) {
                fprintf( stderr,
                         "main: Could not generate B block (%d,%d)\n", 
                         i, j );
                return 3;
                }
    t2 = mrun() - t1;
    printf( "Matrices A and B were generated successfully in %le seconds\n",
            t2  );

    /* Determine if blocks will be printed or not */
    printf( "\n***** Reading A and B blocks *****\n" );
    printf( "Print blocks (yY or other)? " );
    c = getchar();
    opt_print = c == 'y' || c == 'Y';
    printf( "opt_print = %d\n", opt_print );
    c = getchar();  /* remove return from stdin stream */

    /* Read and print the A and B blocks, one at a time */
    t1 = mrun();
    for ( i = 0 ; i < arows ; i++ )
        for ( j = 0 ; j < acols ; j++ )
            if ( block_readdisk( blk_rows, blk_cols,
                                 "A", i, j,
                                 ablock, mopt_a,
                                 opt_print ) ) {
                fprintf( stderr,
                         "main: Could not read A block (%d,%d)\n", 
                         i, j );
                return 4;
                }
    for ( i = 0 ; i < acols ; i++ )
        for ( j = 0 ; j < bcols ; j++ )
            if ( block_readdisk( blk_rows, blk_cols, 
                                 "B", i, j,
                                 bblock, mopt_b,
                                 opt_print ) ) {
                fprintf( stderr,
                         "main: Could not read B block (%d,%d)\n", 
                         i, j );
                return 4;
                }
    t2 = mrun() - t1;
    printf( "Matrices A and B were read successfully in %le seconds\n", t2 );

    /* That's it, folks! */
    return 0;
    }
