/**
 * Universidade: Pontifícia Universidade Católica do Rio Grande do Sul
 * Unidade: Faculdade de Informática
 * Curso: Bacharelado em Ciência da Computação
 * Disciplina: Computação Gráfica II
 * Turma: 128
 * Trabalho: Trabalho 2
 * Professor: Márcio Pinho
 * Alunos: Emanoel Viana, Gabriell Araujo
 */

#ifdef _MSC_VER
#endif

#ifdef WIN32
#include <windows.h>
#include "gl\glut.h"
#endif

#ifdef __APPLE__
#include <GLUT/glut.h>
#endif

#include <iostream>
#include <sstream>
#include <string>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include "ImageClass.h"
#include "SOIL/SOIL.h"

//constantes
#define LARGURA_JAN 1000
#define ALTURA_JAN 500
#define IMAGENS_NA_PASTA 10
#define ESTRUTURAS_DE_ANALISE 4
#define DENTINA 0
#define CANAL 1
#define PINOS 2
#define FUNDO 3

//estrutura ponto
typedef struct
{
    int x;
    int y;
} ponto;

//estrutura estatística
typedef struct
{
    int quantidade_total_de_pixels;
    int quantidade_de_pixels_vermelhos;
    int quantidade_de_pixels_verdes;
    int quantidade_de_pixels_azuis;
    int quantidade_de_pixels_pretos;
    int verdadeiros_positivos;
    int falsos_positivos;
    int falsos_negativos;
    double percentural_vermelho;
    double percentural_verde;
    double percentural_azul;
    double percentural_preto;
    int matriz_de_classificacao[ESTRUTURAS_DE_ANALISE][ESTRUTURAS_DE_ANALISE];
} estatistica;

//variáveis globais
ImageClass Image, NewImage;
int limiar_inferior, limiar_superior;
unsigned char** r_matrix_aux;
unsigned char** g_matrix_aux;
unsigned char** b_matrix_aux;
unsigned char** matrix_aux;
int kernel_x[3][3];
int kernel_y[3][3];
bool** pixels_fundo;
bool** pixels_pretos;
bool** pixels_vermelhos;
bool** pixels_verdes;
bool** pixels_azuis;
int** matriz_de_pixels_visitados;
ponto* lista_busca_aux;
int contador_lista_aux;
bool ruidos_extremos;
bool detectando_fundo;
int media_extra_para_ruidos_extremos;
bool precisa_desalocar_memoria;
int numero_imagem;
estatistica* estatisticas;
int matriz_global_de_classificacao[ESTRUTURAS_DE_ANALISE][ESTRUTURAS_DE_ANALISE];

//protótipos de funções
int main(int argc,char** argv);
void init();
void reshape(int w,int h);
void display(void);
void keyboard(unsigned char key,int x,int y);
void inicializa_imagem_de_trabalho();
void armazena_imagem_em_matriz_auxiliar();
void carrega_imagem_de_matriz_auxiliar();
void mediana_simples();
int mediana_simples_aux(int x,int y);
void mediana_bloco();
int mediana_bloco_aux(int x,int y);
void media_simples();
int media_simples_aux(int x,int y);
void media_bloco();
int media_bloco_aux(int x,int y);
void calcula_limiar();
void calcular_histograma_da_imagem_modificada();
void imprimir_pixels_fundo();
void imprimir_pixels_pretos();
void imprimir_pixels_vermelhos();
void armazenar_pixels_vermelhos();
void imprimir_pixels_verdes();
void imprimir_pixels_azuis();
void imprimir_todos_os_pixels();
void salvar_pixels();
void sobel();
void ordena_vetor(int vetor[]);
void adiciona_ponto_em_lista_aux(ponto ponto_aux);
ponto remove_ponto_em_lista_aux();
bool verifica_se_ponto_e_valido(int x,int y);
void remover_ruidos();
void remover_ruidos_para_determinar_fundo();
void detectar_pinos();
void detectar_dentina();
void detectar_dentina_aux();
void reduzir_ruidos_ao_maximo();
void segmenta_fundo_de_verde_para_preto(int x_inicial,int y_inicial);
void segmenta_fundo_de_vermelho_para_preto(int x_inicial,int y_inicial);
void segmenta_fundo_de_preto_para_branco(int x_inicial,int y_inicial);
void pinta_de_vermelho_os_pixels_com_limiar_baixo();
void inicializa_estruturas();
void desaloca_memoria();
void executa_algoritmo();
void executa_trabalho();
void processa_imagem();
void carrega_imagem_ground_truth();
void calcula_estatistica();

using namespace std;

/**
 * Calcula dados estatísticos da segmentação efetuada.
 */
