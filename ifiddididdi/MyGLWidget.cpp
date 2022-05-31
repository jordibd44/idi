#include "MyGLWidget.h"

#include <iostream>

MyGLWidget::MyGLWidget (QWidget* parent) : QOpenGLWidget(parent), program(NULL)
{
  setFocusPolicy(Qt::StrongFocus);  // per rebre events de teclat
}

MyGLWidget::~MyGLWidget ()
{
  if (program != NULL)
    delete program;
}

void MyGLWidget::initializeGL ()
{
  // Cal inicialitzar l'ús de les funcions d'OpenGL
  initializeOpenGLFunctions();
  
  glClearColor (0.5, 0.7, 1.0, 1.0); // defineix color de fons (d'esborrat)
  carregaShaders();
  creaBuffersQuadrat();
  creaBuffersTriangle();
}

void MyGLWidget::pintaMuntanya ()  // Caldrà modificar aquest mètode per pintar la muntanya
{
  //muntanya
  modelTransformMuntanya();
  colObj = glm::vec3(0.5,0.3,0.1);
  glUniform3fv (colorLoc, 1, &colObj[0]);
  glBindVertexArray(VAOTriangle);
  glDrawArrays(GL_TRIANGLES, 0, 3);

  //base
  modelTransformBaseGronxador();
  colObj = glm::vec3(0.7,0.,0.);
  glUniform3fv (colorLoc, 1, &colObj[0]);
  glBindVertexArray(VAOTriangle);
  glDrawArrays(GL_TRIANGLES, 0, 3);

  //nieve
  modelTransformNeu();
  colObj = glm::vec3(1.,1.,1.);
  glUniform3fv (colorLoc, 1, &colObj[0]);
  glBindVertexArray(VAOTriangle);
  glDrawArrays(GL_TRIANGLES, 0, 3);
}

void MyGLWidget::pintaGronxador ()  
{ // Caldrà modificar aquest mètode per pintar el gronxador sencer, la barra i els dos seients
  // pintem un quadrat
  //polla gorda
  modelTransformBarraGronxador();
  colObj = glm::vec3(0.2,0.7,0.2);
  glUniform3fv (colorLoc, 1, &colObj[0]);
  glBindVertexArray(VAOQuadrat);
  glDrawArrays(GL_TRIANGLES, 0, 6);

  //seient dreta

  modelTransformSeient(glm::vec3(0.6,0.5,0.));
  colObj = glm::vec3(0.4);
  glUniform3fv (colorLoc, 1, &colObj[0]);
  glBindVertexArray(VAOQuadrat);
  glDrawArrays(GL_TRIANGLES, 0, 6);

  //seient esquerra

  modelTransformSeient(glm::vec3(-0.6,0.5,0.));
  colObj = glm::vec3(0.4);
  glUniform3fv (colorLoc, 1, &colObj[0]);
  glBindVertexArray(VAOQuadrat);
  glDrawArrays(GL_TRIANGLES, 0, 6);
}

void MyGLWidget::paintGL ()
{
// En cas de voler canviar els paràmetres del viewport, descomenteu la crida següent i
// useu els paràmetres que considereu (els que hi ha són els de per defecte)
//  glViewport (0, 0, ample, alt);
  
  glClear (GL_COLOR_BUFFER_BIT);  // Esborrem el frame-buffer

  // Pintem muntanya
  pintaMuntanya();
  
  // Pintem gronxador
  pintaGronxador();
  
  // Desactivem el VAO
  glBindVertexArray(0);
}

void MyGLWidget::resizeGL (int w, int h)
{
// Aquest codi és necessari únicament per a MACs amb pantalla retina.
#ifdef __APPLE__
  GLint vp[4];
  glGetIntegerv (GL_VIEWPORT, vp);
  ample = vp[2];
  alt = vp[3];
#else
  ample = w;
  alt = h;
#endif
}

void MyGLWidget::keyPressEvent(QKeyEvent* event)
{
  makeCurrent();
  switch (event->key()) {
   case Qt::Key_Left:
      if (angleGir < 25){
        angleGir += 5.0;
        paintGL();
      } 
      break;
    case Qt::Key_Right:
      if (angleGir > -25){ 
        angleGir -= 5.0;
        paintGL();}
      break;
    default: event->ignore(); break;
  }
  update();
}

void MyGLWidget::modelTransformMuntanya ()
{
  TG = glm::mat4(1.0);
  TG = glm::translate(TG,glm::vec3(-1,-1,0.));
  TG = glm::scale(TG,glm::vec3(1.6,1.8,0.0));
  glUniformMatrix4fv(transLoc,1,GL_FALSE,&TG[0][0]); 
}

void MyGLWidget::modelTransformBaseGronxador()
{
  TG = glm::mat4(1.0);
  TG = glm::translate(TG,glm::vec3(-0.1,-1,0.));
  TG = glm::scale(TG,glm::vec3(0.2,0.3,0.0));
  glUniformMatrix4fv(transLoc,1,GL_FALSE,&TG[0][0]);
}

