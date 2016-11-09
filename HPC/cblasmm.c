 #include <time.h>
 #include <stdio.h>
 #include <stdlib.h>
 #include <cblas.h>

double mrun();
 
 void init(double* matrix, int row, int column)
 {
   for (int j = 0; j < column; j++){
     for (int i = 0; i < row; i++){
       matrix[j*row + i] = ((double)rand())/RAND_MAX;
     }
   }
 }
 
 void print(const char * name, const double* matrix, int row, int column)
 {
   printf("Matrix %s has %d rows and %d columns:\n", name, row, column);
   for (int i = 0; i < row; i++){
     for (int j = 0; j < column; j++){
       printf("%.3f ", matrix[j*row + i]);
     }
     printf("\n");
   }
   printf("\n");
 }
 
 int main(int argc, char * argv[])
 {
   double t;
   int rowsA, colsB, common;
   int i,j,k;
 
   if (argc != 4){
     printf("Using defaults\n");
     rowsA = 2; colsB = 4; common = 6;
   }
   else{
     rowsA = atoi(argv[1]); colsB = atoi(argv[2]);common = atoi(argv[3]);
   }
 
   double A[rowsA * common]; double B[common * colsB];
   double C[rowsA * colsB];
 
   enum CBLAS_ORDER order = CblasColMajor;
   enum CBLAS_TRANSPOSE transA = CblasNoTrans;
   enum CBLAS_TRANSPOSE transB = CblasNoTrans;
 
   double one = 1.0, zero = 0.0;
 
   srand(time(NULL));
 
   init(A, rowsA, common); init(B, common, colsB);
 
t = mrun();
   cblas_dgemm(order,transA,transB, rowsA, colsB, common ,1.0,A, 
               rowsA ,B, common ,0.0,C, rowsA);
t = mrun() - t;
 
 //  print("A", A, rowsA, common); print("B", B, common, colsB);
 
printf("\nComputing time:%le\n",t);
   return 0;
 }
