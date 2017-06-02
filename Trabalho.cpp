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

//protótipos de funções
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
void ordena_vetor(int vetor[]);

using namespace std;

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
 *
 * Índices do vetor:
 * 0 1 2
 * 3 4 5
 * 6 7 8
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
    cout << "Iniciou Mediana..." << endl;

    int x,y;
    for(x=1; x<Image.SizeX()-1; x++)
    {
        for(y=1; y<Image.SizeY()-1; y++)
        {
			int valor = mediana_simples_aux(x, y);

			NewImage.DrawPixel(x, y,valor,valor,valor);
        }
    }

    cout << "Concluiu Mediana." << endl;
}

/**
 * Este método auxiliar ajuda a calcular a mediana de um pixel.
 *
 * Índices do vetor:
 * 0 1 2
 * 3 4 5
 * 6 7 8
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
    cout << "Iniciou Mediana Bloco..." << endl;

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

    cout << "Concluiu Mediana Bloco." << endl;
}

/**
 * Este método auxiliar ajuda a calcular a média de um pixel.
 *
 * Índices do vetor:
 * 0 1 2
 * 3 4 5
 * 6 7 8
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
 * Este método calcula a média de um pixel.
 */
void media_simples()
{
    cout << "Iniciou Media..." << endl;

    int x,y;
    for(x=1; x<Image.SizeX()-1; x++)
    {
        for(y=1; y<Image.SizeY()-1; y++)
        {
			int valor = media_simples_aux(x, y);

			NewImage.DrawPixel(x, y,valor,valor,valor);
        }
    }

    cout << "Concluiu Media." << endl;
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
void calcular_histograma()
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

    string nome = "0647.png";
    string path = "DadosOriginais/";

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

	//aloca matrizes auxiliares que armazenam pixeis
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

// **********************************************************************
//  void keyboard ( unsigned char key, int x, int y )
// **********************************************************************
void keyboard ( unsigned char key, int x, int y )
{

    switch ( key )
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
        //ToDo
        glutPostRedisplay();
        break;
	//calcula histograma
    case '5':
        calcular_histograma();
        glutPostRedisplay();
        break;
    //calcula mediana do histograma
    case '6':
        //ToDo
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