void calcula_estatistica()
{
    unsigned char r,g,b;
    int x,y;
    int i;

    estatisticas[numero_imagem].quantidade_total_de_pixels = NewImage.SizeX() * NewImage.SizeY();

    //calculando matriz de classificação
    for(x=0; x<Image.SizeX(); x++)
    {
        for(y=0; y<Image.SizeY(); y++)
        {
            Image.ReadPixel(x,y,r,g,b);

            //se o pixel foi atribuído ao grupo dentina
            if(pixels_verdes[x][y] == true)
            {
                //se o pixel correto pertence grupo dentina
                if(r == 0 && g == 255 && b == 0)
                {
                    estatisticas[numero_imagem].matriz_de_classificacao[DENTINA][DENTINA] =
                        estatisticas[numero_imagem].matriz_de_classificacao[DENTINA][DENTINA] + 1;
                }
                //se o pixel correto pertence grupo canal
                if(r == 255 && g == 0 && b == 0)
                {
                    estatisticas[numero_imagem].matriz_de_classificacao[DENTINA][CANAL] =
                        estatisticas[numero_imagem].matriz_de_classificacao[DENTINA][CANAL] + 1;
                }
                //se o pixel correto pertence grupo pinos
                if(r == 0 && g == 0 && b == 255)
                {
                    estatisticas[numero_imagem].matriz_de_classificacao[DENTINA][PINOS] =
                        estatisticas[numero_imagem].matriz_de_classificacao[DENTINA][PINOS] + 1;
                }
                //se o pixel correto pertence grupo fundo
                if(r == 0 && g == 0 && b == 0)
                {
                    estatisticas[numero_imagem].matriz_de_classificacao[DENTINA][FUNDO] =
                        estatisticas[numero_imagem].matriz_de_classificacao[DENTINA][FUNDO] + 1;
                }
            }
            //se o pixel foi atribuído ao grupo canal
            if(pixels_vermelhos[x][y] == true)
            {
                //se o pixel correto pertence grupo dentina
                if(r == 0 && g == 255 && b == 0)
                {
                    estatisticas[numero_imagem].matriz_de_classificacao[CANAL][DENTINA] =
                        estatisticas[numero_imagem].matriz_de_classificacao[CANAL][DENTINA] + 1;
                }
                //se o pixel correto pertence grupo canal
                if(r == 255 && g == 0 && b == 0)
                {
                    estatisticas[numero_imagem].matriz_de_classificacao[CANAL][CANAL] =
                        estatisticas[numero_imagem].matriz_de_classificacao[CANAL][CANAL] + 1;
                }
                //se o pixel correto pertence grupo pinos
                if(r == 0 && g == 0 && b == 255)
                {
                    estatisticas[numero_imagem].matriz_de_classificacao[CANAL][PINOS] =
                        estatisticas[numero_imagem].matriz_de_classificacao[CANAL][PINOS] + 1;
                }
                //se o pixel correto pertence grupo fundo
                if(r == 0 && g == 0 && b == 0)
                {
                    estatisticas[numero_imagem].matriz_de_classificacao[CANAL][FUNDO] =
                        estatisticas[numero_imagem].matriz_de_classificacao[CANAL][FUNDO] + 1;
                }
            }
            //se o pixel foi atribuído ao grupo pinos
            if(pixels_azuis[x][y] == true)
            {
                //se o pixel correto pertence grupo dentina
                if(r == 0 && g == 255 && b == 0)
                {
                    estatisticas[numero_imagem].matriz_de_classificacao[PINOS][DENTINA] =
                        estatisticas[numero_imagem].matriz_de_classificacao[PINOS][DENTINA] + 1;
                }
                //se o pixel correto pertence grupo canal
                if(r == 255 && g == 0 && b == 0)
                {
                    estatisticas[numero_imagem].matriz_de_classificacao[PINOS][CANAL] =
                        estatisticas[numero_imagem].matriz_de_classificacao[PINOS][CANAL] + 1;
                }
                //se o pixel correto pertence grupo pinos
                if(r == 0 && g == 0 && b == 255)
                {
                    estatisticas[numero_imagem].matriz_de_classificacao[PINOS][PINOS] =
                        estatisticas[numero_imagem].matriz_de_classificacao[PINOS][PINOS] + 1;
                }
                //se o pixel correto pertence grupo fundo
                if(r == 0 && g == 0 && b == 0)
                {
                    estatisticas[numero_imagem].matriz_de_classificacao[PINOS][FUNDO] =
                        estatisticas[numero_imagem].matriz_de_classificacao[PINOS][FUNDO] + 1;
                }
            }
            //se o pixel foi atribuído ao grupo fundo
            if(pixels_pretos[x][y] == true)
            {
                //se o pixel correto pertence grupo dentina
                if(r == 0 && g == 255 && b == 0)
                {
                    estatisticas[numero_imagem].matriz_de_classificacao[FUNDO][DENTINA] =
                        estatisticas[numero_imagem].matriz_de_classificacao[FUNDO][DENTINA] + 1;
                }
                //se o pixel correto pertence grupo canal
                if(r == 255 && g == 0 && b == 0)
                {
                    estatisticas[numero_imagem].matriz_de_classificacao[FUNDO][CANAL] =
                        estatisticas[numero_imagem].matriz_de_classificacao[FUNDO][CANAL] + 1;
                }
                //se o pixel correto pertence grupo pinos
                if(r == 0 && g == 0 && b == 255)
                {
                    estatisticas[numero_imagem].matriz_de_classificacao[FUNDO][PINOS] =
                        estatisticas[numero_imagem].matriz_de_classificacao[FUNDO][PINOS] + 1;
                }
                //se o pixel correto pertence grupo fundo
                if(r == 0 && g == 0 && b == 0)
                {
                    estatisticas[numero_imagem].matriz_de_classificacao[FUNDO][FUNDO] =
                        estatisticas[numero_imagem].matriz_de_classificacao[FUNDO][FUNDO] + 1;
                }
            }
        }
    }

    printf("\tDENTINA\tCANAL\tPINOS\tFUNDO");
	printf("\n");
    printf("DENTINA");
    for(i=0; i<ESTRUTURAS_DE_ANALISE; i++)
    {
        printf("\t%d",estatisticas[numero_imagem].matriz_de_classificacao[DENTINA][i]);
    }
	printf("\n");
    printf("CANAL");
    for(i=0; i<ESTRUTURAS_DE_ANALISE; i++)
    {
        printf("\t%d",estatisticas[numero_imagem].matriz_de_classificacao[CANAL][i]);
    }
	printf("\n");
    printf("PINOS");
    for(i=0; i<ESTRUTURAS_DE_ANALISE; i++)
    {
        printf("\t%d",estatisticas[numero_imagem].matriz_de_classificacao[PINOS][i]);
    }
	printf("\n");
    printf("FUNDO");
    for(i=0; i<ESTRUTURAS_DE_ANALISE; i++)
    {
        printf("\t%d",estatisticas[numero_imagem].matriz_de_classificacao[FUNDO][i]);
    }
	printf("\n");
}

/**
 * Carrega a imagem ground truth em Image.
 */
void carrega_imagem_ground_truth()
{
    int r;

    string nome = ".png";
    string path = "imagens/segmentadas/";

    stringstream sstm;
    sstm << numero_imagem << nome;
    nome = sstm.str();

    nome =  path + nome;
    r = Image.Load(nome.c_str());

    if (!r) exit(1);
}

/**
 * Inicializa estruturas e executa o algoritmo para a imagem "numero_imagem".
 */
void processa_imagem()
{
    inicializa_estruturas();
    executa_algoritmo();
    carrega_imagem_ground_truth();
    calcula_estatistica();
}

/**
 * Desaloca estruturas alocadas para o algoritmo.
 */
void desaloca_memoria()
{
    free(r_matrix_aux);
    free(g_matrix_aux);
    free(b_matrix_aux);
    free(matrix_aux);
    free(pixels_azuis);
    free(pixels_verdes);
    free(pixels_vermelhos);
    free(pixels_pretos);
    free(pixels_fundo);
    free(matriz_de_pixels_visitados);
    free(lista_busca_aux);
}

/**
 * Inicializa estruturas usadas para executar o algoritmo.
 */
