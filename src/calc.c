#include "calc.h"

int add(int a, int b) { return a + b; }
int sub(int a, int b) { return a - b; }
int mul(int a, int b) { return a * b; }
int divide(int a, int b) { return a / b; }
int mod(int a, int b) { return a % b; }
int power(int base, int exp) {
    int result = 1;
    for (int i = 0; i < exp; i++) result *= base;
    return result;
}

// New function
int magnitude(int x) {
    return (x < 0) ? -x : x;
}
