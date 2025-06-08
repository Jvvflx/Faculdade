module ula_tb;
	
	reg [31:0] A, B;
	reg [2:0] f;
	wire [31:0] saida;
	
	ula uin(
		.A(A),
		.B(B),
		.f(f),
		.saida(saida)
		);
		
	initial begin
	
		//Teste com função de soma 
		A = 32'b00000000000000000000000000000011; B = 32'b00000000000000000000000000001001; f = 3'b000; #10; // 3 + 9
		//Teste com a função de subtração
		A = 32'b00000000000000000000000000000110; B = 32'b00000000000000000000000000000010; f = 3'b001; #10; // 6 - 2
		//Teste com a função AND
		A = 32'b1100; B = 32'b1010; f = 3'b010; #10; // 1100 && 1010
		//Teste com a função OR
		A = 32'b0011; B = 32'b1100; f = 3'b011; #10; // 0011 || 1100
		//Teste com a função XOR
		A = 32'b1010; B = 32'b0110; f = 3'b100; #10; // 1010 ^ 0110  
		//Teste com a função ~A
		A = 32'b1101; B = 32'b0000; f = 3'b101; #10; // ~1101
		//Teste com a função A
		A = 32'b0011; B = 32'b0000; f = 3'b110; #10; // 0011
		//Teste com a função ~B
		A = 32'b0000; B = 32'b1111; f = 3'b111; #10; // ~1111
		
		$finish;	
	end
	
	initial begin
		$dumpfile("uin.vcd");
		$dumpvars(0,ula_tb);
	end
	
endmodule
