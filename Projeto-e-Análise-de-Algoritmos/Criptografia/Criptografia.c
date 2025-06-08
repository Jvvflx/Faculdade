#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <ctype.h>

// Constante para o número máximo de dígitos
#define MAX_DIGITS 2048
#define PALAVRAS 64

typedef struct big {
    uint32_t digitos[PALAVRAS];
    int size;
    int sinal;
} BigInteger;

typedef struct aes_t {
    uint8_t* c0;
    uint8_t* k;
    uint8_t* ke;
    size_t Nk;
} aes_t;


// Matriz fixa de Galois
int matrizFixa[4][4] = {
    {0x02 , 0x03 , 0x01 , 0x01},
    {0x01 , 0x02 , 0x03 , 0x01},
    {0x01 , 0x01 , 0x02 , 0x03},
    {0x03 , 0x01 , 0x01 , 0x02}
};

int INVmatrizFixa[4][4] = {
    {0x0E , 0x0B , 0x0D , 0x09},
    {0x09 , 0x0E , 0x0B , 0x0D},
    {0x0D , 0x09 , 0x0E , 0x0B},
    {0x0B , 0x0D , 0x09 , 0x0E}
};

// Matriz de substituição de bytes
uint8_t s_box[256] = {
    0x63, 0x7C, 0x77, 0x7B, 0xF2, 0x6B, 0x6F, 0xC5, 0x30, 0x01, 0x67, 0x2B, 0xFE, 0xD7, 0xAB, 0x76,
    0xCA, 0x82, 0xC9, 0x7D, 0xFA, 0x59, 0x47, 0xF0, 0xAD, 0xD4, 0xA2, 0xAF, 0x9C, 0xA4, 0x72, 0xC0,
    0xB7, 0xFD, 0x93, 0x26, 0x36, 0x3F, 0xF7, 0xCC, 0x34, 0xA5, 0xE5, 0xF1, 0x71, 0xD8, 0x31, 0x15,
    0x04, 0xC7, 0x23, 0xC3, 0x18, 0x96, 0x05, 0x9A, 0x07, 0x12, 0x80, 0xE2, 0xEB, 0x27, 0xB2, 0x75,
    0x09, 0x83, 0x2C, 0x1A, 0x1B, 0x6E, 0x5A, 0xA0, 0x52, 0x3B, 0xD6, 0xB3, 0x29, 0xE3, 0x2F, 0x84,
    0x53, 0xD1, 0x00, 0xED, 0x20, 0xFC, 0xB1, 0x5B, 0x6A, 0xCB, 0xBE, 0x39, 0x4A, 0x4C, 0x58, 0xCF,
    0xD0, 0xEF, 0xAA, 0xFB, 0x43, 0x4D, 0x33, 0x85, 0x45, 0xF9, 0x02, 0x7F, 0x50, 0x3C, 0x9F, 0xA8,
    0x51, 0xA3, 0x40, 0x8F, 0x92, 0x9D, 0x38, 0xF5, 0xBC, 0xB6, 0xDA, 0x21, 0x10, 0xFF, 0xF3, 0xD2,
    0xCD, 0x0C, 0x13, 0xEC, 0x5F, 0x97, 0x44, 0x17, 0xC4, 0xA7, 0x7E, 0x3D, 0x64, 0x5D, 0x19, 0x73,
    0x60, 0x81, 0x4F, 0xDC, 0x22, 0x2A, 0x90, 0x88, 0x46, 0xEE, 0xB8, 0x14, 0xDE, 0x5E, 0x0B, 0xDB,
    0xE0, 0x32, 0x3A, 0x0A, 0x49, 0x06, 0x24, 0x5C, 0xC2, 0xD3, 0xAC, 0x62, 0x91, 0x95, 0xE4, 0x79,
    0xE7, 0xC8, 0x37, 0x6D, 0x8D, 0xD5, 0x4E, 0xA9, 0x6C, 0x56, 0xF4, 0xEA, 0x65, 0x7A, 0xAE, 0x08,
    0xBA, 0x78, 0x25, 0x2E, 0x1C, 0xA6, 0xB4, 0xC6, 0xE8, 0xDD, 0x74, 0x1F, 0x4B, 0xBD, 0x8B, 0x8A,
    0x70, 0x3E, 0xB5, 0x66, 0x48, 0x03, 0xF6, 0x0E, 0x61, 0x35, 0x57, 0xB9, 0x86, 0xC1, 0x1D, 0x9E,
    0xE1, 0xF8, 0x98, 0x11, 0x69, 0xD9, 0x8E, 0x94, 0x9B, 0x1E, 0x87, 0xE9, 0xCE, 0x55, 0x28, 0xDF,
    0x8C, 0xA1, 0x89, 0x0D, 0xBF, 0xE6, 0x42, 0x68, 0x41, 0x99, 0x2D, 0x0F, 0xB0, 0x54, 0xBB, 0x16
};

