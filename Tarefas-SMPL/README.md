# Trabalho Prático 0: Roteiro de Aprendizado Prático de Simulação

Tarefas para aprender a usar a ferramenta de simulação de Sistemas Distribuídos: SMPL.

## Como executar

Para executar qualquer uma das tarefas basta rodar um make <nome-da-tarefa> para compilar uma tarefa específica ou apenas um make para compilar todas de uma vez. Exemplo:

```bash
make tarefa1
```

Em seguida escolher um número inteiro de processos para rodar (recomendação: não escolher um número muito grande, pois elas geram muitas mensagens de log) e rodar a tarefa usando ./<nome-da-tarefa> <num-processos>. Exemplo:

```bash
./tarefa1 3
```

Para remover os arquivos executáveis e os arquivos .o basta rodar:

```bash
make clean
```

## Funcionalidades

### Tarefa 0: tempo.c

O programa tempo.c apenas mostra os processos existindo no sistema e executando testes não definidos. Também mostra a falha do processo 1 no tempo 31 e recuperação no tempo 61. 

Obs.: este código era um exemplo feito em sala pelo professor, é apenas uma cópia, salvo uma única alteração feita que é liberar a memória alocada para processo no final da execução.

Exemplo de saída:

```bash
Sou o processo 0 e estou testando no tempo 30.0
Sou o processo 1 e estou testando no tempo 30.0
Sou o processo 2 e estou testando no tempo 30.0
O processo 1 falhou no tempo 31.0
Sou o processo 0 e estou testando no tempo 60.0
Sou o processo 2 e estou testando no tempo 60.0
O processo 1 recuperou no tempo 61.0
Sou o processo 1 e estou testando no tempo 62.0
Sou o processo 0 e estou testando no tempo 90.0
Sou o processo 2 e estou testando no tempo 90.0
Sou o processo 1 e estou testando no tempo 92.0
Sou o processo 0 e estou testando no tempo 120.0
```

### Tarefa 1

A tarefa 1 faz cada um dos processos testar o seguinte no anel e imprimir o status do processo testado. Exemplo de saída:

```bash
O processo 0 testou o processo 1 correto no tempo 30.0
O processo 1 testou o processo 2 correto no tempo 30.0
O processo 2 testou o processo 0 correto no tempo 30.0
O processo 1 falhou no tempo 31.0
O processo 0 testou o processo 1 falho no tempo 60.0
O processo 2 testou o processo 0 correto no tempo 60.0
O processo 1 recuperou no tempo 61.0
O processo 1 testou o processo 2 correto no tempo 62.0
O processo 0 testou o processo 1 correto no tempo 90.0
O processo 2 testou o processo 0 correto no tempo 90.0
O processo 1 testou o processo 2 correto no tempo 92.0
O processo 0 testou o processo 1 correto no tempo 120.0
```

### Tarefa 2

Na tarefa 2, cada processo correto executa testes até encontrar outro processo correto ou testar todos os outros processos falhos. Exemplo de saída:

```bash
O processo 0 testou o processo 1 correto no tempo 30.0
O processo 1 testou o processo 2 correto no tempo 30.0
O processo 2 testou o processo 0 correto no tempo 30.0
O processo 1 falhou no tempo 31.0
O processo 0 testou o processo 1 falho no tempo 60.0
O processo 0 testou o processo 2 correto no tempo 60.0
O processo 2 testou o processo 0 correto no tempo 60.0
O processo 1 recuperou no tempo 61.0
O processo 1 testou o processo 2 correto no tempo 62.0
O processo 0 testou o processo 1 correto no tempo 90.0
O processo 2 testou o processo 0 correto no tempo 90.0
O processo 1 testou o processo 2 correto no tempo 92.0
O processo 0 testou o processo 1 correto no tempo 120.0
```

### Tarefa 3

Agora, cada processo, além de testar até encontrar outro correto, mantém localmente um vetor State[N]. A entrada do vetor State(i)[j] indica o estado do processo j conhecido pelo processo i. O estado de cada processo pode ser: -1 (unknown), 0 (correto) ou 1 (falho). Nesta tarefa ao executar um teste em um processo j, o testador atualiza a entrada correspondente no vetor State[j]. Em cada intervalo de testes, mostra o vetor State[N]. Exemplo de saída:

