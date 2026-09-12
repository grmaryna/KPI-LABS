#include <stdio.h>
#include <math.h>

double rec_up(int i, int n, double x, double Fi) {
    if (i > n) {
        return 0.0;
    }
    double next = Fi * x * log(2) / i;
    return Fi + rec_up(i + 1, n, x, next);
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

    double result = rec_up(1, n, x, 1.0);

    printf("Result (Recursive Ascent): %lf\n", result);
    printf("Check (2^x): %lf\n", pow(2, x));
    
    return 0;
}