// Matriz Inversa de substituição de bytes
uint8_t inv_s_box[256] = {
    0x52, 0x09, 0x6A, 0xD5, 0x30, 0x36, 0xA5, 0x38, 0xBF, 0x40, 0xA3, 0x9E, 0x81, 0xF3, 0xD7, 0xFB,
    0x7C, 0xE3, 0x39, 0x82, 0x9B, 0x2F, 0xFF, 0x87, 0x34, 0x8E, 0x43, 0x44, 0xC4, 0xDE, 0xE9, 0xCB,
    0x54, 0x7B, 0x94, 0x32, 0xA6, 0xC2, 0x23, 0x3D, 0xEE, 0x4C, 0x95, 0x0B, 0x42, 0xFA, 0xC3, 0x4E,
    0x08, 0x2E, 0xA1, 0x66, 0x28, 0xD9, 0x24, 0xB2, 0x76, 0x5B, 0xA2, 0x49, 0x6D, 0x8B, 0xD1, 0x25,
    0x72, 0xF8, 0xF6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xD4, 0xA4, 0x5C, 0xCC, 0x5D, 0x65, 0xB6, 0x92,
    0x6C, 0x70, 0x48, 0x50, 0xFD, 0xED, 0xB9, 0xDA, 0x5E, 0x15, 0x46, 0x57, 0xA7, 0x8D, 0x9D, 0x84,
    0x90, 0xD8, 0xAB, 0x00, 0x8C, 0xBC, 0xD3, 0x0A, 0xF7, 0xE4, 0x58, 0x05, 0xB8, 0xB3, 0x45, 0x06,
    0xD0, 0x2C, 0x1E, 0x8F, 0xCA, 0x3F, 0x0F, 0x02, 0xC1, 0xAF, 0xBD, 0x03, 0x01, 0x13, 0x8A, 0x6B,
    0x3A, 0x91, 0x11, 0x41, 0x4F, 0x67, 0xDC, 0xEA, 0x97, 0xF2, 0xCF, 0xCE, 0xF0, 0xB4, 0xE6, 0x73,
    0x96, 0xAC, 0x74, 0x22, 0xE7, 0xAD, 0x35, 0x85, 0xE2, 0xF9, 0x37, 0xE8, 0x1C, 0x75, 0xDF, 0x6E,
    0x47, 0xF1, 0x1A, 0x71, 0x1D, 0x29, 0xC5, 0x89, 0x6F, 0xB7, 0x62, 0x0E, 0xAA, 0x18, 0xBE, 0x1B,
    0xFC, 0x56, 0x3E, 0x4B, 0xC6, 0xD2, 0x79, 0x20, 0x9A, 0xDB, 0xC0, 0xFE, 0x78, 0xCD, 0x5A, 0xF4,
    0x1F, 0xDD, 0xA8, 0x33, 0x88, 0x07, 0xC7, 0x31, 0xB1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xEC, 0x5F,
    0xEF, 0x60, 0x51, 0x7F, 0xA9, 0x19, 0xB5, 0x4A, 0x0D, 0x2D, 0xE5, 0x7A, 0x9F, 0x93, 0xC9, 0x9C,
    0xA0, 0xE0, 0x3B, 0x4D, 0xAE, 0x2A, 0xF5, 0xB0, 0xC8, 0xEB, 0xBB, 0x3C, 0x83, 0x53, 0x99, 0x61,
    0x17, 0x2B, 0x04, 0x7E, 0xBA, 0x77, 0xD6, 0x26, 0xE1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0C, 0x7D
};