```bash
State(0) no tempo  0.0:
State[0] = 0
State[1] = -1
State[2] = -1

State(1) no tempo  0.0:
State[0] = -1
State[1] = 0
State[2] = -1

State(2) no tempo  0.0:
State[0] = -1
State[1] = -1
State[2] = 0

O processo 0 testou o processo 1 correto no tempo 30.0
State(0) no tempo 30.0:
State[0] = 0
State[1] = 0
State[2] = -1

O processo 1 testou o processo 2 correto no tempo 30.0
State(1) no tempo 30.0:
State[0] = -1
State[1] = 0
State[2] = 0

O processo 2 testou o processo 0 correto no tempo 30.0
State(2) no tempo 30.0:
State[0] = 0
State[1] = -1
State[2] = 0

O processo 1 falhou no tempo 31.0
O processo 0 testou o processo 1 falho no tempo 60.0
O processo 0 testou o processo 2 correto no tempo 60.0
State(0) no tempo 60.0:
State[0] = 0
State[1] = 1
State[2] = 0

O processo 2 testou o processo 0 correto no tempo 60.0
State(2) no tempo 60.0:
State[0] = 0
State[1] = -1
State[2] = 0

O processo 1 recuperou no tempo 61.0
O processo 1 testou o processo 2 correto no tempo 62.0
State(1) no tempo 62.0:
State[0] = -1
State[1] = 0
State[2] = 0

O processo 0 testou o processo 1 correto no tempo 90.0
State(0) no tempo 90.0:
State[0] = 0
State[1] = 0
State[2] = 0

O processo 2 testou o processo 0 correto no tempo 90.0
State(2) no tempo 90.0:
State[0] = 0
State[1] = -1
State[2] = 0

O processo 1 testou o processo 2 correto no tempo 92.0
State(1) no tempo 92.0:
State[0] = -1
State[1] = 0
State[2] = 0

O processo 0 testou o processo 1 correto no tempo 120.0
State(0) no tempo 120.0:
State[0] = 0
State[1] = 0
State[2] = 0
```

### Tarefa 4

Nesta tarefa, quando um processo correto testa outro processo correto ele obtém as informações do estado dos demais processos do sistema, exceto aqueles que testou nesta rodada. Exemplo de saída:

```bash
State(0) no tempo  0.0:
State[0] = 0
State[1] = -1
State[2] = -1

State(1) no tempo  0.0:
State[0] = -1
State[1] = 0
State[2] = -1

State(2) no tempo  0.0:
State[0] = -1
State[1] = -1
State[2] = 0

O processo 0 testou o processo 1 correto no tempo 30.0
O processo 0 obtém info sobre o processo 2 vinda do processo 1
State(0) no tempo 30.0:
State[0] = 0
State[1] = 0
State[2] = -1

O processo 1 testou o processo 2 correto no tempo 30.0
O processo 1 obtém info sobre o processo 0 vinda do processo 2
State(1) no tempo 30.0:
State[0] = -1
State[1] = 0
State[2] = 0

O processo 2 testou o processo 0 correto no tempo 30.0
O processo 2 obtém info sobre o processo 1 vinda do processo 0
State(2) no tempo 30.0:
State[0] = 0
State[1] = 0
State[2] = 0

O processo 1 falhou no tempo 31.0
O processo 0 testou o processo 1 falho no tempo 60.0
O processo 0 testou o processo 2 correto no tempo 60.0
State(0) no tempo 60.0:
State[0] = 0
State[1] = 1
State[2] = 0

O processo 2 testou o processo 0 correto no tempo 60.0
O processo 2 obtém info sobre o processo 1 vinda do processo 0
State(2) no tempo 60.0:
State[0] = 0
State[1] = 1
State[2] = 0

O processo 1 recuperou no tempo 61.0
O processo 1 testou o processo 2 correto no tempo 62.0
O processo 1 obtém info sobre o processo 0 vinda do processo 2
State(1) no tempo 62.0:
State[0] = 0
State[1] = 0
State[2] = 0

O processo 0 testou o processo 1 correto no tempo 90.0
O processo 0 obtém info sobre o processo 2 vinda do processo 1
State(0) no tempo 90.0:
State[0] = 0
State[1] = 0
State[2] = 0

O processo 2 testou o processo 0 correto no tempo 90.0
O processo 2 obtém info sobre o processo 1 vinda do processo 0
State(2) no tempo 90.0:
State[0] = 0
State[1] = 0
State[2] = 0

O processo 1 testou o processo 2 correto no tempo 92.0
O processo 1 obtém info sobre o processo 0 vinda do processo 2
State(1) no tempo 92.0:
State[0] = 0
State[1] = 0
State[2] = 0

O processo 0 testou o processo 1 correto no tempo 120.0
O processo 0 obtém info sobre o processo 2 vinda do processo 1
State(0) no tempo 120.0:
State[0] = 0
State[1] = 0
State[2] = 0
```
