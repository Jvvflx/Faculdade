#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef struct labirinto {
    int larg;
    int altu;
    int comeco_linha;
    int comeco_coluna;
    int saida_linha;
    int saida_coluna;
    char** mapa;
    char* escolhas;
    int escolhas_index;
    char** visitados; // Matriz de visitados em vez de tabela hash
} labirinto;

int resolverLabirinto(labirinto* lab, int linha_atual, int coluna_atual, int linha_anterior, int coluna_anterior) {
    // Verificar se alcançou a saída
    if (linha_atual == lab->saida_linha && coluna_atual == lab->saida_coluna) {
        lab->escolhas_index += sprintf(lab->escolhas + lab->escolhas_index, "FIM@%d,%d\n", linha_atual, coluna_atual);
        return 1; // Saída encontrada
    }
    
    // Verificar se a posição atual já foi visitada
    if (lab->visitados[linha_atual][coluna_atual] == '1') {
        return 0; // Posição já visitada
    }
    
    // Marcar posição atual como visitada
    lab->visitados[linha_atual][coluna_atual] = '1';
    
    // Registrar na primeira iteração
    if (linha_anterior == -1 && coluna_anterior == -1) {
        lab->escolhas_index += sprintf(lab->escolhas + lab->escolhas_index, "INI@%d,%d|", linha_atual, coluna_atual);
    }
    
    // Direções possíveis: Direita, Cima (Frente), Esquerda, Baixo (Trás)
    // Otimização: usar arrays constantes e estáticos para evitar recriá-los em cada chamada recursiva
    static const int dx[4] = {1, 0, -1, 0};
    static const int dy[4] = {0, -1, 0, 1};
    static const char direcoes[4] = {'D', 'F', 'E', 'T'};
    
    for (int i = 0; i < 4; i++) {
        int nova_linha = linha_atual + dy[i];
        int nova_coluna = coluna_atual + dx[i];
        
        // Verificar se a nova posição é válida
        if (nova_linha >= 0 && nova_linha < lab->altu && 
            nova_coluna >= 0 && nova_coluna < lab->larg && 
            (lab->mapa[nova_linha][nova_coluna] == '0' || lab->mapa[nova_linha][nova_coluna] == 'X') && 
            lab->visitados[nova_linha][nova_coluna] == '0') {
            
            // Registrar movimento
            lab->escolhas_index += sprintf(lab->escolhas + lab->escolhas_index, 
                                          "%c->%d,%d|", 
                                          direcoes[i], 
                                          nova_linha, nova_coluna);
            
            // Tentar resolver a partir da nova posição
            if (resolverLabirinto(lab, nova_linha, nova_coluna, linha_atual, coluna_atual)) {
                return 1; // Solução encontrada
            }
        }
    }
    
    // Se chegou aqui, não encontrou saída a partir desta posição, fazer backtracking
    if (linha_anterior != -1) { // Não fazer backtracking no ponto inicial
        lab->escolhas_index += sprintf(lab->escolhas + lab->escolhas_index, 
                                      "BT@%d,%d->%d,%d|", 
                                      linha_atual, coluna_atual,
                                      linha_anterior, coluna_anterior);
    }
    
    // Verificar se voltamos ao ponto inicial e não encontramos saída
    if (linha_atual == lab->comeco_linha && coluna_atual == lab->comeco_coluna && 
        linha_anterior == -1) {
        lab->escolhas_index += sprintf(lab->escolhas + lab->escolhas_index, "FIM@-,-\n");
    }

    return 0; // Não encontrou saída a partir desta posição
}

