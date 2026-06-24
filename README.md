# Sincronização de Processos - Trabalho 1

## Identificação

- **Disciplina:** Programação Concorrente (CIC0202 - UnB)
- **Professor:** Eduardo A. P. Alchieri  

- **Aluno:** Matheus Duarte da Silva
- **Matricula:** 211062277

## Database Connection Pool Simulator

Um simulador de gerenciamento de concorrência e de recursos utilizando a biblioteca **POSIX Pthreads** em C.

Este projeto resolve o clássico problema de Leitores e Escritores (*Readers-Writers*) adicionando restrições físicas de I/O, modelando um *middleware* de conexões de banco de dados com execuções de operações **ETL (Escritores)** e serviços de **Business Intelligence (Leitores)**.

## Apresentando o Problema

Sistemas modernos frequentemente lidam com o gargalo de recursos de Entrada e Saída (I/O).
Bancos de dados relacionais não suportam conexões simultâneas ilimitadas.

O algoritmo implementado atua como um "guarda de trânsito" para a memória compartilhada, garantindo três regras de negócio rigorosas:

1. **Limite Físico de Conexões:** O banco de dados suporta um máximo de $N$ conexões simultâneas (Gargalo de I/O)
   1. Controlado via **Semáforos de Contagem**
2. **Exclusão Mútua Direcional:** Para evitar *dirty reads* e quebra de propriedades ACID, ou o banco processa leituras simultâneas, ou processa escritas simultâneas
   1. Nunca ambos
   2. Controlado via **Mutexes** e **Variáveis de Condição**
3. **Prevenção de *Starvation*:** Implementação de um **Sistema de Escalonamento**.
   1. Se $K$ requisições do mesmo tipo passarem consecutivamente pela ponte, o fluxo é obrigatoriamente invertido para dar chance à fila adversária, garantindo justiça no escalonamento.

## Estrutura do Repositório

O projeto segue a seguinte estrutura de diretórios:

```bash
tree -L3
.
├── build                   # Diretório de build (gerado pelo CMake)
├── CMakeLists.txt          # Configuração do gerador de build 
├── docs                    # Documentação e relatório (PDF)
│   └── relatorio.pdf       # Relatório do Trabalho 
│   └── enunciado.pdf       # Enunciado do Trabalho
├── LICENSE                 # Licença do Projeto (GNU GPLv3)
├── project                 # Arquivos do projeto
│   ├── include             # Headers
│   │   ├── pool.h          # Estruturas de sincronização
│   │   └── worker.h        # Estruturas e funções das threads
│   └── src                 # Implementações (Source)
│       ├── main.c          # Ponto de entrada e instanciação das threads
│       ├── pool.c          # Lógica de escalonamento e bloqueios de acesso
│       └── worker.c        # Simulação de processamento de BI e ETL
```

## Compilando e Executando o Projeto

### Dependências

O projeto utiliza o **C23** como padrão da linguagem C.
Certifique-se de ter um compilador compatível com este padrão (como `gcc 13+`, `clang 16+` ou outros compiladores compatíveis).

É recomendado utilizar um ambiente Linux para compilar e executar o projeto, pois ele depende de bibliotecas POSIX para gerenciamento de threads e sincronização.
Além disso é preciso ter instalado o `cmake` e `make` para o processo de build.

> **NOTA:** É recomendado utilizar Linux pois o projeto foi desenvolvido e testado nesse ambiente

### Compilação (Build)

Na raiz do projeto, execute os seguintes comandos no terminal.

```bash
# Cria o diretório de build baseado no CMake
cmake -S . -B build

# Builda o projeto utilizando o Makefile gerado pelo CMake
cmake --build build

# Executa o projeto
./build/trabalho_pc
```

Após executar o projeto, você verá a saída no terminal com logs de execução das threads de leitura e escrita, incluindo informações sobre bloqueios, desbloqueios e alternância de fluxo.