// Round Constant (1 <= i <= 10 -> i^(i - 1))
const uint8_t rcon[11] = { 0x00, 0x01, 0x02, 0x04,
    0x08, 0x10, 0x20, 0x40, 0x80, 0x1B, 0x36 };

int M[9] = {1,0,0,0,1,1,0,1,1};

uint8_t adicaoGF(uint8_t a, uint8_t b){
    return a ^ b;
}

uint8_t multiplicacaoGF(uint8_t a, uint8_t b){

    uint8_t c = 0, m = 0x1B;

    while(b > 0){
        c = c ^ ((b & 1) * a);
        a = (a << 1) ^ ((a >> 7) * m);
        b = b >> 1;
    }

    return c;
}

void shiftEsq(uint8_t* dados, int i, int larg){
    int shift = i % larg;
        
    if (shift > 0) {
        // Matriz Temporária
        uint8_t* temp = (uint8_t*)malloc(larg * sizeof(uint8_t));
        if (!temp) {
            printf("Memory allocation error\n");
            return;
        }
        
        // Cópiada fileira
        for (int j = 0; j < larg; j++) {
            temp[j] = dados[j];
        }
        
        // Shift da fileira
        for (int j = 0; j < larg; j++) {
            dados[j] = temp[(j + larg + shift) % larg];
        }
        
        free(temp);
    }
}

void shiftDir(uint8_t* dados, int i, int larg){
    int shift = i % larg;
        
    if (shift > 0) {
        // Matriz Temporária
        uint8_t* temp = (uint8_t*)malloc(larg * sizeof(uint8_t));
        if (!temp) {
            printf("Memory allocation error\n");
            return;
        }
        
        // Cópiada fileira
        for (int j = 0; j < larg; j++) {
            temp[j] = dados[j];
        }
        
        // Unshift da fileira
        for (int j = 0; j < larg; j++) {
            dados[j] = temp[(j + larg - shift) % larg];
        }
        
        free(temp);
    }
}

// Função de shift para esquerda
void shiftRows(uint8_t state[4][4]) {
    if (state == NULL) {
        printf("Invalid matrix pointer\n");
        return;
    }

    for (int i = 0; i < 4; i++) {
        shiftEsq(state[i], i, 4);
    }
}

// Função de shift para direita
void INVshiftRows(uint8_t state[4][4]) {
    if (state == NULL) {
        printf("Invalid matrix pointer\n");
        return;
    }

    for (int i = 0; i < 4; i++) {
        shiftDir(state[i], i, 4);
    }
}

void mixColumns(uint8_t state[4][4]){
    
    uint8_t temp[4][4];

    for (int j = 0; j < 4; j++) { // Processa cada coluna separadamente
        for (int i = 0; i < 4; i++) {
            temp[i][j] = 
                multiplicacaoGF(matrizFixa[i][0], state[0][j]) ^
                multiplicacaoGF(matrizFixa[i][1], state[1][j]) ^
                multiplicacaoGF(matrizFixa[i][2], state[2][j]) ^
                multiplicacaoGF(matrizFixa[i][3], state[3][j]);
        }
    }
    
    // Atualização dos valores da matriz
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            state[i][j] = temp[i][j];
        }
    }

}

void INVmixColumns(uint8_t state[4][4]){
    
    uint8_t temp[4][4];

    for (int j = 0; j < 4; j++) { // Processa cada coluna separadamente
        for (int i = 0; i < 4; i++) {
            temp[i][j] = 
                multiplicacaoGF(INVmatrizFixa[i][0], state[0][j]) ^
                multiplicacaoGF(INVmatrizFixa[i][1], state[1][j]) ^
                multiplicacaoGF(INVmatrizFixa[i][2], state[2][j]) ^
                multiplicacaoGF(INVmatrizFixa[i][3], state[3][j]);
        }
    }
    
    // Atualização dos valores da matriz
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            state[i][j] = temp[i][j];
        }
    }

}

void rotWord(uint8_t dados[4]) {
    // [d1, d2, d3, d0] <- [d0, d1, d2, d3]
    uint8_t rotation[4] = { dados[1], dados[2], dados[3],
        dados[0] };
    memcpy(dados, rotation , 4);

}

