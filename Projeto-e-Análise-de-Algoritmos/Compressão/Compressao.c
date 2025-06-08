#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

typedef struct dados {
    int indice;
    int tamanho;
    char** texto;
    float resultado_HUF;
    float resultado_RLE;
    char* texto_comprimido_bin;  
    char* texto_comprimido_hex_HUF;
    char* texto_comprimido_hex_RLE;
} Dados;

typedef struct noHUF {
    int freq;
    unsigned char simbolo;
    struct noHUF* dir;
    struct noHUF* esq;
} noHUF;

typedef struct filaPrior_min {
    noHUF** elementos;
    int tamanho;
} filaPrior_min;

// Estrutura para armazenar os códigos Huffman
typedef struct {
    char* codigo;
} TabelaCodigos[256];

// Função auxiliar para liberar a árvore
static inline void liberarArvore(noHUF* no) {
    if (no == NULL) return;
    liberarArvore(no->esq);
    liberarArvore(no->dir);
    free(no);
}

static inline void split_into_bytes(Dados* sequencia, const char* input) {
    
    size_t len = strlen(input);
    if (len > 0 && input[len-1] == '\n') {
        len--;
    }

    
    sequencia->texto = (char**)malloc(sequencia->tamanho * sizeof(char*));
    if (!sequencia->texto) {
        fprintf(stderr, "Erro na alocação de memória para texto.\n");
        return;
    }

    
    const char* ptr = input;
    int byte_count = 0;
    
    while (*ptr && byte_count < sequencia->tamanho) {
        
        while (*ptr == ' ' || *ptr == '\t') ptr++;
        
        if (!*ptr) break;
        
        
        sequencia->texto[byte_count] = (char*)malloc(3 * sizeof(char));
        if (!sequencia->texto[byte_count]) {
            fprintf(stderr, "Erro na alocação de memória para byte %d.\n", byte_count);
            for (int j = 0; j < byte_count; j++) {
                free(sequencia->texto[j]);
            }
            free(sequencia->texto);
            sequencia->texto = NULL;
            return;
        }
        
        
        sequencia->texto[byte_count][0] = *ptr++;
        sequencia->texto[byte_count][1] = (*ptr && *ptr != ' ' && *ptr != '\t') ? *ptr++ : '\0';
        sequencia->texto[byte_count][2] = '\0';
        
        byte_count++;
    }
}


static inline int binToDec(const char *bin, int length) {
    int dec = 0;
    for (int i = 0; i < length; i++) {
        dec = (dec << 1) | (bin[i] - '0');
    }
    return dec;
}


static inline char* binToHex(const char *bin) {
    int binLength = strlen(bin);
    int hexLength = (binLength + 7) / 8 * 2;
    
    char *hex = (char*)malloc((hexLength + 1) * sizeof(char));
    if (!hex) return NULL;
    
    char *current = hex;
    
    static const char hexChars[] = "0123456789ABCDEF";
    
    for (int i = 0; i < binLength; i += 8) {
        int chunkSize = (binLength - i < 8) ? (binLength - i) : 8;
        
        
        int value = 0;
        for (int j = 0; j < chunkSize; j++) {
            value = (value << 1) | (bin[i + j] - '0');
        }
        
        
        if (chunkSize < 8) {
            value <<= (8 - chunkSize);
        }
        
        *current++ = hexChars[value >> 4];
        *current++ = hexChars[value & 0xF];
    }
    
    *current = '\0';
    return hex;
}


static inline filaPrior_min* criarFilaPrio_min() {
    filaPrior_min* fpm = (filaPrior_min*)malloc(sizeof(filaPrior_min));
    if (!fpm) return NULL;
    
    fpm->elementos = (noHUF**)malloc(256 * sizeof(noHUF*));
    if (!fpm->elementos) {
        free(fpm);
        return NULL;
    }
    
    fpm->tamanho = 0;
    return fpm;
}

