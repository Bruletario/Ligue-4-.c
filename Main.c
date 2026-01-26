#include <stdio.h>
#include <string.h>
#include <stdlib.h>
  
#define LINHAS 6
#define COLUNAS 7
//valores p matriz

const int vazio = 0; //o que está na matriz enquanto não há jogada
const int player1 = 1;
const int player2 = 2;
const int humano = 0;
const int cpu = 1;

//valores p players

struct jogador{
    char nome[50]; //nome do player
    int vitorias; //vítorias que ele tem
    int jogadas; //quantas jogadas ele fez na partida atual
    int modo_de_jogo; //guarda qual o modo de jogo atual
};

struct partida{
    int matriz[LINHAS][COLUNAS]; //matriz que vamos usar
    int JogadorVez; //qual o jogador da vez
    int turnos; //quantos turnos o jogo teve
};

//== PARTE VISUAL ==//

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
            int valor = jogo.matriz[i][j];
            
            if(valor == vazio){
                printf(" %s |", tabuleiro_vazio); //verifica se a coordenada é 0, se for deixa em branco
            } 
            else if(valor == player1){
                
                printf(" %s%s%s |", cor_p1, ficha, cor_reset); //verifica se a coordenada é 1, se for troca para o quadrado e pinta de azul
            } 
            else if(valor == player2) {
                printf(" %s%s%s |", cor_p2, ficha, cor_reset); //verifica se a coordenada é 2, se for troca para o quadrado e pinta de vermelho
            }
        }
        printf("\n"); 
        
        printf("+---+---+---+---+---+---+---+\n"); 
    }
}

void menu_iniciar(){
	
	}


/*teste de visuzalizacao
int main() {

    struct partida jogoAtual;
    
    for(int i=0; i<LINHAS; i++) {
        for(int j=0; j<COLUNAS; j++) {
            jogoAtual.matriz[i][j] = 0;
        }
    }
    
    // simulando uma jogada
    jogoAtual.matriz[5][3] = 1;
    jogoAtual.matriz[4][3] = 2; 
    
    desenhar_tabuleiro(jogoAtual);

    getchar(); //pausa pra ver o resultado

    return 0;
} */

//== Lógica ==// 
// funções que executam a jogada do usuário

//==Main==//
//Laço que permite persistência do jogo
