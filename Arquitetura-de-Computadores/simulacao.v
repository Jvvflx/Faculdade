`timescale 1ns/1ps

// Inclui todos os módulos necessários
`include "Add4.v"
`include "Adder32.v"
`include "ALU.v"
`include "DataMemory.v"
`include "MemoriaDeInstrucoes.v"
`include "mux5bit.v"
`include "muxALUeMem.v"
`include "muxBranch.v"
`include "muxRegeALU.v"
`include "Registradores.v"
`include "ShiftLeft2.v"
`include "SignalExtend.v"
`include "UC.v"
`include "mipsProcessor.v"
`include "ALUControle.v"

module testbench();
    reg clk;
    reg reset;
    integer i, j;
    
    // Instância do processador
    mipsProcessor processor(
        .clk(clk),
        .reset(reset)
    );
    
    // Geração de clock
    initial begin
        clk = 0;
        forever #5 clk = ~clk;
    end
    
    // Procedimento de teste
    initial begin
        // Configuração do arquivo de dump
        $dumpfile("mips_wave.vcd");
        $dumpvars(0, testbench);
        
        // Reset inicial
        reset = 1;
        #15;
        reset = 0;
        
        // Inicializa registradores com valores de teste
        processor.registers.registers[8] = 32'd5;   // $t0 = 5
        processor.registers.registers[9] = 32'd3;   // $t1 = 3
        processor.registers.registers[6] = 32'd7;   // $a2 = 7
        processor.registers.registers[15] = 32'd7;  // $t7 = 7 (para fazer o BEQ funcionar)
        
        // Aguarda execução das instruções
        #80;
        
        // Verifica resultados
        $display("\n=== RESULTADOS DOS TESTES ===");
        
        // Teste 1: ADD
        if (processor.registers.registers[4] === 8) begin
            $display("SUCESSO: ADD - $a0 = %0d (5 + 3 = 8)", processor.registers.registers[4]);
        end else begin
            $display("ERRO: ADD - $a0 = %0d (esperado 8)", processor.registers.registers[4]);
        end
        
        // Teste 2: SUB
        if (processor.registers.registers[11] === 2) begin
            $display("SUCESSO: SUB - $t3 = %0d (5 - 3 = 2)", processor.registers.registers[11]);
        end else begin
            $display("ERRO: SUB - $t3 = %0d (esperado 2)", processor.registers.registers[11]);
        end
        
        // Teste 3: AND
        if (processor.registers.registers[12] === (5 & 3)) begin
            $display("SUCESSO: AND - $t4 = %0d (5 & 3 = %0d)", processor.registers.registers[12], (5 & 3));
        end else begin
            $display("ERRO: AND - $t4 = %0d (esperado %0d)", processor.registers.registers[12], (5 & 3));
        end
        
        // Teste 4: OR
        if (processor.registers.registers[13] === (5 | 3)) begin
            $display("SUCESSO: OR - $t5 = %0d (5 | 3 = %0d)", processor.registers.registers[13], (5 | 3));
        end else begin
            $display("ERRO: OR - $t5 = %0d (esperado %0d)", processor.registers.registers[13], (5 | 3));
        end
        
        // Teste 5: SLT
        if (processor.registers.registers[14] === 0) begin
            $display("SUCESSO: SLT - $t6 = %0d (5 < 3 = 0)", processor.registers.registers[14]);
        end else begin
            $display("ERRO: SLT - $t6 = %0d (esperado 0)", processor.registers.registers[14]);
        end

        // Teste 6: SW (verifica se o valor foi armazenado corretamente na memória)
        if (processor.data_memory.memory[2] === 7) begin
            $display("SUCESSO: SW - memória[2] = %0d (valor armazenado 7)", processor.data_memory.memory[2]);
        end else begin
            $display("ERRO: SW - memória[2] = %0d (esperado 7)", processor.data_memory.memory[2]);
        end

        // Teste 7: LW
        if (processor.registers.registers[15] === 7) begin
            $display("SUCESSO: LW - $t7 = %0d (carregou 7)", processor.registers.registers[15]);
        end else begin
            $display("ERRO: LW - $t7 = %0d (esperado 7)", processor.registers.registers[15]);
        end
        
        
        // Teste 8: BEQ
        // Verifica se a condição de branch é tomada (pulo de 2 ciclos caso $a2 == $a3)
         if (processor.pc === 32'd40) begin  // 8 instruções * 4 bytes = 40
            $display("SUCESSO: BEQ - Branch tomado corretamente quando $t7 == $a2, PC = %h", processor.pc);
        end else begin
            $display("ERRO: BEQ - Branch não funcionou como esperado, PC = %h (esperado 40)", processor.pc);
        end
        
        
        // Verify results
        $display("Final register values:");
        for (i = 0; i < 32; i = i + 1) begin
            $display("R%0d = %0d", i, processor.registers.registers[i]);
        end

        $display("Final memory values:");
        for (i = 0; i < 32; i = i + 1) begin
            $display("mem%0d = %0d", i, processor.data_memory.memory[i]);
        end
        
        #20 $finish;
    end
    
    // Monitor detalhado da execução
    always @(posedge clk) begin
        if (!reset) begin
            $display("\nTime=%0t", $time);
            $display("PC=%h // %0d // %b", processor.pc, processor.pc, processor.pc);
            $display("Instruction=%b", processor.instrucao);
            $display("ALU Operation=%b", processor.ALUOperation);
            $display("Control Signals: RegWrite=%b, MemRead=%b, MemWrite=%b, Branch=%b",
                    processor.RegWrite, processor.MemRead, processor.MemWrite, processor.Branch);
            $display("ALU: A=%0d B=%0d Result=%0d Zero=%b",
                    processor.alu.A,
                    processor.alu.B,
                    processor.alu.ALUResult,
                    processor.alu.Zero);
        end
    end


    // Monitor de execução
    always @(posedge clk) begin
        if (!reset) begin
            $display("\nTime=%0t", $time);
            $display("PC=%h", processor.pc);
            $display("Instruction=%b", processor.instrucao);
            $display("ALU Operation=%b", processor.ALUOperation);
            $display("Branch Signal=%b", processor.Branch);
            $display("ALU Zero=%b", processor.alu_zero);
            $display("Branch Taken=%b", processor.branch_taken);
            $display("$t7 (R15)=%0d", processor.registers.registers[15]);
            $display("$a2 (R6)=%0d", processor.registers.registers[6]);
        end
    end
    
endmodule