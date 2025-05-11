#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ANSI_RESET   "\x1b[0m"
#define ANSI_RED     "\x1b[31m"
#define ANSI_YELLOW  "\x1b[33m"

typedef struct {
    char sigla;   // 'T', 'C', 'B', 'R', 'K', 'P'
    char cor;     // 'B' (Branco), 'P' (Preto)
    int movida;   // 0 = não movida, 1 = já movida
} Peca;

typedef struct {
    Peca* peca;
} Casa;

Casa* tabuleiro[8][8];

int abs(int x) {
    return x < 0 ? -x : x;
}

Peca* criar_peca(char sigla, char cor) {
    Peca* p = malloc(sizeof(Peca));
    p->sigla = sigla;
    p->cor = cor;
    p->movida = 0;
    return p;
}

void inicializar_tabuleiro() {
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++) {
            tabuleiro[i][j] = malloc(sizeof(Casa));
            tabuleiro[i][j]->peca = NULL;
        }

    // Pretas
    tabuleiro[0][0]->peca = criar_peca('T', 'P');
    tabuleiro[0][1]->peca = criar_peca('C', 'P');
    tabuleiro[0][2]->peca = criar_peca('B', 'P');
    tabuleiro[0][3]->peca = criar_peca('R', 'P');
    tabuleiro[0][4]->peca = criar_peca('K', 'P');
    tabuleiro[0][5]->peca = criar_peca('B', 'P');
    tabuleiro[0][6]->peca = criar_peca('C', 'P');
    tabuleiro[0][7]->peca = criar_peca('T', 'P');
    for (int j = 0; j < 8; j++)
        tabuleiro[1][j]->peca = criar_peca('P', 'P');

    // Brancas
    for (int j = 0; j < 8; j++)
        tabuleiro[6][j]->peca = criar_peca('P', 'B');
    tabuleiro[7][0]->peca = criar_peca('T', 'B');
    tabuleiro[7][1]->peca = criar_peca('C', 'B');
    tabuleiro[7][2]->peca = criar_peca('B', 'B');
    tabuleiro[7][3]->peca = criar_peca('R', 'B');
    tabuleiro[7][4]->peca = criar_peca('K', 'B');
    tabuleiro[7][5]->peca = criar_peca('B', 'B');
    tabuleiro[7][6]->peca = criar_peca('C', 'B');
    tabuleiro[7][7]->peca = criar_peca('T', 'B');
}

void imprimir_tabuleiro() {
    printf("\n    ");
    for (char c = 'A'; c <= 'H'; c++) printf(" %c  ", c);
    printf("\n");

    for (int i = 0; i < 8; i++) {
        printf("  +---+---+---+---+---+---+---+---+\n");
        printf("%d ", 8 - i);
        for (int j = 0; j < 8; j++) {
            Casa *casa = tabuleiro[i][j];
            printf("|");
            if (casa->peca) {
                const char *cor = (casa->peca->cor == 'B') ? ANSI_YELLOW : ANSI_RED;
                printf(" %s%c" ANSI_RESET " ", cor, casa->peca->sigla);
            } else {
                printf("   ");
            }
        }
        printf("| %d\n", 8 - i);
    }

    printf("  +---+---+---+---+---+---+---+---+\n");
    printf("    ");
    for (char c = 'A'; c <= 'H'; c++) printf(" %c  ", c);
    printf("\n\n");
}

int caminho_livre(int li, int ci, int lf, int cf) {
    int d_lin = (lf - li) ? (lf - li) / abs(lf - li) : 0;
    int d_col = (cf - ci) ? (cf - ci) / abs(cf - ci) : 0;
    int i = li + d_lin;
    int j = ci + d_col;

    while (i != lf || j != cf) {
        if (tabuleiro[i][j]->peca != NULL) return 0;
        i += d_lin;
        j += d_col;
    }
    return 1;
}

