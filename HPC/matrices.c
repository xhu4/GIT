#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "matrices.h"


#define LEN_FILENAME    256

char    file_info[] = { "matrices.txt" };   /* Matrix information file */

/***************************************************************************** 
 *
 *                             matrix_info_write
 *
 *  Write to a disk file the information about the block sizes and how many
 *  rows and columns (block and actual) will be in three matrices forming
 *
 *      C = AB, where
 *
 *  the number of rows in A and C are the same and the number of columns of A
 *  and C are the same as the number of rows of B (this is a mathematical
 *  requirement to do the multiplication.
 *
 *  The input variable are
 *
 *      blk_rows    Number of rows in a block.
 *      blk_cols    Number of columns in a block.
 *      arows       Number of rows of blocks in A
 *      acols       Number of columns of blocks in A
 *      bcols       Number of columns of blocks in B
 *
 *  The functions returns 0 for success and 1 otherwise.
 *
 *  The routine always returns.
 *
 *****************************************************************************/

int matrix_info_write( int blk_rows, int blk_cols,
                       int arows, int acols, int bcols ) {

    /* Local declarations */
    int     tacols = 0;         /* Total columns in A */
    int     tarows = 0;         /* Total rows    in A */
    int     tbcols = 0;         /* Total columns in B */
    int     tbrows = 0;         /* Total rows    in B */
    int     tccols = 0;         /* Total columns in C */
    int     tcrows = 0;         /* Total rows    in C */

    FILE*   fp;                 /* File pointer */

    /* Try to open the information file */
    if ( (fp = fopen( file_info, "w" )) == NULL ) {
        fprintf( stderr, "matrix_info_save: Could not open %s\n", file_info );
        return 1;
        }

    /* Save to disk the information */
    rewind( fp );
    fprintf( fp, "%d %d\n", blk_rows, blk_cols );
    fprintf( fp, "%d %d %d\n", arows, acols, bcols );

    /* Calculate the actual matrix dimensions */
    tarows = tcrows = arows * blk_rows;
    tacols = tbrows = acols * blk_cols;
    tbcols = tccols = bcols * blk_cols;
    printf( "Matrix        Rows     Columns\n");
    printf( "  A     %10d  %10d\n", tarows, tacols );
    printf( "  B     %10d  %10d\n", tbrows, tbcols );
    printf( "  C     %10d  %10d\n", tcrows, tccols );

    /* Save the actual matrix sizes */
    fprintf( fp, "%d %d\n", tarows, tacols );
    fprintf( fp, "%d %d\n", tbrows, tbcols );
    fprintf( fp, "%d %d\n", tcrows, tccols );

    /* Close the file and return */
    fclose( fp );
    return 0;
    }


/***************************************************************************** 
 *
 *                             matrix_info_read
 *
 *  Save to a disk file the information about the block sizes and how many
 *  rows and columns (block and actual) will be in three matrices forming
 *
 *      C = AB, where
 *
 *  the number of rows in A and C are the same and the number of columns of A
 *  and C are the same as the number of rows of B (this is a mathematical
 *  requirement to do the multiplication.
 *
 *  The input variable are
 *
 *      blk_rows    Pointer to number of rows in a block.
 *      blk_cols    Pointer to number of columns in a block.
 *      arows       Pointer to number of actual rows in A
 *      acols       Pointer to number of actual columns in A
 *      bcols       Pointer to number of actual columns in B
 *
 *  The pointers are used to place the values from the disk file into variables
 *  in the calling program.
 *
 *  The functions returns 0 for success and 1 otherwise.
 *
 *  The routine always returns.
 *
 *****************************************************************************/

