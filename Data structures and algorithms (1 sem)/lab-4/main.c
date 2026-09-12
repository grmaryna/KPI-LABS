#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h> 

#define M 7
#define N 8

int binarySearchDesc(double arr[], int size, double X) {
    int left = 0;
    int right = size - 1;
    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (arr[mid] == X) return mid;
        if (arr[mid] > X) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    return -1;
}

int cmp_desc(const void *a, const void *b) {
    double A = *(double*)a;
    double B = *(double*)b;
    if (A > B) return -1;
    if (A < B) return 1;
    return 0;
}

void fillMatrix(double mat[M][N]) {
    mat[0][0] = (rand() % 200 - 50) / 10.0;
    for (int j = 1; j < N; j++) {
        double delta = (rand() % 10) / 10.0;
        mat[0][j] = mat[0][j-1] - delta;
    }

    mat[0][N-1] = mat[0][N-1];
    for (int i = 1; i < M; i++) {
        double delta = (rand() % 10) / 10.0;
        mat[i][N-1] = mat[i-1][N-1] - delta;
    }

    for (int i = 1; i < M; i++) {
        for (int j = 0; j < N-1; j++) {
            mat[i][j] = (rand() % 200 - 50) / 10.0;
        }
    }
}

int main() {
    srand(time(NULL));

    double A[M][N];
    fillMatrix(A);

    printf("\nGenerated matrix A[%d][%d]:\n", M, N);
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            printf("%6.2f ", A[i][j]);
        }
        printf("\n");
    }

    double X;
    printf("\nEnter number X to search: ");
    scanf("%lf", &X);

    printf("\n--- Search results (Algorithm #2) ---\n");

    int colIndex = binarySearchDesc(A[0], N, X);
    if (colIndex != -1) {
        printf("Value %.2f found in the first row at: [%d][%d]\n", X, 0, colIndex);
        usleep(100000);
    } else {
        printf("Value %.2f not found in the first row.\n", X);
    }

    double lastCol[M];
    for (int i = 0; i < M; i++) lastCol[i] = A[i][N-1];

    int rowIndex = binarySearchDesc(lastCol, M, X);
    if (rowIndex != -1) {
        printf("Value %.2f found in the last column at: [%d][%d]\n", X, rowIndex, N-1);
        usleep(100000);
    } else {
        printf("Value %.2f not found in the last column.\n", X);
    }

    return 0;
}