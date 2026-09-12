#include <stdio.h>
#include <math.h>

double rec_down(int i, int n, double x, double Fi, double sum) {
    sum += Fi;
    if (i >= n) {
        return sum;
    }
    double next = Fi * x * log(2) / i;
    return rec_down(i + 1, n, x, next, sum);
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

    double result = rec_down(1, n, x, 1.0, 0.0);
    
    printf("Result (Recursive Descent): %lf\n", result);
    printf("Check (2^x): %lf\n", pow(2, x));
    
    return 0;
}