static inline void inserir(filaPrior_min* fpm, int freq, unsigned char caractere, noHUF* esq, noHUF* dir) {
    noHUF* novoNo = (noHUF*)malloc(sizeof(noHUF));
    if (!novoNo) return;
    
    novoNo->freq = freq;
    novoNo->simbolo = caractere;
    novoNo->esq = esq;
    novoNo->dir = dir;
    
    int i = fpm->tamanho++;
    while (i > 0) {
        int parent = (i - 1) / 2;
        if (fpm->elementos[parent]->freq <= freq) break;
        
        fpm->elementos[i] = fpm->elementos[parent];
        i = parent;
    }
    fpm->elementos[i] = novoNo;
}

static inline noHUF* extrair_min(filaPrior_min* fpm) {
    if (fpm->tamanho == 0) return NULL;
    
    noHUF* min = fpm->elementos[0];
    fpm->elementos[0] = fpm->elementos[--fpm->tamanho];
    
    int i = 0;
    while (1) {
        int menorFilho = 2 * i + 1;
        if (menorFilho >= fpm->tamanho) break;
        
        if (menorFilho + 1 < fpm->tamanho && 
            fpm->elementos[menorFilho + 1]->freq < fpm->elementos[menorFilho]->freq)
            menorFilho++;
        
        if (fpm->elementos[i]->freq <= fpm->elementos[menorFilho]->freq) break;
        
        noHUF* temp = fpm->elementos[i];
        fpm->elementos[i] = fpm->elementos[menorFilho];
        fpm->elementos[menorFilho] = temp;
        
        i = menorFilho;
    }
    return min;
}


static inline noHUF* construirArvore(int* V, int n) {
    filaPrior_min* fpm = criarFilaPrio_min();
    if (!fpm) return NULL;
    
    for (int i = 0; i < n; i++) {
        if (V[i]) {
            inserir(fpm, V[i], i, NULL, NULL);
        }
    }
    
    
    if (fpm->tamanho == 0) {
        free(fpm->elementos);
        free(fpm);
        return NULL;
    }
    
    
    if (fpm->tamanho == 1) {
        noHUF* result = fpm->elementos[0];
        free(fpm->elementos);
        free(fpm);
        return result;
    }
    
    while (fpm->tamanho > 1) {
        noHUF* x = extrair_min(fpm);
        noHUF* y = extrair_min(fpm);
        inserir(fpm, x->freq + y->freq, '\0', x, y);
    }
    
    noHUF* result = extrair_min(fpm);
    free(fpm->elementos);
    free(fpm);
    return result;
}


static inline void gerarCodigos(noHUF* raiz, char* codigo_atual, int profundidade, TabelaCodigos tabela) {
    if (!raiz) return;
    
    if (!raiz->esq && !raiz->dir) {
        codigo_atual[profundidade] = '\0';
        
        tabela[(unsigned char)raiz->simbolo].codigo = strdup(codigo_atual);
        
        if (profundidade == 0) {
            free(tabela[(unsigned char)raiz->simbolo].codigo);
            tabela[(unsigned char)raiz->simbolo].codigo = strdup("0");
        }
        return;
    }
    
    if (raiz->esq) {
        codigo_atual[profundidade] = '0';
        gerarCodigos(raiz->esq, codigo_atual, profundidade + 1, tabela);
    }
    
    if (raiz->dir) {
        codigo_atual[profundidade] = '1';
        gerarCodigos(raiz->dir, codigo_atual, profundidade + 1, tabela);
    }
}


static inline void calcularFrequencias(char** bytes, int num_bytes, int* freq) {
    memset(freq, 0, 256 * sizeof(int));
    
    for (int i = 0; i < num_bytes; i++) {
        
        unsigned char valor = 0;
        for (int j = 0; j < 2 && bytes[i][j]; j++) {
            char c = bytes[i][j];
            if (c >= '0' && c <= '9')
                valor = (valor << 4) | (c - '0');
            else if (c >= 'A' && c <= 'F')
                valor = (valor << 4) | (c - 'A' + 10);
            else if (c >= 'a' && c <= 'f')
                valor = (valor << 4) | (c - 'a' + 10);
        }
        freq[valor]++;
    }
}