int movimento_valido(Peca* peca, int li, int ci, int lf, int cf) {
    int d_lin = lf - li;
    int d_col = cf - ci;
    Casa* destino = tabuleiro[lf][cf];

    if (peca->sigla == 'K') {
        if (abs(d_lin) <= 1 && abs(d_col) <= 1) return 1;

        // Roque
        if (peca->movida) return 0;
        if (d_lin != 0) return 0;

        // Roque pequeno
        if (d_col == 2 && tabuleiro[li][7]->peca && tabuleiro[li][7]->peca->sigla == 'T' && !tabuleiro[li][7]->peca->movida) {
            if (tabuleiro[li][5]->peca == NULL && tabuleiro[li][6]->peca == NULL)
                return 1;
        }

        // Roque grande
        if (d_col == -2 && tabuleiro[li][0]->peca && tabuleiro[li][0]->peca->sigla == 'T' && !tabuleiro[li][0]->peca->movida) {
            if (tabuleiro[li][1]->peca == NULL && tabuleiro[li][2]->peca == NULL && tabuleiro[li][3]->peca == NULL)
                return 1;
        }

        return 0;
    }

    switch (peca->sigla) {
        case 'P':
            if (peca->cor == 'B') {
                if (d_col == 0 && d_lin == -1 && destino->peca == NULL) return 1;
                if (li == 6 && d_col == 0 && d_lin == -2 && tabuleiro[li - 1][ci]->peca == NULL && destino->peca == NULL) return 1;
                if (abs(d_col) == 1 && d_lin == -1 && destino->peca != NULL && destino->peca->cor == 'P') return 1;
            } else {
                if (d_col == 0 && d_lin == 1 && destino->peca == NULL) return 1;
                if (li == 1 && d_col == 0 && d_lin == 2 && tabuleiro[li + 1][ci]->peca == NULL && destino->peca == NULL) return 1;
                if (abs(d_col) == 1 && d_lin == 1 && destino->peca != NULL && destino->peca->cor == 'B') return 1;
            }
            return 0;
        case 'T':
            return (d_lin == 0 || d_col == 0) && caminho_livre(li, ci, lf, cf);
        case 'B':
            return abs(d_lin) == abs(d_col) && caminho_livre(li, ci, lf, cf);
        case 'C':
            return (abs(d_lin) == 2 && abs(d_col) == 1) || (abs(d_lin) == 1 && abs(d_col) == 2);
        case 'R':
            return ((abs(d_lin) == abs(d_col) || d_lin == 0 || d_col == 0) && caminho_livre(li, ci, lf, cf));
        default:
            return 0;
    }
}

void mover_peca(int li, int ci, int lf, int cf) {
    if (li < 0 || li >= 8 || ci < 0 || ci >= 8 || lf < 0 || lf >= 8 || cf < 0 || cf >= 8) {
        printf("Movimento invalido: coordenadas fora do tabuleiro.\n");
        return;
    }

    Casa* origem = tabuleiro[li][ci];
    Casa* destino = tabuleiro[lf][cf];

    if (!origem->peca) {
        printf("Movimento invalido: nao ha peça na origem.\n");
        return;
    }

    if (!movimento_valido(origem->peca, li, ci, lf, cf)) {
        printf("Movimento invalido para a peça %c.\n", origem->peca->sigla);
        return;
    }

    if (destino->peca && destino->peca->cor == origem->peca->cor) {
        printf("Movimento invalido: não pode capturar peça da mesma cor.\n");
        return;
    }

    // Roque
    if (origem->peca->sigla == 'K' && abs(cf - ci) == 2) {
        if (cf == 6) { // Roque pequeno
            tabuleiro[lf][5]->peca = tabuleiro[lf][7]->peca;
            tabuleiro[lf][7]->peca = NULL;
            tabuleiro[lf][5]->peca->movida = 1;
        } else if (cf == 2) { // Roque grande
            tabuleiro[lf][3]->peca = tabuleiro[lf][0]->peca;
            tabuleiro[lf][0]->peca = NULL;
            tabuleiro[lf][3]->peca->movida = 1;
        }
    }

    if (destino->peca) free(destino->peca);
    destino->peca = origem->peca;
    origem->peca = NULL;
    destino->peca->movida = 1;

    printf("Peca %c (%c) movida de [%d,%d] para [%d,%d]\n",
           destino->peca->sigla, destino->peca->cor, li, ci, lf, cf);
}

int converter_letra_para_indice(char c) {
    if (c >= 'A' && c <= 'H') return c - 'A';
    if (c >= 'a' && c <= 'h') return c - 'a';
    return -1;
}

