module MuxBranch(
    input  wire sel,               // Seleciona o próximo endereço do PC
    input  wire [31:0] pcAtual,     // Endereço atual do PC + 4
    input  wire [31:0] branchAddr,  // Endereço de desvio calculado
    output wire [31:0] proximoPC    // Próximo endereço do PC
);
    assign proximoPC = (sel) ? branchAddr : pcAtual;
endmodule