int matrix_info_read( int *blk_rows, int *blk_cols,
                      int *arows, int *acols,
                      int *brows, int *bcols,
                      int *crows, int *ccols ) {

    /* Local declarations */
    FILE*   fp;                 /* File pointer */

    /* Try to open the information file */
    if ( (fp = fopen( file_info, "r" )) == NULL ) {
        fprintf( stderr, "matrix_info_save: Could not open %s\n", file_info );
        return 1;
        }
    rewind( fp );

    /* Get the values from the disk file */
    fscanf( fp, "%d %d", blk_rows, blk_cols );
    fscanf( fp, "%d %d %d", arows, acols, bcols );

    /* Get the actual matrix sizes */
//    fscanf( fp, "%d %d", arows, acols );
//    fscanf( fp, "%d %d", brows, bcols );
//    fscanf( fp, "%d %d", crows, ccols );

    /* Get the rest values */
    *brows = *acols;
    *crows = *arows;
    *ccols = *bcols;

    /* Close the file and return */
    fclose( fp );
    return 0;
    }


/***************************************************************************** 
 *
 *                              block_filename
 *
 *  Construct the disk file name for a block of a matrix:
 *
 *  The input variables are
 *      mname       The name of the block (disk file name).
 *      row         Which block row.
 *      col         Which block column.
 *      file_name   Address of where to write the file name.
 *      flen        sizeof(file_name) in char's.
 *
 *  The functions returns 0 for success and 1 otherwise.
 *
 *  The routine always returns.
 *
 *****************************************************************************/

int block_filename( char* mname, int row, int col, char* file_name,
                    int flen ) {

    /* Local declarations */
    int         l;                  /* Length of file name */

    /* Check that there is enough space to create the name */
    sprintf( file_name, "%d", row );
    l = strlen( file_name );
    sprintf( file_name, "%d", col );
    l += strlen( file_name );
    if ( strlen( mname ) + l + 2 >= LEN_FILENAME ) {
        fprintf( stderr, "block_filename: mname too long (%s)\n", mname );
        return 1;
        }

    /* Construct the disk file name */
    sprintf( file_name, "%s_%d_%d.bin", mname, row, col );
    return 0;
    }


/***************************************************************************** 
 *
 *                               block_print
 *
 *  Print a block from a matrix.
 *
 *  The input variables are
 *      blk_rows    Number of rows in a block.
 *      blk_cols    Number of columns in a block.
 *      mname       The name of the block (disk file name).
 *      row         Which block row to print.
 *      col         Which block column to print.
 *      pdata       Pointer to blk_rows * blk_cols of continuous doubles.
 *
 *  There is nothing returned by the function.
 *
 *  The routine always returns.
 *
 *****************************************************************************/

void block_print( int blk_rows, int blk_cols,
                  char* mname, int row, int col,
                  double* pdata ) {

    /* Local declarations */
    int         i;                  /* Loop index */
    int         j;                  /* Loop index */

    /* Print a heading with block information in it */
    printf( "block_print: %s.%d.%d -- blk_rows %d, blk_cols %d\n\n",
            mname, row, col, blk_rows, blk_cols );

    /* Print out a heading for the block's data */
    printf( "column:   " );
    for( j = 0 ; j < blk_cols ; j++ )
        printf( "  %15d", j );

    /* Print the data out, row by row */
    for ( i = 0 ; i < blk_rows ; i++ ) {
//        printf( "\nrow %4d: ", i );
        printf( "\n" );
        for( j = 0 ; j < blk_cols ; j++ )
            printf( "  %15.8le", *pdata++ );
        }

    /* All done */
    printf( "\n\n" );

    }


/***************************************************************************** 
 *
 *                             block_write2disk
 *
 *  Writes a block matrix to a disk file:
 *
 *  The input variables are
 *
 *      blk_rows    Number of rows in a block.
 *      blk_cols    Number of columns in a block.
 *      mname       The name of the block (disk file name).
 *      row         Which block row to print.
 *      col         Which block column to print.
 *      pdata       Pointer to blk_rows * blk_cols of continuous doubles.
 *
 *  The data is written to disk in one write as a blk_rows * blk_cols
 *  continuous block of memory.
 *
 *  The functions returns 0 for success and 1 or 2 otherwise.
 *
 *  The routine always returns.
 *
 *****************************************************************************/

