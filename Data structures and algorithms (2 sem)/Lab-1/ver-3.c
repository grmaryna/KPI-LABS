#include <stdio.h>
#include <math.h>

void rec_mixed(int i, int n, double x, double Fi, double *sum) {
    double next = (i < n) ? Fi * x * log(2) / i : 0.0;

    if (i < n) {
        rec_mixed(i + 1, n, x, next, sum);
    }

    *sum += Fi;
}

int main() {
    int n;
    double x;
    printf("Enter n and x: ");
    scanf("%d %lf", &n, &x);
    if (n < 1) {
        printf("n must be >= 1\n");
        return 1;
    }

    double result = 0.0;

    rec_mixed(1, n, x, 1.0, &result);
    
    printf("Result (Mixed): %lf\n", result);
    printf("Check (2^x):   %lf\n", pow(2, x));
    
    return 0;
}
