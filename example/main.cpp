#include <cstdio>


[[obfusc::mba]] int foo(int a, int b) {
    int ret1 = a+b;
    printf("ret1: %d\n", ret1);
    return ret1;
}

__attribute__((mba)) int main(int argc, char *argv[]) {
    int a = 5;
    int b = 12;

    foo(a, b);

    return 0;
}