int block_write2disk( int blk_rows, int blk_cols,
                      char* mname, int row, int col,
                      double* pdata ) {

    /* Local declarations */
    int         i;                          /* Loop index */
    int         j;                          /* Loop index */
    int         bw;                         /* Records written */
    int         ww;                         /* Records to write */
    double*     p;                          /* Pointer to a double */

    char        file_name[LEN_FILENAME];    /* Disk file name */
    FILE*       fp;                         /* File pointer */

    /* Create the file name */
    block_filename( mname, row, col, file_name, LEN_FILENAME );

    /* Open the file */
    if ( (fp = fopen( file_name, "w" )) == NULL ) {
        fprintf( stderr, "block_write2disk: Cannot open %s for writing\n",
                 file_name );
        return 1;
        }

    /* Write the data to disk in one record */
    rewind( fp );
    bw = (int)fwrite( pdata, sizeof(double),
                      (size_t)(ww = blk_rows * blk_cols), fp );
    if ( bw < ww ) {
        fprintf( stderr, "block_write2disk: Wrote %d/%d of data\n", bw, ww );
        return 2;
        }

    /* Completed successfully */
    fclose( fp );
    return 0;
    }


/***************************************************************************** 
 *
 *                              block_readdisk
 *
 *  Reads a block matrix from a disk file:
 *
 *  The input variables are
 *
 *      blk_rows    Number of rows in a block.
 *      blk_cols    Number of columns in a block.
 *      mname       The name of the block (disk file name).
 *      row         Which block row to print.
 *      col         Which block column to print.
 *      mptr        Pointer to blk_rows * blk_cols of continuous doubles.
 *      opt_print   Nonzero to print the blocks.
 *
 *  The data is read to memory pointed by mptr.
 *
 *  The functions returns 0 for success and 1-4 otherwise.
 *
 *  The routine always returns.
 *
 *****************************************************************************/

int block_readdisk( int blk_rows, int blk_cols,
                    char* mname, int row, int col,
                    double** mptr, int mopt,
                    int opt_print ) {

    /* Local declarations */
    char        file_name[LEN_FILENAME];    /* Disk file name */
    int         i;                  /* Loop index */
    int         j;                  /* Loop index */
    int         bw;                 /* Records read */
    int         ww;                 /* Records to read */
    double*     p;                  /* Pointer to a double */
    double*     pr;                 /* Pointer to a row of doubles */
    double*     pdata;              /* Pointer to a block of doubles */
    FILE*       fp;                 /* File pointer */

    /* Check matrix option first */
    if ( ! ( mopt == 0 || mopt == 1 ) ) {
        fprintf( stderr, "block_readdisk: mopt (%d) must be 0 or 1\n", mopt );
        return 1;
        }

    /* Determine where to store the block from disk */
    switch ( mopt ) {
        case 0:
            if ( (pdata = (double*)malloc( blk_rows * blk_cols *
                                           sizeof( double ) )) == NULL ) {
                fprintf( stderr,
                         "block_readisk: Could not allocate %d X %d block\n",
                         blk_rows, blk_cols );
                return 2;
                }
            break;

        case 1:
            pdata = mptr[0];
            break;
        }

    /* Create the file name */
    block_filename( mname, row, col, file_name, LEN_FILENAME );

    /* Open the file */
    if ( (fp = fopen( file_name, "r" )) == NULL ) {
        fprintf( stderr, "block_readdisk: Cannot open %s for reading\n",
                 file_name );
        return 3;
        }

    /* Read the data to disk in one record */
    rewind( fp );
    bw = (int)fread( pdata, sizeof(double),
                     (size_t)(ww = blk_rows * blk_cols), fp );
    fclose( fp );
    if ( bw < ww ) {
        fprintf( stderr, "block_readdisk: Read %d/%d of data\n", bw, ww );
        return 4;
        }

    /* Possibly print the contents */
    if ( opt_print )
        block_print( blk_rows, blk_cols, mname, row, col, pdata );

    /* Unpack the continuous data for C style arrays */
    if ( mopt == 0 ) {
        for ( p = pdata, i = 0 ; i < blk_rows ; i++ )
            for ( pr = mptr[i], j = 0 ; j < blk_cols ; j++ )
                *pr++ = *p++;
        free ( pdata );
        }

    /* Completed successfully */
    return 0;
    }


