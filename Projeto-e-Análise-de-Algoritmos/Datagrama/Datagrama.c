#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// Definição da estrutura do pacote
typedef struct {
    char **lista;
    int tamanho;
    int indice;
} Lista;

// Troca dois pacotes
static inline void troca(Lista *arr1, Lista *arr2) {
    Lista temp = *arr1;
    *arr1 = *arr2;
    *arr2 = temp;
}

// Função para manter a propriedade do heap máximo
void heapify(Lista *arr, int T, int i) {
    int P = i, E = 2 * i + 1, D = 2 * i + 2;

    if (E < T && arr[E].indice > arr[P].indice)
        P = E;
    if (D < T && arr[D].indice > arr[P].indice)
        P = D;
    
    if (P != i) {
        troca(&arr[i], &arr[P]);
        heapify(arr, T, P);
    }
}

// Algoritmo HeapSort
void heapSort(Lista *arr, int n) {
    for (int i = n / 2 - 1; i >= 0; i--)
        heapify(arr, n, i);
    for (int i = n - 1; i > 0; i--) {
        troca(&arr[0], &arr[i]);
        heapify(arr, i, 0);
    }
}

// Função para escrever a saída no arquivo
void escrever_saida(FILE *saida, Lista *lista_Pacotes, int *cont, int totalPacotes) {
    while (*cont < totalPacotes && *cont == lista_Pacotes[*cont].indice) {
        fprintf(saida, "|");
        for (int j = 0; j < lista_Pacotes[*cont].tamanho; j++) {
            fprintf(saida, "%s", lista_Pacotes[*cont].lista[j]);
            if (j != lista_Pacotes[*cont].tamanho - 1)
                fprintf(saida, ",");
        }
        (*cont)++;
    }
    fprintf(saida, "|");
    if (*cont < totalPacotes - 1) {
        fprintf(saida, "\n");
    }
}

int main(int argc, char *argv[]) {
    FILE *entrada = fopen(argv[1], "r");
    FILE *saida = fopen(argv[2], "w");
    
    if (!entrada || !saida) {
        printf("Erro ao abrir os arquivos!\n");
        exit(1);
    }
    
    int totalPacotes, qtdPacotes, cont = 0;
    fscanf(entrada, "%d %d", &totalPacotes, &qtdPacotes);
    
    Lista *lista_Pacotes = (Lista *)malloc(totalPacotes * sizeof(Lista));
    if (!lista_Pacotes) {
        printf("Erro ao alocar memória para os pacotes!\n");
        fclose(entrada);
        fclose(saida);
        exit(1);
    }
    
    for (int i = 0; i < totalPacotes; i++) {
        fscanf(entrada, "%d %d", &lista_Pacotes[i].indice, &lista_Pacotes[i].tamanho);
        lista_Pacotes[i].lista = (char **)malloc(lista_Pacotes[i].tamanho * sizeof(char *));
        
        for (int j = 0; j < lista_Pacotes[i].tamanho; j++) {
            lista_Pacotes[i].lista[j] = (char *)malloc(20 * sizeof(char));
            fscanf(entrada, "%s", lista_Pacotes[i].lista[j]);
        }

        // Verifica se o pacote atual faz parte da sequência
        if (lista_Pacotes[i].indice == cont) {
            heapSort(lista_Pacotes, i + 1);
            escrever_saida(saida, lista_Pacotes, &cont, totalPacotes);
        }
    }
    
    // Liberação de memória
    for (int i = 0; i < totalPacotes; i++) {
        for (int j = 0; j < lista_Pacotes[i].tamanho; j++) {
            free(lista_Pacotes[i].lista[j]);
        }
        free(lista_Pacotes[i].lista);
    }
    free(lista_Pacotes);

    fclose(entrada);
    fclose(saida);
    return 0;
}
