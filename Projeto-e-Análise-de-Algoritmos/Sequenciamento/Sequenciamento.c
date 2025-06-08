#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

typedef struct {
    char* subcadeia;
    int tamanho_subcadeia;
    int tem_correspondencia;
} Genes;

typedef struct {
    char codigo[9];
    int qtd_genes;
    Genes* subcadeias;
    int num_ocorrencias;
    int resultado;
    int indice;
} Doenca;

static char dna[40001];
static int numero_doencas, tamanho_subcadeia;

static void calcular_tabela(int* k, const char* padrao, int m) {
    k[0] = -1;
    for(int i = 1, j = -1; i < m; i++) {
        while(j >= 0 && padrao[j + 1] != padrao[i]) j = k[j];
        if(padrao[j + 1] == padrao[i]) j++;
        k[i] = j;
    }
}

static inline void buscar_KMP(const char* texto, Genes* gene, int n, int m) {
    int k[1001];  // Alocação na stack em vez do heap
    calcular_tabela(k, gene->subcadeia, m);
    
    int limite = m * 9 / 10;
    gene->tem_correspondencia = 0;
    
    for(int i = 0, j = -1; i < n; i++) {
        while(j >= 0 && gene->subcadeia[j + 1] != texto[i]) j = k[j];
        if(gene->subcadeia[j + 1] == texto[i]) j++;
        if(j >= limite) {
            gene->tem_correspondencia = 1;
            break;  // Podemos parar assim que encontrarmos uma correspondência
        }
    }
}

static void merge(Doenca *arr, int low, int mid, int high) {
    int i = low, j = mid + 1, k = 0;
    Doenca temp[high - low + 1];
    
    while(i <= mid && j <= high) {
        if(arr[i].resultado > arr[j].resultado || 
          (arr[i].resultado == arr[j].resultado && arr[i].indice < arr[j].indice)) {
            temp[k++] = arr[i++];
        } else {
            temp[k++] = arr[j++];
        }
    }
    
    while(i <= mid) temp[k++] = arr[i++];
    while(j <= high) temp[k++] = arr[j++];
    
    memcpy(arr + low, temp, sizeof(Doenca) * (high - low + 1));
}

static void mergeSort(Doenca *arr, int low, int high) {
    if(low < high) {
        int mid = (low + high) >> 1;
        mergeSort(arr, low, mid);
        mergeSort(arr, mid + 1, high);
        merge(arr, low, mid, high);
    }
}

static void liberarMemoria(Doenca* lista) {
    if (!lista) return;
    for(int i = 0; i < numero_doencas; i++) {
        if(lista[i].subcadeias) {
            for(int j = 0; j < lista[i].qtd_genes; j++) {
                free(lista[i].subcadeias[j].subcadeia);
            }
            free(lista[i].subcadeias);
        }
    }
    free(lista);
}

int main(int argc, char* argv[]) {
    FILE* entrada = fopen(argv[1], "r");
    FILE* saida = fopen(argv[2], "w");
    if(!entrada || !saida) {
        if(entrada) fclose(entrada);
        if(saida) fclose(saida);
        return 1;
    }

    fscanf(entrada, "%d %s %d", &tamanho_subcadeia, dna, &numero_doencas);
    int n = strlen(dna);
    
    Doenca* lista = calloc(numero_doencas, sizeof(Doenca));
    if(!lista) {
        fclose(entrada);
        fclose(saida);
        return 1;
    }

    for(int i = 0; i < numero_doencas; i++) {
        fscanf(entrada, "%s %d", lista[i].codigo, &lista[i].qtd_genes);
        lista[i].subcadeias = malloc(lista[i].qtd_genes * sizeof(Genes));
        lista[i].indice = i;
        
        if(!lista[i].subcadeias) continue;

        for(int j = 0; j < lista[i].qtd_genes; j++) {
            lista[i].subcadeias[j].subcadeia = malloc(1001);
            if(!lista[i].subcadeias[j].subcadeia) continue;
            
            fscanf(entrada, "%s", lista[i].subcadeias[j].subcadeia);
            int m = strlen(lista[i].subcadeias[j].subcadeia);
            lista[i].subcadeias[j].tamanho_subcadeia = m;
            
            buscar_KMP(dna, &lista[i].subcadeias[j], n, m);
            lista[i].num_ocorrencias += lista[i].subcadeias[j].tem_correspondencia;
        }
        
        lista[i].resultado = (int)roundf(((float)lista[i].num_ocorrencias * 100) / lista[i].qtd_genes);
        printf("%d \n", lista[i].resultado);
    }

    mergeSort(lista, 0, numero_doencas - 1);

    for(int i = 0; i < numero_doencas; i++) {
        fprintf(saida, "%s->%d%%\n", lista[i].codigo, lista[i].resultado);
    }

    printf("%d %s\n", lista[4].qtd_genes, lista[4].subcadeias[1].subcadeia);
    printf("%d %s\n", lista[4].qtd_genes, lista[4].subcadeias[2].subcadeia);
    printf("%d %d\n", lista[4].num_ocorrencias, lista[4].resultado);



    liberarMemoria(lista);
    fclose(entrada);
    fclose(saida);
    return 0;
}
