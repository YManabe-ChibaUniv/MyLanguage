/*
	Calculate prime numbers between 1 and 10000.
*/
function main(void) -> void {
	int32 i, j, flag;
	i = 2;
	j = 2;
	while (i < 10000) {
		flag = 0;
		j = 2;
		while (j < i) {
			if (i MOD j == 0) {
				flag = 1;
				break;
			}
			j = j + 1;
			continue;
		}
		if (flag == 0) {
			println(i);
		}
		i = i + 1;
	}
	return;
}
