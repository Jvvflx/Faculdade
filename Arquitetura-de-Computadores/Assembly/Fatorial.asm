.data                   # Segmento de dados
msg:    .asciiz "Resultado: "  # Mensagem a ser exibida
newline: .asciiz "\n"           # Quebra de linha

.text                   # Segmento de código
.globl main             # Define a entrada principal do programa

main:
    # Define o número para o qual calcular o fatorial (5)
    li $a0, 5           # Carrega 5 no primeiro parâmetro ($a0)
    
    # Chama a função fat
    jal fat             # Chamada para calcular o fatorial

    # Imprime a mensagem "Resultado: "
    li $v0, 4           # syscall para imprimir string
    la $a0, msg         # Carrega o endereço da mensagem
    syscall

    # Imprime o resultado retornado em $v0
    li $v0, 1           # syscall para imprimir inteiro
    move $a0, $v0       # Move o valor de retorno para $a0
    syscall

    # Imprime uma nova linha
    li $v0, 4           # syscall para imprimir string
    la $a0, newline     # Carrega o endereço da quebra de linha
    syscall

    # Finaliza o programa
    li $v0, 10          # syscall para encerrar o programa
    syscall

# Função fat: calcula o fatorial de um número (recursivo)
fat:
    # Verifica se n == 1 (condição base)
    beq $a0, 1, fat_base

    # Salva o endereço de retorno e o parâmetro atual na pilha
    addi $sp, $sp, -8   # Reserva espaço na pilha
    sw $ra, 4($sp)      # Salva o endereço de retorno
    sw $a0, 0($sp)      # Salva o parâmetro $a0 (n)

    # Prepara o parâmetro para a chamada recursiva
    addi $a0, $a0, -1   # Decrementa $a0 (n-1)
    jal fat             # Chamada recursiva para fat(n-1)

    # Restaura o parâmetro original e o endereço de retorno
    lw $a0, 0($sp)      # Restaura o parâmetro original ($a0)
    lw $ra, 4($sp)      # Restaura o endereço de retorno
    addi $sp, $sp, 8    # Libera o espaço na pilha

    # Multiplica o valor retornado (fatorial de n-1) por n
    mul $v0, $v0, $a0   # $v0 = n * fat(n-1)
    jr $ra              # Retorna para o chamador

fat_base:
    li $v0, 1           # Retorna 1 se n == 1 (fatorial de 1 é 1)
    jr $ra              # Retorna para o chamador
