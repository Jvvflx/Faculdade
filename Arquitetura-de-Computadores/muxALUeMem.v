module Mux_ALU_Mem(
    input  wire sel,
    input  wire [31:0] ALUResult,
    input  wire [31:0] readData,
    output wire [31:0] WriteData
);
    assign WriteData = (sel) ? readData : ALUResult;
endmodule
