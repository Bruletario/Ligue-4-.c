#include <stdio.h>
#include <string.h>
#include <stdlib.h> // pra encerrar o jogo e configurar ambiente no terminal
#include <time.h> // Necessário para o rand() funcionar bem

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
const char cor_atencao[] = "\x1b[33m"; //amarelo em hexadecimal pq nao achei em ascii
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
    int resultado;

    do {
        limpar_tela(); //garante o terminal limpo
        printf("====== LIGUE 4 ======\n");
        printf("1. Iniciar novo jogo\n");
        printf("2. Hall da fama\n");
        printf("3. Sair\n");
        printf("Escolha uma opção: ");

        resultado = scanf("%d", &opcao); // captura o que o usuario digitou

        if (resultado != 1 || opcao < 1 || opcao > 3) { // verifica se é numero e se está entre 1 e 3
            printf("%sEntrada inválida! Digite um número entre 1 e 3.%s\n", cor_atencao, cor_reset);
            
            while (getchar() != '\n'); // limpa o buffer, isso aqui é pre evitar algum tipoi de buffer por lixo no buffer
        
            printf("Pressione Enter para tentar novamente...");
            getchar();
        } else {
            break; 
        }

    } while (1); // isso aqui só para no break

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
int altura = -1; // Valor padrao de erro

    if((coluna < 0) || (coluna > 6)){ // Verifica se a coluna existe 
    return altura;
    }

    for(int i = LINHAS - 1; i >= 0; i--){ // Percorre de baixo pra cima
    if(jogo.matriz[i][coluna].ocupante == VAZIO){
    altura = i; // Achou uma linha vazia
    return altura; // Retorna a linha encontrada
    }
}

return altura; // Se chegou aqui a coluna ta cheia (-1)
}

/*
void jogada_cpu(){
    int coluna_rand;
    result_valida = validar(coluna_rand); // Testa se existe 
    if(validar(coluna_rand) != -1){

    }
}
*/



void inserir_ficha(struct partida *jogo, int linha, int coluna, int id_jogador){
    //como validar jogada já garante que tem como jogar, apenas gravamos na memomoria

    jogo->matriz[linha][coluna].ocupante = id_jogador; //substitui o ocupante para o player que jogou
    jogo->matriz[linha][coluna].tipo_ficha = FICHA_COMUM; //substitui para ficha comum
    strcpy(jogo->matriz[linha][coluna].simbolo, ficha); //substitui o simbolo para o do player

}

// verifica se há um vencedor ao decorrer das jogadas
int verificar_vitoria(struct partida jogo, int jogador) {
int cont;

// vrifica se tem 4 fichas na HORIZONTAL
for (int i = 0; i < LINHAS; i++) {
    cont = 0;
    for (int j = 0; j < COLUNAS; j++) {
        if (jogo.matriz[i][j].ocupante == jogador) {
            cont++;
            if (cont == 4) return 1;
        } else {
            cont = 0;
        }
    }
}

// Verifica VERTICAL 
for (int j = 0; j < COLUNAS; j++) {
    cont = 0;
    for (int i = 0; i < LINHAS; i++) {
        if (jogo.matriz[i][j].ocupante == jogador) {
            cont++;
            if (cont == 4) return 1;
        } else {
            cont = 0;
        }
    }
}

// Verifica DIAGONAL(principal) 
for (int i = 0; i < LINHAS - 3; i++) {
    for (int j = 0; j < COLUNAS - 3; j++) {
        if (jogo.matriz[i][j].ocupante == jogador &&
            jogo.matriz[i+1][j+1].ocupante == jogador &&
            jogo.matriz[i+2][j+2].ocupante == jogador &&
            jogo.matriz[i+3][j+3].ocupante == jogador) {
            return 1;
        }
    }
}

// Verifica DIAGONAL(secundária)
for (int i = 3; i < LINHAS; i++) {
    for (int j = 0; j < COLUNAS - 3; j++) {
        if (jogo.matriz[i][j].ocupante == jogador &&
            jogo.matriz[i-1][j+1].ocupante == jogador &&
            jogo.matriz[i-2][j+2].ocupante == jogador &&
            jogo.matriz[i-3][j+3].ocupante == jogador) {
            return 1;
        }
    }
}


return 0;// ninguém venceu
}

