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

void convert_black_and_white()
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

void find_teeth()
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

void detect_image_borders()
{
    cout << "Iniciou DetectImageBorders...." << endl;
    cout << "Concluiu DetectImageBorders." << endl;
}

void convert_to_grayscale()
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

void ordena_vetor(int window[])
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

int auxiliar_mediana(int x, int y)
{
    int vetor[9];
    vetor[0] = NewImage.GetPointIntensity(x - 1,y-1);
    vetor[1] = NewImage.GetPointIntensity(x - 1,y);
    vetor[2] = NewImage.GetPointIntensity(x - 1,y+1);
    vetor[3] = NewImage.GetPointIntensity(x,y-1);
    vetor[4] = NewImage.GetPointIntensity(x,y);
    vetor[5] = NewImage.GetPointIntensity(x,y+1);
    vetor[6] = NewImage.GetPointIntensity(x + 1,y-1);
    vetor[7] = NewImage.GetPointIntensity(x + 1,y);
    vetor[8] = NewImage.GetPointIntensity(x + 1,y+1);

    ordena_vetor(vetor);
    return vetor[4];
}

void mediana()
{
    cout << "-- iniciou mediana..." << endl;
    int x;
    int y;
    for(x = 1; x < Image.SizeX() - 1; x++){
        for(y = 1; y < Image.SizeY() - 1; y++){
            int valor = auxiliar_mediana(x, y);
            ordena_vetor(meuVetor);
            NewImage.DrawPixel(x, y, valor, valor, valor);
        }
    }
    cout << "-- concluiu mediana." << endl;
}

void mediana_bloco()
{
    cout << "Iniciou Mediana Bloco..." << endl;
    int x,y;
    for(x=1; x<Image.SizeX()-1; x += 3)
    {
        for(y=1; y<Image.SizeY()-1; y += 3)
        {
            int valor = auxiliar_mediana(x, y);
            ordena_vetor(meuVetor);

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

void histograma() {
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

int detectar_pinos() {
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

            if (i > LIMIAR)
            {
                /** realizando a pintura nos pinos de azul **/
                NewImage.DrawPixel(x, y, 0, 0, 255);
            }
            //else NewImage.DrawPixel(x, y, 0,0,0);

        }
    }
    cout << "-- pinos detectados" << endl;
}

int detectar_dentina() {
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

            if (i > 50 && i < 100)
            {
                /** realizando a pintura nos pinos de azul **/
                NewImage.DrawPixel(x, y, 0, 255, 0);
            }
            //else NewImage.DrawPixel(x, y, 0,0,0);

        }
    }
    cout << "-- pinos detectados" << endl;
}

void efetua_preenchimento(int x, int y, int vetor[9])
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

            auxiliar_mediana(x,y);
            efetua_preenchimento(x,y,meuVetor);
        }
    }
    cout << "Concluiu Preenche." << endl;
}

void init()
{
    int r;
    string nome = "imagens/originais/1.png";
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
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, w, h);
    gluOrtho2D(0,w,0,h);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

}

void display( void )
{
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f); // Fundo de tela preto
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    float zoomH = (glutGet(GLUT_WINDOW_WIDTH)/2.0)/(double)Image.SizeX();
    float zoomV = (glutGet(GLUT_WINDOW_HEIGHT))/(double)Image.SizeY();
    Image.SetZoomH(zoomH);
    Image.SetZoomV(zoomV);
    Image.SetPos(0, 0);
    NewImage.SetPos(glutGet(GLUT_WINDOW_WIDTH)/2, 0);
    NewImage.SetZoomH(zoomH);
    NewImage.SetZoomV(zoomV);
    Image.Display();
    NewImage.Display();
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
        convert_black_and_white();
        glutPostRedisplay();
        break;
    case 'g':
        convert_to_grayscale();
        glutPostRedisplay();
        break;
    /** histograma **/
    case 'h':
        histograma();
        glutPostRedisplay();
        break;
    /** pinos **/
    case 'p':
        detectar_pinos();
        glutPostRedisplay();
        break;
    /** dentina **/
    case 'd':
        detectar_dentina();
        glutPostRedisplay();
        break;
    /** bordas **/
    case 'b':
        detect_image_borders();
        glutPostRedisplay();
        break;
    case 'i':
        mediana_bloco();
        glutPostRedisplay();
        break;
     case 'm':
        mediana();
        glutPostRedisplay();
        break;

     case 't':
        find_teeth();
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
