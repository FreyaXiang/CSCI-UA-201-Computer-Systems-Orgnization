// Xin Xiang xx985

/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include "lab3.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    int blockRow, blockCol, blockSize;
	int i=0, j=0, diagonal=0, temp=0;
    int a, b, c, d, e; // variables for 64 by 64

    switch (M) {
        // when matrix size is 32 by 32
        case 32:
            blockSize=8;
            for (blockCol=0; blockCol<N; blockCol+=blockSize) {
                for (blockRow=0; blockRow<N; blockRow+=blockSize) {
                    for (i=blockRow; i<blockRow+blockSize; i++) {
                        for (j=blockCol; j<blockCol+blockSize; j++) {						
                            if (i!=j) {
                                B[j][i]=A[i][j];
                            } else {
                                // diagonal
                                // use temp to reduce misses    
                                temp=A[i][j];
                                diagonal=i;
                            }
                        }
                        // diagonal
                        if (blockRow==blockCol) {							
                            B[diagonal][diagonal]=temp;
                        }
                    }	
                }
            }
            break;
        
        // when matrix size is 64 by 64
        case 64:
            blockSize=4;
            for(i=0; i<N; i+=blockSize){
                for(j=0; j<M; j+=blockSize){
                    a = A[i][j];
                    b = A[i+1][j];
                    c = A[i+2][j];
                    d = A[i+2][j+1];
                    e = A[i+2][j+2];

                    B[j+3][i] = A[i][j+3];
                    B[j+3][i+1] = A[i+1][j+3];
                    B[j+3][i+2] = A[i+2][j+3];

                    B[j+2][i] = A[i][j+2];
                    B[j+2][i+1] = A[i+1][j+2];
                    B[j+2][i+2] = e;
                    e = A[i+1][j+1];

                    B[j+1][i] = A[i][j+1];
                    B[j+1][i+1] = e;
                    B[j+1][i+2] = d;

                    B[j][i] = a;
                    B[j][i+1] = b;
                    B[j][i+2] = c;

                    B[j][i+3] = A[i+3][j];
                    B[j+1][i+3] = A[i+3][j+1];
                    B[j+2][i+3] = A[i+3][j+2];
                    a = A[i+3][j+3];
                    
                    B[j+3][i+3] = a;
			   }
		    }
            break;

        case 61:
            blockSize=16;
            for (blockCol=0; blockCol<M; blockCol+=blockSize) {
                for (blockRow=0; blockRow<N; blockRow+=blockSize) {	
                    // Non-square matrix so need to check if i < N and j < M
                    for (i=blockRow; (i<blockRow+blockSize) && (i<N); i++) {
                        for (j=blockCol; (j<blockCol+blockSize) && (j<M); j++) {
                            if (i!=j) {
                                B[j][i]=A[i][j];
                            } else {
                                temp=A[i][j];
                                diagonal=i;
                            }
                        }
                        // diagonal
                        if (blockRow==blockCol) {
                            B[diagonal][diagonal]=temp;
                        }
                    }
                }
            }
            break;
	} 
}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }    

}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc); 

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc); 

}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}

