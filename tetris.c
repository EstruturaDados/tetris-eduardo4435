#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define TAM_FILA 5   // capacidade fixa da fila circular
#define TAM_PILHA 3  // capacidade fixa da pilha

// ---------- Estrutura da peça ----------
typedef struct {
    char tipo;  // letra da peça (T, L, O, I etc.)
    int id;     // número incremental para identificar
} Peca;

// ---------- Estrutura da fila circular ----------
typedef struct {
    Peca itens[TAM_FILA];
    int inicio; // índice do primeiro elemento
    int fim;    // índice da próxima posição livre
    int qtd;    // quantidade atual de elementos
} Fila;

// ---------- Estrutura da pilha ----------
typedef struct {
    Peca itens[TAM_PILHA];
    int topo; // índice do próximo elemento livre
} Pilha;

// ---------- Funções utilitárias ----------
char gerarTipoAleatorio() {
    char tipos[] = {'T','L','O','I','Z','S','J'};
    return tipos[rand() % (sizeof(tipos)/sizeof(tipos[0]))];
}

Peca gerarNovaPeca(int id) {
    Peca p;
    p.tipo = gerarTipoAleatorio();
    p.id = id;
    return p;
}

// ---------- Fila ----------
void inicializarFila(Fila *f) {
    f->inicio = 0;
    f->fim = 0;
    f->qtd = 0;
}

int filaCheia(Fila *f) { return f->qtd == TAM_FILA; }
int filaVazia(Fila *f) { return f->qtd == 0; }

void enfileirar(Fila *f, Peca p) {
    if (filaCheia(f)) return;
    f->itens[f->fim] = p;
    f->fim = (f->fim + 1) % TAM_FILA;
    f->qtd++;
}

Peca desenfileirar(Fila *f) {
    Peca p = {'-', -1};
    if (filaVazia(f)) return p;
    p = f->itens[f->inicio];
    f->inicio = (f->inicio + 1) % TAM_FILA;
    f->qtd--;
    return p;
}

// ---------- Pilha ----------
void inicializarPilha(Pilha *p) {
    p->topo = 0;
}

int pilhaVazia(Pilha *p) { return p->topo == 0; }
int pilhaCheia(Pilha *p) { return p->topo == TAM_PILHA; }

void empilhar(Pilha *p, Peca pec) {
    if (pilhaCheia(p)) return;
    p->itens[p->topo++] = pec;
}

Peca desempilhar(Pilha *p) {
    Peca pec = {'-', -1};
    if (pilhaVazia(p)) return pec;
    pec = p->itens[--p->topo];
    return pec;
}

// ---------- Exibição ----------
void exibirEstado(Fila *f, Pilha *p) {
    printf("\n--- ESTADO ATUAL ---\n");
    printf("Fila de peças: ");
    int idx = f->inicio;
    for (int i=0;i<f->qtd;i++) {
        printf("[ %c %d ] ", f->itens[idx].tipo, f->itens[idx].id);
        idx = (idx+1)%TAM_FILA;
    }
    printf("\n");
    printf("Pilha de reserva (Topo -> Base): ");
    for (int i=p->topo-1;i>=0;i--) {
        printf("[ %c %d ] ", p->itens[i].tipo, p->itens[i].id);
    }
    printf("\n---------------------\n");
}

// ---------- Operações Estratégicas ----------
void jogarPeca(Fila *fila, int *idCounter) {
    if (filaVazia(fila)) {
        printf("Fila vazia!\n");
        return;
    }
    Peca usada = desenfileirar(fila);
    printf("Jogou a peça [ %c %d ]\n", usada.tipo, usada.id);
    // gera nova peça para manter fila cheia
    enfileirar(fila, gerarNovaPeca((*idCounter)++));
}

void reservarPeca(Fila *fila, Pilha *pilha, int *idCounter) {
    if (filaVazia(fila)) { printf("Fila vazia!\n"); return; }
    if (pilhaCheia(pilha)) { printf("Pilha cheia!\n"); return; }
    Peca p = desenfileirar(fila);
    empilhar(pilha, p);
    printf("Reservou a peça [ %c %d ]\n", p.tipo, p.id);
    // gera nova peça para fila
    enfileirar(fila, gerarNovaPeca((*idCounter)++));
}

void usarReservada(Pilha *pilha) {
    if (pilhaVazia(pilha)) { printf("Pilha vazia!\n"); return; }
    Peca p = desempilhar(pilha);
    printf("Usou a peça reservada [ %c %d ]\n", p.tipo, p.id);
}

void trocarPeca(Fila *fila, Pilha *pilha) {
    if (filaVazia(fila) || pilhaVazia(pilha)) {
        printf("Não é possível trocar!\n"); return;
    }
    // topo da pilha
    Peca temp = pilha->itens[pilha->topo-1];
    // frente da fila
    Peca *frente = &fila->itens[fila->inicio];
    pilha->itens[pilha->topo-1] = *frente;
    *frente = temp;
    printf("Troca simples realizada.\n");
}

void trocarMultiplas(Fila *fila, Pilha *pilha) {
    if (fila->qtd < 3 || pilha->topo < 3) {
        printf("Não há 3 peças em cada estrutura para trocar.\n");
        return;
    }
    // Troca as 3 primeiras da fila com as 3 do topo da pilha
    for (int i=0;i<3;i++) {
        int idxFila = (fila->inicio+i)%TAM_FILA;
        int idxPilha = pilha->topo-1 - i;
        Peca temp = fila->itens[idxFila];
        fila->itens[idxFila] = pilha->itens[idxPilha];
        pilha->itens[idxPilha] = temp;
    }
    printf("Troca múltipla realizada entre 3 peças da fila e 3 da pilha.\n");
}

// ---------- Programa Principal ----------
int main() {
    srand(time(NULL));
    Fila fila;
    Pilha pilha;
    inicializarFila(&fila);
    inicializarPilha(&pilha);

    int idCounter=0;
    // Preenche a fila inicial
    for (int i=0;i<TAM_FILA;i++) enfileirar(&fila, gerarNovaPeca(idCounter++));

    int opcao;
    do {
        exibirEstado(&fila,&pilha);
        printf("\nMenu de ações:\n");
        printf("1 - Jogar peça da frente da fila\n");
        printf("2 - Enviar peça da fila para pilha de reserva\n");
        printf("3 - Usar peça da pilha de reserva\n");
        printf("4 - Trocar peça da frente da fila com topo da pilha\n");
        printf("5 - Trocar os 3 primeiros da fila com as 3 peças da pilha\n");
        printf("0 - Sair\n");
        printf("Opção: ");
        scanf("%d",&opcao);

        switch(opcao){
            case 1: jogarPeca(&fila,&idCounter); break;
            case 2: reservarPeca(&fila,&pilha,&idCounter); break;
            case 3: usarReservada(&pilha); break;
            case 4: trocarPeca(&fila,&pilha); break;
            case 5: trocarMultiplas(&fila,&pilha); break;
            case 0: printf("Encerrando programa...\n"); break;
            default: printf("Opção inválida!\n"); break;
        }
    } while(opcao!=0);

    return 0;
}
