# Competição de Ordenação — O Desafio dos Milissegundos

Ordena os 10.000.000 de inteiros de `ListahAhSerOrdenada.txt` em ordem crescente, medindo **apenas o tempo da ordenação** com `clock()`.

## Equipe
1. Jefferson Marques Rocha — Líder
2. ...

## Estratégia
O arquivo tem 10 milhões de números, todos entre **1 e 2.500.000** (muitos repetidos). Por isso o programa usa um algoritmo **híbrido**, escolhido automaticamente:

| Situação | Algoritmo | Complexidade |
|---|---|---|
| Intervalo de valores pequeno (`max - min + 1 <= 2N`) — **caso da competição** | Counting Sort | O(N + K) |
| Qualquer outra entrada (inclusive negativos) | Radix Sort LSD (base 256, 4 passadas) | O(N) |

Os dois são **não comparativos**, por isso batem Quick Sort / Merge Sort (O(N log N)) para inteiros. Não usam recursão e toda memória alocada é liberada com `free()`.

## Medição de tempo
- A leitura do arquivo (`fread` + conversão) acontece **antes** do cronômetro.
- `clock()` é disparado imediatamente antes e depois de `ordenarVetor()`.
- A verificação e a gravação da saída acontecem **depois** do cronômetro.

## Verificação
Depois de ordenar, o programa confere:
- se cada elemento é `<=` ao próximo (ordem crescente);
- se a quantidade e a soma dos elementos são iguais às de antes (nenhum elemento perdido).

> Obs.: como o arquivo tem valores repetidos, a ordem é crescente **não estrita** (repetidos ficam lado a lado); nenhum elemento é removido.

## Como compilar e executar
```bash
gcc -O3 -march=native -o ordenacao ordenacao.c
./ordenacao ListahAhSerOrdenada.txt
# opcional: gravar o resultado ordenado
./ordenacao ListahAhSerOrdenada.txt saida.txt
```
No Windows (MinGW): `gcc -O3 -o ordenacao.exe ordenacao.c` e `ordenacao.exe ListahAhSerOrdenada.txt`.

## Resultado
```
Elementos lidos: 10000000
Algoritmo: Counting Sort
Tempo de ordenação: 0.015328 segundos (15.328000 ms)
Verificação: OK (ordem crescente, 10000000 elementos, soma 7812508749999)
Menor: 1 | Maior: 2500000
```

![Execução](print.png)
