module Mux5bit(
    input  wire RegDst,
    input  wire [4:0] in0,
    input  wire [4:0] in1,
    output wire [4:0] WriteRegister
);
    assign WriteRegister = (RegDst) ? in1 : in0;
endmodule   
