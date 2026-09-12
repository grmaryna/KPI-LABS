#include <stdio.h>

int main() {

    short x, res;
    scanf("%hd", &x);

    if(x>=2 && x<=7){
        res=(x*x*x)+14;
        printf("res=%hd\n", res);
    }
    else if((x>-13 && x<=-3) || (x>14)){
        res=-4*(x*x*x)+3*x-7;
        printf("res=%hd\n", res);
    }
    else{
        printf("Error\n");
    }

    return 0;
}