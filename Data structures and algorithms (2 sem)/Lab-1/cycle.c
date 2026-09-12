#include <stdio.h>
#include <math.h>

int main() {
    int n;
    double x;
    printf("Enter n and x: ");
    scanf("%d %lf", &n, &x);
    if (n < 1) {
        printf("n must be >= 1\n");
        return 1;
    }

    double Fi = 1.0;
    double sum = 0.0;

    for (int i = 1; i <= n; i++) {
        sum += Fi;
        Fi = Fi * x * log(2) / i;
    }

    printf("Result (Loop): %lf\n", sum);
    printf("Check (2^x):  %lf\n", pow(2, x));
    return 0;
}