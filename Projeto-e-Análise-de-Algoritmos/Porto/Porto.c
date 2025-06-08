#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define TAMANHO_HASH 50000

typedef struct container {
    char* codigo;
    char* cnpj;
    char* peso;
    int indice;
    int erro;
    struct container* prox;
} Container;

Container *tabelaHash[TAMANHO_HASH] = {NULL};

unsigned int funcaoHash(char *codigo) {
    unsigned int hash = 0;
    for (int i = 0; codigo[i] != '\0'; i++) {
        hash = (hash * 31 + codigo[i]) % TAMANHO_HASH;
    }
    return hash;
}

Container *criarNo(char *codigo, char *cnpj, char *peso, int32_t indice) {
    Container *novo = (Container *)malloc(sizeof(Container));
    if (!novo) {
        printf("Erro de alocação\n");
        exit(1);
    }
    novo->codigo = strdup(codigo);
    novo->cnpj = strdup(cnpj);
    novo->peso = strdup(peso);
    novo->indice = indice;
    novo->erro = 0;
    novo->prox = NULL;
    return novo;
}

void inserirHash(char *codigo, char *cnpj, char *peso, int32_t indice) {
    unsigned int pos = funcaoHash(codigo);
    Container *novo = criarNo(codigo, cnpj, peso, indice);
    novo->prox = tabelaHash[pos];
    tabelaHash[pos] = novo;
}

Container *buscarHash(char *codigo){
    unsigned int pos = funcaoHash(codigo);
    Container *atual = tabelaHash[pos];
    while (atual != NULL){
        if (strcmp(atual->codigo, codigo) == 0) {
            return atual;
        }
        atual = atual->prox;
    }
    return NULL;
}

Container* alocarCadastros(int cadastros){
    Container *matriz = (Container *)calloc(cadastros, sizeof(Container));
    if(!matriz){
        printf("Erro de alocação\n");
        exit(1);
    }
    for(int i = 0; i < cadastros; i++){
        matriz[i].codigo = (char *)malloc(12 * sizeof(char));
        matriz[i].cnpj = (char *)malloc(19 * sizeof(char));
        matriz[i].peso = (char *)malloc(19 * sizeof(char));
        matriz[i].indice = i;
        matriz[i].erro = 0;
    }
    return matriz;
}

int compare(char* s1, char* s2){
    return strcmp(s1, s2) != 0;
}

int verificaPeso(char* p1, char* p2){
    int Pa = atoi(p1);
    int Pb = atoi(p2);
    if (Pa == 0) return 0; 
    return (int)((100.0 * abs(Pa - Pb) / Pa) + 0.5);
}

int resultadoParcial(char* p1, char*p2){
    int Pa = atoi(p1);
    int Pb = atoi(p2);
    int dif = abs(Pa - Pb);


    return dif;
}

void merge(Container *matriz, int32_t low, int32_t mid, int32_t high){
    int32_t n1 = mid - low + 1;
    int32_t n2 = high - mid;

    Container *left = (Container *)malloc(n1 * sizeof(Container));
    Container *right = (Container *)malloc(n2 * sizeof(Container));

    for (int32_t i = 0; i < n1; i++) left[i] = matriz[low + i];
    for (int32_t i = 0; i < n2; i++) right[i] = matriz[mid + 1 + i];

    int32_t i = 0, j = 0, k = low;

    while (i < n1 && j < n2) {//Verifica se o erro é de CNPJ
        if (left[i].erro < right[j].erro) {
            matriz[k++] = left[i++];
        } 
        else if (left[i].erro > right[j].erro) {
            matriz[k++] = right[j++];
        } 
        else if (left[i].erro == 2) {  // Se erros forem de peso, comparar pesos
            int pesoA = atoi(left[i].peso);
            int pesoB = atoi(right[j].peso);

            if (pesoA > pesoB) {
                matriz[k++] = left[i++];
            } 
            else if (pesoA < pesoB) {
                matriz[k++] = right[j++];
            } 
            else { // Se pesos forem iguais, desempatar pelo índice
                if (left[i].indice < right[j].indice) {
                    matriz[k++] = left[i++];
                } else {
                    matriz[k++] = right[j++];
                }
            }
        } 
        else { // Caso erro sejam ambos de CNPJ, desempatar pelo índice
            if (left[i].indice < right[j].indice) {
                matriz[k++] = left[i++];
            } else {
                matriz[k++] = right[j++];
            }
        }
    }

    while (i < n1) matriz[k++] = left[i++];
    while (j < n2) matriz[k++] = right[j++];

    free(left);
    free(right);
}


