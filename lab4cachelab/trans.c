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

void exch(int M, int N, int B[M][N], int a0, int a1, int a2, int a3) {
    int tmp = B[a0][a1];
    B[a0][a1] = B[a2][a3];
    B[a2][a3] = tmp;
}

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
    int i, j;
    int i1, j1;
    int a0, a1, a2, a3, a4, a5, a6, a7;

    if (M == 32) {
        for (i = 0; i < N; i+=8) {
            for (j = 0; j < M; j+=8)
                for (i1 = i; i1 < i+8 && i1 < N; i1++) {
                    for (j1 = j; j1 < j+8 && j1 < M; j1++) {
                        if (i1 == j1) a0 = A[i1][j1];
                        else B[j1][i1] = A[i1][j1];
                    }
                    if (i == j) B[i1][i1] = a0; // 对角线的值放到该 block 最后面才 Store
                }
        }    
    } else if (M == 64) {
        for (i = 0; i < N; i+=8) {
            for (j = 0; j < M; j+=8) {
                for (j1 = j; j1 < j+4; j1++) {
                        a0 = A[j1][i];
                        a1 = A[j1][i+1];
                        a2 = A[j1][i+2];
                        a3 = A[j1][i+3];
                        a4 = A[j1][i+4];
                        a5 = A[j1][i+5];
                        a6 = A[j1][i+6];
                        a7 = A[j1][i+7];
                        B[i][j1] = a0;
                        B[i+1][j1] = a1;
                        B[i+2][j1] = a2;
                        B[i+3][j1] = a3;
                        B[i][j1+4] = a4;
                        B[i+1][j1+4] = a5;
                        B[i+2][j1+4] = a6;
                        B[i+3][j1+4] = a7;
                }
                for (i1 = i; i1 < i+4; i1++) {
                    // 横向取出右上
                    a4 = B[i1][j+4];
                    a5 = B[i1][j+5];
                    a6 = B[i1][j+6];
                    a7 = B[i1][j+7];
                    // 纵向取出A前面部分
                    a0 = A[j+4][i1];
                    a1 = A[j+5][i1];
                    a2 = A[j+6][i1];
                    a3 = A[j+7][i1];
                    // 放入右上横向
                    B[i1][j+4] = a0;
                    B[i1][j+5] = a1;
                    B[i1][j+6] = a2;
                    B[i1][j+7] = a3;
                    // 放入左下横向
                    B[i1+4][j] = a4;
                    B[i1+4][j+1] = a5;
                    B[i1+4][j+2] = a6;
                    B[i1+4][j+3] = a7;
                }
                for (i1 = i+4; i1 < i+8; i1++) {
                    // 取出A后面部分
                    a4 = A[j+4][i1];
                    a5 = A[j+5][i1];
                    a6 = A[j+6][i1];
                    a7 = A[j+7][i1];
                    // 放入右下
                    B[i1][j+4] = a4;
                    B[i1][j+5] = a5;
                    B[i1][j+6] = a6;
                    B[i1][j+7] = a7;
                }
            }
        }    
    } else {
        for (i = 0; i < N; i+=16) {
            for (j = 0; j < M; j+=16)
                for (i1 = i; i1 < i+16 && i1 < N; i1++) {
                    for (j1 = j; j1 < j+16 && j1 < M; j1++) {
                        if (i1 == j1) a0 = A[i1][j1];
                        else B[j1][i1] = A[i1][j1];
                    }
                    if (i == j) B[i1][i1] = a0;
                }
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
    int i, j;
    int i1;
    int SIZE = 8;
    int a0, a1, a2, a3, a4, a5, a6, a7;

    for (i = 0; i < N; i+=SIZE) {
        for (j = 0; j < M; j+=SIZE)
            for (i1 = i; i1 < i+SIZE && i1 < N; i1++) {
                    a0 = A[i1][j];
                    a1 = A[i1][j+1];
                    a2 = A[i1][j+2];
                    a3 = A[i1][j+3];
                    a4 = A[i1][j+4];
                    a5 = A[i1][j+5];
                    a6 = A[i1][j+6];
                    a7 = A[i1][j+7];
                    B[j][i1] = a0;
                    B[j+1][i1] = a1;
                    B[j+2][i1] = a2;
                    B[j+3][i1] = a3;
                    B[j+4][i1] = a4;
                    B[j+5][i1] = a5;
                    B[j+6][i1] = a6;
                    B[j+7][i1] = a7;
            }
    }    

    /** for (i = 0; i < N; i+=8) { */
    /**     for (j = 0; j < M; j+=8) { */
    /**         for (i1 = i; i1 < i+4 && i1 < N; i1++) { */
    /**             j1 = i1 - i + j + 4; */
    /**             a0 = A[i1][j]; */
    /**             a1 = A[i1][j+1]; */
    /**             a2 = A[i1][j+2]; */
    /**             a3 = A[i1][j+3]; */
    /**             a4 = A[i1][j+4]; */
    /**             a5 = A[i1][j+5]; */
    /**             a6 = A[i1][j+6]; */
    /**             a7 = A[i1][j+7]; */
    /**             B[j][i1] = a0; */
    /**             B[j+1][i1] = a1; */
    /**             B[j+2][i1] = a2; */
    /**             B[j+3][i1] = a3; */
    /**             B[j1][i] = a4; */
    /**             B[j1][i+1] = a5; */
    /**             B[j1][i+2] = a6; */
    /**             B[j1][i+3] = a7; */
    /**         } */
    /**         for (i1 = i+4; i1 < i+8 && i1 < N; i1++) { */
    /**             j1 = i1 - i + j; */
    /**             a0 = A[i1][j]; */
    /**             a1 = A[i1][j+1]; */
    /**             a2 = A[i1][j+2]; */
    /**             a3 = A[i1][j+3]; */
    /**             a4 = A[i1][j+4]; */
    /**             a5 = A[i1][j+5]; */
    /**             a6 = A[i1][j+6]; */
    /**             a7 = A[i1][j+7]; */
    /**             B[j][i1] = a0; */
    /**             B[j+1][i1] = a1; */
    /**             B[j+2][i1] = a2; */
    /**             B[j+3][i1] = a3; */
    /**             B[j1][i+4] = a4; */
    /**             B[j1][i+5] = a5; */
    /**             B[j1][i+6] = a6; */
    /**             B[j1][i+7] = a7; */
    /**         } */
    /**         // 交换 */
    /**         exch(M, N, B, j+5, i, j+4, i+1); */
    /**         exch(M, N, B, j+6, i, j+4, i+2); */
    /**         exch(M, N, B, j+7, i, j+4, i+3); */
    /**         exch(M, N, B, j+6, i+1, j+5, i+2); */
    /**         exch(M, N, B, j+7, i+1, j+5, i+3); */
    /**         exch(M, N, B, j+7, i+2, j+6, i+3); */
    /**  */
    /**         exch(M, N, B, j+5, i+4, j+4, i+5); */
    /**         exch(M, N, B, j+6, i+4, j+4, i+6); */
    /**         exch(M, N, B, j+7, i+4, j+4, i+7); */
    /**         exch(M, N, B, j+6, i+5, j+5, i+6); */
    /**         exch(M, N, B, j+7, i+5, j+5, i+7); */
    /**         exch(M, N, B, j+7, i+6, j+6, i+7); */
    /**     } */
    /** }     */
}


char trans_test_desc[] = "Simple row-wise scan transpose";
void trans_test(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;
    int i1, j1;
    int SIZE = 8;

    for (i = 0; i < N; i+=SIZE) {
        for (j = 0; j < M; j+=SIZE)
            for (i1 = i; i1 < i+SIZE && i1 < N; i1++)
                for (j1 = j; j1 < j+SIZE && j1 < M; j1++) {
                    tmp = A[i1][j1];
                    B[j1][i1] = tmp;
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
    // registerTransFunction(trans, trans_desc); 

    // registerTransFunction(trans_test, trans_test_desc); 
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