void inicializa_estruturas()
{
    if(precisa_desalocar_memoria == true)
    {
        desaloca_memoria();
    }
    else
    {
        precisa_desalocar_memoria = true;
    }

    int r,i,x,y;

    detectando_fundo = false;

    media_extra_para_ruidos_extremos = 12;
    ruidos_extremos = false;

    if(numero_imagem >= 5)
    {
        ruidos_extremos = true;
    }

    string nome = ".png";
    string path = "imagens/originais/";
    string path_ground_truth = "imagens/segmentadas/";

    stringstream sstm;
    sstm << numero_imagem << nome;
    nome = sstm.str();

    nome =  path + nome;
    r = Image.Load(nome.c_str());

    if (!r) exit(1);

    NewImage.SetSize(Image.SizeX(), Image.SizeY(), Image.Channels());

    //limiares iniciais
    limiar_inferior = 0;
    limiar_superior = 70;

    //aloca matrizes auxiliares
    r_matrix_aux = (unsigned char**) malloc(Image.SizeX() * sizeof (unsigned char*));
    for (i = 0; i < Image.SizeX(); i++)
    {
        r_matrix_aux[i] = (unsigned char*) malloc(Image.SizeY() * sizeof (unsigned char));
    }
    g_matrix_aux = (unsigned char**) malloc(Image.SizeX() * sizeof (unsigned char*));
    for (i = 0; i < Image.SizeX(); i++)
    {
        g_matrix_aux[i] = (unsigned char*) malloc(Image.SizeY() * sizeof (unsigned char));
    }
    b_matrix_aux = (unsigned char**) malloc(Image.SizeX() * sizeof (unsigned char*));
    for (i = 0; i < Image.SizeX(); i++)
    {
        b_matrix_aux[i] = (unsigned char*) malloc(Image.SizeY() * sizeof (unsigned char));
    }
    matrix_aux = (unsigned char**) malloc(Image.SizeX() * sizeof (unsigned char*));
    for (i = 0; i < Image.SizeX(); i++)
    {
        matrix_aux[i] = (unsigned char*) malloc(Image.SizeY() * sizeof (unsigned char));
    }
    pixels_azuis = (bool**) malloc(Image.SizeX() * sizeof (bool*));
    for (i = 0; i < Image.SizeX(); i++)
    {
        pixels_azuis[i] = (bool*) malloc(Image.SizeY() * sizeof (bool));
    }
    pixels_verdes = (bool**) malloc(Image.SizeX() * sizeof (bool*));
    for (i = 0; i < Image.SizeX(); i++)
    {
        pixels_verdes[i] = (bool*) malloc(Image.SizeY() * sizeof (bool));
    }
    pixels_vermelhos = (bool**) malloc(Image.SizeX() * sizeof (bool*));
    for (i = 0; i < Image.SizeX(); i++)
    {
        pixels_vermelhos[i] = (bool*) malloc(Image.SizeY() * sizeof (bool));
    }
    pixels_pretos = (bool**) malloc(Image.SizeX() * sizeof (bool*));
    for (i = 0; i < Image.SizeX(); i++)
    {
        pixels_pretos[i] = (bool*) malloc(Image.SizeY() * sizeof (bool));
    }
    pixels_fundo = (bool**) malloc(Image.SizeX() * sizeof (bool*));
    for (i = 0; i < Image.SizeX(); i++)
    {
        pixels_fundo[i] = (bool*) malloc(Image.SizeY() * sizeof (bool));
    }
    matriz_de_pixels_visitados = (int**) malloc(Image.SizeX() * sizeof (int*));
    for (i = 0; i < Image.SizeX(); i++)
    {
        matriz_de_pixels_visitados[i] = (int*) malloc(Image.SizeY() * sizeof (int));
    }

    //inicializa lista de auxílio para a execução de algoritmos de segmentação
    int size_aux = Image.SizeX() * Image.SizeY();
    lista_busca_aux = (ponto*) malloc(size_aux * sizeof (ponto));
    contador_lista_aux = 0;

    //inicializa kernel para cálculo do filtro sobel
    kernel_x[0][0] = -1;
    kernel_x[0][1] = 0;
    kernel_x[0][2] = 1;
    kernel_x[1][0] = -2;
    kernel_x[1][1] = 0;
    kernel_x[1][2] = 2;
    kernel_x[2][0] = -1;
    kernel_x[2][1] = 0;
    kernel_x[2][2] = 1;
    kernel_y[0][0] = 1;
    kernel_y[0][1] = 2;
    kernel_y[0][2] = 1;
    kernel_y[1][0] = 0;
    kernel_y[1][1] = 0;
    kernel_y[1][2] = 0;
    kernel_y[2][0] = -1;
    kernel_y[2][1] = -2;
    kernel_y[2][2] = -1;

    //zera valores de matrizes
    for(x=0; x<Image.SizeX(); x++)
    {
        for(y=0; y<Image.SizeY(); y++)
        {
            pixels_azuis[x][y] = false;
            pixels_vermelhos[x][y] = false;
            pixels_verdes[x][y] = false;
            pixels_pretos[x][y] = false;
            pixels_fundo[x][y] = false;
            matriz_de_pixels_visitados[x][y] = 0;
        }
    }
}

/**
 * Executa o algoritmo de segmentação para todas as imagens da pasta.
 */
void executa_trabalho()
{
    for(numero_imagem=0; numero_imagem<IMAGENS_NA_PASTA; numero_imagem++)
    {
        processa_imagem();
        carrega_imagem_ground_truth();
        calcula_estatistica();
    }
}

/**
 * Executa o algoritmo de segmentação da imagem.
 */
void executa_algoritmo()
{
    unsigned char r,g,b;
    int x,y;

    //reduz ao maximo os ruidos da imagem
    detectando_fundo = true;
    reduzir_ruidos_ao_maximo();
    detectando_fundo = false;

    //define como fundo todos os pixels pretos
    for(x=0; x<Image.SizeX(); x++)
    {
        for(y=0; y<Image.SizeY(); y++)
        {
            NewImage.ReadPixel(x,y,r,g,b);

            if(r==0 && g==0 && b==0)
            {
                pixels_fundo[x][y] = true;
            }
            else
            {
                pixels_azuis[x][y] = false;
                pixels_verdes[x][y] = false;
                pixels_vermelhos[x][y] = false;
                pixels_pretos[x][y] = false;
            }
        }
    }

    //inicializa_imagem_de_trabalho();
    detectar_pinos();

    inicializa_imagem_de_trabalho();
    remover_ruidos();

    //armazena quais pixels devem ser pintados de verde
    for(x=0; x<Image.SizeX(); x++)
    {
        for(y=0; y<Image.SizeY(); y++)
        {
            NewImage.ReadPixel(x,y,r,g,b);

            if(r==0 && g==0 && b==0)
            {
                pixels_pretos[x][y] = true;
            }
            else
            {
                if(pixels_azuis[x][y] == false)
                {
                    pixels_verdes[x][y] = true;
                }
            }
        }
    }

    //pinta de vermelho os pixels de limiar baixo
    pinta_de_vermelho_os_pixels_com_limiar_baixo();

    //remove pixels vermelhos da região do fundo
    segmenta_fundo_de_vermelho_para_preto(100,100);

    //imprime pixels pretos em cima dos pixels vermelhos fora dos canais
    imprimir_pixels_pretos();

    //armazena os pixels vermelhos definitivos
    armazenar_pixels_vermelhos();

    //imprime pixels verdes
    imprimir_pixels_verdes();

    //segmenta fundo de preto para branco
    segmenta_fundo_de_preto_para_branco(100,100);

    //armazena pixels pretos restantes que deveriam ser canais
    for(x=0; x<Image.SizeX(); x++)
    {
        for(y=0; y<Image.SizeY(); y++)
        {
            NewImage.ReadPixel(x,y,r,g,b);

            if(r==0 && g==0 && b==0)
            {
                pixels_vermelhos[x][y] = true;
            }
        }
    }

    //detecta pinos
    detectar_pinos();

    //imprime as configurações finais dos pixels
    imprimir_pixels_pretos();
    imprimir_pixels_verdes();
    imprimir_pixels_fundo();
    imprimir_pixels_vermelhos();
    imprimir_pixels_azuis();

    armazena_imagem_em_matriz_auxiliar();
    salvar_pixels();
}

/**
 * Lê os pixels da imagem e armazena pixels vermelho.
 */
void armazenar_pixels_vermelhos()
{
    unsigned char r,g,b;
    int x,y;

    for(x=0; x<Image.SizeX(); x++)
    {
        for(y=0; y<Image.SizeY(); y++)
        {
            NewImage.ReadPixel(x,y,r,g,b);

            if(r==255 && g==0 && b==0)
            {
                pixels_vermelhos[x][y] = true;
            }
        }
    }
}

/**
 * Lê os pixels da imagem e armazena todos os pixels de acordo com suas cores.
 */
void salvar_pixels()
{
    unsigned char r,g,b;
    int x,y;

    for(x=0; x<Image.SizeX(); x++)
    {
        for(y=0; y<Image.SizeY(); y++)
        {
            pixels_fundo[x][y] = true;
            pixels_pretos[x][y] = false;
            pixels_vermelhos[x][y] = false;
            pixels_verdes[x][y] = false;
            pixels_azuis[x][y] = false;
        }
    }

    for(x=0; x<Image.SizeX(); x++)
    {
        for(y=0; y<Image.SizeY(); y++)
        {
            NewImage.ReadPixel(x,y,r,g,b);

            if(r==0 && g==0 && b==0)
            {
                pixels_pretos[x][y] = true;
            }
            if(r==255 && g==0 && b==0)
            {
                pixels_vermelhos[x][y] = true;
            }
            if(r==0 && g==255 && b==0)
            {
                pixels_verdes[x][y] = true;
            }
            if(r==0 && g==0 && b==255)
            {
                pixels_azuis[x][y] = true;
            }
        }
    }
}

