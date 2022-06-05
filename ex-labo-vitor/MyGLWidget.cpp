// MyGLWidget.cpp
#include "MyGLWidget.h"
#include <iostream>
#include <stdio.h>

#define printOpenGLError() printOglError(__FILE__, __LINE__)
#define CHECK() printOglError(__FILE__, __LINE__,__FUNCTION__)
#define DEBUG() std::cout << __FILE__ << " " << __LINE__ << " " << __FUNCTION__ << std::endl;

int MyGLWidget::printOglError(const char file[], int line, const char func[]) 
{
    GLenum glErr;
    int    retCode = 0;

    glErr = glGetError();
    const char * error = 0;
    switch (glErr)
    {
        case 0x0500:
            error = "GL_INVALID_ENUM";
            break;
        case 0x501:
            error = "GL_INVALID_VALUE";
            break;
        case 0x502: 
            error = "GL_INVALID_OPERATION";
            break;
        case 0x503:
            error = "GL_STACK_OVERFLOW";
            break;
        case 0x504:
            error = "GL_STACK_UNDERFLOW";
            break;
        case 0x505:
            error = "GL_OUT_OF_MEMORY";
            break;
        default:
            error = "unknown error!";
    }
    if (glErr != GL_NO_ERROR)
    {
        printf("glError in file %s @ line %d: %s function: %s\n",
                             file, line, error, func);
        retCode = 1;
    }
    return retCode;
}

MyGLWidget::~MyGLWidget() {
}

void MyGLWidget::initializeGL ()
{
  // Cal inicialitzar l'ús de les funcions d'OpenGL
  initializeOpenGLFunctions();  

  glClearColor(0.5, 0.7, 1.0, 1.0); // defineix color de fons (d'esborrat)
  glEnable(GL_DEPTH_TEST);
  carregaShaders();
  pintaCubos = true;
  anglePat = 0;
  angle_i = 0;
  angle_cub[0] = 0;
  angle_cub[1] = float(2*M_PI/3);
  angle_cub[2] = 2*float(2*M_PI/3);

  iniEscena ();
  iniCamera ();
}


void MyGLWidget::paintGL ()   // Mètode que has de modificar
{
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Pintem el terra
  glBindVertexArray (VAO_Terra);
  modelTransformTerra ();
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  float scale_i = 2.0f;
  if (pintaCubos) {
    for (int i = 0; i < 3; ++i) {
      glBindVertexArray(VAO_Cub);
      modelTransformCub(scale_i/0.5, angle_cub[i]);
      glDrawArrays(GL_TRIANGLES, 0, 36);

      scale_i += 0.5;
      //angle_i += float(2*M_PI/3);
    }
  }

  glBindVertexArray (VAO_Patr);
  modelTransformPatricio();
  if (!pintaCubos) glDrawArrays(GL_TRIANGLES, 0, patr.faces().size()*3);
}

void MyGLWidget::modelTransformCub (float escala, float angle) 
{
  TG = glm::mat4(1.0f);
  TG = glm::rotate(TG, angle, glm::vec3(0,1,0));
  TG = glm::translate(TG, glm::vec3(5, 0, 0));
  TG = glm::scale(TG, glm::vec3(escala, escala, escala));
  glUniformMatrix4fv (transLoc, 1, GL_FALSE, &TG[0][0]);


  // En aquest mètode has de substituir aquest codi per construir la 
  // transformació geomètrica (TG) del cub usant els paràmetres adientment

}

void MyGLWidget::modelTransformPatricio ()    // Mètode que has de modificar
{
  TG = glm::mat4(1.f);
  TG = glm::rotate(TG, anglePat, glm::vec3(0,1,0));
  TG = glm::translate(TG, glm::vec3(5, 0, 0));
  TG = glm::rotate(TG, float(-M_PI/2), glm::vec3(0,1,0));
  TG = glm::scale(TG, glm::vec3(escala*2, escala*2, escala*2));
  TG = glm::translate(TG, -centreBasePat);
  glUniformMatrix4fv (transLoc, 1, GL_FALSE, &TG[0][0]);
}

