module ALUControl(
    input wire [1:0] ALUOp,        // Sinal ALUOp da unidade de controle principal
    input wire [5:0] funct,        // Campo funct da instrução
    output reg [3:0] ALUOperation  // Código da operação ALU
);

    always @(*) begin
        case (ALUOp)
            2'b00: ALUOperation = 4'b0010; // LW e SW (Soma)
            2'b01: ALUOperation = 4'b0110; // BEQ (Subtração)
            2'b10: begin
                case (funct)
                    6'b100000: ALUOperation = 4'b0010; // ADD
                    6'b100010: ALUOperation = 4'b0110; // SUB
                    6'b100100: ALUOperation = 4'b0000; // AND
                    6'b100101: ALUOperation = 4'b0001; // OR
                    6'b101010: ALUOperation = 4'b0111; // SLT
                    6'b100111: ALUOperation = 4'b1100; // NOR
                    default:   ALUOperation = 4'b0000; // Operação inválida
                endcase
            end
            default: ALUOperation = 4'b0000; // Operação inválida
        endcase
    end

endmodule