// Função de Merge Sort para ordenar pela diferença de CNPJ ou Peso
void mergeSort(Container *matriz, int32_t low, int32_t high) {
    if(low < high){
        int32_t mid = (low + high) / 2;
        mergeSort(matriz, low, mid);
        mergeSort(matriz, mid + 1, high);
        merge(matriz, low, mid, high);
    }
}

void escreverArquivo(char* arquivo, int tamanhoSaida, Container* matriz) {
    FILE* fp = fopen(arquivo, "w");
    if(!fp){
        printf("Erro ao abrir arquivo de saída\n");
        exit(1);
    }

    for(int i = 0; i < tamanhoSaida; i++){
        if(matriz[i].erro == 1){
            fprintf(fp, "%s:%s<->%s\n", matriz[i].codigo, matriz[i].cnpj, matriz[i].peso);
        } 
        else{
            fprintf(fp, "%s:%skg(%s%%)\n", matriz[i].codigo, matriz[i].cnpj, matriz[i].peso);
        }
    }
    fclose(fp);
}

void aberturaDoArquivo(char *ArquivoEntrada, char *ArquivoSaida){
    FILE *arquivo = fopen(ArquivoEntrada, "r");

    if (!arquivo){
        printf("Erro ao abrir o arquivo\n");
        return;
    }

    int32_t cadastros, selecionados, cadastrosComDesvio = 0;

    fscanf(arquivo, "%d", &cadastros);

    Container *matrizDeEntrada = alocarCadastros(cadastros);

    for(int i = 0; i < cadastros; i++){
        fscanf(arquivo, "%s %s %s", matrizDeEntrada[i].codigo, matrizDeEntrada[i].cnpj, matrizDeEntrada[i].peso);
        inserirHash(matrizDeEntrada[i].codigo, matrizDeEntrada[i].cnpj, matrizDeEntrada[i].peso, matrizDeEntrada[i].indice);
    }

    fscanf(arquivo, "%d", &selecionados);

    Container *matrizDeDesvios = alocarCadastros(selecionados);

    for(int i = 0; i < selecionados; i++){

        char codigoTemp[12], cnpjTemp[19], pesoTemp[19];

        fscanf(arquivo, "%s %s %s", codigoTemp, cnpjTemp, pesoTemp);

        Container *Cadastro = buscarHash(codigoTemp);

        if(!Cadastro) continue;

        if(compare(Cadastro->cnpj, cnpjTemp)){
            matrizDeDesvios[cadastrosComDesvio++] = (Container){strdup(codigoTemp), strdup(Cadastro->cnpj), strdup(cnpjTemp), Cadastro->indice, 1, NULL};
        } 
        else{
            int saida = verificaPeso(Cadastro->peso, pesoTemp);
            int resultado = resultadoParcial(Cadastro->peso, pesoTemp);
            if(saida > 10){
                char * temp1 = (char*)malloc(19 * sizeof(char));
                sprintf(temp1, "%d", saida);
                char * temp2 = (char*)malloc(19 * sizeof(char));
                sprintf(temp2, "%d", resultado);
                
                matrizDeDesvios[cadastrosComDesvio++] = (Container){strdup(codigoTemp), strdup(temp2), strdup(temp1), Cadastro->indice, 2, NULL};
            }
        }
    }

    fclose(arquivo);

    mergeSort(matrizDeDesvios, 0, cadastrosComDesvio - 1);

    escreverArquivo(ArquivoSaida, cadastrosComDesvio, matrizDeDesvios);

    for (int i = 0; i < cadastros; i++) {
        free(matrizDeEntrada[i].codigo);
        free(matrizDeEntrada[i].cnpj);
        free(matrizDeEntrada[i].peso);
    }
    free(matrizDeEntrada);

    for (int i = 0; i < cadastrosComDesvio; i++) {
        free(matrizDeDesvios[i].codigo);
        free(matrizDeDesvios[i].cnpj);
        free(matrizDeDesvios[i].peso);
    }
    free(matrizDeDesvios);
}

int main(int argc, char* argv[]) {
    aberturaDoArquivo(argv[1],argv[2]);
    return 0;
}
