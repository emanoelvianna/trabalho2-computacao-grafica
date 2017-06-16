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

//estruturas
typedef struct
{
    int x;
    int y;
} ponto;

//variáveis globais
ImageClass Image, NewImage;
int limiar_inferior, limiar_superior;
int histograma[255];
unsigned char** r_matrix_aux;
unsigned char** g_matrix_aux;
unsigned char** b_matrix_aux;
unsigned char** matrix_aux;
int kernel_x[3][3];
int kernel_y[3][3];
const int LIMIAR = 120;
const int TEETH = 59;
const int PINOS = 120;
int meuVetor[9];
int branco = 0;
int preto = 0;
bool** pixels_azuis;
bool** pixels_verdes;
bool** pixels_vermelhos;
bool** pixels_pretos;
int** matriz_de_pixels_visitados;
ponto* lista_busca_aux;
int contador_lista_aux;

//protótipos de funções
void sobel();
void inicializa_imagem_de_trabalho();
void armazena_imagem_em_matriz_auxiliar();
void carrega_imagem_de_matriz_auxiliar();
void calcula_limiar();
void calcular_histograma();
void mediana_simples();
int mediana_simples_aux(int x,int y);
void mediana_bloco();
int mediana_bloco_aux(int x,int y);
void media_simples();
int media_simples_aux(int x,int y);
void media_bloco();
int media_bloco_aux(int x,int y);
void ordena_vetor(int vetor[]);
void remover_ruidos();
void destaca_o_que_nao_e_fundo();
void detectar_dentina_aux();
void detectar_dentina();
void detectar_pinos();
void imprimir_pixels_azuis();
void imprimir_pixels_vermelhos();
void imprimir_pixels_verdes();
void imprimir_pixels_pretos();
void imprimir_todos_os_pixels();
void detectar_fundo_preto_apos_detectar_dentinas(int x_inicial,int y_inicial);
void adiciona_ponto_em_lista_aux(ponto ponto_aux);
ponto remove_ponto_em_lista_aux();
bool o_ponto_e_valido(int x,int y);
void detectar_dentinas_e_pinos();
void remover_bordas_brancas(int x_inicial, int y_inicial);
void pinta_pixels_brancos_de_vermelho();

using namespace std;

/**
 * Sequência de passos que detecta as dentinas e pinos da imagem.
 */
void detectar_dentinas_e_pinos()
{
    int x,y;

    //zera valores de pixels
    for(x=0; x<Image.SizeX(); x++)
    {
        for(y=0; y<Image.SizeY(); y++)
        {
            pixels_azuis[x][y] = false;
            pixels_vermelhos[x][y] = false;
            pixels_verdes[x][y] = false;
            pixels_pretos[x][y] = false;
        }
    }

    printf("[DETECTANDO DENTINAS > ");
    detectar_dentina();
    printf("DETECTANDO PINOS > ");
    detectar_pinos();
    imprimir_pixels_verdes();
    imprimir_pixels_azuis();
    printf("DETECTANDO FUNDO > ");
    detectar_fundo_preto_apos_detectar_dentinas(100,100);
    detectar_fundo_preto_apos_detectar_dentinas(Image.SizeX()/2,Image.SizeY()/2);
    imprimir_pixels_pretos();
    printf("CONCLUIDO]\n");

	//remover_bordas_brancas(Image.SizeX()/2,Image.SizeY()/2);
	//imprimir_pixels_pretos();
	//pinta_pixels_brancos_de_vermelho();
	//imprimir_pixels_vermelhos();
}

/**
 * Armazena coordenadas de pixels brancos na matriz de mpixels vermelhos.
 */
void pinta_pixels_brancos_de_vermelho()
{
    unsigned char r,g,b;
    int x,y;

    for(x=0; x<Image.SizeX(); x++)
    {
        for(y=0; y<Image.SizeY(); y++)
        {
			NewImage.ReadPixel(x,y,r,g,b);
	
            if(r == 255 && g == 255 && b == 255)
            {
                pixels_vermelhos[x][y] = true;
            }
        }
    }
}

/**
 * Verifica se um ponto x,y é válido.
 */
bool o_ponto_e_valido(int x, int y)
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

    //printf("CONTADOR DA LISTA AUX: %d\n",contador_lista_aux);

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
 * Detecta o fundo preto, mas somente após detectar dentinas.
 */
