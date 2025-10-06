#include <stdio.h>
#include "calc.h"

int main(int argc, char *argv[]) {
    printf("10 + 2 = %d\n", add(10, 2));
    printf("10 - 2 = %d\n", sub(10, 2));
    printf("10 * 2 = %d\n", mul(10, 2));
    printf("10 / 2 = %d\n", divide(10, 2));
    printf("10 %% 2 = %d\n", mod(10, 2));
    printf("10^2 = %d\n", power(10, 2));
    printf("10^3 = %d\n", power(10, 3));

    // New function call
    printf("magnitude(-15) = %d\n", magnitude(-15));

    return 0;
}