void subWord(uint8_t data[4]) {
    // [sbox[d0], sbox[d1], sbox[d2], sbox[d3]] <- [d0, d1, d2, d3]
    data[0] = s_box[data[0]];
    data[1] = s_box[data[1]];
    data[2] = s_box[data[2]];
    data[3] = s_box[data[3]];
}

void subBytes(uint8_t data[4][4]) {

    for(int i = 0; i < 4; i++){
        data[i][0] = s_box[data[i][0]];
        data[i][1] = s_box[data[i][1]];
        data[i][2] = s_box[data[i][2]];
        data[i][3] = s_box[data[i][3]];
    }

}

void INVsubBytes(uint8_t data[4][4]) {

    for(int i = 0; i < 4; i++){
        data[i][0] = inv_s_box[data[i][0]];
        data[i][1] = inv_s_box[data[i][1]];
        data[i][2] = inv_s_box[data[i][2]];
        data[i][3] = inv_s_box[data[i][3]];
    }

}

// Função auxiliar para escrever 4 bytes de `in` para `out` no índice `outIndex`
void WriteWord(uint8_t* out, uint8_t outIndex, uint8_t* in, uint8_t inIndex) {
    out[outIndex] = in[inIndex];
    out[outIndex + 1] = in[inIndex + 1];
    out[outIndex + 2] = in[inIndex + 2];
    out[outIndex + 3] = in[inIndex + 3];
}

// Função auxiliar para escrever 4 bytes de `in` com XOR em `out` no índice `outIndex`
void WriteWordXor(uint8_t* out, uint8_t outIndex, uint8_t* in, uint8_t inIndex, uint8_t* xorValue) {
    out[outIndex] = out[outIndex] ^ xorValue[0];
    out[outIndex + 1] = out[outIndex + 1] ^ xorValue[1];
    out[outIndex + 2] = out[outIndex + 2] ^ xorValue[2];
    out[outIndex + 3] = out[outIndex + 3] ^ xorValue[3];
}

// Procedimento KeyExpansion
void KeyExpansion(uint8_t* out, uint8_t* in, uint8_t Nk) {
    const uint8_t Nr = (Nk == 4) ? 10 : (Nk == 6) ? 12 : 14;
    uint8_t temp[4];

    // Primeira rodada é a própria chave
    for(uint8_t i = 0; i < Nk; i++) {
        WriteWord(out, i * 4, in, i * 4);  // Corrigir o índice com *4
    }

    // Gerando as rodadas a partir das anteriores
    for(uint8_t i = Nk; i < (Nr + 1) * 4; i++) {  // Ajuste na condição de loop

        WriteWord(temp, 0, out, (i - 1) * 4);  // Corrigir o índice com *4

        if (i % Nk == 0) {
            rotWord(temp); 
            subWord(temp);
            temp[0] = temp[0] ^ rcon[i / Nk];  // Certifique-se de que rcon está inicializado
        }
        else if (Nk > 6 && i % Nk == 4) {
            subWord(temp);
        }

        WriteWordXor(out, i * 4, out, (i - Nk) * 4, temp);  // Corrigir o índice com *4
    }
}

// corrigir função de retorno de chave
void getRoundKey(uint8_t* expandedKey, int round, uint8_t roundKey[4][4]) {
    int startIndex = round * 16; 

    for (int col = 0; col < 4; col++) {
        for (int row = 0; row < 4; row++) {
            roundKey[row][col] = expandedKey[startIndex + col*4 + row];
        }
    }
}

// 
void addRoundKey(uint8_t state[4][4], uint8_t* k, int Nr) {
    uint8_t temp[4][4];
    uint8_t ki[4][4];

    // Obtemos a chave da rodada
    getRoundKey(k, Nr, ki);

    // Realizamos a operação XOR entre o bloco de dados e a chave da rodada
    for (int i = 0; i < 4; i++) {
        state[i][0] = adicaoGF(state[i][0], ki[i][0]);
        state[i][1] = adicaoGF(state[i][1], ki[i][1]);
        state[i][2] = adicaoGF(state[i][2], ki[i][2]);
        state[i][3] = adicaoGF(state[i][3], ki[i][3]);
    }
}

