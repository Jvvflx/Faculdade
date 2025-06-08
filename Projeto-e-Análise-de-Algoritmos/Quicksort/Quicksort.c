#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

typedef struct objeto
{
    char ordenacao[3];
    int qtd;
    int index;
}Objeto;


int escreverArquivo(Objeto* matriz, int* matriz2, int32_t tamanhoSaida, char *saida, int i) {
    FILE* fp = fopen(saida, "w");
    if(fp == NULL) {
        printf("Erro ao abrir arquivo de saída\n");
        return 0;
    }


    fprintf(fp, "%d:N(%d)", i, matriz2[i]);
    for(int j = 0; j < 6; j++){
        fprintf(fp, ",%s(%d)",matriz[j].ordenacao, matriz[j].qtd);
    }
    fprintf(fp,"\n");

    if(i == tamanhoSaida){
        fclose(fp);
    }

    return 1;

}

// Função para alocar e retornar um vetor dinâmico
int* vetor(int x) {
    int* vetor = (int*)malloc(x * sizeof(int));
    return vetor;
}

// Função para trocar elementos
void troca(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

void trocaEspecial(Objeto* a, Objeto* b) {
    Objeto temp = *a;
    *a = *b;
    *b = temp;
}

int particaoLomutoEspecial(Objeto* arr, int low, int high) {
    Objeto pivot = arr[high]; // Escolhe o último elemento como pivô
    int i = low - 1;          // Índice do menor elemento

    for (int j = low; j < high; j++) {
        // Critério de ordenação: primeiro por `qtd`, depois por `index`
        if (arr[j].qtd < pivot.qtd || 
           (arr[j].qtd == pivot.qtd && arr[j].index < pivot.index)) {
            i++;
            trocaEspecial(&arr[i], &arr[j]);
        }
    }

    // Coloca o pivô na posição correta
    trocaEspecial(&arr[i + 1], &arr[high]);
    return i + 1;
}


void quicksortLomutoEspecial(Objeto* arr, int low, int high) {
    
    if (low < high) {
        int pi = particaoLomutoEspecial(arr, low, high);

        quicksortLomutoEspecial(arr, low, pi - 1);  // Ordena a parte esquerda
        quicksortLomutoEspecial(arr, pi + 1, high); // Ordena a parte direita
    }
}


// Função de partição Lomuto com contagem de trocas
void particaoLomutoPadrao(int* arr, int low, int high, int* resultado) {
    int pivot = arr[high];  // Escolhe o último elemento como pivô
    int i = low - 1;        // Índice do menor elemento
    int trocas = 0;

    for (int j = low; j < high; j++) {
        if (arr[j] <= pivot) {
            i++;
            troca(&arr[i], &arr[j]);
            trocas++;
        }
    }
    troca(&arr[i + 1], &arr[high]);
    trocas++;

    resultado[0] = i + 1;  // Índice do pivô
    resultado[1] = trocas; // Número de trocas
}

void particaoLomutoRando(int* arr, int low, int high, int* resultado) {
    int tamanho = high - low + 1;
    int pivotIndex = low + (abs(arr[low]) % tamanho);
    int pivot = arr[pivotIndex];
    troca(&arr[pivotIndex], &arr[high]);

    int i = low - 1;
    int trocas = 1;

    for (int j = low; j < high; j++) {
        if (arr[j] <= pivot) {
            i++;
            troca(&arr[i], &arr[j]);
            trocas++;
        }
    }
    troca(&arr[i + 1], &arr[high]);
    trocas++;

    resultado[0] = i + 1;
    resultado[1] = trocas;
}


void particaoLomutoMediana(int* arr, int low, int high, int* resultado) {
    // Calcula os índices correspondentes a 1/4, 1/2 e 3/4 do intervalo
    int tamanho = high - low + 1;
    int q1 = low + tamanho / 4;
    int q2 = low + tamanho / 2;
    int q3 = low + 3 * tamanho / 4;

    int trocas = 0; // Conta a troca inicial do pivô para o final

    // Calcula a mediana de arr[q1], arr[q2], arr[q3]
    int mediana;
    if ((arr[q1] >= arr[q2] && arr[q1] <= arr[q3]) || (arr[q1] >= arr[q3] && arr[q1] <= arr[q2])) {
        mediana = q1;
    } else if ((arr[q2] >= arr[q1] && arr[q2] <= arr[q3]) || (arr[q2] >= arr[q3] && arr[q2] <= arr[q1])) {
        mediana = q2;
    } else {
        mediana = q3;
    }

    // Troca a mediana para o início do array
    troca(&arr[high], &arr[mediana]);
    trocas++;    

    // Após as trocas, o elemento mediano está em arr[q2]
    int pivot = arr[high];

    int i = low - 1;

    for (int j = low; j < high; j++) {
        if (arr[j] <= pivot) {
            i++;
            troca(&arr[i], &arr[j]);
            trocas++;
        }
    }

    troca(&arr[i + 1], &arr[high]); // Coloca o pivô na posição correta
    trocas++;

    resultado[0] = i + 1; // Índice do pivô
    resultado[1] = trocas; // Total de trocas realizadas
}





// Função Quicksort usando partição Lomuto com contagem de trocas
int quicksortLomuto(int* arr, int low, int high,char* tipo) {
    int chamadas = 1; //Chamada da quicksortlomuto

    int resultado[2];  // Array para armazenar índice do pivô e trocas
    
    if (strcmp(tipo,"p") == 0 && low < high) {
        particaoLomutoPadrao(arr, low, high, resultado);

        int pi = resultado[0];
        chamadas += resultado[1];

        chamadas += quicksortLomuto(arr, low, pi - 1,tipo);  // Ordena a parte esquerda
        chamadas += quicksortLomuto(arr, pi + 1, high,tipo); // Ordena a parte direita
    }

    if (strcmp(tipo, "m") == 0 && low < high) {
        particaoLomutoMediana(arr, low, high, resultado);

        int pi = resultado[0];
        chamadas += resultado[1];

        chamadas += quicksortLomuto(arr, low, pi - 1, tipo);
        chamadas += quicksortLomuto(arr, pi + 1, high, tipo);
    }

    if (strcmp(tipo, "r") == 0 && low < high) {
        particaoLomutoRando(arr, low, high, resultado);

        int pi = resultado[0];
        chamadas += resultado[1];

        chamadas += quicksortLomuto(arr, low, pi - 1, tipo);
        chamadas += quicksortLomuto(arr, pi + 1, high, tipo);
    }

    return chamadas;
}

void particaoHoarePadrao(int* arr, int low, int high, int* resultado) {
    int pivot = arr[low]; // Escolhe o primeiro elemento como pivô
    int x = low - 1;
    int y = high + 1;
    int trocas = 0;

    while (1) {
        // Encontra o próximo elemento maior ou igual ao pivô da esquerda
        do {
            x++;
        } while (arr[x] < pivot);

        // Encontra o próximo elemento menor ou igual ao pivô da direita
        do {
            y--;
        } while (arr[y] > pivot);

        // Se os índices se cruzaram, a partição está completa
        if (x >= y) {
            resultado[0] = y;  // Posição do pivô (lado esquerdo)
            resultado[1] = trocas; // Total de trocas realizadas
            return;
        }

        // Troca os elementos
        troca(&arr[x], &arr[y]);
        trocas++;
    }
}


void particaoHoareMediana(int* arr, int low, int high, int* resultado) {
    int n = high - low + 1;
    int q1 = low + n / 4;
    int q2 = low + n / 2;
    int q3 = low + 3 * n / 4;
    int trocas = 0;

    // Calcula a mediana de arr[q1], arr[q2], arr[q3]
    int mediana;
    if ((arr[q1] >= arr[q2] && arr[q1] <= arr[q3]) || (arr[q1] >= arr[q3] && arr[q1] <= arr[q2])) {
        mediana = q1;
    } else if ((arr[q2] >= arr[q1] && arr[q2] <= arr[q3]) || (arr[q2] >= arr[q3] && arr[q2] <= arr[q1])) {
        mediana = q2;
    } else {
        mediana = q3;
    }

    // Troca a mediana para o início do array
    troca(&arr[low], &arr[mediana]);
    trocas++;

    int pivot = arr[low];
    int x = low - 1;
    int y = high + 1;

    while (1) {
        // Move x para a direita até encontrar um elemento maior ou igual ao pivô
        do {
            x++;
        } while (arr[x] < pivot);

        // Move y para a esquerda até encontrar um elemento menor ou igual ao pivô
        do {
            y--;
        } while (arr[y] > pivot);

        // Se os índices se cruzarem, a partição está concluída
        if (x >= y) {
            resultado[0] = y;  // Retorna a posição do pivô
            resultado[1] = trocas; // Retorna o número de trocas realizadas
            return;
        }

        // Troca os elementos
        troca(&arr[x], &arr[y]);
        trocas++;
    }
}


void particaoHoareRando(int* arr, int low, int high, int* resultado) {
    int tamanho = high - low + 1;
    int pivotIndex = low + (abs(arr[low]) % tamanho);
    int pivot = arr[pivotIndex];
    int x = low - 1;
    int y = high + 1;
    int trocas = 0;

    troca(&arr[pivotIndex],&arr[low]);
    trocas++;

    while (1) {
        // Encontra o próximo elemento maior ou igual ao pivô da esquerda
        do {
            x++;
        } while (arr[x] < pivot);

        // Encontra o próximo elemento menor ou igual ao pivô da direita
        do {
            y--;
        } while (arr[y] > pivot);

        // Se os índices se cruzaram, a partição está completa
        if (x >= y) {
            resultado[0] = y;  // Posição do pivô (lado esquerdo)
            resultado[1] = trocas; // Total de trocas realizadas
            return;
        }

        // Troca os elementos
        troca(&arr[x], &arr[y]);
        trocas++;
    }
}

int quicksortHoare(int* arr, int low, int high, char* tipo){
    int chamadas = 1; //Chamada da quicksortlomuto

    int resultado[2];  // Array para armazenar índice do pivô e trocas

    if(strcmp(tipo, "p") == 0 && low < high) {        
        // Particionamento do vetor
        particaoHoarePadrao(arr, low, high,resultado);

        int pi = resultado[0];
        chamadas += resultado[1];

        // Divisão em subvetores
        chamadas += quicksortHoare(arr, low, pi,tipo);
        chamadas += quicksortHoare(arr, pi + 1, high,tipo);

    }
    
    if(strcmp(tipo, "m") == 0 && low < high) {        
        // Particionamento do vetor
        particaoHoareMediana(arr, low, high,resultado);

        int pi = resultado[0];
        chamadas += resultado[1];

        // Divisão em subvetores
        chamadas += quicksortHoare(arr, low, pi,tipo);
        chamadas += quicksortHoare(arr, pi + 1, high,tipo);

    }

    if(strcmp(tipo, "r") == 0 && low < high) {        
        // Particionamento do vetor
        particaoHoareRando(arr, low, high,resultado);

        int pi = resultado[0];
        chamadas += resultado[1];

        // Divisão em subvetores
        chamadas += quicksortHoare(arr, low, pi,tipo);
        chamadas += quicksortHoare(arr, pi + 1, high,tipo);

    }
    return chamadas;
}

// Função para abrir e processar o arquivo
void aberturaDoArquivo(char *ArquivoEntrada, char *ArquivoSaida) {
    FILE *arquivo = fopen(ArquivoEntrada, "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo de entrada\n");
        return;
    }

    FILE *fp = fopen(ArquivoSaida, "w");
    if (fp == NULL) {
        printf("Erro ao abrir o arquivo de saída\n");
        fclose(arquivo);
        return;
    }

    int32_t total_vetores;
    if (fscanf(arquivo, "%d\n", &total_vetores) != 1) {
        printf("Erro ao ler o número de vetores\n");
        fclose(arquivo);
        fclose(fp);
        return;
    }

    printf("Total de vetores: %d\n", total_vetores);
    int *numeros_vetores = (int *)malloc(total_vetores * sizeof(int));

    for (int i = 0; i < total_vetores; i++) {
        if (fscanf(arquivo, "%d\n", &numeros_vetores[i]) != 1) {
            printf("Erro ao ler a quantidade de números do vetor %d\n", i);
            free(numeros_vetores);
            fclose(arquivo);
            fclose(fp);
            return;
        }

        int tamanho = numeros_vetores[i];
        int *vetor_padrao = vetor(tamanho);
        int *vetores[6] = {
            vetor_padrao,
            vetor(tamanho),
            vetor(tamanho),
            vetor(tamanho),
            vetor(tamanho),
            vetor(tamanho)
        };

        // Leitura e cópia inicial dos números
        for (int j = 0; j < tamanho; j++) {
            fscanf(arquivo, "%d", &vetor_padrao[j]);
            for (int k = 1; k < 6; k++) {
                vetores[k][j] = vetor_padrao[j];
            }
        }

        Objeto objetos[6];
        objetos[0] = (Objeto){"LP", quicksortLomuto(vetores[0], 0, tamanho - 1, "p"), 0};
        objetos[1] = (Objeto){"LM", quicksortLomuto(vetores[1], 0, tamanho - 1, "m"), 1};
        objetos[2] = (Objeto){"LA", quicksortLomuto(vetores[2], 0, tamanho - 1, "r"), 2};
        objetos[3] = (Objeto){"HP", quicksortHoare(vetores[3], 0, tamanho - 1, "p"), 3};
        objetos[4] = (Objeto){"HM", quicksortHoare(vetores[4], 0, tamanho - 1, "m"), 4};
        objetos[5] = (Objeto){"HA", quicksortHoare(vetores[5], 0, tamanho - 1, "r"), 5};

        // Ordenação dos objetos pelo Lomuto especial
        quicksortLomutoEspecial(objetos, 0, 5);

        // Escrita no arquivo de saída
        fprintf(fp, "%d:N(%d)", i, tamanho);
        for (int j = 0; j < 6; j++) {
            fprintf(fp, ",%s(%d)", objetos[j].ordenacao, objetos[j].qtd);
        }
        if(i < total_vetores-1){
            fprintf(fp, "\n");
        }

        // Liberação de memória dos vetores alocados
        for (int k = 0; k < 6; k++) {
            free(vetores[k]);
        }
    }

    free(numeros_vetores);
    fclose(arquivo);
    fclose(fp);
}


int main(int argc, char *argv[]) {
    aberturaDoArquivo(argv[1],argv[2]);   // Chama a função de leitura do arquivo   
    return 0;
}