void MyGLWidget::iniCamera ()
{
  angleY = 0.65;
  angleX = -1.2;
  camPlanta = false;
  ra=float(width())/height();
  raw = 1.0;
  fov = float(M_PI/3.0);
  fov_ini = fov;
  zn = radiEsc;
  zf = 3*radiEsc;
  distancia = 2*radiEsc;
  obs = glm::vec3(0, distancia, 0);
  vrp = glm::vec3(centreEsc);
  up = glm::vec3(1, 0, 0);
  left = bottom = -radiEsc;
  right = top = radiEsc;
  
  projectTransform ();
  viewTransform ();
}

void MyGLWidget::viewTransform ()    // Mètode que has de modificar
{
  if (!camPlanta)
    ExamGLWidget::viewTransform();
  else
  {
    glm::mat4 View;  // Matriu de projecció
    View = glm::lookAt(obs, vrp, up);
    glUniformMatrix4fv (viewLoc, 1, GL_FALSE, &View[0][0]);
  
  }
}

void MyGLWidget::projectTransform ()
{
  if (!camPlanta)
    ExamGLWidget::projectTransform();
  else
  {
    glm::mat4 Proj;
    Proj = glm::ortho(left, right, bottom, top, zn, zf);
    glUniformMatrix4fv (projLoc, 1, GL_FALSE, &Proj[0][0]);
  }
}

void MyGLWidget::resizeGL (int w, int h) 
{
#ifdef __APPLE__
  // Aquest codi és necessari únicament per a MACs amb pantalla retina.
  GLint vp[4];
  glGetIntegerv (GL_VIEWPORT, vp);
  ample = vp[2];
  alt = vp[3];
#else
  ample = w;
  alt = h;
#endif

  ra = float(ample)/float(alt);
  raw = ra;
  if (!camPlanta) {
    raw = ra;
    if (ra < 1) fov = 2.0 * atan(tan(fov_ini/2.0)/ra);
  }
  else {
    if (raw > 1) {
      left = -raw * radiEsc;
      right = raw * radiEsc;
      bottom = -radiEsc;
      top = radiEsc;
    }
    else if (raw < 1) {
      left = -radiEsc;
      right = radiEsc;
      bottom = -radiEsc/raw;
      top = radiEsc/raw;
    }
	}  
  viewTransform();
  projectTransform();
}

void MyGLWidget::keyPressEvent(QKeyEvent* event) {
  makeCurrent();
  switch (event->key()) {
  case Qt::Key_V: {
      if (pintaCubos) pintaCubos = false;
      else pintaCubos = true;
    break;
	}
  case Qt::Key_1: {
      anglePat = angle_cub[0];
      emit actualizaDial(0);
    break;
	}
  case Qt::Key_2: {
      anglePat = angle_cub[1];
      emit actualizaDial(1);
    break;
	}
  case Qt::Key_3: {
      anglePat = angle_cub[2];
      emit actualizaDial(2);
    break;
	}
  case Qt::Key_F: {
    if (blanco) {
      colFoc = glm::vec3(1,1,1);
      enviaColFocus();
    }
    else {
      colFoc = glm::vec3(1,1,0);
      enviaColFocus();
    }
    break;
	}
  case Qt::Key_C: {
      if (!camPlanta) {
        camPlanta = true;
        viewTransform();
        projectTransform();
      }
      else {
        camPlanta = false;
        viewTransform();
        projectTransform();
      }
    break;
	}
  case Qt::Key_Right: {
      anglePat += float(2*M_PI/3);
      for (int i = 0; i < 3; ++i)
        angle_cub[i] += float(2*M_PI/3);
    break;
	}
  case Qt::Key_Left: {
      anglePat -= float(2*M_PI/3);
      for (int i = 0; i < 3; ++i)
        angle_cub[i] -= float(2*M_PI/3);
    break;
	}
  case Qt::Key_R: {
      reset();
    break;
	}
  default: ExamGLWidget::keyPressEvent(event); break;
  }
  update();
}

void MyGLWidget::reset() {
  initializeGL();
  viewTransform();
  projectTransform();
  emit actualizaDial(0);
}

void MyGLWidget::cambiaCamara() {
  makeCurrent();
  camPlanta = !camPlanta;
  viewTransform();
  projectTransform();
  update();
}

void MyGLWidget::cambiaPat(int n) {
  makeCurrent();
  if (n == 0) anglePat = angle_cub[0];
  else if (n == 1) anglePat = angle_cub[1];
  else if (n == 2) anglePat = angle_cub[2];
  update();
}