int rei_em_xeque(char cor) {
    int i, j;
    int pos_rei_x = -1, pos_rei_y = -1;
    // Encontrar o rei da cor
    for (i = 0; i < 8; i++) {
        for (j = 0; j < 8; j++) {
            if (tabuleiro[i][j]->peca && tabuleiro[i][j]->peca->sigla == 'K' && tabuleiro[i][j]->peca->cor == cor) {
                pos_rei_x = i;
                pos_rei_y = j;
                break;
            }
        }
        if (pos_rei_x != -1) break;
    }

    // Verificar se alguma peça adversária pode atacar o rei
    for (i = 0; i < 8; i++) {
        for (j = 0; j < 8; j++) {
            if (tabuleiro[i][j]->peca && tabuleiro[i][j]->peca->cor != cor) {
                if (movimento_valido(tabuleiro[i][j]->peca, i, j, pos_rei_x, pos_rei_y)) {
                    return 1; // Rei está em xeque
                }
            }
        }
    }
    return 0; // Rei não está em xeque
}

int main() {
    inicializar_tabuleiro();
    char entrada[32];
    char origem[3], destino[3];
    char vez = 'B'; // As brancas começam

    while (1) {
        imprimir_tabuleiro();
        printf("Vez das %s: ", vez == 'B' ? "Brancas" : "Pretas");
        printf("Digite o movimento (ex. E2 E4) ou digite 'sair': ");

        fgets(entrada, sizeof(entrada), stdin);
        entrada[strcspn(entrada, "\n")] = 0;  // Remove newline

        if (strcmp(entrada, "sair") == 0) break;

        if (sscanf(entrada, "%2s %2s", origem, destino) != 2) {
            printf("Formato invalido. Use por exemplo: E2 E4\n");
            continue;
        }

        int ci = converter_letra_para_indice(origem[0]);
        int li = 8 - (origem[1] - '0');
        int cf = converter_letra_para_indice(destino[0]);
        int lf = 8 - (destino[1] - '0');

        if (ci < 0 || cf < 0 || li < 0 || li >= 8 || lf < 0 || lf >= 8) {
            printf("Coordenadas invalidas.\n");
            continue;
        }

        Casa* casa_origem = tabuleiro[li][ci];
        if (!casa_origem->peca || casa_origem->peca->cor != vez) {
            printf("Movimento invalido: escolha uma peca da sua cor.\n");
            continue;
        }

        // Loop até que o movimento seja válido
        while (!movimento_valido(casa_origem->peca, li, ci, lf, cf)) {
            printf("Movimento invalido para a peça %c. Tente novamente (ex. E2 E4): ", casa_origem->peca->sigla);
            fgets(entrada, sizeof(entrada), stdin);
            entrada[strcspn(entrada, "\n")] = 0;  // Remove newline

            if (sscanf(entrada, "%2s %2s", origem, destino) != 2) {
                printf("Formato invalido. Use por exemplo: E2 E4\n");
                continue;
            }

            ci = converter_letra_para_indice(origem[0]);
            li = 8 - (origem[1] - '0');
            cf = converter_letra_para_indice(destino[0]);
            lf = 8 - (destino[1] - '0');

            if (ci < 0 || cf < 0 || li < 0 || li >= 8 || lf < 0 || lf >= 8) {
                printf("Coordenadas invalidas.\n");
                continue;
            }

            casa_origem = tabuleiro[li][ci];
            if (!casa_origem->peca || casa_origem->peca->cor != vez) {
                printf("Movimento invalido: escolha uma peca da sua cor.\n");
                continue;
            }
        }

        mover_peca(li, ci, lf, cf);

        if (rei_em_xeque(vez)) {
            printf("Movimento invalido: seu rei ficaria em xeque.\n");
            // Desfaz o movimento (simples: voltar peça à posição original)
            tabuleiro[li][ci]->peca = tabuleiro[lf][cf]->peca;
            tabuleiro[li][ci]->peca->movida = 0;
            tabuleiro[lf][cf]->peca = NULL;
            continue;
        }

        // Muda vez
        vez = (vez == 'B') ? 'P' : 'B';

        if (rei_em_xeque(vez)) {
            printf(ANSI_RED "Rei das %s está em xeque!\n" ANSI_RESET, vez == 'B' ? "Brancas" : "Pretas");
        }
    }

    printf("Jogo encerrado.\n");
    return 0;
}