module ControlUnit(
    input  wire [31:0] instrucao,     // Instrução completa
    output reg       RegDst,
    output reg       ALUSrc,
    output reg       MemtoReg,
    output reg       RegWrite,
    output reg       MemRead,
    output reg       MemWrite,
    output reg       Branch,
    output reg       Jump,
    output reg [1:0] ALUOp            // Alterado para 2 bits conforme padrão MIPS
);

    // Extrai opcode da instrução
    wire [5:0] opcode = instrucao[31:26];
    wire [5:0] funct = instrucao[5:0];

    always @(*) begin
        // Default values
        RegDst   = 0;
        ALUSrc   = 0;
        MemtoReg = 0;
        RegWrite = 0;
        MemRead  = 0;
        MemWrite = 0;
        Branch   = 0;
        Jump     = 0;
        ALUOp    = 2'b00;

        case (opcode)
            6'b000000: begin // Tipo R
                RegDst   = 1;
                RegWrite = 1;
                ALUOp    = 2'b10;
            end
            6'b100011: begin // LW
                ALUSrc   = 1;
                MemtoReg = 1;
                RegWrite = 1;
                MemRead  = 1;
                ALUOp    = 2'b00;
            end
            6'b101011: begin // SW
                ALUSrc   = 1;
                MemWrite = 1;
                ALUOp    = 2'b00;
            end
            6'b000100: begin // BEQ
                Branch   = 1;
                ALUOp    = 2'b01;
            end
            6'b001000: begin // ADDI
                ALUSrc   = 1;
                RegWrite = 1;
                ALUOp    = 2'b00;
            end
        endcase
    end
endmodule