/***************************************************************************** 
 *
 *                              block_allocate
 *
 *  Allocate a block matrix using one of two methods:
 *
 *  The input variables are
 *      blk_rows    Number of rows in a block.
 *      blk_cols    Number of columns in a block.
 *      opt_ran     Nonzero for random entries in the block.
 *      opt_print   Nonzero to print the blocks.
 *
 *  The pointer to the array (or NULL) is returned.
 *
 *  The routine always returns.
 *
 *****************************************************************************/

double** block_allocate( int blk_rows, int blk_cols, int mopt ) {

    /* Local declarations */
    int         i;                  /* Loop index */
    int         j;                  /* Loop index */
    double**    pb;                 /* Pointer to array */
    double*     p;                  /* Pointer to one or more doubles */

    /* Check option first */
    if ( ! ( mopt == 0 || mopt == 1 ) ) {
        fprintf( stderr, "block_allocate: mopt (%d) must be 0 or 1\n", mopt );
        return NULL;
        }

    /* Allocate the pointer to the array */
    if ( (pb = (double**)malloc( blk_rows * sizeof(double*) )) == NULL ) {
        fprintf( stderr, "block_allocate: Could allocate array\n" );
        return NULL;
        }

    switch ( mopt ) {

        /* Allocate rows one at a time */
        case 0:
            for ( i = 0 ; i < blk_rows ; i++ )
                if ( (pb[i] = (double*)malloc( blk_cols * sizeof(double) )) == NULL ) {
                    fprintf( stderr,
                             "block_allocate: Could allocate row %d of %d\n",
                             i, blk_rows );
                    for ( j = i-1 ; j >= 0 ; j-- )
                        free( pb[j] );
                    free( pb );
                    return NULL;
                    }
            break;

        /* Allocate one big block and then construct edge vector */
        case 1:
            if ( (p = (double*)malloc( blk_rows * blk_cols *
                                       sizeof(double) )) == NULL ) {
                fprintf( stderr,
                         "block_allocate: Could not allocate %d X %d block\n",
                         blk_rows, blk_cols );
                free( pb );
                return NULL;
                }
            for( i = 0 ; i < blk_rows ; i++, p += blk_cols )
                pb[i] = p;
            break;
        }

    /* All done: return the pointer to the block array */
    return pb;
    }


/***************************************************************************** 
 *
 *                              block_generate
 *
 *  Generate a block matrix using one of two methods:
 *
 *  The input variables are
 *
 *      blk_rows    Number of rows in a block.
 *      blk_cols    Number of columns in a block.
 *      mname       Matrix name
 *      row         Which block row to generate.
 *      col         Which block column to generate.
 *      mptr        Pointer returned by block_allocate for array
 *      mopt        If mopt = 0, then the matrix is allocated one row at a
 *                  time similar to what C might do.
 *                  If mopt = 1, then the matrix is allocated as only one
 *                  block of memory and we construct row (edge) pointers.
 *      opt_ran     Nonzero for random entries in the block.
 *      opt_print   Nonzero to print the blocks.
 *
 *  The actal file name on disk will mname.i.j with the values filled in.
 *  The complete file name is assumed to be less than LEN_FILENAME characters.
 *
 *  The data is written to disk in one write as a blk_rows * blk_cols
 *  continuous block of memory.
 *
 *  The function returns the data placed on disk in the array pointed to by
 *  mptr (which must have been allocated by block_allocate or its equivalent).
 *
 *  The functions returns 0 for success and 1 otherwise.
 *
 *  The routine always returns.
 *
 *****************************************************************************/

