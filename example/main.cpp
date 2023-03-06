#include <cstdio>


[[obfusc::bcf]] int foo(int a) {
    return a * 2;
}

__attribute__((mba)) int main(int argc, char *argv[]) {
    int a = 5;
    int b = 2;

    char ret1 = a+b;
    int ret2 = a-b;

    printf("ret1: %d\nret2: %d\n", ret1, ret2);

    //ret += foo(ret);
    //ret += ret2;

    return ret1;
}