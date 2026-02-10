
#include <stdio.h>
#include <string.h>
#include <stdlib.h> // pra encerrar o jogo e configurar ambiente no terminal
#include <time.h> // Necessário para o rand() funcionar bem

// definição para tratar pausa visual tanto para win quanto para linux
#ifdef _WIN32
#include <windows.h>
#define SLEEP_MS(ms) Sleep(ms)
#else
#include <unistd.h>
#define SLEEP_MS(ms) usleep((ms) * 1000)
#endif

//constantes globais como #define para substituicao literal do valor

#define LINHAS 6 //linhas da matriz
#define COLUNAS 7 //colunas matriz
#define VAZIO 0 //o que está na matriz enquanto não há jogada
#define PLAYER_1 1 //valor do p1
#define PLAYER_2 2 //valor do p2
#define FICHA_COMUM 1 //ficha normal
#define FICHA_EXPLOSIVA 2 // ficha explosiva
#define FICHA_PORTAL 3 // ficha portal
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
int fichas_comuns; //p usar no inventario
int fichas_explosivas;//p usar no inventario
int fichas_portal;//p usar no inventario
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
const char ficha[] = "█"; // ficha do normal
const char ficha_exp[] = "⛞"; // Ficha Explosiva
const char ficha_por[] = "U"; // Ficha Portal
const char tabuleiro_vazio[] = " ";

void limpar_tela(){
#ifdef _WIN32 //verica se é o win e limpa a tela
system("cls");
#elif linux //verica se é o linux e limpa a tela
system("clear");
#endif
}

void limpar_buffer(){
while((getchar()) != '\n'); // função de limpeza de buffer, adicionada para tentar resolver o problema de overflow do buffer na hora de ler o nome dos players
}

void configurar_ambiente_win(){ //garantir que nao vai ter problerma do bloco no windows
#ifdef _WIN32
system("chcp 65001");
#endif
}

void delay_visual(int ms){ // garantir que não tenha problema na pausa visual
SLEEP_MS(ms);
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
        
        limpar_buffer(); // limpa o buffer, isso aqui é pre evitar algum tipoi de erro por lixo no buffer
    
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
    fgets(p1->nome, 50, stdin);
    if (strchr(p1->nome, '\n') == NULL) { //verifica se  não tem quebra de linha no vetor, se não tiver o usuario colocou muitos caracteres, logo ele precisa limpar o buffer
        limpar_buffer();} 
    else {p1->nome[strcspn(p1->nome, "\n")] = 0;} // se nao, vai ler as entradas do teclado e vai substituir o enter pelo nulo

    p2->tipo = 0; //humano
    printf("Digite o nome do Jogador 2: ");
    fgets(p2->nome, 50, stdin);
    if (strchr(p2->nome, '\n') == NULL) {
        limpar_buffer();} 
    else {p2->nome[strcspn(p2->nome, "\n")] = 0;}
}

// pvc
else if (modo == 2) {
    p1->tipo = 0; //humano
    printf("Digite o nome do Jogador 1: ");
    fgets(p1->nome, 50, stdin);
    if (strchr(p1->nome, '\n') == NULL) {
        limpar_buffer();} 
    else {p1->nome[strcspn(p1->nome, "\n")] = 0;}

    p2->tipo = 1; // 1 = CPU
    //usamos strcpy para definir o nome fixo
    strcpy(p2->nome, "Computador"); 
    
    printf("\nSeu adversario sera o: %s\n", p2->nome);
        printf("Pressione Enter para continuar...");
        getchar(); // Pausa 
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
    getchar(); //pausa
}
}

void exibir_cabecalho(){
printf(" LIGUE 4++ \n");

printf("   1   2   3   4   5   6   7   \n");
}

void desenhar_tabuleiro(struct partida jogo){

limpar_tela();
exibir_cabecalho();

printf("+---+---+---+---+---+---+---+\n");

for(int i=0; i<LINHAS; i++){
    printf("|"); 
    
    for(int j=0; j<COLUNAS; j++){
        int valor = jogo.matriz[i][j].ocupante; // coordenada da jogada
         int tipo = jogo.matriz[i][j].tipo_ficha;  // tipo da fihca
        
        if(valor == VAZIO){
            printf(" %s |", tabuleiro_vazio); //verifica se a coordenada é 0, se for deixa em branco
        } 
        else{
         // define a cor com base no dono da ficha 
            const char *cor = (valor == PLAYER_1) ? cor_p1 : cor_p2; 
            const char *simbolo_peca;

            // define tipo da ficha
            if (tipo == FICHA_EXPLOSIVA) simbolo_peca = ficha_exp;
            else if (tipo == FICHA_PORTAL) simbolo_peca = ficha_por;
            else simbolo_peca = ficha;

            // imprime a ficha
            printf(" %s%s%s |", cor, simbolo_peca, cor_reset);
        }
    }
    printf("\n+---+---+---+---+---+---+---+\n"); 
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
}}
return altura; // Se chegou aqui a coluna ta cheia (-1)
}