void writeOutput(uint8_t* c, uint8_t state[4][4]){

    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            c[4*j + i] = state[i][j];
        }
    }
}

void readInput(uint8_t state[4][4], const uint8_t* m){

    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            state[i][j] = m[4*j + i];
        }
    }
}

void Cipher(uint8_t* c, const uint8_t* m, uint8_t* k, uint8_t Nr) {

    uint8_t state[4][4];
    
    readInput(state , m);
    addRoundKey(state , k, 0);
    
    for(uint8_t i = 1; i < Nr; i++) {

        subBytes(state);
        shiftRows(state);
        mixColumns(state);
        addRoundKey(state , k, i);
    
    }
    
    subBytes(state);
    shiftRows(state);
    addRoundKey(state , k, Nr);
    writeOutput(c, state);

}

void Decipher(uint8_t* m, const uint8_t* c, uint8_t* k, uint8_t Nr) {
    
    uint8_t state[4][4];
    
    readInput(state , c);
    addRoundKey(state , k, Nr);
    
    for(int8_t i = Nr - 1; i >= 1; i--) {
    
        INVshiftRows(state);
        INVsubBytes(state);
        addRoundKey(state , k, i);
        INVmixColumns(state);
    
    }
    
    INVshiftRows(state);
    INVsubBytes(state);
    addRoundKey(state , k, 0);
    writeOutput(m, state);
    
}


// Inicializa um BigInteger com um valor inteiro
void initialize(BigInteger *num, int value) {
    memset(num->digitos, 0, sizeof(uint32_t) * PALAVRAS);
    
    if (value == 0) {
        num->size = 1;
        num->sinal = 1;
    } else {
        num->size = 1;
        if (value < 0) {
            num->sinal = -1;
            value = -value;
        } else {
            num->sinal = 1;
        }
        
        // Converte o valor para a representação de 32 bits
        num->digitos[0] = (uint32_t)value;
        
        // Corrigido: verificação para valores maiores que 32 bits
        uint64_t val64 = (uint64_t)value;  // Usa uint64_t para suportar o shift de 32 bits
        if (val64 > UINT32_MAX) {
            int i = 1;
            while (val64 > 0) {
                val64 >>= 32;  // Agora seguro pois val64 é uint64_t
                if (val64 > 0) {
                    num->digitos[i++] = (uint32_t)val64;
                    num->size = i;
                }
            }
        }
    }
}

void print(BigInteger *num) {
    printf("BigInteger: sinal=%d, size=%d, valor=", num->sinal, num->size);
    
    // Imprime as palavras de trás para frente (mais significativas primeiro)
    for (int i = num->size - 1; i >= 0; i--) {
        printf("%08X", num->digitos[i]);  // Imprime em hexadecimal
    }
    printf("\n");
}

void functionPrint(FILE* arq, BigInteger *num, char* saida) {
    fprintf(arq, "%s=", saida);
    
    // Imprime as palavras de trás para frente (mais significativas primeiro)
    for (int i = num->size - 1; i >= 0; i--) {
        fprintf(arq, "%08X", num->digitos[i]);  // Imprime em hexadecimal
    }
    fprintf(arq, "\n");
}

// Copia um BigInteger para outro
void copy(BigInteger *dest, BigInteger *src) {
    memcpy(dest->digitos, src->digitos, sizeof(uint32_t) * PALAVRAS);
    dest->size = src->size;
    dest->sinal = src->sinal;
}

// Compara dois BigIntegers: retorna -1 se a < b, 0 se a = b, 1 se a > b
int compare(BigInteger *a, BigInteger *b) {
    if (a->sinal != b->sinal)
        return (a->sinal > b->sinal) ? 1 : -1;
    
    if (a->size != b->size)
        return ((a->size > b->size) == (a->sinal > 0)) ? 1 : -1;
    
    for (int i = a->size - 1; i >= 0; i--) {
        if (a->digitos[i] != b->digitos[i])
            return ((a->digitos[i] > b->digitos[i]) == (a->sinal > 0)) ? 1 : -1;
    }
    
    return 0;
}

