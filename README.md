# Implementação 5 — Algoritmo de Banker

Infraestrutura de Software — CESAR School

## Como compilar

```
make        # gera o binario ./banker
make clean  # remove binario e arquivos objeto
```

## Como executar

```
./banker <recurso_1> <recurso_2> ... <recurso_N>
```

Cada argumento representa o numero de instancias disponiveis de um tipo de recurso.
Exemplo, com 3 recursos (10, 5 e 7 instancias):

```
./banker 10 5 7
```

O programa le `customer.txt` e `commands.txt` (devem estar na mesma pasta do executavel)
e gera `result.txt` com o log de alocacoes, liberacoes, negacoes e os blocos de estado.

## Formato dos arquivos de entrada

**customer.txt** — uma linha por cliente, valores de MAXIMUM separados por virgula.
O numero de linhas define o numero de clientes; o numero de colunas de cada linha deve
ser igual ao numero de argumentos passados na linha de comando (numero de recursos).

```
7,5,3
3,2,2
9,0,2
```

**commands.txt** — uma linha por comando:

- `RQ <cliente> <v1> ... <vN>` — requisicao de recursos
- `RL <cliente> <v1> ... <vN>` — liberacao de recursos
- `*` — imprime o bloco de estado atual (MAXIMUM | ALLOCATION | NEED de cada cliente + AVAILABLE)

## Estrutura do codigo

- `main.c` — ponto de entrada: valida argumentos, le customer.txt, roda o loop de comandos e escreve result.txt
- `banker.h` — structs (Customer, BankerState) e assinaturas do algoritmo
- `banker_safety.c` — algoritmo de verificacao de estado seguro (is_safe_state, Work/Finish)
- `banker_ops.c` — request_resources e release_resources
- `commands.h` / `commands.c` — leitura de commands.txt

## Tratamento de erros

O programa encerra com mensagem de erro (sem crash) nos seguintes casos:

- numero de argumentos da CLI diferente do numero de recursos em customer.txt
- argumento da CLI que nao e um numero inteiro valido
- customer.txt ou commands.txt inexistente ou que nao pode ser aberto
- customer.txt com numero de colunas inconsistente entre linhas, ou com valor nao-numerico
- commands.txt com comando desconhecido ou linha incompleta (faltando valores)

## Observacao sobre o formato de result.txt

O bloco de estado (`MAXIMUM | ALLOCATION | NEED` / `AVAILABLE`) usa um espaco como
separador entre valores, em vez do alinhamento de colunas do exemplo do enunciado.
Essa decisao foi tomada porque o alinhamento do exemplo so funciona para numeros de
1 digito, e o enunciado explicita que o numero de clientes e de recursos nao e fixo
— um separador unico funciona corretamente para qualquer quantidade de digitos,
clientes ou recursos. O conteudo das linhas e identico ao esperado.