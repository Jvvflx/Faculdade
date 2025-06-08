module MemoriaDeInstrucoes(
    input wire [31:0] addr,      // Endereço da instrução
    output wire [31:0] instrucao // Instrução lida
);

    // Memória de instruções (8 palavras de 32 bits)
    reg [31:0] memoria [7:0];    // 8 palavras de 32 bits
    
    // Inicialização da memória com o programa
    initial begin
        $readmemb("teste.bin", memoria, 0, 7);  // Lê o arquivo binário com 8 palavras
    end
    
    // Leitura da memória (acesso aos 8 endereços válidos)
    assign instrucao = memoria[addr[9:2]];  // Utiliza os 4 bits mais baixos para acessar as 8 palavras
    
endmodule