int main(int argc, char* argv[]) {
    FILE* entrada = fopen(argv[1], "r");
    FILE* saida = fopen(argv[2], "w");

    if (!entrada || !saida) {
        if (entrada) fclose(entrada);
        if (saida) fclose(saida);
        printf("Erro ao abrir os arquivos.\n");
        return 1;
    }

    int total_labirintos = 0;
    fscanf(entrada, "%d\n", &total_labirintos);

    labirinto* lista_labirintos = (labirinto*) malloc(total_labirintos * sizeof(labirinto));
    if (!lista_labirintos) {
        printf("Erro de alocação\n");
        fclose(entrada);
        fclose(saida);
        return 1;
    }

    // Buffer para leitura reutilizável
    char* buffer = NULL;
    size_t buffer_size = 0;

    for (int i = 0; i < total_labirintos; i++) {
        fscanf(entrada, "%d %d\n", &lista_labirintos[i].larg, &lista_labirintos[i].altu);
        
        // Armazenar valores frequentemente acessados em variáveis locais
        int altura = lista_labirintos[i].altu;
        int largura = lista_labirintos[i].larg;
        
        // Inicializações
        lista_labirintos[i].saida_coluna = -1;
        lista_labirintos[i].saida_linha = -1;
        lista_labirintos[i].escolhas_index = 0;

        // Aloca buffer para escolhas com tamanho pré-calculado
        // Estimativa: cada célula pode gerar até 4 direções + backtracking
        int tamanho_estimado = altura * largura * 30; // 30 chars por movimento em média
        lista_labirintos[i].escolhas = (char*) malloc(tamanho_estimado * sizeof(char));
        if (!lista_labirintos[i].escolhas) {
            printf("Erro de alocação\n");
            fclose(entrada);
            fclose(saida);
            return 1;
        }
        lista_labirintos[i].escolhas[0] = '\0';

        // Inicializar matriz de visitados (substitui a tabela hash)
        lista_labirintos[i].visitados = (char**) malloc(altura * sizeof(char*));
        if (!lista_labirintos[i].visitados) {
            printf("Erro de alocação\n");
            fclose(entrada);
            fclose(saida);
            return 1;
        }
        
        for (int a = 0; a < altura; a++) {
            lista_labirintos[i].visitados[a] = (char*) malloc(largura * sizeof(char));
            if (!lista_labirintos[i].visitados[a]) {
                printf("Erro de alocação\n");
                fclose(entrada);
                fclose(saida);
                return 1;
            }
            // Inicializa todas as posições como não visitadas ('0')
            memset(lista_labirintos[i].visitados[a], '0', largura);
        }

        // Alocar mapa
        lista_labirintos[i].mapa = (char**) malloc(altura * sizeof(char*));
        if (!lista_labirintos[i].mapa) {
            printf("Erro de alocação\n");
            fclose(entrada);
            fclose(saida);
            return 1;
        }

        // Realocar buffer se necessário
        size_t needed_buffer_size = largura * 2 + 3;
        if (needed_buffer_size > buffer_size) {
            buffer_size = needed_buffer_size;
            char* novo_buffer = (char*) realloc(buffer, buffer_size * sizeof(char));
            if (!novo_buffer) {
                printf("Erro de alocação\n");
                if (buffer) free(buffer);
                fclose(entrada);
                fclose(saida);
                return 1;
            }
            buffer = novo_buffer;
        }

        for (int a = 0; a < altura; a++) {
            lista_labirintos[i].mapa[a] = (char*) malloc((largura + 1) * sizeof(char));
            if (!lista_labirintos[i].mapa[a]) {
                printf("Erro de alocação\n");
                fclose(entrada);
                fclose(saida);
                return 1;
            }

            if (fgets(buffer, buffer_size, entrada) == NULL) {
                printf("Erro ao ler o arquivo\n");
                fclose(entrada);
                fclose(saida);
                return 1;
            }

            int j = 0;
            int len_buffer = strlen(buffer);
            for (int k = 0; k < len_buffer && j < largura; k += 2) {
                lista_labirintos[i].mapa[a][j] = buffer[k];
                if (buffer[k] == 'X') {      
                    lista_labirintos[i].comeco_linha = a;
                    lista_labirintos[i].comeco_coluna = j;
                }
                // Condição otimizada para detectar saída
                if (buffer[k] == '0' && (a == 0 || a == altura - 1 || j == 0 || j == largura - 1)) {
                    lista_labirintos[i].saida_linha = a;
                    lista_labirintos[i].saida_coluna = j;
                }
                j++;
            }
            lista_labirintos[i].mapa[a][j] = '\0';
        }

        fprintf(saida, "L%d:", i);
        
        // Resolver labirinto começando da posição inicial
        resolverLabirinto(&lista_labirintos[i], 
                         lista_labirintos[i].comeco_linha, 
                         lista_labirintos[i].comeco_coluna, 
                         -1, -1);  // -1, -1 indicam que não há posição anterior

        fprintf(saida, "%s", lista_labirintos[i].escolhas);
        
        // Liberar memória do labirinto atual logo após processá-lo
        for (int j = 0; j < altura; j++) {
            free(lista_labirintos[i].mapa[j]);
            free(lista_labirintos[i].visitados[j]);
        }
        free(lista_labirintos[i].mapa);
        free(lista_labirintos[i].visitados);
        free(lista_labirintos[i].escolhas);
    }

    // Liberar buffer de leitura
    if (buffer) free(buffer);
    
    // Liberar lista de labirintos
    free(lista_labirintos);
    
    fclose(entrada);
    fclose(saida);
    
    return 0;
}
