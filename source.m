function main(void) -> void {
	int32 a, b, c;
	string x, y, z;
	a = 32;
	b = 16;
	c = b;
	x = "abc";
	y = "xyz";
	z = x;
	print(a, "+", b, "=");
	println(a + b);
	println(x + z);
	println(y + string(c));
	return;
}
