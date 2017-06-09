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
#include "ImageClass.h"
#include "SOIL/SOIL.h"

//constantes
#define LARGURA_JAN 1000
#define ALTURA_JAN 500

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

//protótipos de funções
void sobel();
void inicializa_imagem_de_trabalho();
void armazena_imagem_em_matriz_auxiliar();
void carrega_imagem_de_matriz_auxiliar();
void calcula_limiar();
void calcular_histograma();
void mediana_simples();
int mediana_simples_aux(int x, int y);
void mediana_bloco();
int mediana_bloco_aux(int x, int y);
void media_simples();
int media_simples_aux(int x, int y);
void media_bloco();
int media_bloco_aux(int x, int y);
void ordena_vetor(int vetor[]);

using namespace std;

/**
 * Aplica o filtro de sobel na imagem.
 */
void sobel()
{
    cout << "Iniciou Sobel...";

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

    cout << "Concluiu Sobel.";
}

/**
 * Armazena o rgb da imagem em uma estrutura auxiliar.
 */
void armazena_imagem_em_matriz_auxiliar()
{
    unsigned char r,g,b;
    int x,y;

    for(x=1; x<Image.SizeX()-1; x++)
    {
        for(y=1; y<Image.SizeY()-1; y++)
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

    for(x=1; x<Image.SizeX()-1; x++)
    {
        for(y=1; y<Image.SizeY()-1; y++)
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
    cout << "Inicializando imagem..." << endl;

    unsigned char r,g,b;
    int x,y;

    for(x=1; x<Image.SizeX()-1; x++)
    {
        for(y=1; y<Image.SizeY()-1; y++)
        {
            Image.ReadPixel(x,y,r,g,b);

            NewImage.DrawPixel(x,y,r,g,b);
        }
    }

    cout << "Imagem Inicializada" << endl;
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
    cout << "Iniciou MEDIANA simples..." << endl;

    int x;
    int y;
    for(x = 1; x < Image.SizeX() - 1; x++){
        for(y = 1; y < Image.SizeY() - 1; y++){
            int valor = mediana_simples_aux(x, y);
            NewImage.DrawPixel(x, y, valor, valor, valor);
        }
    }

    cout << "Concluiu MEDIANA simples." << endl;
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
    cout << "Iniciou MEDIANA em bloco..." << endl;

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

    cout << "Concluiu MEDIANA em bloco." << endl;
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
    cout << "Realizando MEDIA em bloco." << endl;

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

    cout << "Concluiu MEDIA em bloco." << endl;
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
    cout << "Realizando MEDIA simples." << endl;

    int x,y;
    for(x=1; x<Image.SizeX()-1; x++)
    {
        for(y=1; y<Image.SizeY()-1; y++)
        {
            int valor = media_simples_aux(x, y);

            NewImage.DrawPixel(x, y,valor,valor,valor);
        }
    }

    cout << "Concluiu MEDIA simples." << endl;
}

/**
 * Desenha a imagem de acordo com o limiar global especificado.
 */
void calcula_limiar()
{
    cout << "Iniciou Limiar...";

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
                NewImage.DrawPixel(x, y,0,0,0);
            }
            else
            {
                //pinta o pixel de branco
                NewImage.DrawPixel(x, y, 255,255,255);
            }
        }
    }

    cout << "Concluiu Limiar." << endl;
}

/**
 * Calcula histograma da imagem.
 */
void calcular_histograma_imagem_original()
{
    cout << "Iniciou Histograma...";

    unsigned char r,g,b;
    int x,y;
    int i;

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
        printf("%d = %d\n",i,histograma[i]);
    }
    printf("total da soma = %d\n\n",soma);

    cout << "Concluiu Histograma.";
}

/**
 * Calcula histograma da imagem modificada.
 */
void calcular_histograma_imagem_modificada() {
    int x, y, i = 0;
    int frequencia[256];

    /** preenche o vetor auxiliar **/
    for(i = 0; i < 256; i++) {
        frequencia[i] = 0;
    }

    /** calculando os valores de frequencia de cada cor **/
    for(x = 0; x < Image.SizeX(); x++){
        for(y = 0; y < Image.SizeY(); y++){
            int cor = NewImage.GetPointIntensity(x,y);
            frequencia[cor]++;
        }
    }

    /** imprimindo os valores de frequencia de cada cor **/
    for(i = 0; i < 256; i++) {
        cout << i << "=" << frequencia[i] << endl;
    }
}

int detectar_dentina() {
    unsigned char r,g,b;
    int x,y;
    int i;
    cout << "-- detectar dentina...." << endl;
    for(x=0; x<Image.SizeX(); x++)
    {
        for(y=0; y<Image.SizeY(); y++)
        {
            i = NewImage.GetPointIntensity(x,y);
            NewImage.ReadPixel(x,y,r,g,b);

            if (i > 7 && i < 50 )
            {
                /** realizando a pintura nos pinos de azul **/
                NewImage.DrawPixel(x, y, 0, 255, 0);
            }
            //else NewImage.DrawPixel(x, y, 0,0,0);

        }
    }
    cout << "-- dentina detectada" << endl;
}