int block_generate( int blk_rows, int blk_cols,
                    char* mname, int row, int col,
                    double** mptr, int mopt, 
                    int opt_ran, int opt_print ) {

    /* Local declarations */
    int         i;                  /* Loop index */
    int         j;                  /* Loop index */
    double*     p;                  /* Pointer to a double */
    double*     pb;                 /* Pointer to a block of doubles */
    double*     pr;                 /* Pointer to a row of doubles */
    double      val;                /* Predictable value */

// printf( "Entering block_generate\n" );

    /* Generate the block and store it in the right format */
    if ( mopt )
        pb = mptr[0];
    else if ( (pb = (double*)malloc( blk_rows * blk_cols * sizeof( double ) )) == NULL ) {
        fprintf( stderr, "block_generate: could not allocate memory\n" );
        return 2;
        }

    /* Generate random data */
    if ( opt_ran )
        for ( p = pb, i = 0 ; i < blk_rows ; i++ )
            for ( j = 0 ; j < blk_cols ; j++ )
                *p++ = ((double)rand() / (double)RAND_MAX) - 0.5;

    /* Generate predictable data */
    else {
        val = 1000 * row + col;
        if ( val != 0.0 )
            if ( strcmp( mname, "A") ) val = -val;
        for ( p = pb, i = 0 ; i < blk_rows ; i++ )
            for ( j = 0 ; j < blk_cols ; j++ )
                *p++ = val;
        }

    /* Possibly print the data */
    if ( opt_print )
        block_print( blk_rows, blk_cols, mname, row, col, pb );

    /* Write the data to disk */
    block_write2disk( blk_rows, blk_cols, mname, row, col, pb );

    /* Copy data into the array if it is stored by row pointers (versus one
       big block of memory */
    if ( mopt == 0 ) {
        for ( p = pb, i = 0 ; i < blk_rows ; i++ )
            for ( pr = mptr[i], j = 0 ; j < blk_cols ; j++ )
                *pr++ = *p++;
	free(pb);
        }

    /* Completed successfully */
    return 0;
    }


/*****************************************************************************
 *
 *                              block_multiply
 *
 *  Multiply two blocks:
 *
 *  The input variables are
 *
 *      blk_rows    Number of rows in a block.
 *      blk_cols    Number of columns in a block.
 *      lbptr       Pointer to left  multiplication block.
 *      rbptr       Pointer to right multiplication block.
 *      mopt        If mopt = 0, then the matrix is allocated one row at a
 *                  time similar to what C might do.
 *                  If mopt = 1, then the matrix is allocated as only one
 *                  block of memory and we construct row (edge) pointers.
 *      opt_add	    If opt_add = 1, then the result of the multiplication 
 *		    will add to block pointed by sbptr.
 *
 *  The pointer to the result block (or NULL) is returned.
 *
 *  The result block is allocated inside this function.
 *
 *****************************************************************************/

int block_multiply( int blk_rows, int blk_cols,
                         double** lbptr, double** rbptr,
                         double** sbptr, int opt_add) {
    
    /* Local declarations */
    int             i;                  /* Loop index */
    int             j;                  /* Loop index */
    int             k;                  /* Loop index */
    
    /* Initialize the solution block if not in Addition mode */
    if( opt_add == 0 )
	for ( i = 0 ; i < blk_rows ; i++ )
	    for ( j = 0 ; j < blk_cols ; j++ )
		*( *( sbptr + i ) + j) = 0;

    /* Calculate the multiplication */
    for ( i = 0 ; i < blk_rows ; i++ )
	for ( j = 0 ; j < blk_cols ; j++ )
	    for ( k = 0 ; k < blk_rows ; k++ )
		*( *( sbptr + i ) + j ) += (*( *( lbptr + i ) + k )) * (*( *( rbptr + k ) + j ));

    /* Completed Successfully */
    return 0;
}
