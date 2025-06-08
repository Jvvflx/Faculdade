module subtratorcompleto(
	input A, B, bin,
	output D, bout
);
	
	//Modelo estrutural da instância de subtração
	wire x,w,y,z;
	
	xor(x,A,B);
	xor(D,x,bin);
	not (w,A);
	and(y,w,B);
	and(z,y,bin);
	or(bout,y,z);
	
endmodule