void trocar_turno(struct partida *jogo) {
    if (jogo->jogador_atual == PLAYER_1) { //se o jogador atual for o player 1, muda para o dois
        jogo->jogador_atual = PLAYER_2;
    } else {jogo->jogador_atual = PLAYER_1;} // se nao, joga o player 1
    jogo->turno++; //mais um turno é somado
}

//==Main==//
int main() {
struct partida jogo;
int opcao_menu;
int coluna_escolhida;
int linha_encontrada; // Vai receber o retorno do validar
int tipo_atual;

// Semente para números aleatórios
srand(time(NULL));

configurar_ambiente_win();

opcao_menu = mostar_menu_principal();

if (opcao_menu == 1){ 
// Seleção de Modo
limpar_tela();
printf("1. Player vs Player\n");
printf("2. Player vs CPU\n");
printf("3. CPU vs CPU\n");
printf("Escolha o modo: ");
scanf("%d", &jogo.modo_jogo);

solicitar_nomes(&jogo.j1, &jogo.j2, jogo.modo_jogo);

iniciar_tabuleiro(&jogo);
jogo.jogador_atual = PLAYER_1;
jogo.turno = 1;
jogo.game_on = 1;
jogo.venceu = 0;

// Loop do jogo
while (jogo.game_on == 1) {
    
    desenhar_tabuleiro(jogo);

    struct jogador *jogador_vez;
    if(jogador_vez == &jogo.j1) {
        printf("%s\n--- Turno: %d ---%s\n",cor_p2, jogo.turno, cor_reset);}
    else {
        printf("%s\n--- Turno: %d ---%s\n",cor_p1, jogo.turno, cor_reset);};
    
    // Define quem é o jogador da vez para facilitar os ifs abaixo
    if(jogo.jogador_atual == PLAYER_1) jogador_vez = &jogo.j1;
    else (jogador_vez = &jogo.j2);

    if(jogador_vez == &jogo.j1) {
        printf("%sVez de: %s%s\n",cor_p1, jogador_vez->nome, cor_reset);}
    else {
        printf("%sVez de: %s%s\n",cor_p2, jogador_vez->nome, cor_reset);}
    tipo_atual = jogador_vez->tipo;

    // obtem coluna
    if (tipo_atual == 0) { // Humano
        printf("Escolha uma coluna (1 a 7): ");
        scanf("%d", &coluna_escolhida);
        coluna_escolhida = coluna_escolhida - 1; // Ajusta para 0-6
    } else { // CPU
        printf("Computador pensando...\n");
        // Tenta gerar um numero aleatorio
        coluna_escolhida = rand() % COLUNAS; 
    }

    // Aqui verificamos se a jogada é valida

    linha_encontrada = validar(coluna_escolhida, jogo);

    if (linha_encontrada != -1) {
        // Caso validar retorne um valor valido:

        inserir_ficha(&jogo, linha_encontrada, coluna_escolhida, jogo.jogador_atual);
        
        // verificando a vitoria e imprimindo mensagem informando quem ganhou 
        if (verificar_vitoria(jogo, jogo.jogador_atual)) {
             desenhar_tabuleiro(jogo);
             printf("\n %s VENCEU O JOGO!\n", jogador_vez->nome);
             jogo.game_on = 0;
             jogo.venceu = 1;
             getchar(); getchar();
            break;
        }                 


        trocar_turno(&jogo);
        
    } else {
        // Se validar retornou -1(ou seja,erro):
        if(tipo_atual == 0){ // Só avisa se for humano
            printf("\nJOGADA INVALIDA! Coluna cheia ou inexistente.\n");
            printf("Pressione Enter para tentar de novo...");
            getchar(); getchar();
        }
        // Se for CPU, ele só vai repetir o loop e tentar outro numero
    }
    
    // Pausa visual pro modo CPU vs CPU,ainda ta bem rapido entao talvez mude
    if (jogo.modo_jogo == 3) {
        for(int k=0; k<300000000; k++); 
    }
}
}
else if(opcao_menu == 2) printf("%s Essa área ainda nao foi desbloqueada, jovem Jedi! Aguarde até a próxima atualizacao. :) \n%s",cor_atencao,cor_reset);
else if (opcao_menu ==3){
printf("%sMuito obrigado por jogar o nosso jogo! :)\n%s",cor_atencao,cor_reset);
exit(0);
}

return 0; 
}
