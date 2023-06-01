function main(void) -> void {
    float32 i, j;
    i = -100.0;
    j = 100.0;
    while (i < j) {
        if (/* comment */i / 10.0 > 0.0 AND TRUE) {
            println(i);
        }
        i = i + 1.0;
    }
}