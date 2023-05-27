/*
	main function
*/
function main(void) -> void {
	int32 a, b, c;
	string x, y, z;
	a = 32;
	b = 16;
	c = b;
	a = a * (b + (c - a)) - b;
	a = a mod 10;
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
