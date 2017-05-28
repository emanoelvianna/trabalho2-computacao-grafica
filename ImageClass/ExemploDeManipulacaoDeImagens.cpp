#include <iostream>
#include <cmath>

using namespace std;

#ifdef _MSC_VER

#endif

#ifdef WIN32
#include <windows.h>
#include "gl\glut.h"
#endif

#ifdef __APPLE__
#include <GLUT/glut.h>
#endif

#include "SOIL/SOIL.h"

#include "ImageClass.h"

ImageClass Image, NewImage;

const int LIMIAR = 120;
#define LARGURA_JAN 1000
#define ALTURA_JAN 500

void ConvertBlackAndWhite()
{
    unsigned char r,g,b;
    int x,y;
    int i;
    cout << "Iniciou Black & White....";
    for(x=0; x<Image.SizeX(); x++)
    {
        for(y=0; y<Image.SizeY(); y++)
        {
            i = Image.GetPointIntensity(x,y); // VERIFICA O TOM DE CINZA DA IMAGEM
            Image.ReadPixel(x,y,r,g,b);

            if (i < LIMIAR)
                NewImage.DrawPixel(x, y,0,0,0);  // exibe um ponto PRETO na imagem
            else
                NewImage.DrawPixel(x, y, 255,255,255); // exibe um ponto VERMELHO na imagem

        }
    }
    cout << "Concluiu Black & White." << endl;
}

void detectarPinos() {
    unsigned char r, g, b;
    int x, y;
    int i;
    for(x = 0; x < Image.SizeX(); x++)
    {
        for(y=0; y<Image.SizeY(); y++)
        {
            i = Image.GetPointIntensity(x,y); /** VERIFICA O TOM DE CINZA DA IMAGEM **/
            Image.ReadPixel(x, y, r, g, b);
            if (i > LIMIAR)
                NewImage.DrawPixel(x, y, 0, 0, 255);  /** exibe um ponto PRETO na imagem **/
            else
                NewImage.DrawPixel(x, y,0,0,0);  // exibe um ponto PRETO na imagem
        }
    }
}

void DetectImageBorders()
{
    cout << "Iniciou DetectImageBorders...." << endl;

    cout << "Concluiu DetectImageBorders." << endl;

}

void ConvertToGrayscale()
{
    cout << "Iniciou ConvertToGrayscale..." << endl;
    int x, y;
    int i;
    for(x = 0; x < Image.SizeX(); x++)
    {
        for(y = 0; y < Image.SizeY(); y++)
        {
            i = Image.GetPointIntensity(x, y); // Le o TOM DE CINZA DA IMAGEM
            NewImage.DrawPixel(x, y, i, i, i);  // exibe um ponto CINZA na imagem da direita
        }
    }
    cout << "Concluiu ConvertToGrayscale." << endl;
}

void InvertImage()
{
    cout << "Iniciou InvertImage..." << endl;

    cout << "Concluiu InvertImage." << endl;
}

