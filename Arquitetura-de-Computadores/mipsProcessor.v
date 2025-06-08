module mipsProcessor(
    input wire clk,
    input wire reset
);

    // Sinais de controle da UC
    wire RegDst, ALUSrc, MemtoReg, RegWrite, MemRead, MemWrite, Branch, Jump;
    wire [1:0] ALUOp;
    wire [3:0] ALUOperation;

    // Sinais de dados
    wire [31:0] pc_atual;
    wire [31:0] proxima_instrucao;
    wire [31:0] instrucao;
    wire [31:0] read_data1, read_data2;
    wire [31:0] write_data;
    wire [31:0] alu_result;
    wire [31:0] mem_read_data;
    wire [31:0] alu_input2;
    wire [31:0] sign_extended;
    wire [31:0] branch_addr;
    wire [31:0] shifted_addr;
    wire alu_zero;
    wire [4:0] write_reg;
    wire branch_taken;

    // Registrador do PC
    reg [31:0] pc;
    wire [31:0] pc_plus_4;

    // PC Logic
    always @(posedge clk or posedge reset) begin
        if (reset)
            pc <= 32'b0;
        else
            pc <= proxima_instrucao;
    end

    // Módulo Add4 para incrementar PC
    Add4 pc_incrementer(
        .in(pc),
        .out(pc_plus_4)
    );

    // Memória de Instruções
    MemoriaDeInstrucoes instr_mem(
        .addr(pc),
        .instrucao(instrucao)
    );

    // Unidade de Controle
    ControlUnit control_unit(
        .instrucao(instrucao),
        .RegDst(RegDst),
        .ALUSrc(ALUSrc),
        .MemtoReg(MemtoReg),
        .RegWrite(RegWrite),
        .MemRead(MemRead),
        .MemWrite(MemWrite),
        .Branch(Branch),
        .Jump(Jump),
        .ALUOp(ALUOp)
    );

    // Unidade de Controle da ALU
    ALUControl alu_control(
        .ALUOp(ALUOp),
        .funct(instrucao[5:0]),
        .ALUOperation(ALUOperation)
    );

    // Mux para selecionar registrador de destino
    Mux5bit reg_dst_mux(
        .RegDst(RegDst),
        .in0(instrucao[20:16]),  // rt
        .in1(instrucao[15:11]),  // rd
        .WriteRegister(write_reg)
    );

    // Banco de Registradores
    Registradores registers(
        .clk(clk),
        .ReadRegister1(instrucao[25:21]),  // rs
        .ReadRegister2(instrucao[20:16]),  // rt
        .WriteRegister(write_reg),
        .WriteData(write_data),
        .RegWrite(RegWrite),
        .ReadData1(read_data1),
        .ReadData2(read_data2)
    );

    // Extensão de Sinal
    SignExtend sign_extend(
        .in(instrucao[15:0]),
        .out(sign_extended)
    );

    // Mux para selecionar segundo operando da ALU
    MuxRegALU alu_src_mux(
        .ALUSrc(ALUSrc),
        .ReadData2(read_data2),
        .immExt(sign_extended),
        .ALUInput2(alu_input2)
    );

    // ALU
    ALU alu(
        .A(read_data1),
        .B(alu_input2),
        .ALUOperation(ALUOperation),
        .ALUResult(alu_result),
        .Zero(alu_zero)
    );

    // Memória de Dados
    DataMemory data_memory(
        .clk(clk),
        .MemRead(MemRead),
        .MemWrite(MemWrite),
        .address(alu_result),
        .writeData(read_data2),
        .readData(mem_read_data)
    );

    // Mux para selecionar dados de escrita no registrador
    Mux_ALU_Mem write_data_mux(
        .sel(MemtoReg),
        .ALUResult(alu_result),
        .readData(mem_read_data),
        .WriteData(write_data)
    );

    // Deslocamento para endereço de branch
    ShiftLeft2 branch_shifter(
        .in(sign_extended),
        .out(shifted_addr)
    );

    // Somador para endereço de branch
    Adder32 branch_adder(
        .a(pc_plus_4),
        .b(shifted_addr),
        .sum(branch_addr)
    );

    // AND para decidir se faz branch
    assign branch_taken = Branch & alu_zero;

    // Mux para selecionar próximo PC
    MuxBranch branch_mux(
        .sel(branch_taken),
        .pcAtual(pc_plus_4),
        .branchAddr(branch_addr),
        .proximoPC(proxima_instrucao)
    );

endmodule