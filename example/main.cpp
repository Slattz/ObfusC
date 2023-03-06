
[[obfusc::bcf]] int foo(int a) {
    return a * 2;
}

__attribute__((mba)) int main(int argc, char *argv[]) {
    int a = 5;
    int b = 2;

    int ret = (a-b);

    ret += foo(ret);

    return ret;
}