/**
 * Sequência de passos que reduz ao máximo os ruídos da imagem.
 */
void reduzir_ruidos_ao_maximo()
{
    detectar_dentina();
    detectar_pinos();
    imprimir_pixels_verdes();
    imprimir_pixels_azuis();
    segmenta_fundo_de_verde_para_preto(100,100);
    segmenta_fundo_de_verde_para_preto(Image.SizeX()/2,Image.SizeY()/2);
}

/**
 * Pinta de vermelho os pixels com limiar baixo.
 */
void pinta_de_vermelho_os_pixels_com_limiar_baixo()
{
    unsigned char r,g,b;
    int x,y;

    for(x=0; x<Image.SizeX(); x++)
    {
        for(y=0; y<Image.SizeY(); y++)
        {
            NewImage.ReadPixel(x,y,r,g,b);

            if(r <= 10 && g <= 10 && b <= 10)
            {
                NewImage.DrawPixel(x,y,255,0,0);
            }
        }
    }
}

/**
 * Verifica se um ponto x,y é válido.
 */
bool verifica_se_ponto_e_valido(int x, int y)
{
    if(x >= 0 && x < Image.SizeX())
    {
        if(y >= 0 && y < Image.SizeY())
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

/**
 * Remove o primeiro elemento da lista auxiliar.
 */
ponto remove_ponto_em_lista_aux()
{
    int i;

    ponto ponto_aux = lista_busca_aux[0];

    for(i=0; i<contador_lista_aux-1; i++)
    {
        lista_busca_aux[i] = lista_busca_aux[i+1];
    }

    contador_lista_aux--;

    return ponto_aux;
}

/**
 * Adiciona um ponto na lista auxiliar.
 */
void adiciona_ponto_em_lista_aux(ponto ponto_aux)
{
    lista_busca_aux[contador_lista_aux] = ponto_aux;

    contador_lista_aux++;
}

/**
 * Retorna um vetor com as coordenadas adjacentes de um ponto.
 */
void vetor_de_coordenadas(int x, int y, ponto vetor[9])
{
    vetor[0].x = x-1;
    vetor[0].y = y-1;

    vetor[1].x = x-1;
    vetor[1].y = y;

    vetor[2].x = x-1;
    vetor[2].y = y+1;

    vetor[3].x = x;
    vetor[3].y = y-1;

    vetor[4].x = x;
    vetor[4].y = y;

    vetor[5].x = x;
    vetor[5].y = y+1;

    vetor[6].x = x+1;
    vetor[6].y = y-1;

    vetor[7].x = x+1;
    vetor[7].y = y;

    vetor[8].x = x+1;
    vetor[8].y = y+1;
}

/**
 * Identifica pixels pretos do fundo e os pinta de branco.
 */
void segmenta_fundo_de_preto_para_branco(int x_inicial, int y_inicial)
{
    int contador = 0;
    unsigned char r,g,b;
    int x,y;

    contador_lista_aux = 0;

    for(x=0; x<Image.SizeX(); x++)
    {
        for(y=0; y<Image.SizeY(); y++)
        {
            matriz_de_pixels_visitados[x][y] = 0;
            pixels_pretos[x][y] = false;
        }
    }

    //cria ponto inicial
    ponto ponto_inicial;
    ponto_inicial.x = x_inicial;
    ponto_inicial.y = y_inicial;

    adiciona_ponto_em_lista_aux(ponto_inicial);

    matriz_de_pixels_visitados[ponto_inicial.x][ponto_inicial.y] = 1;

    while(contador_lista_aux > 0)
    {
        //if(contador % 100000 == 0)
        //{
        //	printf("CONTADOR = %d --- LISTA = %d]\n", contador, contador_lista_aux);
        //}

        contador++;
        int k;

        ponto ponto_atual = remove_ponto_em_lista_aux();

        int x_atual = ponto_atual.x;
        int y_atual = ponto_atual.y;

        ponto vetor[9];

        vetor_de_coordenadas(x_atual,y_atual,vetor);

        int resultado = matriz_de_pixels_visitados[x_atual][y_atual];

        //pixels_pretos[x_atual][y_atual] = false;
        NewImage.DrawPixel(x_atual,y_atual,255,255,255);

        for(k=0; k<9; k++)
        {
            int x_novo = vetor[k].x;
            int y_novo = vetor[k].y;

            if(verifica_se_ponto_e_valido(x_novo, y_novo) == true)
            {
                resultado = matriz_de_pixels_visitados[x_novo][y_novo];

                if(resultado == 0)
                {
                    //marca pixel adjacente
                    matriz_de_pixels_visitados[x_novo][y_novo] = 1;

                    NewImage.ReadPixel(x_novo,y_novo,r,g,b);

                    //se o pixel é preto
                    if(r == 0 && g == 0 && b == 0)
                    {
                        ponto ponto_novo;
                        ponto_novo.x = x_novo;
                        ponto_novo.y = y_novo;
                        adiciona_ponto_em_lista_aux(ponto_novo);
                    }
                }
            }
        }
    }

    //printf(" [CONTADOR = %d --- LISTA = %d] ", contador, contador_lista_aux);
}

/**
 * Identifica pixels vermelhos do fundo e os pinta de preto.
 */
void segmenta_fundo_de_vermelho_para_preto(int x_inicial, int y_inicial)
{
    int contador = 0;
    unsigned char r,g,b;
    int x,y;

    contador_lista_aux = 0;

    for(x=0; x<Image.SizeX(); x++)
    {
        for(y=0; y<Image.SizeY(); y++)
        {
            matriz_de_pixels_visitados[x][y] = 0;
            pixels_pretos[x][y] = false;
        }
    }

    //cria ponto inicial
    ponto ponto_inicial;
    ponto_inicial.x = x_inicial;
    ponto_inicial.y = y_inicial;

    adiciona_ponto_em_lista_aux(ponto_inicial);

    matriz_de_pixels_visitados[ponto_inicial.x][ponto_inicial.y] = 1;

    while(contador_lista_aux > 0)
    {
        //if(contador % 100000 == 0)
        //{
        //	printf("CONTADOR = %d --- LISTA = %d]\n", contador, contador_lista_aux);
        //}

        contador++;
        int k;

        ponto ponto_atual = remove_ponto_em_lista_aux();

        int x_atual = ponto_atual.x;
        int y_atual = ponto_atual.y;

        ponto vetor[9];

        vetor_de_coordenadas(x_atual,y_atual,vetor);

        int resultado = matriz_de_pixels_visitados[x_atual][y_atual];

        pixels_pretos[x_atual][y_atual] = true;
        pixels_vermelhos[x_atual][y_atual] = false;

        for(k=0; k<9; k++)
        {
            int x_novo = vetor[k].x;
            int y_novo = vetor[k].y;

            if(verifica_se_ponto_e_valido(x_novo, y_novo) == true)
            {
                resultado = matriz_de_pixels_visitados[x_novo][y_novo];

                if(resultado == 0)
                {
                    //marca pixel adjacente
                    matriz_de_pixels_visitados[x_novo][y_novo] = 1;

                    NewImage.ReadPixel(x_novo,y_novo,r,g,b);

                    //se o pixel é verde
                    if( (r == 255 && g == 0 && b == 0) || (r == 0 && g == 0 && b == 0))
                    {
                        ponto ponto_novo;
                        ponto_novo.x = x_novo;
                        ponto_novo.y = y_novo;
                        adiciona_ponto_em_lista_aux(ponto_novo);
                    }
                }
            }
        }
    }

    //printf(" [CONTADOR = %d --- LISTA = %d] ", contador, contador_lista_aux);
}

/**
 * Identifica pixels verdes do fundo e os pinta de preto.
 */
void segmenta_fundo_de_verde_para_preto(int x_inicial, int y_inicial)
{
    int contador = 0;
    unsigned char r,g,b;
    int x,y;

    contador_lista_aux = 0;

    for(x=0; x<Image.SizeX(); x++)
    {
        for(y=0; y<Image.SizeY(); y++)
        {
            matriz_de_pixels_visitados[x][y] = 0;
        }
    }

    //cria ponto inicial
    ponto ponto_inicial;
    ponto_inicial.x = x_inicial;
    ponto_inicial.y = y_inicial;

    adiciona_ponto_em_lista_aux(ponto_inicial);

    matriz_de_pixels_visitados[ponto_inicial.x][ponto_inicial.y] = 1;

    while(contador_lista_aux > 0)
    {
        contador++;
        int k;

        ponto ponto_atual = remove_ponto_em_lista_aux();

        int x_atual = ponto_atual.x;
        int y_atual = ponto_atual.y;

        ponto vetor[9];

        vetor_de_coordenadas(x_atual,y_atual,vetor);

        int resultado = matriz_de_pixels_visitados[x_atual][y_atual];

        pixels_pretos[x_atual][y_atual] = true;
        pixels_verdes[x_atual][y_atual] = false;
        NewImage.DrawPixel(x_atual,y_atual,0,0,0);
        pixels_fundo[x_atual][y_atual] = true;

        for(k=0; k<9; k++)
        {
            int x_novo = vetor[k].x;
            int y_novo = vetor[k].y;

            if(verifica_se_ponto_e_valido(x_novo, y_novo) == true)
            {
                resultado = matriz_de_pixels_visitados[x_novo][y_novo];

                //se este pixel ainda não foi visitado
                if(resultado == 0)
                {
                    //marca pixel adjacente
                    matriz_de_pixels_visitados[x_novo][y_novo] = 1;

                    NewImage.ReadPixel(x_novo,y_novo,r,g,b);

                    //se o pixel é verde
                    if( (r == 0 && g == 255 && b == 0) || (r == 0 && g == 0 && b == 0))
                    {
                        ponto ponto_novo;
                        ponto_novo.x = x_novo;
                        ponto_novo.y = y_novo;
                        adiciona_ponto_em_lista_aux(ponto_novo);
                    }
                }
            }
        }
    }

    //printf(" [CONTADOR = %d --- LISTA = %d] ", contador, contador_lista_aux);
}

/**
 * Desenha todas as cores de pixels.
 */
void imprimir_todos_os_pixels()
{
    //unsigned char r,g,b;
    int x,y;

    for(x=0; x<Image.SizeX(); x++)
    {
        for(y=0; y<Image.SizeY(); y++)
        {
            if(pixels_azuis[x][y] == true)
            {
                NewImage.DrawPixel(x,y,0,0,255);
            }
            else if(pixels_verdes[x][y] == true)
            {
                NewImage.DrawPixel(x,y,0,255,0);
            }
            else if(pixels_vermelhos[x][y] == true)
            {
                NewImage.DrawPixel(x,y,255,0,0);
            }
            else if(pixels_pretos[x][y] == true)
            {
                NewImage.DrawPixel(x,y,0,0,0);
            }
        }
    }
}

/**
 * Desenha pixels azuis.
 */
void imprimir_pixels_azuis()
{
    //unsigned char r,g,b;
    int x,y;

    for(x=0; x<Image.SizeX(); x++)
    {
        for(y=0; y<Image.SizeY(); y++)
        {

            if(pixels_azuis[x][y] == true)
            {
                NewImage.DrawPixel(x,y,0,0,255);
            }
        }
    }
}

/**
 * Desenha pixels do fundo.
 */
void imprimir_pixels_fundo()
{
    //unsigned char r,g,b;
    int x,y;

    for(x=0; x<Image.SizeX(); x++)
    {
        for(y=0; y<Image.SizeY(); y++)
        {

            if(pixels_fundo[x][y] == true)
            {
                NewImage.DrawPixel(x,y,0,0,0);
            }
        }
    }
}

/**
 * Desenha pixels verdes.
 */
void imprimir_pixels_verdes()
{
    //unsigned char r,g,b;
    int x,y;

    for(x=0; x<Image.SizeX(); x++)
    {
        for(y=0; y<Image.SizeY(); y++)
        {

            if(pixels_verdes[x][y] == true)
            {
                NewImage.DrawPixel(x,y,0,255,0);
            }
        }
    }
}

/**
 * Desenha pixels vermelhos.
 */
void imprimir_pixels_vermelhos()
{
    //unsigned char r,g,b;
    int x,y;

    for(x=0; x<Image.SizeX(); x++)
    {
        for(y=0; y<Image.SizeY(); y++)
        {

            if(pixels_vermelhos[x][y] == true)
            {
                NewImage.DrawPixel(x,y,255,0,0);
            }
        }
    }
}

/**
 * Desenha pixels pretos.
 */
void imprimir_pixels_pretos()
{
    //unsigned char r,g,b;
    int x,y;

    for(x=0; x<Image.SizeX(); x++)
    {
        for(y=0; y<Image.SizeY(); y++)
        {

            if(pixels_pretos[x][y] == true)
            {
                NewImage.DrawPixel(x,y,0,0,0);
                //printf("[%d ; %d]\n",x,y);
            }
        }
    }
}

/**
 * Remove ruídos objetivando o fundo da imagem.
 */
void remover_ruidos_para_determinar_fundo()
{
    int i;

    for(i=0; i<1; i++)
    {
        media_bloco();
    }

    for(i=0; i<26; i++)
    {
        media_simples();
    }

    for(i=0; i<1; i++)
    {
        mediana_bloco();
    }

    if(ruidos_extremos == true)
    {
        for(i=0; i<8; i++)
        {
            media_simples();
        }

        for(i=0; i<media_extra_para_ruidos_extremos-2; i++)
        {
            media_simples();
        }
    }
    else
    {
        for(i=0; i<8; i++)
        {
            media_simples();
        }
    }
}

/**
 * Remove os ruídos da imagem.
 */
void remover_ruidos()
{
    int i;

    for(i=0; i<1; i++)
    {
        media_bloco();
    }

    for(i=0; i<26; i++)
    {
        media_simples();
    }

    for(i=0; i<1; i++)
    {
        mediana_bloco();
    }

    if(ruidos_extremos == true)
    {
        for(i=0; i<8; i++)
        {
            media_simples();
        }

        for(i=0; i<media_extra_para_ruidos_extremos; i++)
        {
            media_simples();
        }
    }
    else
    {
        for(i=0; i<2; i++)
        {
            media_simples();
        }
    }
}

/**
 * Aplica o filtro de sobel na imagem.
 */
void sobel()
{
    //cout << "Iniciou Sobel...";

    int x,y;

    for(x=1; x<Image.SizeX()-1; x++)
    {
        for(y=1; y<Image.SizeY()-1; y++)
        {
            int magnitude = 0;
            int valor_x = 0;
            int valor_y = 0;

            valor_x += kernel_x[0][0] * NewImage.GetPointIntensity(x-1,y-1);
            valor_x += kernel_x[0][1] * NewImage.GetPointIntensity(x,y-1);
            valor_x += kernel_x[0][2] * NewImage.GetPointIntensity(x+1,y-1);
            valor_x += kernel_x[1][0] * NewImage.GetPointIntensity(x-1,y);
            valor_x += kernel_x[1][1] * NewImage.GetPointIntensity(x,y);
            valor_x += kernel_x[1][2] * NewImage.GetPointIntensity(x+1,y);
            valor_x += kernel_x[2][0] * NewImage.GetPointIntensity(x-1,y+1);
            valor_x += kernel_x[2][1] * NewImage.GetPointIntensity(x,y+1);
            valor_x += kernel_x[2][2] * NewImage.GetPointIntensity(x+1,y+1);
            valor_y += kernel_y[0][0] * NewImage.GetPointIntensity(x-1,y-1);
            valor_y += kernel_y[0][1] * NewImage.GetPointIntensity(x,y-1);
            valor_y += kernel_y[0][2] * NewImage.GetPointIntensity(x+1,y-1);
            valor_y += kernel_y[1][0] * NewImage.GetPointIntensity(x-1,y);
            valor_y += kernel_y[1][1] * NewImage.GetPointIntensity(x,y);
            valor_y += kernel_y[1][2] * NewImage.GetPointIntensity(x+1,y);
            valor_y += kernel_y[2][0] * NewImage.GetPointIntensity(x-1,y+1);
            valor_y += kernel_y[2][1] * NewImage.GetPointIntensity(x,y+1);
            valor_y += kernel_y[2][2] * NewImage.GetPointIntensity(x+1,y+1);

            valor_x = valor_x * valor_x;
            valor_y = valor_y * valor_y;
            magnitude = valor_x + valor_y;
            magnitude = (int) sqrt(magnitude);

            //armazena a magnitude dos pixels em uma matriz auxiliar
            matrix_aux[x][y] = magnitude;
        }
    }

    //insere valores calculados na matriz de saída
    for(x=1; x<Image.SizeX()-1; x++)
    {
        for(y=1; y<Image.SizeY()-1; y++)
        {
            unsigned char valor = matrix_aux[x][y];

            NewImage.DrawPixel(x,y,valor,valor,valor);
        }
    }

    //cout << "Concluiu Sobel.";
}

/**
 * Armazena o rgb da imagem em uma estrutura auxiliar.
 */
void armazena_imagem_em_matriz_auxiliar()
{
    unsigned char r,g,b;
    int x,y;

    for(x=0; x<Image.SizeX(); x++)
    {
        for(y=0; y<Image.SizeY(); y++)
        {
            NewImage.ReadPixel(x,y,r,g,b);

            r_matrix_aux[x][y] = r;
            g_matrix_aux[x][y] = g;
            b_matrix_aux[x][y] = b;
        }
    }

    printf("MATRIZ SALVA!\n");
}

/**
 * Carrega o rgb da imagem da estrutura auxiliar.
 */
void carrega_imagem_de_matriz_auxiliar()
{
    unsigned char r,g,b;
    int x,y;

    for(x=0; x<Image.SizeX(); x++)
    {
        for(y=0; y<Image.SizeY(); y++)
        {
            r = r_matrix_aux[x][y];
            g = g_matrix_aux[x][y];
            b = b_matrix_aux[x][y];

            NewImage.DrawPixel(x,y,r,g,b);
        }
    }

    printf("MATRIZ CARREGADA!\n");
}

/**
 * Copia os pixels da imagem original para a imagem de trabalho.
 */
void inicializa_imagem_de_trabalho()
{
    //cout << "Inicializando imagem..." << endl;

    unsigned char r,g,b;
    int x,y;

    for(x=0; x<Image.SizeX(); x++)
    {
        for(y=0; y<Image.SizeY(); y++)
        {
            Image.ReadPixel(x,y,r,g,b);

            NewImage.DrawPixel(x,y,r,g,b);
        }
    }

    //cout << "Imagem Inicializada" << endl;
}

/**
 * Este método auxiliar ajuda a calcular a mediana de um pixel.
 */
int mediana_simples_aux(int x, int y)
{
    int vetor[9];

    vetor[0] = NewImage.GetPointIntensity(x-1,y-1);
    vetor[1] = NewImage.GetPointIntensity(x,y-1);
    vetor[2] = NewImage.GetPointIntensity(x+1,y-1);

    vetor[3] = NewImage.GetPointIntensity(x-1,y);
    vetor[4] = NewImage.GetPointIntensity(x,y);
    vetor[5] = NewImage.GetPointIntensity(x+1,y);

    vetor[6] = NewImage.GetPointIntensity(x-1,y+1);
    vetor[7] = NewImage.GetPointIntensity(x,y+1);
    vetor[8] = NewImage.GetPointIntensity(x+1,y+1);

    ordena_vetor(vetor);

    return vetor[4];
}

/**
 * Este método calcula a mediana de um pixel.
 */
void mediana_simples()
{
    //cout << "Iniciou MEDIANA simples..." << endl;

    int x;
    int y;
    for(x = 1; x < Image.SizeX() - 1; x++)
    {
        for(y = 1; y < Image.SizeY() - 1; y++)
        {
            int valor = mediana_simples_aux(x, y);
            NewImage.DrawPixel(x, y, valor, valor, valor);
        }
    }

    //cout << "Concluiu MEDIANA simples." << endl;
}

/**
 * Este método auxiliar ajuda a calcular a mediana de um pixel.
 */
int mediana_bloco_aux(int x, int y)
{
    int vetor[9];

    vetor[0] = NewImage.GetPointIntensity(x-1,y-1);
    vetor[1] = NewImage.GetPointIntensity(x,y-1);
    vetor[2] = NewImage.GetPointIntensity(x+1,y-1);

    vetor[3] = NewImage.GetPointIntensity(x-1,y);
    vetor[4] = NewImage.GetPointIntensity(x,y);
    vetor[5] = NewImage.GetPointIntensity(x+1,y);

    vetor[6] = NewImage.GetPointIntensity(x-1,y+1);
    vetor[7] = NewImage.GetPointIntensity(x,y+1);
    vetor[8] = NewImage.GetPointIntensity(x+1,y+1);

    ordena_vetor(vetor);

    return vetor[4];
}

/**
 * Este método calcula a mediana de um pixel.
 */
void mediana_bloco()
{
    //cout << "Iniciou MEDIANA em bloco..." << endl;

    int x,y;
    for(x=1; x<Image.SizeX()-1; x += 3)
    {
        for(y=1; y<Image.SizeY()-1; y += 3)
        {
            int valor = mediana_simples_aux(x, y);

            NewImage.DrawPixel(x-1,y-1,valor,valor,valor);
            NewImage.DrawPixel(x,y-1,valor,valor,valor);
            NewImage.DrawPixel(x+1,y-1,valor,valor,valor);

            NewImage.DrawPixel(x-1,y,valor,valor,valor);
            NewImage.DrawPixel(x,y,valor,valor,valor);
            NewImage.DrawPixel(x+1,y,valor,valor,valor);

            NewImage.DrawPixel(x-1,y+1,valor,valor,valor);
            NewImage.DrawPixel(x,y+1,valor,valor,valor);
            NewImage.DrawPixel(x+1,y+1,valor,valor,valor);
        }
    }

    //cout << "Concluiu MEDIANA em bloco." << endl;
}

/**
 * Este método auxiliar ajuda a calcular a média de um pixel.
 */
int media_bloco_aux(int x, int y)
{
    int acumula = 0;

    int vetor[9];

    vetor[0] = NewImage.GetPointIntensity(x-1,y-1);
    vetor[1] = NewImage.GetPointIntensity(x,y-1);
    vetor[2] = NewImage.GetPointIntensity(x+1,y-1);

    vetor[3] = NewImage.GetPointIntensity(x-1,y);
    vetor[4] = NewImage.GetPointIntensity(x,y);
    vetor[5] = NewImage.GetPointIntensity(x+1,y);

    vetor[6] = NewImage.GetPointIntensity(x-1,y+1);
    vetor[7] = NewImage.GetPointIntensity(x,y+1);
    vetor[8] = NewImage.GetPointIntensity(x+1,y+1);

    for(int i = 0; i < 9; i++)
    {
        acumula = acumula + vetor[i];
    }

    ordena_vetor(vetor);

    return (int) acumula / 9;
}

/**
 * Este método calcula a média de um pixel.
 */
void media_bloco()
{
    //cout << "Realizando MEDIA em bloco." << endl;

    int x,y;
    for(x=1; x<Image.SizeX()-1; x++)
    {
        for(y=1; y<Image.SizeY()-1; y++)
        {
            int valor = media_bloco_aux(x, y);

            NewImage.DrawPixel(x-1,y-1,valor,valor,valor);
            NewImage.DrawPixel(x,y-1,valor,valor,valor);
            NewImage.DrawPixel(x+1,y-1,valor,valor,valor);

            NewImage.DrawPixel(x-1,y,valor,valor,valor);
            NewImage.DrawPixel(x,y,valor,valor,valor);
            NewImage.DrawPixel(x+1,y,valor,valor,valor);

            NewImage.DrawPixel(x-1,y+1,valor,valor,valor);
            NewImage.DrawPixel(x,y+1,valor,valor,valor);
            NewImage.DrawPixel(x+1,y+1,valor,valor,valor);
        }
    }

    //cout << "Concluiu MEDIA em bloco." << endl;
}

/**
 * auxiliar para ajudar a calcular a média de um pixel.
 */
int media_simples_aux(int x, int y)
{
    int acumula = 0;

    int vetor[9];

    vetor[0] = NewImage.GetPointIntensity(x-1,y-1);
    vetor[1] = NewImage.GetPointIntensity(x,y-1);
    vetor[2] = NewImage.GetPointIntensity(x+1,y-1);

    vetor[3] = NewImage.GetPointIntensity(x-1,y);
    vetor[4] = NewImage.GetPointIntensity(x,y);
    vetor[5] = NewImage.GetPointIntensity(x+1,y);

    vetor[6] = NewImage.GetPointIntensity(x-1,y+1);
    vetor[7] = NewImage.GetPointIntensity(x,y+1);
    vetor[8] = NewImage.GetPointIntensity(x+1,y+1);

    for(int i = 0; i < 9; i++)
    {
        acumula = acumula + vetor[i];
    }

    ordena_vetor(vetor);

    return (int) acumula / 9;
}

/**
 *  calcula a média de um pixel.
 */
void media_simples()
{
    //cout << "Realizando MEDIA simples." << endl;

    int x,y;
    for(x=1; x<Image.SizeX()-1; x++)
    {
        for(y=1; y<Image.SizeY()-1; y++)
        {
            int valor = media_simples_aux(x, y);

            NewImage.DrawPixel(x, y,valor,valor,valor);
        }
    }

    //cout << "Concluiu MEDIA simples." << endl;
}

/**
 * Desenha a imagem de acordo com o limiar global especificado.
 */
void calcula_limiar()
{
    //cout << "Iniciou Limiar...";

    unsigned char r,g,b;
    int x,y;
    int i;

    for(x=0; x<Image.SizeX(); x++)
    {
        for(y=0; y<Image.SizeY(); y++)
        {
            i = NewImage.GetPointIntensity(x,y);
            NewImage.ReadPixel(x,y,r,g,b);

            if (i >= limiar_inferior && i < limiar_superior)
            {
                //pinta o pixel de preto
                NewImage.DrawPixel(x, y,255,255,255);
            }
            else
            {
                //pinta o pixel de branco
                NewImage.DrawPixel(x, y, 0,0,0);
            }
        }
    }

    //cout << "Concluiu Limiar." << endl;
}

/**
 * Calcula o histograma da imagem modificada.
 */
void calcular_histograma_da_imagem_modificada()
{
    int x, y, i = 0;
    int frequencia[255];

    /** preenche o vetor auxiliar **/
    for(i = 0; i < 255; i++)
    {
        frequencia[i] = 0;
    }

    /** calculando os valores de frequencia de cada cor **/
    for(x = 0; x < Image.SizeX(); x++)
    {
        for(y = 0; y < Image.SizeY(); y++)
        {
            int cor = NewImage.GetPointIntensity(x,y);
            frequencia[cor]++;
        }
    }

    /** imprimindo os valores de frequencia de cada cor **/
    for(i = 0; i < 255; i++)
    {
        cout << i << "=" << frequencia[i] << endl;
    }
}

/**
 * Detecta a dentina.
 */
void detectar_dentina()
{
    inicializa_imagem_de_trabalho();

    detectar_dentina_aux();
}

/**
 * Método auxiliar para detectar as dentinas.
 */
void detectar_dentina_aux()
{
    unsigned char r,g,b;
    int x,y;
    int i;

    //zera matriz de pixels verdes
    for(x=0; x<Image.SizeX(); x++)
    {
        for(y=0; y<Image.SizeY(); y++)
        {
            pixels_verdes[x][y] = false;
        }
    }

    if(detectando_fundo == false)
    {
        //remove ruídos
        remover_ruidos();
    }
    else
    {
        //remove ruídos
        remover_ruidos_para_determinar_fundo();
    }

    //calcula limiar
    limiar_inferior = 0;
    limiar_superior = 7;
    calcula_limiar();

    //calcula média
    for(i=0; i<3; i++)
    {
        media_simples();
    }

    //calcula sobel
    for(i=0; i<2; i++)
    {
        sobel();
    }

    //calcula mediana
    for(i=0; i<3; i++)
    {
        media_simples();
    }

    //armazena quais pixels devem ser pintados de verde
    for(x=0; x<Image.SizeX(); x++)
    {
        for(y=0; y<Image.SizeY(); y++)
        {
            NewImage.ReadPixel(x,y,r,g,b);

            if(r==0 && g==0 && b==0)
            {
                //se o pixel não entra em conflito com os pixels azuis
                if(pixels_azuis[x][y] != true)
                {
                    pixels_verdes[x][y] = true;
                }
            }

        }
    }
}

/**
 * Detecta os pinos.
 */
void detectar_pinos()
{
    inicializa_imagem_de_trabalho();

    unsigned char r,g,b;
    int x,y;
    int i;

    //calcula limiar
    limiar_inferior = 0;
    limiar_superior = 120;
    calcula_limiar();

    //calcula mediana
    for(i=0; i<5; i++)
    {
        mediana_bloco();
    }
    for(i=0; i<5; i++)
    {
        mediana_simples();
    }

    //armazena quais pixels devem ser pintados de azul
    for(x=0; x<Image.SizeX(); x++)
    {
        for(y=0; y<Image.SizeY(); y++)
        {
            NewImage.ReadPixel(x,y,r,g,b);

            if(r==0 && g==0 && b==0)
            {
                pixels_azuis[x][y] = true;
            }
            else
            {
                pixels_pretos[x][y] = true;
            }

        }
    }
}
/**
 * Ordena um vetor.
 */
void ordena_vetor(int vetor[])
{
    int temp, i, j;
    for(i = 0; i < 9; i++)
    {
        temp = vetor[i];
        for(j = i-1; j >= 0 && temp < vetor[j]; j--)
        {
            vetor[j+1] = vetor[j];
        }
        vetor[j+1] = temp;
    }
}

/**
 * Inicializa dados do programa.
 */
void init()
{
    int i;

    precisa_desalocar_memoria = false;
    numero_imagem = 0;

    estatisticas = (estatistica*) malloc(IMAGENS_NA_PASTA * sizeof (estatistica));

    for(i=0; i<IMAGENS_NA_PASTA; i++)
    {
        estatisticas[i].quantidade_total_de_pixels = 0;
        estatisticas[i].quantidade_de_pixels_vermelhos = 0;
        estatisticas[i].quantidade_de_pixels_verdes = 0;
        estatisticas[i].quantidade_de_pixels_azuis = 0;
        estatisticas[i].quantidade_de_pixels_pretos = 0;
        estatisticas[i].verdadeiros_positivos = 0;
        estatisticas[i].falsos_positivos = 0;
        estatisticas[i].falsos_negativos = 0;
        estatisticas[i].percentural_vermelho = 0.0;
        estatisticas[i].percentural_verde = 0.0;
        estatisticas[i].percentural_azul = 0.0;
        estatisticas[i].percentural_preto = 0.0;

        int j,k;

        for(j=0; j<ESTRUTURAS_DE_ANALISE; j++)
        {
            for(k=0; k<ESTRUTURAS_DE_ANALISE; k++)
            {
                estatisticas[i].matriz_de_classificacao[j][k] = 0;
            }
        }
    }
}

/**
 * Trata o redimensionamento da janela OpenGL
 */
void reshape( int w, int h )
{

    //reset the coordinate system before modifying
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //set the viewport to be the entire window
    glViewport(0, 0, w, h);
    gluOrtho2D(0,w,0,h);
    //set the clipping volume
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

/**
 * Exibe imagem.
 */
void display(void)
{
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f); // Fundo de tela preto
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);

    //ajusta o zoom da imagem para que apareça na metade da janela
    float zoomH = (glutGet(GLUT_WINDOW_WIDTH)/2.0)/(double)Image.SizeX();
    float zoomV = (glutGet(GLUT_WINDOW_HEIGHT))/(double)Image.SizeY();
    Image.SetZoomH(zoomH);
    Image.SetZoomV(zoomV);

    //posiciona a imagem no canto inferior esquerdo da janela
    Image.SetPos(0, 0);

    //posiciona a imagem nova na metada da direita da janela
    NewImage.SetPos(glutGet(GLUT_WINDOW_WIDTH)/2, 0);

    //ajusta o zoom da imagem para que apareça na metade da janela
    NewImage.SetZoomH(zoomH);
    NewImage.SetZoomV(zoomV);

    //coloca as imagens na tela
    Image.Display();
    NewImage.Display();

    //mostra a tela OpenGL
    glutSwapBuffers();
}

/**
 * Trata eventos do teclado.
 */
void keyboard(unsigned char key, int x, int y)
{

    switch(key)
    {
    //encerra o programa
    case 27:
        NewImage.Delete();
        Image.Delete();
        exit(0);
        break;
    //inicializa imagem de trabalho
    case '0':
        inicializa_imagem_de_trabalho();
        glutPostRedisplay();
        break;
    //calcula mediana simples
    case '1':
        mediana_simples();
        glutPostRedisplay();
        break;
    //calcula mediana de bloco
    case '2':
        mediana_bloco();
        glutPostRedisplay();
        break;
    //calcula média simples
    case '3':
        media_simples();
        glutPostRedisplay();
        break;
    //calcula média de bloco
    case '4':
        media_bloco();
        glutPostRedisplay();
        break;
    //calcula histograma
    case '5':
        calcular_histograma_da_imagem_modificada();
        glutPostRedisplay();
        break;
    //calcula limiar
    case '7':
        calcula_limiar();
        glutPostRedisplay();
        break;
    //salva imagem em matriz
    case '8':
        armazena_imagem_em_matriz_auxiliar();
        glutPostRedisplay();
        break;
    //carrega imagem de matriz
    case '9':
        carrega_imagem_de_matriz_auxiliar();
        glutPostRedisplay();
        break;
    //decrementa limiar inferior
    case 'q':
        limiar_inferior = limiar_inferior - 1;
        printf("[ %d ; %d ]\n",limiar_inferior,limiar_superior);
        glutPostRedisplay();
        break;
    //incrementa limiar inferior
    case 'w':
        limiar_inferior = limiar_inferior + 1;
        printf("[ %d ; %d ]\n",limiar_inferior,limiar_superior);
        glutPostRedisplay();
        break;
    //decrementa limiar superior
    case 'e':
        limiar_superior = limiar_superior - 1;
        printf("[ %d ; %d ]\n",limiar_inferior,limiar_superior);
        glutPostRedisplay();
        break;
    //incrementa limiar superior
    case 'r':
        limiar_superior = limiar_superior + 1;
        printf("[ %d ; %d ]\n",limiar_inferior,limiar_superior);
        glutPostRedisplay();
        break;
    //escolhe imagem anterior
    case 'b':
        numero_imagem--;
        if(numero_imagem < 0)
        {
            numero_imagem = 0;
        }
        printf("IMAGEM ESCOLHIDA = %d\n",numero_imagem);
        break;
    //escolhe imagem seguinte
    case 'n':
        numero_imagem++;
        if(numero_imagem >= IMAGENS_NA_PASTA)
        {
            numero_imagem = IMAGENS_NA_PASTA-1;
        }
        printf("IMAGEM ESCOLHIDA = %d\n",numero_imagem);
        break;
    //calcula filtro de sobel
    case 's':
        sobel();
        glutPostRedisplay();
        break;
    //detecta as dentinas
    case 'd':
        detectar_dentina();
        glutPostRedisplay();
        break;
    //detecta os pinos
    case 'p':
        detectar_pinos();
        glutPostRedisplay();
        break;
    //remove ruidos da imagem
    case 'k':
        remover_ruidos();
        glutPostRedisplay();
        break;
    //imprime pixels azuis
    case 'y':
        imprimir_pixels_azuis();
        glutPostRedisplay();
        break;
    //imprime pixels verdes
    case 'u':
        imprimir_pixels_verdes();
        glutPostRedisplay();
        break;
    //imprime pixels vermelhos
    case 'i':
        imprimir_pixels_vermelhos();
        glutPostRedisplay();
        break;
    //imprime pixels pretos
    case 'o':
        imprimir_pixels_pretos();
        glutPostRedisplay();
        break;
    //imprime pixels do fundo
    case 'f':
        imprimir_pixels_fundo();
        glutPostRedisplay();
        break;
    //executa o trabalho o algoritmo para todas as imagens
    case 't':
        executa_trabalho();
        glutPostRedisplay();
        break;
    //processa imagem escolhida
    case 'a':
        processa_imagem();
        glutPostRedisplay();
        break;
    //executa o trabalho o algoritmo para todas as imagens
    case 'z':
        carrega_imagem_ground_truth();
        glutPostRedisplay();
        break;
    default:
        break;
    }
}

/**
 * Função main.
 */
int main (int argc, char** argv)
{
    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);
    glutInitWindowPosition(10,10);

    //define o tamanho da janela gráfica do programa
    glutInitWindowSize(LARGURA_JAN, ALTURA_JAN);
    glutCreateWindow("Image Loader");

    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);

    glutMainLoop();

    return 0;
}
