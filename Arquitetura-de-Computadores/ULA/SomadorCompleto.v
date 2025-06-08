module somadorcompleto(
	input A, B, cin,
	output S, cout
);
	
	//Modelo estrutural da instância de soma
	wire x,y,z;
	
	xor(x,A,B);
	xor(S,x,cin);
	and(y,A,B);
	and(z,x,cin);
	or(cout,y,z);
	
endmodule
