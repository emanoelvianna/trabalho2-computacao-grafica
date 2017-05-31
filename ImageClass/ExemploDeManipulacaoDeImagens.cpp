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

int meuVetor[9];

ImageClass Image, NewImage;

const int LIMIAR = 120;
const int TEETH = 59;
const int PINOS = 120;
int branco = 0;
int preto = 0;

#define LARGURA_JAN 1000
#define ALTURA_JAN 500
#define TAMANHO 9

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

            if (i > LIMIAR)
            {
                NewImage.DrawPixel(x, y,255,255,255);  // exibe um ponto PRETO na imagem
            }
            else NewImage.DrawPixel(x, y, 0,0,0); // exibe um ponto VERMELHO na imagem

        }
    }
    cout << "Concluiu Black & White." << endl;
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

void DetectImageBorders()
{
    cout << "Iniciou DetectImageBorders...." << endl;
    cout << "Concluiu DetectImageBorders." << endl;
}

void ConvertToGrayscale()
{
    cout << "Iniciou ConvertToGrayscale..." << endl;
    int x,y;
    int i;
    for(x=0; x<Image.SizeX(); x++)
    {
        for(y=0; y<Image.SizeY(); y++)
        {
            i = Image.GetPointIntensity(x,y); // Le o TOM DE CINZA DA IMAGEM
            NewImage.DrawPixel(x, y,i,i,i);  // exibe um ponto CINZA na imagem da direita
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

void Mediana()
{
    cout << "-- iniciou mediana..." << endl;
    int x;
    int y;
    for(x = 1; x < Image.SizeX() - 1; x++){
        for(y = 1; y < Image.SizeY() - 1; y++){
            MontaVetor(x,y, meuVetor);
            OrdenaVetor(meuVetor);
            NewImage.DrawPixel(x, y, meuVetor[4], meuVetor[4], meuVetor[4]);
        }
    }
    cout << "-- concluiu mediana." << endl;
}

void Histograma() {
    int x, y, i = 0;
    int frequencia[256];

    /** preenche o vetor auxiliar **/
    for(i = 0; i < 256; i++) {
        frequencia[i] = 0;
    }

    /** calculando os valores de frequencia de cada cor **/
    for(x = 0; x < Image.SizeX(); x++){
        for(y = 0; y < Image.SizeY(); y++){
            int cor = Image.GetPointIntensity(x,y);
            frequencia[cor]++;
        }
    }

    /** imprimindo os valores de frequencia de cada cor **/
    for(i = 0; i < 256; i++) {
        cout << i << "=" << frequencia[i] << endl;
    }
}

int DetectarPinos() {
    unsigned char r,g,b;
    int x,y;
    int i;
    cout << "-- detectarPinos...." << endl;
    for(x=0; x<Image.SizeX(); x++)
    {
        for(y=0; y<Image.SizeY(); y++)
        {
            i = Image.GetPointIntensity(x,y);
            Image.ReadPixel(x,y,r,g,b);

            if (i > PINOS)
            {
                /** realizando a pintura nos pinos de azul **/
                NewImage.DrawPixel(x, y, 255, 255, 255);
            }
            else NewImage.DrawPixel(x, y, 0,0,0);

        }
    }
    cout << "-- pinos detectados" << endl;
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

void init()
{
    int r;
    // Carrega a uma image
    string nome = "imagens/originais/1.png";
//    string nome = "Ruido2.bmp";

    string path = "";
// No Code::Blocks para MacOS eh necessario usar um path absoluto
// string path = "ArquivosCodeBlocks/Imagens/";

    nome =  path + nome;
    cout << "imagem a ser carregada: *" << nome << "*" << endl;
    r = Image.Load(nome.c_str()); // Carrega uma imagem


    if (!r) exit(1); // Erro na carga da imagem
    else cout << ("Imagem carregada!\n");

    // Ajusta o tamnho da imagem da direita, para que ela
    // passe a ter o mesmo tamnho da imagem recem carregada
    // Caso precise alterar o tamanho da nova imagem, mude os parâmetros
    // da na chamada abaixo
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
    case 27: /** Termina o programa qdo **/
        NewImage.Delete();
        Image.Delete();
        exit ( 0 );
        break;
    case '2':
        ConvertBlackAndWhite();
        glutPostRedisplay();
        break;
    case 'g':
        ConvertToGrayscale();
        glutPostRedisplay();
        break;
    case 'h':
        Histograma();
        glutPostRedisplay();
        break;
    case 'd':
        DetectarPinos();
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
     case 'm':
        Mediana();
        glutPostRedisplay();
        break;

     case 't':
        findTeeth();
        //Mediana();
        preenche();
        preenche();

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
    case GLUT_KEY_UP:
        break;
    case GLUT_KEY_DOWN:
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

    glutInitWindowSize  ( LARGURA_JAN, ALTURA_JAN);
    glutCreateWindow    ( "Image Loader" );

    init();

    glutDisplayFunc ( display );
    glutReshapeFunc ( reshape );
    glutKeyboardFunc ( keyboard );
    glutSpecialFunc ( arrow_keys );
    //glutIdleFunc ( display );

    glutMainLoop ( );
    return 0;
}
