#include <cstdio>
#include <cstdint>

typedef int valType;

[[obfusc::mba]] valType foo(valType a, valType b) {
    valType ret1 = a+b;
    printf("ret1: %d\n", ret1);
    return ret1;
}

__attribute__((mba)) int main(int argc, char *argv[]) {
    valType a = 13;
    valType b = 11;

    foo(a, b);

    return 0;
}