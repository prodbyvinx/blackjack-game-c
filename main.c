#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Limpa o buffer do teclado
void limpaBuffer() {
  int ch;
  while ((ch = getchar()) != '\n' && ch != EOF)
    ;
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
  int eliminado;
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

void imprimirCarta(Carta carta) {
  const char *valores[] = {"",      "Ás",     "Dois", "Três", "Quatro",
                           "Cinco", "Seis",   "Sete", "Oito", "Nove",
                           "Dez",   "Valete", "Dama", "Rei"};
  const char *naipes[] = {"Paus", "Ouros", "Copas", "Espadas"};

  printf("%s de %s\n", valores[carta.valor], naipes[carta.naipe]);
}

void imprimirBaralho(Baralho baralho) {
  for (int i = 0; i < 52; i++) {
    imprimirCarta(baralho.cartas[i]);
  }
}

int calcularPontuacao(Jogador *jogador) {
  int soma = 0;
  for (int i = 0; i < jogador->numCartas; i++) {
    int valor = jogador->mao[i].valor;
    if (valor > 10) {
      valor = 10; // Valete, Dama e Rei têm valor 10
    }
    soma += valor;
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
      imprimirCarta(jogador[i].mao[j]);
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

  if (jogadorA->pontuacao == -1)
    return 1;
  if (jogadorB->pontuacao == -1)
    return -1;

  int diferencaA = 21 - jogadorA->pontuacao;
  int diferencaB = 21 - jogadorB->pontuacao;

  return diferencaA - diferencaB;
}

int main() {
  Baralho baralho;
  int qtdjogador = 0;
  Jogador jogador[13]; // Inclui espaço para a CPU
  int cartaAtual = 0;

  printf("Digite o número de jogadores (máximo 12): ");
  scanf("%d", &qtdjogador);
  limpaBuffer();

  // Nome dos jogadores humanos
  for (int i = 0; i < qtdjogador; i++) {
    printf("Digite o nome do jogador %d: ", i + 1);
    fgets(jogador[i].nome, 20, stdin);
    jogador[i].nome[strcspn(jogador[i].nome, "\n")] =
        '\0'; // Remove a quebra de linha
  }

  // Nome da CPU
  strcpy(jogador[qtdjogador].nome, "CPU");
  qtdjogador++; // Inclui a CPU na contagem de jogadores

  inicializarBaralho(&baralho);
  embaralharBaralho(&baralho);
  distribuirCartas(&baralho, jogador, qtdjogador, &cartaAtual);
  imprimirMaos(jogador, qtdjogador);

  // Lógica para jogadores humanos pegarem novas cartas
  for (int i = 0; i < qtdjogador - 1; i++) { // Exceto CPU
    char opcao;
    do {
      printf("%s, você deseja pegar uma nova carta? (s/n): ", jogador[i].nome);
      scanf(" %c", &opcao);
      limpaBuffer(); // Limpa o buffer após a leitura do caractere
      if (opcao == 's' || opcao == 'S') {
        pegarNovaCarta(&baralho, &jogador[i], &cartaAtual);
        if (jogador[i].pontuacao != -1) {
          printf("Nova mão de %s (Pontuação: %d):\n", jogador[i].nome,
                 jogador[i].pontuacao);
          for (int j = 0; j < jogador[i].numCartas; j++) {
            imprimirCarta(jogador[i].mao[j]);
          }
        }
      }
    } while ((opcao == 's' || opcao == 'S') && jogador[i].pontuacao != -1);
  }

  // Lógica para CPU pegar novas cartas
  while (qtdjogador > 1 && jogador[qtdjogador - 1].pontuacao < 17) {
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
        imprimirCarta(jogador[i].mao[j]);
      }
    }
    printf("\n");
  }

  // Determinar o vencedor
  if (jogador[0].pontuacao !=
      -1) { // Se o jogador com maior pontuação não foi eliminado
    if (jogador[0].pontuacao ==
        jogador[1].pontuacao) { // Empate na pontuação mais alta
      if (strcmp(jogador[0].nome, "CPU") == 0 ||
          strcmp(jogador[1].nome, "CPU") == 0) {
        printf("A CPU venceu o jogo com pontuação %d!\n", jogador[0].pontuacao);
      } else {
        printf("O jogo terminou em empate entre %s e %s com pontuação %d!\n",
               jogador[0].nome, jogador[1].nome, jogador[0].pontuacao);
      }
    } else { // Não há empate
      if (strcmp(jogador[0].nome, "CPU") == 0 && jogador[1].pontuacao != -1) {
        // CPU não deve vencer se há um humano não eliminado com pontuação menor
        printf("O vencedor é %s com pontuação %d!\n", jogador[1].nome,
               jogador[1].pontuacao);
      } else if (strcmp(jogador[1].nome, "CPU") == 0 &&
                 jogador[0].pontuacao != -1) {
        printf("O vencedor é %s com pontuação %d!\n", jogador[0].nome,
               jogador[0].pontuacao);
      } else {
        printf("O vencedor é %s com pontuação %d!\n", jogador[0].nome,
               jogador[0].pontuacao);
      }
    }
  } else { // Todos os jogadores humanos foram eliminados
    printf(
        "Todos os jogadores humanos foram eliminados. A CPU venceu o jogo!\n");
  }

  return 0;
}