// Converte string hexadecimal para BigInteger
void hexToBigInteger(const char *hex, BigInteger *num) {
    memset(num->digitos, 0, sizeof(uint32_t) * PALAVRAS);
    num->sinal = 1;
    
    int len = strlen(hex);
    int numWords = (len + 7) / 8; // Calcula quantas palavras de 32 bits serão necessárias
    num->size = numWords > 0 ? numWords : 1;
    
    // Processa a string hex de trás para frente, convertendo cada grupo de 8 caracteres em uma palavra de 32 bits
    for (int i = 0; i < numWords; i++) {
        int startPos = len - (i + 1) * 8;
        int charsInWord = 8;
        
        if (startPos < 0) {
            charsInWord += startPos;
            startPos = 0;
        }
        
        // Converte os caracteres deste grupo em uma palavra de 32 bits
        uint32_t word = 0;
        for (int j = 0; j < charsInWord; j++) {
            char c = hex[startPos + j];
            int valor;
            
            // Converte caractere hexadecimal para valor numérico
            if (c >= '0' && c <= '9') {
                valor = c - '0';
            } else if (c >= 'A' && c <= 'F') {
                valor = 10 + (c - 'A');
            } else if (c >= 'a' && c <= 'f') {
                valor = 10 + (c - 'a');
            } else {
                // Caractere inválido - ignorar
                continue;
            }
            
            word = (word << 4) | valor;
        }
        
        num->digitos[i] = word;
    }
    
    // Remover zeros à esquerda
    while (num->size > 1 && num->digitos[num->size - 1] == 0) {
        num->size--;
    }
}

// Subtrai dois BigIntegers: result = a - b
void subtract(BigInteger *result, BigInteger *a, BigInteger *b) {
    // Se b > a, troca e inverte o sinal
    if (compare(a, b) < 0) {
        subtract(result, b, a);
        result->sinal = -result->sinal;
        return;
    }
    
    // Inicializa o resultado
    memset(result->digitos, 0, sizeof(uint32_t) * PALAVRAS);
    result->sinal = a->sinal;
    result->size = a->size;
    
    uint32_t borrow = 0;
    for (int i = 0; i < a->size; i++) {
        uint64_t diff = (uint64_t)a->digitos[i] - borrow;
        
        if (i < b->size) {
            diff -= b->digitos[i];
        }
        
        if (diff > UINT32_MAX) {
            // Houve empréstimo
            result->digitos[i] = (uint32_t)diff;
            borrow = 1;
        } else {
            result->digitos[i] = (uint32_t)diff;
            borrow = 0;
        }
    }
    
    // Elimina zeros à esquerda
    while (result->size > 1 && result->digitos[result->size - 1] == 0) {
        result->size--;
    }
}

// Soma dois BigIntegers: result = a + b
void add(BigInteger *result, BigInteger *a, BigInteger *b) {
    // Inicializa o resultado
    memset(result->digitos, 0, sizeof(uint32_t) * PALAVRAS);
    result->sinal = 1; // Assume-se inicialmente que é positivo
    
    // Determina o tamanho máximo entre a e b
    int maxSize = (a->size > b->size) ? a->size : b->size;
    result->size = maxSize;
    
    uint32_t carry = 0;
    for (int i = 0; i < maxSize; i++) {
        uint64_t sum = carry;
        
        if (i < a->size)
            sum += a->digitos[i];
        
        if (i < b->size)
            sum += b->digitos[i];
        
        result->digitos[i] = (uint32_t)sum;
        carry = (uint32_t)(sum >> 32);
    }
    
    // Se houver carry após processar todos os dígitos, adiciona uma nova palavra
    if (carry > 0) {
        if (result->size < PALAVRAS) {
            result->digitos[result->size] = carry;
            result->size++;
        } else {
            // Tratamento de overflow - depende da implementação desejada
        }
    }
}