static inline void comprimirSequencia_HUF(Dados* sequencia) {
    int freq[256] = {0};
    TabelaCodigos tabela = {0};
    
    calcularFrequencias(sequencia->texto, sequencia->tamanho, freq);
    
    noHUF* raiz = construirArvore(freq, 256);
    if (!raiz) {
        
        sequencia->texto_comprimido_bin = strdup("");
        sequencia->texto_comprimido_hex_HUF = strdup("");
        sequencia->resultado_HUF = 0.0f;
        return;
    }
    
    char codigo_atual[256] = {0};
    gerarCodigos(raiz, codigo_atual, 0, tabela);
    
    
    size_t tamanho_comprimido = 0;
    for (int i = 0; i < sequencia->tamanho; i++) {
        unsigned char valor = 0;
        for (int j = 0; j < 2 && sequencia->texto[i][j]; j++) {
            char c = sequencia->texto[i][j];
            if (c >= '0' && c <= '9')
                valor = (valor << 4) | (c - '0');
            else if (c >= 'A' && c <= 'F')
                valor = (valor << 4) | (c - 'A' + 10);
            else if (c >= 'a' && c <= 'f')
                valor = (valor << 4) | (c - 'a' + 10);
        }
        if (tabela[valor].codigo) {
            tamanho_comprimido += strlen(tabela[valor].codigo);
        }
    }
    
    sequencia->texto_comprimido_bin = (char*)malloc((tamanho_comprimido + 1) * sizeof(char));
    if (!sequencia->texto_comprimido_bin) {
        for (int i = 0; i < 256; i++) {
            if (tabela[i].codigo) free(tabela[i].codigo);
        }
        liberarArvore(raiz);
        return;
    }
    
    char* current = sequencia->texto_comprimido_bin;
    for (int i = 0; i < sequencia->tamanho; i++) {
        unsigned char valor = 0;
        for (int j = 0; j < 2 && sequencia->texto[i][j]; j++) {
            char c = sequencia->texto[i][j];
            if (c >= '0' && c <= '9')
                valor = (valor << 4) | (c - '0');
            else if (c >= 'A' && c <= 'F')
                valor = (valor << 4) | (c - 'A' + 10);
            else if (c >= 'a' && c <= 'f')
                valor = (valor << 4) | (c - 'a' + 10);
        }
        
        if (tabela[valor].codigo) {
            char* codigo = tabela[valor].codigo;
            int len = strlen(codigo);
            memcpy(current, codigo, len);
            current += len;
        }
    }
    *current = '\0';
    
    sequencia->texto_comprimido_hex_HUF = binToHex(sequencia->texto_comprimido_bin);
    
    int taxa_original = sequencia->tamanho * 2;
    int taxa_comprimida = strlen(sequencia->texto_comprimido_hex_HUF);
    sequencia->resultado_HUF = (taxa_comprimida > 0 && taxa_original > 0) ? 
                               ((float)taxa_comprimida / taxa_original * 100.0f) : 0.0f;
    
    for (int i = 0; i < 256; i++) {
        if (tabela[i].codigo) {
            free(tabela[i].codigo);
        }
    }
    
    liberarArvore(raiz);
}


