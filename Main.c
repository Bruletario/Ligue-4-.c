#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define LINHAS 6
#define COLUNAS 7
//valores p matriz

const int vazio = 0; //o que está na matriz enquanto não há jogada
const int player1 = 1;
const int player2 = 2;
//valores p players

struct jogador{
    char nome[50]; //nome do player
    int vitórias; //vítorias que ele tem
    int jogadas; //quantas jogadas ele fez na partida atual
};

struct partida{
    int matriz[LINHAS][COLUNAS]; //matriz que vamos usar
    int JogadorVez; //qual o jogador da vez
    int turnos; //quantos turnos o jogo teve
};

//== PARTE VISUAL ==//

const char *cor_p1 = "\x1b[34m"; //cor azul para o p1
const char *cor_p2 = "\x1b[31m"; //cor vermelha para o p2
const char *ficha = "█";
const char *tabuleiro_vazio = " ";

int limpar_tela(){
    #ifdef _WIN32 //verica se é o win e limpa a tela
    system("cls");
    #elif __linux__ //verica se é o linux e limpa a tela
    system("clear");
    #endif;
}

int exibir_cabecalho(){
    printf("== LIGUE 4++ ==\n");
    printf("1   2   3   4   5   6   7\n");
}



int desenhar_tabuleiro(struct partida jogo){

    limpar_tela();
    exibir_cabecalho();
    printf("--------------------------");

    for(int i=0;i<5;i++){
        printf("|");
            for(int i=0; j<6; j++){
                partida.matriz[i][j];
            }

    }
