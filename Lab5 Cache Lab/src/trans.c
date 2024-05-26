/* 20220127 Yoojin Lim */
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
#include "cachelab.h"

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
    //index variables used in for loop
    int m, n, nn, k;
    //local variables
    int tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;

    if ((M == 32) && (N == 32)) {
        for (m = 0; m < M; m += 8) {
            for (n = 0; n < N; n += 8) {
                if(m != n) {
                    for (nn = n; nn < n + 8; nn++) {
                        tmp0 = A[nn][m];
                        tmp1 = A[nn][m + 1];
                        tmp2 = A[nn][m + 2];
                        tmp3 = A[nn][m + 3];
                        tmp4 = A[nn][m + 4];
                        tmp5 = A[nn][m + 5];
                        tmp6 = A[nn][m + 6];
                        tmp7 = A[nn][m + 7];

                        B[m][nn] = tmp0;
                        B[m + 1][nn] = tmp1;
                        B[m + 2][nn] = tmp2;
                        B[m + 3][nn] = tmp3;
                        B[m + 4][nn] = tmp4;
                        B[m + 5][nn] = tmp5;
                        B[m + 6][nn] = tmp6;
                        B[m + 7][nn] = tmp7;
                    }                    
                } else {
                    for (k = 0; k < 4; k++) {
                        tmp0 = A[n + k][m];
                        tmp1 = A[n + k][m + 1];
                        tmp2 = A[n + k][m + 2];
                        tmp3 = A[n + k][m + 3];
                        tmp4 = A[n + k][m + 4];
                        tmp5 = A[n + k][m + 5];
                        tmp6 = A[n + k][m + 6];
                        tmp7 = A[n + k][m + 7];

                        B[m + k][n] = tmp0;
                        B[m + k][n + 1] = tmp1;
                        B[m + k][n + 2] = tmp2;
                        B[m + k][n + 3] = tmp3;
                        B[m + k][n + 4] = tmp4;
                        B[m + k][n + 5] = tmp5;
                        B[m + k][n + 6] = tmp6;
                        B[m + k][n + 7] = tmp7;
                    }

                    tmp0 = B[m][n + 1];
                    tmp1 = B[m][n + 2];
                    tmp2 = B[m][n + 3];
                    tmp3 = B[m + 1][n];
                    tmp4 = B[m + 2][n];
                    tmp5 = B[m + 3][n];
                    
                    B[m][n + 1] = tmp3;
                    B[m][n + 2] = tmp4;
                    B[m][n + 3] = tmp5;
                    B[m + 1][n] = tmp0;
                    B[m + 2][n] = tmp1;
                    B[m + 3][n] = tmp2;
                    
                    tmp0 = B[m + 1][n + 2];
                    tmp1 = B[m + 1][n + 3];
                    tmp2 = B[m + 2][n + 1];
                    tmp3 = B[m + 3][n + 1];
                    tmp4 = B[m + 2][n + 3];
                    tmp5 = B[m + 3][n + 2];

                    B[m + 2][n + 1] = tmp0;
                    B[m + 3][n + 1] = tmp1;
                    B[m + 1][n + 2] = tmp2;
                    B[m + 1][n + 3] = tmp3;
                    B[m + 3][n + 2] = tmp4;
                    B[m + 2][n + 3] = tmp5;

                    tmp0 = B[m][n + 4];
                    tmp1 = B[m][n + 5];
                    tmp2 = B[m + 1][n + 4];
                    tmp3 = B[m + 1][n + 5];
                    tmp4 = B[m + 2][n + 4];
                    tmp5 = B[m + 2][n + 5];
                    tmp6 = B[m + 3][n + 4];
                    tmp7 = B[m + 3][n + 5];

                    B[m + 4][n] = tmp0;
                    B[m + 5][n] = tmp1;
                    B[m + 4][n + 1] = tmp2;
                    B[m + 5][n + 1] = tmp3;
                    B[m + 4][n + 2] = tmp4;
                    B[m + 5][n + 2] = tmp5;
                    B[m + 4][n + 3] = tmp6;
                    B[m + 5][n + 3] = tmp7;

                    tmp0 = B[m][n + 6];
                    tmp1 = B[m][n + 7];
                    tmp2 = B[m + 1][n + 6];
                    tmp3 = B[m + 1][n + 7];
                    tmp4 = B[m + 2][n + 6];
                    tmp5 = B[m + 2][n + 7];
                    tmp6 = B[m + 3][n + 6];
                    tmp7 = B[m + 3][n + 7];

                    B[m + 6][n] = tmp0;
                    B[m + 7][n] = tmp1;
                    B[m + 6][n + 1] = tmp2;
                    B[m + 7][n + 1] = tmp3;
                    B[m + 6][n + 2] = tmp4;
                    B[m + 7][n + 2] = tmp5;
                    B[m + 6][n + 3] = tmp6;
                    B[m + 7][n + 3] = tmp7;

                    for (nn = n + 4; nn < n + 8; nn++) {
                        tmp0 = A[nn][m];
                        tmp1 = A[nn][m + 1];
                        tmp2 = A[nn][m + 2];
                        tmp3 = A[nn][m + 3];
                        tmp4 = A[nn][m + 4];
                        tmp5 = A[nn][m + 5];
                        tmp6 = A[nn][m + 6];
                        tmp7 = A[nn][m + 7];

                        B[m][nn] = tmp0;
                        B[m + 1][nn] = tmp1;
                        B[m + 2][nn] = tmp2;
                        B[m + 3][nn] = tmp3;
                        B[m + 4][nn] = tmp4;
                        B[m + 5][nn] = tmp5;
                        B[m + 6][nn] = tmp6;
                        B[m + 7][nn] = tmp7;
                    }      
                }
            }
        }
    } else if((M == 64) && (N == 64)) {
        for (m = 0; m < M; m += 8) {
            for (n = 0; n < N; n += 8) {
                if (n < (N - 16)) {
                    for (k = 0; k < 4; k++) {
                        tmp0 = A[n + k][m];
                        tmp1 = A[n + k][m + 1];
                        tmp2 = A[n + k][m + 2];
                        tmp3 = A[n + k][m + 3];
                        tmp4 = A[n + k][m + 4];
                        tmp5 = A[n + k][m + 5];
                        tmp6 = A[n + k][m + 6];
                        tmp7 = A[n + k][m + 7];

                        B[m + k][n + 8] = tmp0;
                        B[m + k][n + 9] = tmp1;
                        B[m + k][n + 10] = tmp2;
                        B[m + k][n + 11] = tmp3;
                        B[m + k][n + 12] = tmp4;
                        B[m + k][n + 13] = tmp5;
                        B[m + k][n + 14] = tmp6;
                        B[m + k][n + 15] = tmp7;
                    }
                    for (k = 0; k < 4; k++) {
                        tmp0 = A[n + k + 4][m];
                        tmp1 = A[n + k + 4][m + 1];
                        tmp2 = A[n + k + 4][m + 2];
                        tmp3 = A[n + k + 4][m + 3];
                        tmp4 = A[n + k + 4][m + 4];
                        tmp5 = A[n + k + 4][m + 5];
                        tmp6 = A[n + k + 4][m + 6];
                        tmp7 = A[n + k + 4][m + 7];

                        B[m + k][n + 16] = tmp0;
                        B[m + k][n + 17] = tmp1;
                        B[m + k][n + 18] = tmp2;
                        B[m + k][n + 19] = tmp3;
                        B[m + k][n + 20] = tmp4;
                        B[m + k][n + 21] = tmp5;
                        B[m + k][n + 22] = tmp6;
                        B[m + k][n + 23] = tmp7;
                    }

                    //B의 상단부
                    tmp0 = B[m][n + 8];
                    tmp1 = B[m + 1][n + 8];
                    tmp2 = B[m + 2][n + 8];
                    tmp3 = B[m + 3][n + 8];

                    tmp4 = B[m][n + 9];
                    tmp5 = B[m + 1][n + 9];
                    tmp6 = B[m + 2][n + 9];
                    tmp7 = B[m + 3][n + 9];

                    B[m][n] = tmp0;
                    B[m][n + 1] = tmp1;
                    B[m][n + 2] = tmp2;
                    B[m][n + 3] = tmp3;

                    B[m + 1][n] = tmp4;
                    B[m + 1][n + 1] = tmp5;
                    B[m + 1][n + 2] = tmp6;
                    B[m + 1][n + 3] = tmp7;

                    tmp0 = B[m][n + 10];
                    tmp1 = B[m + 1][n + 10];
                    tmp2 = B[m + 2][n + 10];
                    tmp3 = B[m + 3][n + 10];

                    tmp4 = B[m][n + 11];
                    tmp5 = B[m + 1][n + 11];
                    tmp6 = B[m + 2][n + 11];
                    tmp7 = B[m + 3][n + 11];

                    B[m + 2][n] = tmp0;
                    B[m + 2][n + 1] = tmp1;
                    B[m + 2][n + 2] = tmp2;
                    B[m + 2][n + 3] = tmp3;

                    B[m + 3][n] = tmp4;
                    B[m + 3][n + 1] = tmp5;
                    B[m + 3][n + 2] = tmp6;
                    B[m + 3][n + 3] = tmp7;

                    tmp0 = B[m][n + 16];
                    tmp1 = B[m + 1][n + 16];
                    tmp2 = B[m + 2][n + 16];
                    tmp3 = B[m + 3][n + 16];

                    tmp4 = B[m][n + 17];
                    tmp5 = B[m + 1][n + 17];
                    tmp6 = B[m + 2][n + 17];
                    tmp7 = B[m + 3][n + 17];

                    B[m][n + 4] = tmp0;
                    B[m][n + 5] = tmp1;
                    B[m][n + 6] = tmp2;
                    B[m][n + 7] = tmp3;

                    B[m + 1][n + 4] = tmp4;
                    B[m + 1][n + 5] = tmp5;
                    B[m + 1][n + 6] = tmp6;
                    B[m + 1][n + 7] = tmp7;

                    tmp0 = B[m][n + 18];
                    tmp1 = B[m + 1][n + 18];
                    tmp2 = B[m + 2][n + 18];
                    tmp3 = B[m + 3][n + 18];

                    tmp4 = B[m][n + 19];
                    tmp5 = B[m + 1][n + 19];
                    tmp6 = B[m + 2][n + 19];
                    tmp7 = B[m + 3][n + 19];

                    B[m + 2][n + 4] = tmp0;
                    B[m + 2][n + 5] = tmp1;
                    B[m + 2][n + 6] = tmp2;
                    B[m + 2][n + 7] = tmp3;

                    B[m + 3][n + 4] = tmp4;
                    B[m + 3][n + 5] = tmp5;
                    B[m + 3][n + 6] = tmp6;
                    B[m + 3][n + 7] = tmp7;

                    //B의 하단부
                    tmp0 = B[m][n + 12];
                    tmp1 = B[m + 1][n + 12];
                    tmp2 = B[m + 2][n + 12];
                    tmp3 = B[m + 3][n + 12];

                    tmp4 = B[m][n + 13];
                    tmp5 = B[m + 1][n + 13];
                    tmp6 = B[m + 2][n + 13];
                    tmp7 = B[m + 3][n + 13];

                    B[m + 4][n] = tmp0;
                    B[m + 4][n + 1] = tmp1;
                    B[m + 4][n + 2] = tmp2;
                    B[m + 4][n + 3] = tmp3;

                    B[m + 5][n] = tmp4;
                    B[m + 5][n + 1] = tmp5;
                    B[m + 5][n + 2] = tmp6;
                    B[m + 5][n + 3] = tmp7;

                    tmp0 = B[m][n + 14];
                    tmp1 = B[m + 1][n + 14];
                    tmp2 = B[m + 2][n + 14];
                    tmp3 = B[m + 3][n + 14];

                    tmp4 = B[m][n + 15];
                    tmp5 = B[m + 1][n + 15];
                    tmp6 = B[m + 2][n + 15];
                    tmp7 = B[m + 3][n + 15];

                    B[m + 6][n] = tmp0;
                    B[m + 6][n + 1] = tmp1;
                    B[m + 6][n + 2] = tmp2;
                    B[m + 6][n + 3] = tmp3;

                    B[m + 7][n] = tmp4;
                    B[m + 7][n + 1] = tmp5;
                    B[m + 7][n + 2] = tmp6;
                    B[m + 7][n + 3] = tmp7;

                    tmp0 = B[m][n + 20];
                    tmp1 = B[m + 1][n + 20];
                    tmp2 = B[m + 2][n + 20];
                    tmp3 = B[m + 3][n + 20];

                    tmp4 = B[m][n + 21];
                    tmp5 = B[m + 1][n + 21];
                    tmp6 = B[m + 2][n + 21];
                    tmp7 = B[m + 3][n + 21];

                    B[m + 4][n + 4] = tmp0;
                    B[m + 4][n + 5] = tmp1;
                    B[m + 4][n + 6] = tmp2;
                    B[m + 4][n + 7] = tmp3;

                    B[m + 5][n + 4] = tmp4;
                    B[m + 5][n + 5] = tmp5;
                    B[m + 5][n + 6] = tmp6;
                    B[m + 5][n + 7] = tmp7;

                    tmp0 = B[m][n + 22];
                    tmp1 = B[m + 1][n + 22];
                    tmp2 = B[m + 2][n + 22];
                    tmp3 = B[m + 3][n + 22];

                    tmp4 = B[m][n + 23];
                    tmp5 = B[m + 1][n + 23];
                    tmp6 = B[m + 2][n + 23];
                    tmp7 = B[m + 3][n + 23];

                    B[m + 6][n + 4] = tmp0;
                    B[m + 6][n + 5] = tmp1;
                    B[m + 6][n + 6] = tmp2;
                    B[m + 6][n + 7] = tmp3;

                    B[m + 7][n + 4] = tmp4;
                    B[m + 7][n + 5] = tmp5;
                    B[m + 7][n + 6] = tmp6;
                    B[m + 7][n + 7] = tmp7;
                } else {
                    for (nn = n; nn < n + 8; nn += 4) {
                        tmp0 = A[nn][m];
                        tmp1 = A[nn][m + 1];
                        tmp2 = A[nn][m + 2];
                        tmp3 = A[nn][m + 3];
                        tmp4 = A[nn][m + 4];
                        tmp5 = A[nn][m + 5];
                        tmp6 = A[nn][m + 6];
                        tmp7 = A[nn][m + 7];

                        B[m][nn] = tmp0;
                        B[m + 1][nn] = tmp1;
                        B[m + 2][nn] = tmp2;
                        B[m + 3][nn] = tmp3;

                        tmp0 = A[nn + 1][m];
                        tmp1 = A[nn + 1][m + 1];
                        tmp2 = A[nn + 1][m + 2];
                        tmp3 = A[nn + 1][m + 3];

                        B[m][nn + 1] = tmp0;
                        B[m + 1][nn + 1] = tmp1;
                        B[m + 2][nn + 1] = tmp2;
                        B[m + 3][nn + 1] = tmp3;

                        tmp0 = A[nn + 2][m];
                        tmp1 = A[nn + 2][m + 1];
                        tmp2 = A[nn + 2][m + 2];
                        tmp3 = A[nn + 2][m + 3];

                        B[m][nn + 2] = tmp0;
                        B[m + 1][nn + 2] = tmp1;
                        B[m + 2][nn + 2] = tmp2;
                        B[m + 3][nn + 2] = tmp3;

                        tmp0 = A[nn + 3][m];
                        tmp1 = A[nn + 3][m + 1];
                        tmp2 = A[nn + 3][m + 2];
                        tmp3 = A[nn + 3][m + 3];

                        B[m][nn + 3] = tmp0;
                        B[m + 1][nn + 3] = tmp1;
                        B[m + 2][nn + 3] = tmp2;
                        B[m + 3][nn + 3] = tmp3;

                        tmp0 = A[nn + 3][m + 4];
                        tmp1 = A[nn + 3][m + 5];
                        tmp2 = A[nn + 3][m + 6];
                        tmp3 = A[nn + 3][m + 7];
                        
                        B[m + 4][nn] = tmp4;
                        B[m + 5][nn] = tmp5;
                        B[m + 6][nn] = tmp6;
                        B[m + 7][nn] = tmp7;

                        B[m + 4][nn + 3] = tmp0;
                        B[m + 5][nn + 3] = tmp1;
                        B[m + 6][nn + 3] = tmp2;
                        B[m + 7][nn + 3] = tmp3;

                        tmp0 = A[nn + 1][m + 4];
                        tmp1 = A[nn + 1][m + 5];
                        tmp2 = A[nn + 1][m + 6];
                        tmp3 = A[nn + 1][m + 7];
                        tmp4 = A[nn + 2][m + 4];
                        tmp5 = A[nn + 2][m + 5];
                        tmp6 = A[nn + 2][m + 6];
                        tmp7 = A[nn + 2][m + 7];

                        B[m + 4][nn + 1] = tmp0;
                        B[m + 5][nn + 1] = tmp1;
                        B[m + 6][nn + 1] = tmp2;
                        B[m + 7][nn + 1] = tmp3;
                        B[m + 4][nn + 2] = tmp4;
                        B[m + 5][nn + 2] = tmp5;
                        B[m + 6][nn + 2] = tmp6;
                        B[m + 7][nn + 2] = tmp7;
                    }
                }               
            }
        }
    } else if((M == 61) && (N == 67)) {
        for (n = 0; n < 64; n += 8) {
            for (m = 0; m < 56; m += 8) {
                for (nn = n; nn < n + 8; nn++) {
                    tmp0 = A[nn][m];
                    tmp1 = A[nn][m + 1];
                    tmp2 = A[nn][m + 2];
                    tmp3 = A[nn][m + 3];
                    tmp4 = A[nn][m + 4];
                    tmp5 = A[nn][m + 5];
                    tmp6 = A[nn][m + 6];
                    tmp7 = A[nn][m + 7];

                    B[m][nn] = tmp0;
                    B[m + 1][nn] = tmp1;
                    B[m + 2][nn] = tmp2;
                    B[m + 3][nn] = tmp3;
                    B[m + 4][nn] = tmp4;
                    B[m + 5][nn] = tmp5;
                    B[m + 6][nn] = tmp6;
                    B[m + 7][nn] = tmp7;
                }
            }
        }
        m = 64;
        for (n = 0; n < 60; n += 2) {
            tmp0 = A[m][n];
            tmp1 = A[m][n + 1];
            tmp2 = A[m + 1][n];
            tmp3 = A[m + 1][n + 1];
            tmp4 = A[m + 2][n];
            tmp5 = A[m + 2][n + 1];

            B[n][m] = tmp0;
            B[n + 1][m] = tmp1;
            B[n][m + 1] = tmp2;
            B[n + 1][m + 1] = tmp3;
            B[n][m + 2] = tmp4;
            B[n + 1][m + 2] = tmp5;
        }

        n = 60;
        tmp0 = A[m][n];
        tmp1 = A[m + 1][n];
        tmp2 = A[m + 2][n];

        B[n][m] = tmp0;
        B[n][m + 1] = tmp1;
        B[n][m + 2] = tmp2;

        n = 56;
        for (m = 0; m < 64; m++) {
            tmp0 = A[m][n];
            tmp1 = A[m][n + 1];
            tmp2 = A[m][n + 2];
            tmp3 = A[m][n + 3];
            tmp4 = A[m][n + 4]; 

            B[n][m] = tmp0;   
            B[n + 1][m] = tmp1;
            B[n + 2][m] = tmp2;
            B[n + 3][m] = tmp3;
            B[n + 4][m] = tmp4;        
        }
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
 *     evaluate each of the registered functions and sunnarize their
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