void OrdenaVetor(int window[])
{
    int temp, i , j;
    for(i = 0; i < 9; i++)
    {
        temp = window[i];
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

    Vetor[0] = Image.GetPointIntensity(x - 1,y-1);
    Vetor[1] = Image.GetPointIntensity(x - 1,y);
    Vetor[2] = Image.GetPointIntensity(x - 1,y+1);
    Vetor[3] = Image.GetPointIntensity(x,y-1);
    Vetor[4] = Image.GetPointIntensity(x,y);
    Vetor[5] = Image.GetPointIntensity(x,y+1);
    Vetor[6] = Image.GetPointIntensity(x + 1,y-1);
    Vetor[7] = Image.GetPointIntensity(x + 1,y);
    Vetor[8] = Image.GetPointIntensity(x + 1,y+1);
}

void Mediana()
{
    int Vetor[9];
    int x, y;
    int i;
    for(x = 0; x < Image.SizeX(); x++)
    {
        for(y = 0; y < Image.SizeY(); y++)
        {
            MontaVetor(x,y, Vetor); // Coloca em VETOR os valores das intensidades ao redor do ponto x,y.
            OrdenaVetor(Vetor);
            NewImage.DrawPixel(x, y, Vetor[0], Vetor[0], Vetor[0]);
        }
    }
}

void init()
{
    int r;
    /** Carrega a uma image **/
    string nome = "imagens/originais/6.png";

    string path = "";

    nome =  path + nome;
    cout << "imagem a ser carregada: *" << nome << "*" << endl;
    r = Image.Load(nome.c_str()); // Carrega uma imagem


    if (!r) exit(1); // Erro na carga da imagem
    else cout << ("Imagem carregada!\n");

    NewImage.SetSize(Image.SizeX(), Image.SizeY(), Image.Channels());
    cout << "Nova Imagem Criada" << endl;

}

void reshape( int w, int h )
{

    // Reset the coordinate system before modifying
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // Set the viewport to be the entire window
    glViewport(0, 0, w, h);
    gluOrtho2D(0,w,0,h);

    // Set the clipping volume
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

}

void display( void )
{
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f); // Fundo de tela preto
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);

// Ajusta o ZOOM da imagem para que apareca na metade da janela
    float zoomH = (glutGet(GLUT_WINDOW_WIDTH)/2.0)/(double)Image.SizeX();
    float zoomV = (glutGet(GLUT_WINDOW_HEIGHT))/(double)Image.SizeY();
    Image.SetZoomH(zoomH);
    Image.SetZoomV(zoomV);
// posiciona a imagem no canto inferior esquerdo da janela
    Image.SetPos(0, 0);

// posiciona a imagem nova na metada da direita da janela
    NewImage.SetPos(glutGet(GLUT_WINDOW_WIDTH)/2, 0);

// Ajusta o ZOOM da imagem para que apareca na metade da janela
    NewImage.SetZoomH(zoomH);
    NewImage.SetZoomV(zoomV);

// Coloca as imagens na tela
    Image.Display();
    NewImage.Display();

// Mostra a tela OpenGL
    glutSwapBuffers();
}

void keyboard ( unsigned char key, int x, int y )
{

    switch ( key )
    {
    case 27: // Termina o programa qdo
        NewImage.Delete();
        Image.Delete();
        exit ( 0 );   // a tecla ESC for pressionada
        break;

    /** converter para preto e branco **/
    case '2':
        ConvertBlackAndWhite();
        glutPostRedisplay();
        break;

    /** detectar pinos **/
    case 'p':
        detectarPinos();
        glutPostRedisplay();
        break;
    case 'g':
        ConvertToGrayscale();
        glutPostRedisplay();
        break;
    case 'b':
        DetectImageBorders();
        glutPostRedisplay();
        break;
    case 'i':
        InvertImage();
        glutPostRedisplay();
        break;

     /** realizar mediana **/
     case 'm':
        Mediana();
        glutPostRedisplay();
        break;
    default:
        break;
    }
}

void arrow_keys ( int a_keys, int x, int y )
{
    switch ( a_keys )
    {
    case GLUT_KEY_UP:       // When Up Arrow Is Pressed...
        break;
    case GLUT_KEY_DOWN:     // When Down Arrow Is Pressed...

        break;
    default:
        break;
    }
}

int main ( int argc, char** argv )
{
    glutInit            ( &argc, argv );

    glutInitDisplayMode (GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB );
    glutInitWindowPosition (10,10);

    // Define o tamanho da janela gráfica do programa
    glutInitWindowSize  ( LARGURA_JAN, ALTURA_JAN);
    glutCreateWindow    ( "Image Loader" );

    init ();

    glutDisplayFunc ( display );
    glutReshapeFunc ( reshape );
    glutKeyboardFunc ( keyboard );
    glutSpecialFunc ( arrow_keys );
    //glutIdleFunc ( display );

    glutMainLoop ( );
    return 0;
}
