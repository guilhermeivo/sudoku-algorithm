# Sudoku

Implementação baseada no algoritmo de backtracking devido ser necessário buscar todas as alternativas para encontrar uma solução válida.

## Solução

Fazer uma varredura na linha, coluna e na sub região (chamado de quadrante no código) da posição que deseja inserir um elemento. Não pode estar fora do [1-9] e nem ja ter um número com o mesmo valor.

# Compilação

```sh
$ gcc main.c -Wall -D DEBUG
```

```sh
$ ./sudoku input.txt
# OR
$ ./sudoku # with random values
```