// Multiplica dois BigIntegers: result = a * b
void multiply(BigInteger *result, BigInteger *a, BigInteger *b) {
    // Inicializa o resultado com zero
    memset(result->digitos, 0, sizeof(uint32_t) * PALAVRAS);
    result->size = 1;
    result->sinal = a->sinal * b->sinal;
    
    // Verificação rápida: se algum dos operandos for zero, o resultado é zero
    if ((a->size == 1 && a->digitos[0] == 0) || (b->size == 1 && b->digitos[0] == 0)) {
        return;
    }
    
    // Algoritmo de multiplicação longa, palavra por palavra
    for (int i = 0; i < a->size; i++) {
        uint32_t carry = 0;
        
        for (int j = 0; j < b->size && (i + j) < PALAVRAS; j++) {
            uint64_t prod = (uint64_t)a->digitos[i] * (uint64_t)b->digitos[j] + 
                           (uint64_t)result->digitos[i + j] + (uint64_t)carry;
            
            result->digitos[i + j] = (uint32_t)prod;
            carry = (uint32_t)(prod >> 32);
        }
        
        // Propaga o carry restante
        int k = i + b->size;
        while (carry > 0 && k < PALAVRAS) {
            uint64_t sum = (uint64_t)result->digitos[k] + carry;
            result->digitos[k] = (uint32_t)sum;
            carry = (uint32_t)(sum >> 32);
            k++;
        }
    }
    
    // Atualiza o tamanho do resultado
    result->size = a->size + b->size;
    if (result->size > PALAVRAS)
        result->size = PALAVRAS;
    
    // Elimina zeros à esquerda
    while (result->size > 1 && result->digitos[result->size - 1] == 0) {
        result->size--;
    }
}

// Divide um BigInteger por 2: result = num / 2
void divideBy2(BigInteger *result, BigInteger *num) {
    // Inicializa o resultado
    copy(result, num);
    
    // Divisão por 2 é um shift à direita em cada palavra
    uint32_t carry = 0;
    for (int i = result->size - 1; i >= 0; i--) {
        uint32_t nextCarry = result->digitos[i] & 1; // Bit que será carregado para a próxima palavra
        result->digitos[i] = (result->digitos[i] >> 1) | (carry << 31);
        carry = nextCarry;
    }
    
    // Elimina zeros à esquerda
    while (result->size > 1 && result->digitos[result->size - 1] == 0) {
        result->size--;
    }
}

// Calcula módulo: result = a % m
void modulus(BigInteger *result, BigInteger *a, BigInteger *m) {
    // Cria cópias de trabalho
    BigInteger temp;
    copy(&temp, a);
    temp.sinal = 1; // Trabalhamos com valores positivos
    
    BigInteger m_positive;
    copy(&m_positive, m);
    m_positive.sinal = 1;
    
    // Inicializa o resultado
    copy(result, &temp);
    
    // Algoritmo de subtração rápida para módulo
    while (compare(result, &m_positive) >= 0) {
        // Estimativa de quantas vezes podemos subtrair m_positive
        // Comparamos apenas as palavras mais significativas para uma estimativa rápida
        int shift = (result->size - m_positive.size) * 32;
        BigInteger shifted_m;
        copy(&shifted_m, &m_positive);
        
        // Desloca m_positive para a esquerda o máximo possível sem exceder result
        if (shift > 0) {
            // Implementação simplificada do deslocamento à esquerda
            for (int i = 0; i < shift; i++) {
                BigInteger doubled;
                add(&doubled, &shifted_m, &shifted_m);
                copy(&shifted_m, &doubled);
                
                // Se excedeu o resultado, volta um passo
                if (compare(&shifted_m, result) > 0) {
                    divideBy2(&shifted_m, &shifted_m);
                    break;
                }
            }
        }
        
        // Subtrai a versão deslocada de m_positive
        BigInteger subtraction_result;
        subtract(&subtraction_result, result, &shifted_m);
        copy(result, &subtraction_result);
    }
    
    // Ajusta o sinal conforme a definição matemática de módulo
    if (a->sinal == -1 && (result->size > 1 || result->digitos[0] > 0)) {
        BigInteger subtraction_result;
        subtract(&subtraction_result, &m_positive, result);
        copy(result, &subtraction_result);
    }
}

