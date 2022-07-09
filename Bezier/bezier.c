//Marlon Silveira Basilio

#include <GL/gl.h>
#include <GL/glut.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

float res_width = 1080.0f;
float res_height = 800.0f;
float T;


typedef struct {
    float x;
    float y;
}ponto_T;

int num_pontos, contPonto;
int raio_ponto = 10;
int interruptor;

ponto_T *pts_de_controle;
ponto_T *controle;

// Desenha a curva de Bezier
void desenhaCurva () {
    
    ponto_T p;
    float n = 0.001f;

    glColor3f(0.99f, 0.0f, 0.0f);
    glLineWidth(4);

    // desenho da linha
    glBegin(GL_LINE_STRIP);

    // T vai variar de 0.000 ate 0.999
    for (T = 0.000f; T < 1.000f; T = T + n){
        controle = malloc (sizeof(ponto_T) * num_pontos);
        memcpy(controle, pts_de_controle, sizeof(ponto_T) * num_pontos);
    // Algoritmo de De Casteljau    
        for (int i = 1; i < num_pontos; i++){
            for(int j = 0; j < num_pontos - i; j++){
                controle[j].x = (1 - T) * controle[j].x + T * controle[j + 1].x;
                controle[j].y = (1 - T) * controle[j].y + T * controle[j + 1].y;
            }
        }

    glVertex2f(controle[0].x, controle[0].y);
    free(controle);
    }
    glVertex2f(pts_de_controle[num_pontos-1].x, pts_de_controle[num_pontos-1].y);

    glEnd();
    glFlush();
}


// Funcao que desenha os pontos de controle
void desenhaPontos () {
    glEnable(GL_POINT_SMOOTH);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    glPointSize(2*raio_ponto);
    glColor3f(0.5f, 0.5f, 0.5f);

    glBegin(GL_POINTS);

    for (int i = 0; i < num_pontos; ++i){
    glVertex2f(pts_de_controle[i].x, pts_de_controle[i].y);
    }

    glEnd();

    glFlush();
}
// Funcao principal que desenha os pontos e as curvas
void Desenha () {

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glClear(GL_COLOR_BUFFER_BIT);

    desenhaCurva();
    desenhaPontos();
}

// Altera a resolucao da tela
void AlteraTamanhoJanela (GLsizei w, GLsizei h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, res_width, 0, res_height);
}

// Encontra a distancia entre os pontos
float distancia (ponto_T p1, ponto_T p2) {
    float dist = sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2));
    return dist;
}

// Se a distancia for menor que o raio, significa que o ponto estrangeiro esta dentro do ponto definido, se for maior
// significa que esta fora
int Dentro (ponto_T ponto_estrangeiro, ponto_T ponto_definido, float raio_ponto) {

    float dist_estrangeiro = distancia(ponto_estrangeiro, ponto_definido);

    if (dist_estrangeiro > raio_ponto){
        return 0;
    }
    else{
        return 1;
    }
}

// Procura o ponto que foi clicado para ser arrastado
int encontra_Ponto (ponto_T ponto_estrangeiro) {
    for (int i = 0; i < num_pontos; i++)
        if (Dentro(ponto_estrangeiro, pts_de_controle[i], raio_ponto))
        return i;
    return -1;
}

// Adiciona um novo ponto de controle
void add_ponto(ponto_T p) {
    num_pontos++;
    if (num_pontos > 1)
        pts_de_controle = realloc(pts_de_controle, num_pontos * sizeof(ponto_T));
    pts_de_controle[num_pontos-1] = p;
}

// Remove o ultimo ponto de controle
void remove_ponto(){
    if (num_pontos > 1){
    num_pontos--;
    pts_de_controle = realloc(pts_de_controle, num_pontos * sizeof(ponto_T));
    }
}

    // Funcoes dos botoes do mouse
void clique(int button, int state, int x, int y) {
    
    ponto_T p;
    p.x = x;
    p.y = res_height - y;
    // Botao esquerdo adiciona um novo ponto
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        contPonto = encontra_Ponto(p);
        if (contPonto == -1)
        add_ponto(p);
    }
    // Botao direito remove o ultimo ponto
    else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN){
    remove_ponto();
    }
    // Botao do meio troca a cor de fundo
    else if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN){
        if (interruptor == 1){
        glClearColor(0.9f, 0.9f, 0.9f, 1.00f);
        interruptor = 0; 
        }
        else{
        glClearColor(0.0f, 0.0f, 0.0f, 1.00f);
        interruptor = 1;
        }
    }

    glutPostRedisplay();
}

// ao clicar em cima de um ponto ja existente, o ponto podera ser arrastado
void arrasta_ponto (int x, int y) {

    ponto_T p;
    p.x = x;
    p.y = res_height - y;

    if (contPonto !=-1) {
        pts_de_controle[contPonto] = p;
    }

    glutPostRedisplay();
}

// inicializa algumas variaveis globais e define a tela de fundo
void Inicializa(void) {

    num_pontos = 0;
    interruptor = 0;
    pts_de_controle = (ponto_T*) malloc(sizeof(ponto_T));

    if (pts_de_controle == NULL) {
        printf("Erro não foi possível inicializar os pontos de controle");
        exit(1);
    }

    contPonto = -1;

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // tela preta
}

int main(int argc, char *argv[]) {
	glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(res_width,res_height);
    glutInitWindowPosition((glutGet(GLUT_SCREEN_WIDTH)-res_width)/2,(glutGet(GLUT_SCREEN_HEIGHT)-res_height)/2);
    glutCreateWindow("Curva de Bezier");
    glutDisplayFunc(Desenha);
    glutReshapeFunc(AlteraTamanhoJanela);
    glutMouseFunc(clique);
    glutMotionFunc(arrasta_ponto);
    Inicializa();
    glutMainLoop();
    free(pts_de_controle);
	return 0;
}