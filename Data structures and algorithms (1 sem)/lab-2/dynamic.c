#include <stdio.h>
#include <math.h>
#include <stdlib.h>

int main() {
    int N = 0;
    printf("Input N:");
    scanf("%d", &N);

    if (N < 1) {
        printf("Error\n");
        exit(1);
    }
    
    double P = 1;
    double S = 0;
    int OpCount = 0;

    for(int i = 1; i <= N; i++) {
                S = S + (4 * (double)i / 3 - 1);
                OpCount += 4;

        P = P * (i + sqrt(i)) / S;
        OpCount += 4;
    }
    
    printf("Result: %.7f\n", P);
    printf("Operation count: %d\n", OpCount);
}