/*
	main function
*/
function main(void) -> void {
	int32 a, b, c, d;
	string x, y, z;
	a = 32;
	b = 16;
	c = b;
	a = a * (b + (c - a)) + b - 1;
	d = a MOD 3;
	print(a, " mod 3 = ");
	if (NOT d AND (TRUE OR FALSE) AND 0 < d + 2 == 2 >= 1 OR 1 > 2) {
		println("0");
	}
	else if (d == 1 AND (a == a OR 1 != 0)) {
		println("1");
	}
	else {
		println("2");
	}
	x = "abc";
	y = "xyz";
	z = x;
	/* This is comment */
	print(a, "+", b, "=");
	println(a + b);
	println(x + z);
	println(y, c);
	return;
}