void inserir_ficha(struct partida *jogo, int linha, int coluna, int id_jogador, int tipo_ficha){
//como validar jogada já garante que tem como jogar, apenas gravamos na memomoria

jogo->matriz[linha][coluna].ocupante = id_jogador; //substitui o ocupante para o player que jogou
jogo->matriz[linha][coluna].tipo_ficha = tipo_ficha; //substitui para ficha escolhida
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
}}
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
return 1;}
}
}

// Verifica DIAGONAL(secundária)
for (int i = 3; i < LINHAS; i++) {
for (int j = 0; j < COLUNAS - 3; j++) {
if (jogo.matriz[i][j].ocupante == jogador &&
jogo.matriz[i-1][j+1].ocupante == jogador &&
jogo.matriz[i-2][j+2].ocupante == jogador &&
jogo.matriz[i-3][j+3].ocupante == jogador) {
return 1;}
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

int selecionar_modo(){
int modo;
do {
limpar_tela();
printf("=== SELECIONE O MODO DE JOGO ===\n");
printf("1. Player vs Player\n");
printf("2. Player vs CPU\n");
printf("3. CPU vs CPU\n");
printf("Escolha: ");
if (scanf("%d", &modo) != 1 || modo < 1 || modo > 3) { //verifica se o que foi digitado é número ou está entre 1 e 3, senao estiver, retorna para selecao
printf("%sEntrada inválida!%s\n", cor_atencao, cor_reset);
limpar_buffer();
getchar(); //congela a tela
} else {
limpar_buffer(); // estava pulando a vez do player colocar o nome pq etava ficando o enter na memoria, esse getchar vai limpar o buffer e corrigir isso.
break;
}
} while (1);
return modo; //devolve o modo escolhido
}

int verificar_empate(struct partida *jogo){
    //soma das fichas
    int valor_j1 = jogo->j1.fichas_comuns + jogo->j1.fichas_explosivas + jogo->j1.fichas_portal;
    int valor_j2 = jogo->j2.fichas_comuns + jogo->j2.fichas_explosivas + jogo->j2.fichas_portal;

    //verifica se as fichas acabaram ou se o acabou o n de jogadas possiveis
    if((valor_j1 ==0)||(valor_j2 ==0)||(jogo->turno == LINHAS*COLUNAS)){
        desenhar_tabuleiro(*jogo);
        jogo->game_on = 0; //encerra o loop
        return 1; //true p empate
    }
    return 0; // false, continua o jogo
}

void aplicar_gravidade(struct partida *jogo, int coluna) {
    
    for (int i = LINHAS - 1; i > 0; i--) { //de baixo pra cima
        if (jogo->matriz[i][coluna].ocupante == VAZIO) { //se a cel atual estiver vazia Procura a primeira ficha acima dela para puxar para baixo
            for (int j = i - 1; j >= 0; j--) {
                if (jogo->matriz[j][coluna].ocupante != VAZIO) {
                    //copia os dados da ficha de cima para a posicaoo atual
                    jogo->matriz[i][coluna] = jogo->matriz[j][coluna];
                    // limpa a posição antiga la de cima
                    jogo->matriz[j][coluna].ocupante = VAZIO;
                    jogo->matriz[j][coluna].tipo_ficha = VAZIO;
                    strcpy(jogo->matriz[j][coluna].simbolo, " ");
                    break; // Sai do loop menor
                }
            }
        }
    }
}

//funcao teste para tentar animar o portal, nao esta funcionando muito bem
void animar_portal(struct partida *jogo, int linha, int coluna) {
    //coloca o portal na matriz
    jogo->matriz[linha][coluna].ocupante = jogo->jogador_atual; 
    jogo->matriz[linha][coluna].tipo_ficha = FICHA_PORTAL;

    //mostra p o player
    desenhar_tabuleiro(*jogo);
    printf("\n%s[PORTAL] Ativando portal na coluna %d...%s\n", cor_atencao, coluna + 1, cor_reset);
    
    //pausa visual
    delay_visual(1200);

    //efeito de engolir que essa ficha tem
    jogo->matriz[linha][coluna].ocupante = VAZIO;
    jogo->matriz[linha][coluna].tipo_ficha = VAZIO;
}

int usar_ficha_portal(struct partida *jogo, int coluna) {
    if (coluna < 0 || coluna >= COLUNAS) return 0;

    int linha_visual = validar(coluna, *jogo); //verifica onde o portal vai aparecer 
    
    if (linha_visual == -1) linha_visual = 0; // se a coluna tiver cheia, aparece no topo

    //adicionei a logica de animacao
    animar_portal(jogo, linha_visual, coluna);

    //percorre pra remover a ficha se encontrar algo que não seja vazio
    for (int i = 0; i < LINHAS; i++) { 
        if (jogo->matriz[i][coluna].ocupante != VAZIO && i != linha_visual) {
            jogo->matriz[i][coluna].ocupante = VAZIO;
            jogo->matriz[i][coluna].tipo_ficha = VAZIO;
            strcpy(jogo->matriz[i][coluna].simbolo, " ");

            printf("%s>>> O Portal absorveu a ficha abaixo! <<<%s\n", cor_atencao, cor_reset);
            
            // organiza com a gravidade
            aplicar_gravidade(jogo, coluna);
            delay_visual(800);
            return 1;
        }
    }

    // se a coluna estava vazia abaixo do portal
    printf("%s>>> O Portal desapareceu no vazio! <<<%s\n", cor_atencao, cor_reset);
    delay_visual(1200);
    return 1;
}

// essa função controla o inicio da partida

void iniciar_partida(struct partida *jogo){
iniciar_tabuleiro(jogo); //inica o tabuleiro
jogo->jogador_atual = PLAYER_1; //define o player 1 como jogador inicial
jogo->turno = 1; //começa no 1 turno
jogo->game_on = 1; // jogo esta acontecendo

// Inicializa fichas
jogo->j1.fichas_comuns = 21; jogo->j1.fichas_explosivas = 0; jogo->j1.fichas_portal = 0; //  acessa pela struct jogo
jogo->j2.fichas_comuns = 21; jogo->j2.fichas_explosivas = 0; jogo->j2.fichas_portal = 0; 

//enquanto o jogo estiver em 1 o while vale
while (jogo->game_on) {

    desenhar_tabuleiro(*jogo);
    
    struct jogador *jogador_vez = (jogo->jogador_atual == PLAYER_1) ? &jogo->j1 : &jogo->j2;
    const char *cor_v = (jogo->jogador_atual == PLAYER_1) ? cor_p1 : cor_p2;

    printf("\n%s--- Turno: %d ---%s\n", cor_atencao, jogo->turno, cor_reset);
    printf("%sVez de: %s%s\n", cor_v, jogador_vez->nome, cor_reset);

    int coluna = -1; //recebe a coluna do player
    int tipo_f = FICHA_COMUM; // define o tipo padrão antes da escolha
    int jogada_realizada = 0; //verifica se a jogada foi feita

    if (jogador_vez->tipo == 0) { //player
        int escolha_valida = 0;
        
        // Loop para seleção do tipo de ficha
        while (!escolha_valida) {
            printf("Inventário: [C] Comum:%d | [E] Explosiva:%d | [P] Portal:%d\n", 
                    jogador_vez->fichas_comuns, jogador_vez->fichas_explosivas, jogador_vez->fichas_portal);
            printf("Escolha o tipo de ficha: ");
            char input;
            if (scanf(" %c", &input) != 1) {
                limpar_buffer();
                continue;
            }
            limpar_buffer();
            if (input == 'c' || input == 'C') tipo_f = FICHA_COMUM;
            else if (input == 'e' || input == 'E') tipo_f = FICHA_EXPLOSIVA;
            else if (input == 'p' || input == 'P') tipo_f = FICHA_PORTAL;
            else {
                printf("%sOpção inválida!%s\n", cor_atencao, cor_reset);
                continue;
            }

            // Valida se o jogador possui a ficha no inventário
            if ((tipo_f == FICHA_COMUM && jogador_vez->fichas_comuns > 0) ||
                (tipo_f == FICHA_EXPLOSIVA && jogador_vez->fichas_explosivas > 0) ||
                (tipo_f == FICHA_PORTAL && jogador_vez->fichas_portal > 0)) {
                escolha_valida = 1;
            } else {
                printf("%sVocê não tem essa ficha!%s\n", cor_atencao, cor_reset);
            }
        }

        printf("Escolha uma coluna (1-7): ");
        if (scanf("%d", &coluna) != 1) { //tratamento de erro, se nao for itneiro cai nesse caos
            limpar_buffer(); 
            coluna = -1;     // força a coluna a ser inválida para cair no else
        } else {
            coluna--; // Ajuste de índice se a leitura foi ok
        }
    } else { // se nao, quem joga é a cpu
        printf("Computador pensando...\n");
        delay_visual(1500); // Pausa de 1,5 segundos para simular o pensamento da CPU
            do coluna = rand() % COLUNAS; // tenta gerar um numero aleatorio
                while (validar(coluna, *jogo) == -1); // se a jogada n for valida ele tenta novamente
        tipo_f = FICHA_COMUM; // CPU por enquanto só usa comum
    }

    if (tipo_f == FICHA_PORTAL) { 
            jogada_realizada = usar_ficha_portal(jogo, coluna); // se for a portal, utiliza ela
            if (jogada_realizada) { 
                jogador_vez->fichas_portal--; //remove do inventario
            }
        } 
        else {
            int linha = validar(coluna, *jogo); // se nao, verifica se é uma jogada valida
            if (linha != -1) { // se for valida, faz a jogada se for comum ou explosiva
                inserir_ficha(jogo, linha, coluna, jogo->jogador_atual, tipo_f);
                if (tipo_f == FICHA_COMUM) jogador_vez->fichas_comuns--;
                else if (tipo_f == FICHA_EXPLOSIVA) jogador_vez->fichas_explosivas--;
                jogada_realizada = 1;
            } else {
                printf("\n%sCOLUNA CHEIA!%s\n", cor_atencao, cor_reset); // se nao, jogada invalida
                delay_visual(1000);
            }
        }

if (jogada_realizada) {

            // se for portal checamos vitória dos dois 
            // Se foi comum checamos apenas para o atual
            //tambem verifica empate e apos 10 turnos da fichas bonus
            int venceu = (tipo_f == FICHA_PORTAL) ? 
                         (verificar_vitoria(*jogo, PLAYER_1) || verificar_vitoria(*jogo, PLAYER_2)) :
                         verificar_vitoria(*jogo, jogo->jogador_atual);

            if (venceu) {
                desenhar_tabuleiro(*jogo);
                printf("\n%s VITÓRIA! %s\n", cor_v, cor_reset);
                jogo->game_on = 0;
            } 
            else if (verificar_empate(jogo)) {
                printf("\nEMPATE!\n");
                jogo->game_on = 0;
            } 
            else {
                trocar_turno(jogo);
                //fihcas bonus
                if (jogo->turno > 1 && (jogo->turno - 1) % 10 == 0) {
                    jogo->j1.fichas_explosivas++; jogo->j1.fichas_portal++;
                    jogo->j2.fichas_explosivas++; jogo->j2.fichas_portal++;
                    printf("\n%s[BÔNUS] Fichas especiais recebidas!%s\n", cor_atencao, cor_reset);
                    delay_visual(1000);
                }
            }
        }
    }
}

//menu pos jogo para dar opcao ao usuario e tratamento de erro, usamos o mesmo no menu principa
int pos_jogo(){
int opcao;
int resultado;
do{
printf("\n============================\n");
printf("1. Jogar Novamente\n");
printf("2. Voltar ao Menu Principal\n");
printf("3. Sair do Jogo\n");
printf("Escolha uma opção: ");

    resultado = scanf("%d", &opcao); // captura o que o usuario digitou

    if (resultado != 1 || opcao < 1 || opcao > 3) { // verifica se é numero e se está entre 1 e 3
        printf("%sEntrada inválida! Digite um número entre 1 e 3.%s\n", cor_atencao, cor_reset);
        
        limpar_buffer(); // limpa o buffer, isso aqui é pre evitar algum tipoi de erro por lixo no buffer
    
        printf("Pressione Enter para tentar novamente...");
        getchar();
    } else {
        break; 
    }

} while (1); // isso aqui só para no break

return opcao;
}

//==Main==//
int main() {
struct partida jogo = {0}; //recebe o estado - deixamos zerado para evitar lixo de momoria
int opcao_menu;

srand(time(NULL)); 
configurar_ambiente_win();

while (1) {
    opcao_menu = mostar_menu_principal();

    if (opcao_menu == 1) { // se for 1 inicia o jogo
        jogo.modo_jogo = selecionar_modo(); //seleciona modos
        solicitar_nomes(&jogo.j1, &jogo.j2, jogo.modo_jogo); //solicita nomes
        
        int jogar_de_novo = 1; //jogar novamente recebe 1 para podermos jogar novamente
        while (jogar_de_novo == 1) {
            iniciar_partida(&jogo); 
            
            int acao = pos_jogo(); //escolher acao pos jogo
            if (acao == 1) jogar_de_novo = 1;
            else if (acao == 3) exit(0);
                else jogar_de_novo = 0; // se nao quiser jogar novamente recebe zero
        }
    } 
    else if (opcao_menu == 2)  {
        limpar_tela();
        printf("%sCalma, jovem padawan! Ainda nao desenvolvemos essa parte, espere ate a proxima atualização :)%s\n", cor_atencao, cor_reset);
        printf("Pressione Enter para voltar ao menu...");
        limpar_buffer(); 
        getchar();} 
        
    else if (opcao_menu == 3) {
        printf("Que a força esteja com você!\n");
        break;
    }
}
return 0;
}