// Calcular (g^a mod p)^b mod p
void calcularExponenciacaoModular(BigInteger *result, BigInteger *g, BigInteger *a, BigInteger *b, BigInteger *p) {
    // Calcula g^a mod p
    BigInteger ga_mod_p;
    initialize(&ga_mod_p, 1);
    
    BigInteger zero;
    initialize(&zero, 0);
    
    // Cópia de g para trabalho
    BigInteger gWork;
    copy(&gWork, g);
    
    // Cópia de a para trabalho
    BigInteger aWork;
    copy(&aWork, a);
    
    // Exponenciação modular g^a mod p
    while (compare(&aWork, &zero) > 0) {
        if (aWork.digitos[0] & 1) {
            BigInteger temp;
            multiply(&temp, &ga_mod_p, &gWork);
            modulus(&ga_mod_p, &temp, p);
        }
        
        BigInteger temp;
        multiply(&temp, &gWork, &gWork);
        modulus(&gWork, &temp, p);
        
        divideBy2(&aWork, &aWork);
    }
    
    // Calcula (g^a mod p)^b mod p
    initialize(result, 1);
    
    // Usa ga_mod_p como base
    BigInteger baseWork;
    copy(&baseWork, &ga_mod_p);
    
    // Cópia de b para trabalho
    BigInteger bWork;
    copy(&bWork, b);
    
    // Exponenciação modular para (g^a mod p)^b mod p
    while (compare(&bWork, &zero) > 0) {
        if (bWork.digitos[0] & 1) {
            BigInteger temp;
            multiply(&temp, result, &baseWork);
            modulus(result, &temp, p);
        }
        
        BigInteger temp;
        multiply(&temp, &baseWork, &baseWork);
        modulus(&baseWork, &temp, p);
        
        divideBy2(&bWork, &bWork);
    }
}

void truncate_to_size(BigInteger *res, const BigInteger *a) {
    // Check if truncation is needed
    if (res->size > a->size) {
        // Set the new size of res to match a's size
        res->size = a->size;
        
        // Zero out any excess digits
        for (int i = res->size; i < PALAVRAS; i++) {
            res->digitos[i] = 0;
        }
        
        // Check if the result is now zero
        int is_zero = 1;
        for (int i = 0; i < res->size; i++) {
            if (res->digitos[i] != 0) {
                is_zero = 0;
                break;
            }
        }
        
        
    }
}

int main(int argc, char* argv[]) {
    FILE* entrada = fopen("Entrada", "r"); 
    FILE* saida = fopen("Saida.txt", "w");

    if (!entrada || !saida) {
        if (entrada) fclose(entrada);
        if (saida) fclose(saida);
        printf("Erro ao abrir os arquivos.\n");
        return 1;
    }

    int total_operacoes = 0;

    // Variáveis para os expoentes privados em Diffie-Hellman
    BigInteger a, b;
    // Variáveis para o gerador e o primo
    BigInteger g, p;
    // Variáveis para os resultados das exponenciações
    BigInteger chave_compartilhada;
    BigInteger res1, res2, res3;


    fscanf(entrada, "%d\n", &total_operacoes);

    for(int i = 0; i < total_operacoes; i++) {

        char operacao[3] = "";

        fscanf(entrada, "%s ", operacao);

        char string_A[257], string_B[257];
        char string_G[2049], string_P[2049];
        char string_M[10000], string_C[10000];

        // Sequência de Leitura caso a ocorrência seja de um Diffie-Hellman
        if(operacao[0] == 'd' && operacao[1] == 'h') {

            fscanf(entrada, "%s %s ", string_A, string_B);
            fscanf(entrada, "%s %s", string_G, string_P);

            // Converter strings para BigInteger
            hexToBigInteger(string_A, &a);
            hexToBigInteger(string_B, &b);
            hexToBigInteger(string_G, &g);
            hexToBigInteger(string_P, &p);

            calcularExponenciacaoModular(&chave_compartilhada, &g, &a, &b, &p);

            truncate_to_size(&chave_compartilhada, &a);  
            
            functionPrint(saida, &chave_compartilhada, "s");
        }

        else if(operacao[0] == 'd'){

            fscanf(entrada, "%s\n", string_M);

            fprintf(saida, "%s\n", string_M);

        }

        else if(operacao[0] == 'e'){

            fscanf(entrada, "%s\n", string_C);

            fprintf(saida, "%s\n", string_C);

        }
    }

    

    fclose(entrada);
    fclose(saida);
    
    return 0;
}
