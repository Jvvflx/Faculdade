module MuxRegALU(
    input  wire ALUSrc,           // Seleciona entre registrador ou imediato
    input  wire [31:0] ReadData2, // Segundo operando vindo dos registradores
    input  wire [31:0] immExt,    // Imediato estendido
    output wire [31:0] ALUInput2  // Entrada da ALU
);
    assign ALUInput2 = (ALUSrc) ? immExt : ReadData2;
endmodule
