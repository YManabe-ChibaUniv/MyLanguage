function main(void) -> void {
    int32 i, j;
    i = -100;
    j = 100;
    while (i < j) {
        if (i MOD 10 == 0) {
            println(i);
        }
        i = i + 1;
    }
}