void MyGLWidget::modelTransformBarraGronxador()
{
  angleRadians = angleGir * M_PI / 180.0;
  TG = glm::mat4(1.0);
  TG = glm::translate(TG,glm::vec3(0.,-0.7,0.));
  TG = glm::rotate(TG,angleRadians,glm::vec3(0.0, 0.0, 1.0));
  TG = glm::translate(TG,glm::vec3(0.,0.1,0.));
  TG = glm::scale(TG,glm::vec3(2.8,0.4,0.));
  TG = glm::translate(TG,glm::vec3(0.25,0.25,0.));
  glUniformMatrix4fv(transLoc,1,GL_FALSE,&TG[0][0]);
}

void MyGLWidget::modelTransformSeient(glm::vec3 posBase)
{
  angleRadians = angleGir * M_PI / 180.0;
  TG = glm::mat4(1.0);
  TG = glm::translate(TG,glm::vec3(0.,posBase.y-1.2,0.0));
  TG = glm::rotate(TG,angleRadians,glm::vec3(0.0, 0.0, 1.0));
  TG = glm::translate(TG,glm::vec3(posBase.x,0.25,0.));
  //lo escalamos
  TG = glm::scale(TG,glm::vec3(0.4,0.2,0.));
  //trasladamos al centro del cuadrado sin escalar
  TG = glm::translate(TG,glm::vec3(0.25,0.25,0.));

  glUniformMatrix4fv(transLoc,1,GL_FALSE,&TG[0][0]);
}

void MyGLWidget::modelTransformNeu()
{
  TG = glm::mat4(1.0);
  TG = glm::translate(TG,glm::vec3(-0.4,0.4,0.));
  TG = glm::scale(TG,glm::vec3(0.4,0.4,0.));
  glUniformMatrix4fv(transLoc,1,GL_FALSE,&TG[0][0]);
}

void MyGLWidget::creaBuffersQuadrat ()
{
  glm::vec3 Vertices[6];  // vèrtexs amb X, Y i Z
  Vertices[0] = glm::vec3(-0.5, -0.5, 0);
  Vertices[1] = glm::vec3( 0, -0.5, 0);
  Vertices[2] = glm::vec3(-0.5, 0, 0);
  Vertices[3] = glm::vec3( 0, -0.5, 0);
  Vertices[4] = glm::vec3( 0, 0, 0);
  Vertices[5] = glm::vec3( -0.5, 0, 0);
  
  // Creació del Vertex Array Object (VAO) que usarem per pintar el tronc
  glGenVertexArrays(1, &VAOQuadrat);
  glBindVertexArray(VAOQuadrat);

  // Creació del buffer amb les dades dels vèrtexs
  GLuint VBO;
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
  // Activem l'atribut que farem servir per vèrtex (només el 0 en aquest cas)	
  glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vertexLoc);

  // Desactivem el VAO
  glBindVertexArray(0);
}

void MyGLWidget::creaBuffersTriangle ()
{
  glm::vec3 Vertices[3];  // Quatre vèrtexs amb X, Y i Z
  Vertices[0] = glm::vec3( 0, 0, 0);
  Vertices[1] = glm::vec3( 1, 0, 0);
  Vertices[2] = glm::vec3( 0.5, 1, 0);

  // Creació del Vertex Array Object (VAO) que usarem per pintar el tronc
  glGenVertexArrays(1, &VAOTriangle);
  glBindVertexArray(VAOTriangle);

  // Creació del buffer amb les dades dels vèrtexs
  GLuint VBO;
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
  // Activem l'atribut que farem servir per vèrtex (només el 0 en aquest cas)
  glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vertexLoc);

  // Desactivem el VAO
  glBindVertexArray(0);
}

void MyGLWidget::carregaShaders()
{
  // Creem els shaders per al fragment shader i el vertex shader
  QOpenGLShader fs (QOpenGLShader::Fragment, this);
  QOpenGLShader vs (QOpenGLShader::Vertex, this);
  // Carreguem el codi dels fitxers i els compilem
  fs.compileSourceFile("shaders/basicShader.frag");
  vs.compileSourceFile("shaders/basicShader.vert");
  // Creem el program
  program = new QOpenGLShaderProgram(this);
  // Li afegim els shaders corresponents
  program->addShader(&fs);
  program->addShader(&vs);
  // Linkem el program
  program->link();
  // Indiquem que aquest és el program que volem usar
  program->bind();

  // Obtenim identificador per a l'atribut “vertex” del vertex shader
  vertexLoc = glGetAttribLocation (program->programId(), "vertex");

  // Obtenim els identificadors dels uniforms
  transLoc = glGetUniformLocation(program->programId(),"TG");
  colorLoc = glGetUniformLocation(program->programId(), "color");
}
