#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <stdint.h>

#define TAMANHO_PLACA 8
#define TAMANHO_CODIGO 14

typedef struct carga {
    char codigo[TAMANHO_CODIGO];
    float valor;
    int peso;
    int volume;
    int usada;
} Carga;

typedef struct veiculo {
    char placa[TAMANHO_PLACA];
    int peso;
    int volume;
    Carga* cargas;
    float valor;
    int pesoTotal;
    int volumeTotal;
    int numCargas;
} Veiculo;

static float*** matriz = NULL;
static Carga* cargasPendentes;
static int Total_cargas, Total_veiculos;
static int matriz_n = 0, matriz_p = 0, matriz_v = 0;

Veiculo* alocarVeiculos(int quant) {
    Veiculo* matriz = (Veiculo*)malloc(quant * sizeof(Veiculo));
    if (!matriz) {
        printf("Erro de alocação\n");
        exit(1);
    }
    for (int i = 0; i < quant; i++) {
        matriz[i].cargas = NULL;
        matriz[i].peso = 0;
        matriz[i].volume = 0;
        matriz[i].valor = 0;
        matriz[i].pesoTotal = 0;
        matriz[i].volumeTotal = 0;
        matriz[i].numCargas = 0;
    }
    return matriz;
}

Carga* alocarCargas(int quant) {
    Carga* matriz = (Carga*)malloc(quant * sizeof(Carga));
    if (!matriz) {
        printf("Erro de alocação\n");
        exit(1);
    }
    for (int i = 0; i < quant; i++) {
        matriz[i].codigo[0] = '\0';
        matriz[i].usada = 0;
    }
    return matriz;
}

float*** realocarMatriz3D(int n, int p, int v) {
    if (matriz == NULL) {
        // Primeira alocação
        float*** m = (float***)malloc((n + 1) * sizeof(float**));
        for (int i = 0; i <= n; i++) {
            m[i] = (float**)malloc((p + 1) * sizeof(float*));
            for (int j = 0; j <= p; j++) {
                m[i][j] = (float*)calloc((v + 1), sizeof(float));
            }
        }
        matriz_n = n;
        matriz_p = p;
        matriz_v = v;
        return m;
    }

    // Se todas as dimensões são menores ou iguais, manter a matriz atual
    if (n <= matriz_n && p <= matriz_p && v <= matriz_v) {
        return matriz;
    }

    // Determinar as novas dimensões máximas
    int novo_n = (n > matriz_n) ? n : matriz_n;
    int novo_p = (p > matriz_p) ? p : matriz_p;
    int novo_v = (v > matriz_v) ? v : matriz_v;

    // Criar nova matriz com dimensões maiores
    float*** nova_matriz = (float***)malloc((novo_n + 1) * sizeof(float**));
    for (int i = 0; i <= novo_n; i++) {
        nova_matriz[i] = (float**)malloc((novo_p + 1) * sizeof(float*));
        for (int j = 0; j <= novo_p; j++) {
            nova_matriz[i][j] = (float*)calloc((novo_v + 1), sizeof(float));
        }
    }

    // Copiar valores antigos
    for (int i = 0; i <= matriz_n; i++) {
        for (int j = 0; j <= matriz_p; j++) {
            for (int k = 0; k <= matriz_v; k++) {
                nova_matriz[i][j][k] = matriz[i][j][k];
            }
        }
    }

    // Liberar matriz antiga
    for (int i = 0; i <= matriz_n; i++) {
        for (int j = 0; j <= matriz_p; j++) {
            free(matriz[i][j]);
        }
        free(matriz[i]);
    }
    free(matriz);

    matriz_n = novo_n;
    matriz_p = novo_p;
    matriz_v = novo_v;
    
    return nova_matriz;
}

void adicionarCarga(Veiculo* v, Carga* c) {
    if (v->cargas == NULL) {
        v->cargas = alocarCargas(Total_cargas);
    }
    v->cargas[v->numCargas] = *c;
    v->pesoTotal += c->peso;
    v->volumeTotal += c->volume;
    v->valor += c->valor;
    v->numCargas++;
    c->usada = 1;
}

// Função auxiliar para verificar se uma posição está disponível
int verificarPosicaoDisponivel(int n, int p, int vol, Carga* cargas) {
    // Simula o processo de recuperação para ver se as cargas necessárias estão disponíveis
    int tempP = p;
    int tempVol = vol;
    int posicaoValida = 0;
    
    for (int i = n; i > 0 && tempP > 0 && tempVol > 0; i--) {
        if (matriz[i][tempP][tempVol] != matriz[i-1][tempP][tempVol]) {
            // Se encontrou uma carga que seria necessária mas já está em uso
            if (cargas[i-1].usada) {
                posicaoValida = 1;
                break;
            }
            if (tempP >= cargas[i-1].peso && tempVol >= cargas[i-1].volume) {
                tempP -= cargas[i-1].peso;
                tempVol -= cargas[i-1].volume;
            }
        }
    }
    
    return posicaoValida;
}

