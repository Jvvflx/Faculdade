module ula(
    input [31:0] A,       // Entrada A de 32 bits
    input [31:0] B,       // Entrada B de 32 bits
    input [2:0] f,        // Controle da operação
    output [31:0] saida,  // Saída de 32 bits
    output v              // Aviso de overflow
);
    wire [31:0] carry;    // Sinais de carry entre os bits

    // Geração dos multiplexadores para bit a bit
    genvar i;
    generate
        for (i = 0; i < 32; i = i + 1) begin : generate_mux
            if (i == 0) begin
                // Bit menos significativo recebe carry in = 0
                mux instaciamux (
                    .S(saida[i]),
                    .A(A[i]),
                    .B(B[i]),
                    .cin(1'b0),
                    .f(f),
                    .cout(carry[i])
                );
            end else begin
                // Os demais Bits recebem carry in do bit anterior
                mux instaciamux (
                    .S(saida[i]),
                    .A(A[i]),
                    .B(B[i]),
                    .cin(carry[i-1]),
                    .f(f),
                    .cout(carry[i])
                );
            end
        end
    endgenerate

    // Calcular o overflow
    // Overflow ocorre apenas para soma/subtração (f = 000 ou f = 001)
    assign v = (f == 3'b000 || f == 3'b001) && 
               (carry[30] ^ carry[31]); // Comparação entre o penúltimo e último carry

endmodule