void detectar_fundo_preto_apos_detectar_dentinas(int x_inicial, int y_inicial)
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

        for(k=0; k<9; k++)
        {
            int x_novo = vetor[k].x;
            int y_novo = vetor[k].y;

            if(o_ponto_e_valido(x_novo, y_novo) == true)
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
 * Varre o fundo da imagem removendo bordas brancas.
 */
void remover_bordas_brancas(int x_inicial, int y_inicial)
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

        NewImage.ReadPixel(x_atual,y_atual,r,g,b);


		//se pixel é branco, sinaliza como preto
		if(r == 255 && g == 255 && b == 255)
		{
			pixels_pretos[x_atual][y_atual] = true;
			pixels_verdes[x_atual][y_atual] = false;
			pixels_azuis[x_atual][y_atual] = false;
			pixels_vermelhos[x_atual][y_atual] = false;
		}

        for(k=0; k<9; k++)
        {
            int x_novo = vetor[k].x;
            int y_novo = vetor[k].y;

            if(o_ponto_e_valido(x_novo, y_novo) == true)
            {
                resultado = matriz_de_pixels_visitados[x_novo][y_novo];

                //se este pixel ainda não foi visitado
                if(resultado == 0)
                {
                    //marca pixel adjacente
                    matriz_de_pixels_visitados[x_novo][y_novo] = 1;

                    NewImage.ReadPixel(x_novo,y_novo,r,g,b);

                    //se o pixel é verde
                    if( (r == 255 && g == 255 && b == 255) || (r == 0 && g == 0 && b == 0))
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
 * Pinta de vermelho tudo que não é fundo.
 * Isso ajuda a identificar se existem bordas fechadas.
 */
void destaca_o_que_nao_e_fundo()
{
    unsigned char r,g,b;
    int x,y;

    for(x=0; x<Image.SizeX(); x++)
    {
        for(y=0; y<Image.SizeY(); y++)
        {
            NewImage.ReadPixel(x,y,r,g,b);

            if( (r <= 0) || (g <= 0) || (b <= 0) )
            {
                NewImage.DrawPixel(x,y,255,255,255);
            }
            else
            {
                NewImage.DrawPixel(x,y,0,255,0);
            }
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

    for(i=0; i<8; i++)
    {
        media_simples();
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
 * Copia os pixeis da imagem original para a imagem de trabalho.
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
 * Calcula histograma da imagem.
 */
void calcular_histograma_imagem_original()
{
    //cout << "Iniciou Histograma...";

    unsigned char r,g,b;
    int x,y;
    int i;

    for(i = 0; i < 255; i++)
    {
        histograma[i] = 0;
    }

    for(x=0; x<Image.SizeX(); x++)
    {
        for(y=0; y<Image.SizeY(); y++)
        {
            Image.ReadPixel(x,y,r,g,b);
            histograma[r] = histograma[r] + 1;
        }
    }

    printf("HISTOGRAMA\n");
    int soma = 0;
    for(i = 0; i < 255; i++)
    {
        soma = soma + histograma[i];
        //printf("%d = %d\n",i,histograma[i]);
    }
    printf("total da soma = %d\n\n",soma);

    //cout << "Concluiu Histograma.";
}

/**
 * Calcula histograma da imagem modificada.
 */
void calcular_histograma_imagem_modificada()
{
    int x, y, i = 0;
    int frequencia[256];

    /** preenche o vetor auxiliar **/
    for(i = 0; i < 256; i++)
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
    for(i = 0; i < 256; i++)
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

    //remove ruídos
    remover_ruidos();

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
    //printf("DETECTANDO PINOS -> ");

    inicializa_imagem_de_trabalho();

    unsigned char r,g,b;
    int x,y;
    int i;

    //zera valores da matriz de pixels azuis
    for(x=0; x<Image.SizeX(); x++)
    {
        for(y=0; y<Image.SizeY(); y++)
        {
            pixels_azuis[x][y] = false;
        }
    }

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

        }
    }

    //printf("CONCLUIDO\n");
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
    int r,i;

    string nome = "imagens/originais/1.png";
    //string nome = "imagem_de_teste_01.png";
    string path = "";

    nome =  path + nome;
    r = Image.Load(nome.c_str());

    if (!r) exit(1);

    NewImage.SetSize(Image.SizeX(), Image.SizeY(), Image.Channels());

    //limiares iniciais
    limiar_inferior = 0;
    limiar_superior = 70;

    //inicializa histograma
    for(i = 0; i < 255; i++)
    {
        histograma[i] = 0;
    }

    //aloca matrizes auxiliares que armazenam pixels
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
    matriz_de_pixels_visitados = (int**) malloc(Image.SizeX() * sizeof (int*));
    for (i = 0; i < Image.SizeX(); i++)
    {
        matriz_de_pixels_visitados[i] = (int*) malloc(Image.SizeY() * sizeof (int));
    }

    int size_aux = Image.SizeX() * Image.SizeY();
    //aloca lista de auxílio para a execução algoritmos de segmentação
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
void display( void )
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

void OrdenaVetor(int window[])
{
    branco = 0;
    preto = 0;
    int temp, i, j;
    for(i = 0; i < 9; i++)
    {
        temp = window[i];

        if(window[i] == 255)
        {
            branco++;
        }
        else
        {
            preto++;
        }

        for(j = i-1; j >= 0 && temp < window[j]; j--)
        {
            window[j+1] = window[j];
        }
        window[j+1] = temp;
    }
}

void MontaVetor(int Px, int Py, int Vetor[9])
{
    int x = Px;
    int y = Py;

    Vetor[0] = NewImage.GetPointIntensity(x - 1,y-1);
    Vetor[1] = NewImage.GetPointIntensity(x - 1,y);
    Vetor[2] = NewImage.GetPointIntensity(x - 1,y+1);
    Vetor[3] = NewImage.GetPointIntensity(x,y-1);
    Vetor[4] = NewImage.GetPointIntensity(x,y);
    Vetor[5] = NewImage.GetPointIntensity(x,y+1);
    Vetor[6] = NewImage.GetPointIntensity(x + 1,y-1);
    Vetor[7] = NewImage.GetPointIntensity(x + 1,y);
    Vetor[8] = NewImage.GetPointIntensity(x + 1,y+1);
}

void minha_mediana()
{
    //cout << "-- iniciou mediana..." << endl;
    int x;
    int y;
    for(x = 1; x < Image.SizeX() - 1; x++)
    {
        for(y = 1; y < Image.SizeY() - 1; y++)
        {
            MontaVetor(x, y, meuVetor);
            OrdenaVetor(meuVetor);
            NewImage.DrawPixel(x, y, meuVetor[4], meuVetor[4], meuVetor[4]);
        }
    }
    //cout << "-- concluiu mediana." << endl;
}

void efetuaPreenchimento(int x, int y, int vetor[9])
{
    int i;
    for(i = 0; i < 9; i++)
    {
        if(vetor[i] == 255)
        {
            branco++;
        }
        else
        {
            preto++;
        }
    }
    if(branco > preto)
    {
        NewImage.DrawPixel(x - 1,y-1, 255,255,255);
        NewImage.DrawPixel(x - 1,y, 255,255,255);
        NewImage.DrawPixel(x - 1,y+1, 255,255,255);
        NewImage.DrawPixel(x,y-1, 255,255,255);
        NewImage.DrawPixel(x,y, 255,255,255);
        NewImage.DrawPixel(x,y+1, 255,255,255);
        NewImage.DrawPixel(x + 1,y-1, 255,255,255);
        NewImage.DrawPixel(x + 1,y, 255,255,255);
        NewImage.DrawPixel(x + 1,y+1, 255,255,255);
    }
    else
    {
        NewImage.DrawPixel(x - 1,y-1,0,0,0);
        NewImage.DrawPixel(x - 1,y,0,0,0);
        NewImage.DrawPixel(x - 1,y+1,0,0,0);
        NewImage.DrawPixel(x,y-1,0,0,0);
        NewImage.DrawPixel(x,y,0,0,0);
        NewImage.DrawPixel(x,y+1,0,0,0);
        NewImage.DrawPixel(x + 1,y-1, 0,0,0);
        NewImage.DrawPixel(x + 1,y,0,0,0);
        NewImage.DrawPixel(x + 1,y+1,0,0,0);

    }
}

void preenche()
{
    //cout << "Iniciou Preenche..." << endl;
    int x;
    int y;

    for(x = 1; x < Image.SizeX() - 1; x = x + 3)
    {
        for(y = 1; y < Image.SizeY() - 1; y = y + 3)
        {

            MontaVetor(x,y,meuVetor);
            efetuaPreenchimento(x,y,meuVetor);
        }
    }
    //cout << "Concluiu Preenche." << endl;
}

void findTeeth()
{
    unsigned char r,g,b;
    int x,y;
    int i;
    //cout << "Iniciou FINDING TEETH.....";
    for(x=0; x<Image.SizeX(); x++)
    {
        for(y=0; y<Image.SizeY(); y++)
        {
            i = NewImage.GetPointIntensity(x,y); // VERIFICA O TOM DE CINZA DA IMAGEM
            NewImage.ReadPixel(x,y,r,g,b);

            if (TEETH < i && i < 77)
            {
                NewImage.DrawPixel(x, y,255,255,255);  // exibe um ponto BRANCO na imagem
            }
            else NewImage.DrawPixel(x, y, 0,0,0); // exibe um ponto PRETO na imagem

        }
    }
    //cout << "Concluiu FINDING TEETH." << endl;


}

/** sequencia de metodos para remover ruidos da imagem **/
void run_1()
{
    inicializa_imagem_de_trabalho();
    media_bloco();
    media_simples();
    media_simples();
    media_simples();
    media_simples();
    media_simples();
    media_simples();
    media_simples();
    media_simples();
    media_simples();
    media_simples();
    media_simples();
    media_simples();
    media_simples();
    media_simples();
    media_simples();
    media_simples();
    media_simples();
    media_simples();
    media_simples();
    media_simples();
    media_simples();
    media_simples();
    media_simples();
    media_simples();
    media_simples();
    media_simples();
    mediana_bloco();
    media_simples();
    media_simples();
    media_simples();
    media_simples();
    media_simples();
    media_simples();
    media_simples();
    media_simples();
}

void run_2()
{
    //inicializa_imagem_de_trabalho();
    media_bloco();
    media_bloco();
    mediana_simples();
    mediana_bloco();
    mediana_bloco();
    mediana_bloco();
    media_simples();
    media_bloco();
    media_simples();
    media_bloco();
    mediana_simples();
    mediana_simples();
    mediana_bloco();
    mediana_simples();
    minha_mediana();
    media_simples();
    media_simples();
    media_simples();
    media_simples();
    media_simples();
    media_simples();
    //sobel();
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
        calcular_histograma_imagem_original();
        glutPostRedisplay();
        break;
    //calcula mediana do histograma
    case '6':
        //TODO
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
        glutPostRedisplay();    // obrigatório para redesenhar a tela
        break;
    //incrementa limiar superior
    case 'r':
        limiar_superior = limiar_superior + 1;
        printf("[ %d ; %d ]\n",limiar_inferior,limiar_superior);
        glutPostRedisplay();
        break;
    //remove ruidos da imagem
    case 'k':
        remover_ruidos();
        glutPostRedisplay();
        break;
    //calcula filtro de sobel
    case 's':
        sobel();
        glutPostRedisplay();
        break;
    //destaca o que não é fundo da imagem
    case 'l':
        destaca_o_que_nao_e_fundo();
        glutPostRedisplay();
        break;
    //sequencia de metodos para remover ruidos
    case 'b':
        run_1();
        glutPostRedisplay();
        break;
    case 'h':
        calcular_histograma_imagem_modificada();
        glutPostRedisplay();
        break;
    case 'n':
        minha_mediana();
        glutPostRedisplay();
        break;
    case 't':
        findTeeth();
        //Mediana();
        preenche();
        glutPostRedisplay();
        break;
    case 'm':
        run_2();
        glutPostRedisplay();
        break;
    case 'p':
        detectar_pinos();
        glutPostRedisplay();
        break;
    case 'd':
        detectar_dentina();
        glutPostRedisplay();
        break;
    case 'y':
        imprimir_pixels_azuis();
        glutPostRedisplay();
        break;
    case 'u':
        imprimir_pixels_verdes();
        glutPostRedisplay();
        break;
    case 'i':
        imprimir_pixels_vermelhos();
        glutPostRedisplay();
        break;
    case 'o':
        imprimir_pixels_pretos();
        glutPostRedisplay();
        break;
    case 'j':
        detectar_dentinas_e_pinos();
        glutPostRedisplay();
        break;
    case 'g':
        detectar_fundo_preto_apos_detectar_dentinas(Image.SizeX()/2,Image.SizeY()/2);
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
