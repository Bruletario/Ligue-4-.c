#include <stdio.h>
#include <string.h>
#include <stdlib.h>


//constantes globais como #define para substituicao literal do valor

#define LINHAS 6 //linhas da matriz
#define COLUNAS 7 //colunas matriz
#define VAZIO 0 //o que está na matriz enquanto não há jogada
#define PLAYER_1 1 //valor do p1
#define PLAYER_2 2 //valor do p2
#define FICHA_COMUM 1 //ficha normal
#define PVP 1 //player x player
#define PVC 2 //player x cpu
#define CVC 3// cpu x cpu

// a matriz vai ser uma struct para podermos guardar outros valores alem da coordeanda, como cor, tipo de ficha, etc.
struct celula{
    int ocupante; //quem ocupa a celula vazio, p1 ou p2
    int tipo_ficha; //vai ser util pra implementar outras fihcas
    char simbolo[5]; // simbolo ou vazio
};

//valores p players

struct jogador{
    char nome[50]; //nome do player
    int id;
    int tipo; // se e humano ou cpu
    int jogadas; //quantas jogadas ele fez na partida atual
    int vitorias; //quantas vitorias no total 
    char cor_code[10]; //Codigo da cor
};

struct partida{
    struct celula matriz[LINHAS][COLUNAS]; //matriz que vamos usar
    struct jogador j1;
    struct jogador j2;
    int jogador_atual; //qual o jogador da vez
    int turno; //turno atual
    int modo_jogo;
    int venceu; // 0 nao ou 1 sim
    int game_on; // 0 nao ou 1 sim
};

//== PARTE VISUAL ==//

//cores e ficha
const char cor_reset[] = "\033[0m"; //padrao
const char cor_p1[] = "\033[1;34m"; // Azul
const char cor_p2[] = "\033[1;31m"; // Vermelho
const char ficha[] = "█";          // ficha do jogo
const char tabuleiro_vazio[] = " ";

void limpar_tela(){
    #ifdef _WIN32 //verica se é o win e limpa a tela
    system("cls");
    #elif __linux__ //verica se é o linux e limpa a tela
    system("clear");
    #endif
}

void configurar_ambiente_win(){ //garantir que nao vai ter problerma do bloco no windows
    #ifdef _WIN32
    system("chcp 65001");
    #endif
}

//vai guardar o que o usuario escolheu
int mostar_menu_principal(){
    int opcao;

    printf("====== LIGUE 4 ======\n");
    printf("1. Iniciar novo jogo\n");
    printf("2. Hall da fama\n");
    printf("3. Sair\n");
    printf("Escolha uma opção:\n");

    scanf("%d", &opcao);

    return opcao;
}

void solicitar_nomes(struct jogador *p1, struct jogador *p2, int modo) {
    
    //ids dos players, ponteiros
    p1->id = 1; 
    p2->id = 2;

    limpar_tela(); 
    printf("=== CONFIGURACAO DOS JOGADORES ===\n\n");

    //pvp
    if (modo == 1) {
        p1->tipo = 0; // 0 = humano
        printf("Digite o nome do Jogador 1: ");
        scanf(" %[^\n]", p1->nome); // %[^\n] define que o scanf vai ler tudo até achar um enter

        p2->tipo = 0; //humano
        printf("Digite o nome do Jogador 2: ");
        scanf(" %[^\n]", p2->nome);
    }
    
    // pvc
    else if (modo == 2) {
        p1->tipo = 0; //humano
        printf("Digite o nome do Jogador 1: ");
        scanf(" %[^\n]", p1->nome);

        p2->tipo = 1; // 1 = CPU
        //usamos strcpy para definir o nome fixo
        strcpy(p2->nome, "Computador"); 
        
        printf("\nSeu adversario sera o: %s\n", p2->nome);
        printf("Pressione Enter para continuar...");
        getchar(); getchar(); // Pausa 
    }

    // cvc
    else if (modo == 3) {
        p1->tipo = 1; //cou
        strcpy(p1->nome, "PC Azul");

        p2->tipo = 1; //cpu
        strcpy(p2->nome, "PC Vermelho");

        printf("Modo Simulacao selecionado.\n");
        printf("Partida: %s vs %s\n", p1->nome, p2->nome);

        printf("Pressione Enter para iniciar a simulacao...");
        getchar(); getchar(); //pausa
    }
}

void exibir_cabecalho(){
    printf("         LIGUE 4++  \n");
    printf("  1   2   3   4   5   6   7  \n");
}

void desenhar_tabuleiro(struct partida jogo){

    limpar_tela();
    exibir_cabecalho();

    printf("+---+---+---+---+---+---+---+\n");

    for(int i=0; i<LINHAS; i++){
        printf("|"); 
        
        for(int j=0; j<COLUNAS; j++){
            int valor = jogo.matriz[i][j].ocupante; // coordenada da jogada
            
            if(valor == VAZIO){
                printf(" %s |", tabuleiro_vazio); //verifica se a coordenada é 0, se for deixa em branco
            } 
            else if(valor == PLAYER_1){
                
                printf(" %s%s%s |", cor_p1, ficha, cor_reset); //verifica se a coordenada é 1, se for troca para o quadrado e pinta de azul
            } 
            else if(valor == PLAYER_2) {
                printf(" %s%s%s |", cor_p2, ficha, cor_reset); //verifica se a coordenada é 2, se for troca para o quadrado e pinta de vermelho
            }
        }
        printf("\n"); 
        
        printf("+---+---+---+---+---+---+---+\n"); 
    }
}

//== Lógica ==// 

void iniciar_tabuleiro(struct partida *jogo){

    for(int i=0; i<LINHAS; i++){
        for(int j=0; j<COLUNAS; j++){

        jogo->matriz[i][j].ocupante = VAZIO; // altera o endereço de memoria matriz e preenche ela com 0 (vazio)
        jogo->matriz[i][j].tipo_ficha = FICHA_COMUM; // define que as fichas são padrão
        strcpy(jogo->matriz[i][j].simbolo, " "); //define os simbolos da matriz para "nada"

         }
    }       
}



int validar(int coluna, struct partida jogo){
    int altura = -1;
    for(int i = 0; i<LINHAS;i++){
        if(jogo.matriz[i][coluna].ocupante == 0){
            altura = i;
            break;
        }
    }

    return altura;
}


//==Main==//
//Laço que permite persistência do jogo
