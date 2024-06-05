#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <locale.h>

// Limpa o buffer do teclado
void limpaBuffer() {
  int ch;
  while ((ch = fgetc(stdin)) != EOF && ch != '\n') {
  }
}

typedef enum { PAUS, OUROS, COPAS, ESPADAS } Naipe;

typedef enum {
  AS = 1,
  DOIS,
  TRES,
  QUATRO,
  CINCO,
  SEIS,
  SETE,
  OITO,
  NOVE,
  DEZ,
  VALETE,
  DAMA,
  REI
} Valor;

typedef struct {
  Naipe naipe;
  Valor valor;
} Carta;

typedef struct {
  Carta cartas[52];
} Baralho;

typedef struct {
  char nome[20];
  Carta mao[5];
  int pontuacao;
  int numCartas;
} Jogador;

void inicializarBaralho(Baralho *baralho) {
  int index = 0;
  for (int n = PAUS; n <= ESPADAS; n++) {
    for (int v = AS; v <= REI; v++) {
      baralho->cartas[index].naipe = (Naipe)n;
      baralho->cartas[index].valor = (Valor)v;
      index++;
    }
  }
}

void embaralharBaralho(Baralho *baralho) {
  srand(time(NULL));
  for (int i = 51; i > 0; i--) {
    int j = rand() % (i + 1);
    Carta temp = baralho->cartas[i];
    baralho->cartas[i] = baralho->cartas[j];
    baralho->cartas[j] = temp;
  }
}

void imprimirCartaASCII(Carta carta) {
  const char *valores[] = {"", "A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K"};
  const char *naipes[] = {"♣", "♦", "♥", "♠"};

  printf("┌─────────┐\n");
  if (carta.valor == DEZ) {
    printf("│%s       │\n", valores[carta.valor]);
  } else {
    printf("│%s        │\n", valores[carta.valor]);
  }
  printf("│         │\n");
  printf("│         │\n");
  printf("│    %s    │\n", naipes[carta.naipe]);
  printf("│         │\n");
  printf("│         │\n");
  if (carta.valor == DEZ) {
    printf("│       %s│\n", valores[carta.valor]);
  } else {
    printf("│        %s│\n", valores[carta.valor]);
  }
  printf("└─────────┘\n");
}

void imprimirBaralho(Baralho baralho) {
  for (int i = 0; i < 52; i++) {
    imprimirCartaASCII(baralho.cartas[i]);
  }
}

int calcularPontuacao(Jogador *jogador) {
  int soma = 0;
  for (int i = 0; i < jogador->numCartas; i++) {
    if (jogador->mao[i].valor >= VALETE && jogador->mao[i].valor <= REI) {
      soma += 10;
    } else {
      soma += jogador->mao[i].valor;
    }
  }
  return soma;
}

void distribuirCartas(Baralho *baralho, Jogador jogador[], int qtdjogador,
                      int *cartaAtual) {
  for (int i = 0; i < qtdjogador; i++) {
    for (int j = 0; j < 2; j++) {
      jogador[i].mao[j] = baralho->cartas[(*cartaAtual)++];
    }
    jogador[i].numCartas = 2;
    jogador[i].pontuacao = calcularPontuacao(&jogador[i]);
  }
}

void imprimirMaos(Jogador jogador[], int qtdjogador) {
  for (int i = 0; i < qtdjogador; i++) {
    printf("Mão do jogador %s (Pontuação: %d):\n", jogador[i].nome,
           jogador[i].pontuacao);
    for (int j = 0; j < jogador[i].numCartas; j++) {
      imprimirCartaASCII(jogador[i].mao[j]);
    }
    printf("\n");
  }
}

void pegarNovaCarta(Baralho *baralho, Jogador *jogador, int *cartaAtual) {
  if (jogador->numCartas < 5) {
    jogador->mao[jogador->numCartas] = baralho->cartas[(*cartaAtual)++];
    jogador->numCartas++;
    jogador->pontuacao = calcularPontuacao(jogador);

    // Verifica se o jogador fez 21 pontos
    if (jogador->pontuacao == 21) {
      printf("%s fez 21 pontos e venceu o jogo!\n", jogador->nome);
      exit(0); // Encerra o jogo imediatamente
    } else if (jogador->pontuacao > 21) {
      printf("%s foi eliminado com %d pontos!\n", jogador->nome,
             jogador->pontuacao);
      // Se o jogador for eliminado, sua pontuação é definida como -1 para
      // identificação posterior
      jogador->pontuacao = -1;
    }
  }
}

int compararPontuacao(const void *a, const void *b) {
  Jogador *jogadorA = (Jogador *)a;
  Jogador *jogadorB = (Jogador *)b;
  return jogadorB->pontuacao - jogadorA->pontuacao;
}

