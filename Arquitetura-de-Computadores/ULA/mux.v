module mux(
    input A,
    input B,
    input cin,             // Carry in
    input [2:0] f,         // Controle da operação
    output S,              // Resultado
    output cout            // Carry out
);

    wire [2:0] notf;       // Bits invertidos de f
    wire soma_res, sub_res, and_res, or_res, xor_res, notA_res, notB_res, a_res;
    wire soma_cout, sub_cout; // Carry out para soma e subtração

    // Negação dos bits de controle f
    not(notf[0], f[0]);
    not(notf[1], f[1]);
    not(notf[2], f[2]);

    // Operações individuais
    somadorcompleto somador(
        .A(A),
        .B(B),
        .cin(cin),
        .S(soma_res),
        .cout(soma_cout)
    ); // Soma com cin e cout

    subtratorcompleto subtrator(
        .A(A),
        .B(B),
        .bin(cin),
        .D(sub_res),
        .bout(sub_cout)
    ); // Subtração com cin (interpretação como borrow in)

    and (and_res, A, B); // AND
    or (or_res, A, B);   // OR
    xor (xor_res, A, B); // XOR
    not (notA_res, A);   // NOT A
    not (notB_res, B);   // NOT B
    assign a_res = A;    // Passa A diretamente

    // Seleção da operação com base no valor de f
    assign S = (notf[2] & notf[1] & notf[0] & soma_res)  | // f = 000 -> Soma
               (notf[2] & notf[1] & f[0] & sub_res)      | // f = 001 -> Subtração
               (notf[2] & f[1] & notf[0] & and_res)      | // f = 010 -> AND
               (notf[2] & f[1] & f[0] & or_res)          | // f = 011 -> OR
               (f[2] & notf[1] & notf[0] & xor_res)      | // f = 100 -> XOR
               (f[2] & notf[1] & f[0] & notA_res)        | // f = 101 -> NOT A
               (f[2] & f[1] & notf[0] & a_res)           | // f = 110 -> A
               (f[2] & f[1] & f[0] & notB_res);            // f = 111 -> NOT B

    // Seleção do carry out com base na operação
    assign cout = (notf[2] & notf[1] & notf[0] & soma_cout) | // f = 000 -> Carry out da soma
                  (notf[2] & notf[1] & f[0] & sub_cout);     // f = 001 -> Borrow out da subtração

endmodule
