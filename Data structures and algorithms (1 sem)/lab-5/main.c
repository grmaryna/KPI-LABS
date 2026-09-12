#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define M 7
#define N 7

void fillMatrix(int matrix[M][N], int min, int max) {
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            matrix[i][j] = rand() % (max - min + 1) + min;
        }
    }
}

void printMatrix(int matrix[M][N]) {
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            printf("%4d", matrix[i][j]);
        }
        printf("\n");
    }
}

void selectionSortOddRowsFirstColumn(int matrix[M][N]) {
    int col = 0;

    for (int i = 1; i < M - 1; i += 2) {
        int maxIndex = i;

        for (int j = i + 2; j < M; j += 2) {
            if (matrix[j][col] > matrix[maxIndex][col]) {
                maxIndex = j;
            }
        }

        // міняємо МІСЦЯМИ ВЕСЬ РЯДОК
        if (maxIndex != i) {
            for (int k = 0; k < N; k++) {
                int temp = matrix[i][k];
                matrix[i][k] = matrix[maxIndex][k];
                matrix[maxIndex][k] = temp;
            }
        }
    }
}

int main(void) {
    srand((unsigned int)time(NULL));

    int min = 10, max = 99;

    // ===== Test 1: random =====
    int matrix1[M][N];
    fillMatrix(matrix1, min, max);

    printf("Random matrix:\n");
    printMatrix(matrix1);

    selectionSortOddRowsFirstColumn(matrix1);

    printf("\nAfter sorting:\n");
    printMatrix(matrix1);

    // ===== Test 2: reverse sorted =====
    int matrix2[M][N];
    fillMatrix(matrix2, min, max);

    for (int i = 1; i < M; i += 2) {
        matrix2[i][0] = 10 + i * 5;
    }

    printf("\nReverse sorted (before):\n");
    printMatrix(matrix2);

    selectionSortOddRowsFirstColumn(matrix2);

    printf("\nAfter sorting:\n");
    printMatrix(matrix2);

    // ===== Test 3: already sorted =====
    int matrix3[M][N];
    fillMatrix(matrix3, min, max);

    for (int i = 1; i < M; i += 2) {
        matrix3[i][0] = 100 - i * 5;
    }

    printf("\nAlready sorted (before):\n");
    printMatrix(matrix3);

    selectionSortOddRowsFirstColumn(matrix3);

    printf("\nAfter sorting:\n");
    printMatrix(matrix3);

    return 0;
}