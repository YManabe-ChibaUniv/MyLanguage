function main(void) -> void {
    int32 i, j;
    i = -100;
    j = 100;
    while (i < j) {
        if (/* comment */i / 10 > 0 AND i MOD 10 == 0) {
            println(i);
        }
        i = i + 1;
    }
}