int main() {
  int empate = 0;
  setlocale(LC_ALL, "Portuguese");
  Baralho baralho;
  int qtdjogador = 0;
  Jogador jogador[12]; // Inclui espaço para a CPU
  int cartaAtual = 0;

  printf("Digite aqui a quantidade de jogadores -> ");
  scanf("%d", &qtdjogador);
  limpaBuffer();

  // Nome dos jogadores humanos
  for (int i = 0; i < qtdjogador; i++) {
    printf("Digite o nome do jogador %d -> ", i + 1);
    fgets(jogador[i].nome, 20, stdin);
    // Remover o newline do final do nome do jogador
    jogador[i].nome[strcspn(jogador[i].nome, "\n")] = '\0';
  }

  // Nome da CPU
  strcpy(jogador[qtdjogador].nome, "CPU");
  qtdjogador++; // Incluir a CPU na contagem de jogadores

  inicializarBaralho(&baralho);
  embaralharBaralho(&baralho);
  distribuirCartas(&baralho, jogador, qtdjogador, &cartaAtual);
  imprimirMaos(jogador, qtdjogador);

  for (int i = 0; i < qtdjogador; i++) {
    if (jogador[i].pontuacao > 21) {
      printf("%s foi eliminado com %d pontos!\n", jogador[i].nome,
             jogador[i].pontuacao);
      // Se o jogador for eliminado, sua pontuação é definida como -1 para
      // identificação posterior
      jogador[i].pontuacao = -1;
    }
  }

  // Lógica para jogadores humanos pegarem novas cartas
  for (int i = 0; i < qtdjogador - 1; i++) { // Exceto CPU
    char opcao;
    do {
      if (jogador[i].pontuacao != -1) {
        printf("%s, você deseja pegar uma nova carta? (s/n): ", jogador[i].nome);
        scanf(" %c", &opcao);
      }
      if (opcao == 's' || opcao == 'S') {
        pegarNovaCarta(&baralho, &jogador[i], &cartaAtual);
        if (jogador[i].pontuacao != -1) {
          printf("Nova mão de %s (Pontuação: %d):\n", jogador[i].nome,
                 jogador[i].pontuacao);
                      for (int j = 0; j < jogador[i].numCartas; j++) {
                        imprimirCartaASCII(jogador[i].mao[j]);
                      }
                    }
                  }
                } while ((opcao == 's' || opcao == 'S') && jogador[i].pontuacao != -1);
              }

              // Lógica para CPU pegar novas cartas
              while (qtdjogador > 1 && jogador[qtdjogador - 1].pontuacao < 17 && jogador[qtdjogador - 1].pontuacao != -1) {
                pegarNovaCarta(&baralho, &jogador[qtdjogador - 1], &cartaAtual);
              }

              imprimirMaos(jogador, qtdjogador);

              // Ordenar jogadores pela pontuação
              qsort(jogador, qtdjogador, sizeof(Jogador), compararPontuacao);

              // Exibir resultados
              printf("Resultado final:\n");
              for (int i = 0; i < qtdjogador; i++) {
                if (jogador[i].pontuacao == -1) {
                  printf("%s foi eliminado!\n", jogador[i].nome);
                } else {
                  printf("%d. %s (Pontuação: %d)\n", i + 1, jogador[i].nome,
                         jogador[i].pontuacao);
                  for (int j = 0; j < jogador[i].numCartas; j++) {
                    imprimirCartaASCII(jogador[i].mao[j]);
                  }
                }
                printf("\n");
              }

              // Determinar o vencedor ou empate
              int maxPontuacao = jogador[0].pontuacao;
              int countEmpate = 0;
              for (int i = 1; i < qtdjogador; i++) {
                if (jogador[i].pontuacao == maxPontuacao) {
                  countEmpate++;
                }
              }

              if (countEmpate > 0) {
                printf("O jogo terminou em empate entre %d jogadores com pontuação %d!\n", countEmpate + 1, maxPontuacao);
              } else if (strcmp(jogador[0].nome, "CPU") != 0 && jogador[0].pontuacao != -1) {
                printf("O vencedor é %s com pontuação %d!\n", jogador[0].nome,
                       jogador[0].pontuacao);
              } else if (jogador[0].pontuacao != -1) {
                printf("A CPU venceu o jogo com pontuação %d!\n", jogador[0].pontuacao);
              } else {
                printf("Todos os jogadores humanos foram eliminados. A CPU venceu o jogo!\n");
              }

              printf("Aperte qualquer tecla para fechar...");
              getchar();
              return 0;
            }