int detectar_pinos() {
    unsigned char r,g,b;
    int x,y;
    int i;
    cout << "-- detectar pinos...." << endl;
    for(x=0; x<Image.SizeX(); x++)
    {
        for(y=0; y<Image.SizeY(); y++)
        {
            i = NewImage.GetPointIntensity(x,y);
            NewImage.ReadPixel(x,y,r,g,b);

            if (i > 100 && i < 200 )
            {
                /** realizando a pintura nos pinos de azul **/
                NewImage.DrawPixel(x, y, 0, 0, 255);
            }
            //else NewImage.DrawPixel(x, y, 0,0,0);

        }
    }
    cout << "-- pinos detectados" << endl;
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

    string nome = "imagens/originais/1.png";;
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
    int temp, i , j;
    for(i = 0; i < 9; i++)
    {
        temp = window[i];

        if(window[i] == 255){branco++;}
            else{preto++;}

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
    cout << "-- iniciou mediana..." << endl;
    int x;
    int y;
    for(x = 1; x < Image.SizeX() - 1; x++){
        for(y = 1; y < Image.SizeY() - 1; y++){
            MontaVetor(x, y, meuVetor);
            OrdenaVetor(meuVetor);
            NewImage.DrawPixel(x, y, meuVetor[4], meuVetor[4], meuVetor[4]);
        }
    }
    cout << "-- concluiu mediana." << endl;
}

void efetuaPreenchimento(int x, int y, int vetor[9])
{
    int i;
    for(i = 0; i < 9; i++) {
        if(vetor[i] == 255) {
            branco++;
        } else {
            preto++;
        }
    }
    if(branco > preto) {
        NewImage.DrawPixel(x - 1,y-1, 255,255,255);
        NewImage.DrawPixel(x - 1,y, 255,255,255);
        NewImage.DrawPixel(x - 1,y+1, 255,255,255);
        NewImage.DrawPixel(x,y-1, 255,255,255);
        NewImage.DrawPixel(x,y, 255,255,255);
        NewImage.DrawPixel(x,y+1, 255,255,255);
        NewImage.DrawPixel(x + 1,y-1, 255,255,255);
        NewImage.DrawPixel(x + 1,y, 255,255,255);
        NewImage.DrawPixel(x + 1,y+1, 255,255,255);
    } else {
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
    cout << "Iniciou Preenche..." << endl;
    int x;
    int y;

    for(x = 1; x < Image.SizeX() - 1; x = x + 3){
        for(y = 1; y < Image.SizeY() - 1; y = y + 3){

            MontaVetor(x,y,meuVetor);
            efetuaPreenchimento(x,y,meuVetor);
        }
    }
    cout << "Concluiu Preenche." << endl;
}

void findTeeth()
{

    unsigned char r,g,b;
    int x,y;
    int i;
    cout << "Iniciou FINDING TEETH.....";
    for(x=0; x<Image.SizeX(); x++)
    {
        for(y=0; y<Image.SizeY(); y++)
        {
            i = Image.GetPointIntensity(x,y); // VERIFICA O TOM DE CINZA DA IMAGEM
            Image.ReadPixel(x,y,r,g,b);

            if (TEETH < i && i < 77)
            {
                NewImage.DrawPixel(x, y,255,255,255);  // exibe um ponto BRANCO na imagem
            }
            else NewImage.DrawPixel(x, y, 0,0,0); // exibe um ponto PRETO na imagem

        }
    }
    cout << "Concluiu FINDING TEETH." << endl;


}

/** sequencia de metodos para remover ruidos da imagem **/
void run_1() {
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

void run_2() {
    inicializa_imagem_de_trabalho();
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
        limiar_inferior = limiar_inferior - 10;
        printf("[ %d ; %d ]\n",limiar_inferior,limiar_superior);
        glutPostRedisplay();
        break;
    //incrementa limiar inferior
    case 'w':
        limiar_inferior = limiar_inferior + 10;
        printf("[ %d ; %d ]\n",limiar_inferior,limiar_superior);
        glutPostRedisplay();
        break;
    //decrementa limiar superior
    case 'e':
        limiar_superior = limiar_superior - 10;
        printf("[ %d ; %d ]\n",limiar_inferior,limiar_superior);
        glutPostRedisplay();    // obrigatório para redesenhar a tela
        break;
    //incrementa limiar superior
    case 'r':
        limiar_superior = limiar_superior + 10;
        printf("[ %d ; %d ]\n",limiar_inferior,limiar_superior);
        glutPostRedisplay();
        break;
    //calcula filtro de sobel
    case 's':
        sobel();
        glutPostRedisplay();
        break;
    /** sequencia de metodos para remover ruidos  **/
    case 'b':
        run_1();
        glutPostRedisplay();
        break;
    /** sequencia de metodos para remover ruidos  **/
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
    case 'm':
        run_2();
        glutPostRedisplay();
    case 'p':
        detectar_pinos();
        glutPostRedisplay();
    case 'd':
        detectar_dentina();
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