void resolverKnapsack(Veiculo* v, Carga* cargas, int n) {
    // Realocar matriz apenas se necessário
    matriz = realocarMatriz3D(n, v->peso, v->volume);
    
    // Verificar se o valor já está calculado na matriz
    if (matriz[n][v->peso][v->volume] != 0) {
        float melhorValor = 0;
        int melhorPeso = v->peso;
        int melhorVolume = v->volume;
        int encontrouDisponivel = 1;
        
        // Primeiro tenta usar a posição atual se estiver disponível
        if (verificarPosicaoDisponivel(n, v->peso, v->volume, cargas)) {
            melhorValor = matriz[n][v->peso][v->volume];
            encontrouDisponivel = 0;
        }
        
        // Se a posição atual está ocupada ou tem valor zero, procura nas vizinhas 
        if (!encontrouDisponivel) {
            // Verifica uma vizinhança na matriz
            for (int p = v->peso - 2; p <= v->peso + 2; p++) {
                for (int vol = v->volume - 2; vol <= v->volume + 2; vol++) {
                    // Verifica se os índices são válidos
                    if (p >= 0 && p <= v->peso && vol >= 0 && vol <= v->volume) {
                        // Verifica se a posição está disponível e tem valor melhor
                        if (verificarPosicaoDisponivel(n, p, vol, cargas) && 
                            matriz[n][p][vol] > melhorValor) {
                            melhorValor = matriz[n][p][vol];
                            melhorPeso = p;
                            melhorVolume = vol;
                            encontrouDisponivel = 0;
                        }
                    }
                }
            }
        }
        
        // Se encontrou uma posição disponível, usa ela
        if (encontrouDisponivel) {
            int p = melhorPeso;
            int vol = melhorVolume;
            
            for (int i = n; i > 0 && p > 0 && vol > 0; i--) {
                if (!cargas[i-1].usada && 
                    matriz[i][p][vol] != matriz[i-1][p][vol] &&
                    p >= cargas[i-1].peso && 
                    vol >= cargas[i-1].volume) {
                    
                    adicionarCarga(v, &cargas[i-1]);
                    p -= cargas[i-1].peso;
                    vol -= cargas[i-1].volume;
                }
            }
            return;
        }
    }
    
    // Preencher a matriz de programação dinâmica
    for (int i = 1; i <= n; i++) {
        for (int p = 0; p <= v->peso; p++) {
            for (int vol = 0; vol <= v->volume; vol++) {
                if (!cargas[i-1].usada && cargas[i-1].peso <= p && cargas[i-1].volume <= vol) {
                    float incluir = cargas[i-1].valor + matriz[i-1][p - cargas[i-1].peso][vol - cargas[i-1].volume];
                    float naoIncluir = matriz[i-1][p][vol];
                    matriz[i][p][vol] = (incluir > naoIncluir) ? incluir : naoIncluir;
                } else {
                    matriz[i][p][vol] = matriz[i-1][p][vol];
                }
            }
        }
    }
    
    // Recuperar a solução
    int p = v->peso;
    int vol = v->volume;
    
    for (int i = n; i > 0 && p > 0 && vol > 0; i--) {
        if (!cargas[i-1].usada && 
            matriz[i][p][vol] != matriz[i-1][p][vol] &&
            p >= cargas[i-1].peso && 
            vol >= cargas[i-1].volume) {
            
            adicionarCarga(v, &cargas[i-1]);
            p -= cargas[i-1].peso;
            vol -= cargas[i-1].volume;
        }
    }
}



int main(int argc, char* argv[]) {
    FILE* entrada = fopen(argv[1], "r");
    if (!entrada) {
        printf("Erro ao abrir o arquivo\n");
        exit(1);
    }

    FILE* saida = fopen(argv[2], "w");

    fscanf(entrada, "%d", &Total_veiculos);
    Veiculo* veiculos = alocarVeiculos(Total_veiculos);

    for (int i = 0; i < Total_veiculos; i++) {
        fscanf(entrada, "%7s %d %d", veiculos[i].placa, &veiculos[i].peso, &veiculos[i].volume);
    }

    fscanf(entrada, "%d", &Total_cargas);
    cargasPendentes = alocarCargas(Total_cargas);

    for (int i = 0; i < Total_cargas; i++) {
        fscanf(entrada, "%13s %f %d %d", 
               cargasPendentes[i].codigo, 
               &cargasPendentes[i].valor, 
               &cargasPendentes[i].peso, 
               &cargasPendentes[i].volume);
    }

    fclose(entrada);

    // Resolver para cada veículo
    for (int i = 0; i < Total_veiculos; i++) {
        resolverKnapsack(&veiculos[i], cargasPendentes, Total_cargas);
    }

    // Imprimir resultados
    for (int i = 0; i < Total_veiculos; i++) {
        int pesoPcrt = (int) (roundf(((float) veiculos[i].pesoTotal) * 100/veiculos[i].peso));
        int volumePrct = (int) (roundf(((float) veiculos[i].volumeTotal) * 100/veiculos[i].volume));

        fprintf(saida,"[%s]R$%.2f,%dKG(%d%%),%dL(%d%%)->", veiculos[i].placa,veiculos[i].valor,veiculos[i].pesoTotal,pesoPcrt,veiculos[i].volumeTotal,volumePrct);
        for (int j = veiculos[i].numCargas-1; j >= 0; j--) {
            fprintf(saida,"%s", veiculos[i].cargas[j].codigo);
            if(j > 0){
                fprintf(saida,",");
            }
        }
        fprintf(saida,"\n");
    }


    float valorPendente = 0;
    int pesoPendente = 0, volumePendente = 0;

    for(int i = 0; i < Total_cargas; i++){
        if(!cargasPendentes[i].usada){
            valorPendente += cargasPendentes[i].valor;
            pesoPendente += cargasPendentes[i].peso;
            volumePendente += cargasPendentes[i].volume;
        }
    }

    fprintf(saida,"PENDENTE:R$%.2f,%dKG,%dL->",valorPendente, pesoPendente, volumePendente);
    for (int i = 0; i < Total_cargas; i++) {
        if (!cargasPendentes[i].usada) {
            fprintf(saida,"%s", cargasPendentes[i].codigo);
            if(i < Total_cargas-1){
                fprintf(saida,",");
            }
        }
        
    }
    fprintf(saida,"\n");

    return 0;
}
