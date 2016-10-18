#ifndef H_MATRICES
#define H_MATRICES

#ifdef __cplusplus
extern "C" {
#endif

/* A routine to save simple information about the matrices on disk */
int matrix_info_write( int blk_rows, int blk_cols,
                       int arows, int acols, int bcols );

int matrix_info_read( int *blk_rows, int *blk_cols,
                      int *arows, int *acols,
                      int *brows, int *bcols,
                      int *crows, int *ccols );

void block_print( int blk_rows, int blk_cols,
                  char* mname, int row, int col,
                  double* pdata );

int block_write2disk( int blk_rows, int blk_cols,
                      char* mname, int row, int col,
                      double* pdata );

int block_readdisk( int blk_rows, int blk_cols,
                    char* mname, int row, int col,
                    double **mptr, int mopt,
                    int opt_print );

double** block_allocate( int blk_rows, int blk_cols, int mopt );

int block_generate( int blk_rows, int blk_cols,
                    char* mname, int row, int col,
                    double** mptr, int mopt, 
                    int opt_ran, int opt_print );
    
int block_multiply( int blk_rows, int blk_cols,
                         double** aptr, double** bptr,
                         double** sbptr, int opt_add);

#ifdef __cplusplus
} /* closing brace for extern "C" */
#endif

#endif