static inline void comprimirSequencia_RLE(Dados* sequencia) {
    sequencia->texto_comprimido_hex_RLE = (char*)malloc(sequencia->tamanho * 4 + 1);
    if (!sequencia->texto_comprimido_hex_RLE) return;
    
    static const char hexChars[] = "0123456789ABCDEF";
    char* current = sequencia->texto_comprimido_hex_RLE;
    
    for (int i = 0; i < sequencia->tamanho; i++) {
        int cont = 1;
        
        while (i + cont < sequencia->tamanho && 
               strcmp(sequencia->texto[i], sequencia->texto[i + cont]) == 0) {
            cont++;
        }
        
        
        *current++ = hexChars[cont >> 4];
        *current++ = hexChars[cont & 0xF];
        
        
        unsigned char valor = 0;
        for (int j = 0; j < 2 && sequencia->texto[i][j]; j++) {
            char c = sequencia->texto[i][j];
            if (c >= '0' && c <= '9')
                valor = (valor << 4) | (c - '0');
            else if (c >= 'A' && c <= 'F')
                valor = (valor << 4) | (c - 'A' + 10);
            else if (c >= 'a' && c <= 'f')
                valor = (valor << 4) | (c - 'a' + 10);
        }
        
        *current++ = hexChars[valor >> 4];
        *current++ = hexChars[valor & 0xF];
        
        i += cont - 1;
    }
    
    *current = '\0';
    
    int taxa_original = sequencia->tamanho * 2;
    int taxa_comprimida = strlen(sequencia->texto_comprimido_hex_RLE);
    sequencia->resultado_RLE = (taxa_comprimida > 0 && taxa_original > 0) ? 
                               ((float)taxa_comprimida / taxa_original * 100.0f) : 0.0f;
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Uso: %s arquivo_entrada arquivo_saida\n", argv[0]);
        return 1;
    }

    FILE* entrada = fopen(argv[1], "r");
    FILE* saida = fopen(argv[2], "w");

    if (!entrada || !saida) {
        fprintf(stderr, "Erro ao abrir os arquivos.\n");
        if (entrada) fclose(entrada);
        if (saida) fclose(saida);
        return 1;
    }

    int qtd_sequencias;
    fscanf(entrada, "%d\n", &qtd_sequencias);

    Dados* listaSequencias = (Dados*)malloc(qtd_sequencias * sizeof(Dados));
    if (!listaSequencias) {
        fprintf(stderr, "Erro ao alocar memória.\n");
        fclose(entrada);
        fclose(saida);
        return 1;
    }

    for (int i = 0; i < qtd_sequencias; i++) {
        fscanf(entrada, "%d ", &listaSequencias[i].tamanho);
        listaSequencias[i].indice = i;

        
        size_t buffer_size = listaSequencias[i].tamanho * 3 + 2;  
        char* buffer = (char*)malloc(buffer_size);
        if (!buffer) {
            fprintf(stderr, "Erro ao alocar buffer.\n");
            fclose(entrada);
            fclose(saida);
            return 1;
        }

        if (fgets(buffer, buffer_size, entrada) == NULL) {
            fprintf(stderr, "Erro ao ler linha.\n");
            free(buffer);
            fclose(entrada);
            fclose(saida);
            return 1;
        }

        split_into_bytes(&listaSequencias[i], buffer);
        free(buffer);

        comprimirSequencia_HUF(&listaSequencias[i]);
        comprimirSequencia_RLE(&listaSequencias[i]);
        
        
        if (listaSequencias[i].resultado_HUF < listaSequencias[i].resultado_RLE) {
            fprintf(saida, "%d->HUF(%.2f%%)=%s\n", i, listaSequencias[i].resultado_HUF, 
                   listaSequencias[i].texto_comprimido_hex_HUF);
        }
        else if (listaSequencias[i].resultado_HUF > listaSequencias[i].resultado_RLE) {
            fprintf(saida, "%d->RLE(%.2f%%)=%s\n", i, listaSequencias[i].resultado_RLE, 
                   listaSequencias[i].texto_comprimido_hex_RLE);
        }
        else {
            fprintf(saida, "%d->HUF(%.2f%%)=%s\n", i, listaSequencias[i].resultado_HUF, 
                   listaSequencias[i].texto_comprimido_hex_HUF);
            fprintf(saida, "%d->RLE(%.2f%%)=%s\n", i, listaSequencias[i].resultado_RLE, 
                   listaSequencias[i].texto_comprimido_hex_RLE);
        }
        
        
        free(listaSequencias[i].texto_comprimido_bin);
        free(listaSequencias[i].texto_comprimido_hex_HUF);
        free(listaSequencias[i].texto_comprimido_hex_RLE);
    }

    
    for (int i = 0; i < qtd_sequencias; i++) {
        for (int j = 0; j < listaSequencias[i].tamanho; j++) {
            free(listaSequencias[i].texto[j]);
        }
        free(listaSequencias[i].texto);
    }
    free(listaSequencias);
    
    fclose(entrada);
    fclose(saida);
    return 0;
}
