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

void MyGLWidget::paintGL ()   // Mètode que has de modificar
{
    //pintem terra
    glBindVertexArray (VAO_Terra);
    modelTransformTerra ();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    
    if (ver == 0){
        //cub 1
        modelTransformCub(4.,0.);
        glBindVertexArray(VAO_Cub);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        
        //cub 2
        modelTransformCub(5.,2.*M_PI/3);
        glBindVertexArray(VAO_Cub);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        
        //cub 3
        modelTransformCub(6.,2.*(2.*M_PI/3));
        glBindVertexArray(VAO_Cub);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        
    // Pintem el Patricio
        modelTransformPatricio ();
        glBindVertexArray (VAO_Patr);
        glDrawArrays(GL_TRIANGLES, 0, patr.faces().size()*3);
    }
    else if (ver == 1){
        // Pintem el Patricio
        modelTransformPatricio ();
        glBindVertexArray (VAO_Patr);
        glDrawArrays(GL_TRIANGLES, 0, patr.faces().size()*3);
        
    }
    else {
        //cub 1
        modelTransformCub(4.,anglecub1);
        glBindVertexArray(VAO_Cub);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        
        //cub 2
        modelTransformCub(5.,anglecub2);
        glBindVertexArray(VAO_Cub);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        
        //cub 3
        modelTransformCub(6.,anglecub3);
        glBindVertexArray(VAO_Cub);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
     }
}

void MyGLWidget::iniEscena ()
{
  anglecub1 = 0.;
  anglecub2 = (2.*M_PI/3);
  anglecub3 = (2.*(2.*M_PI/3));
  anglepat = 0.0;
  ExamGLWidget::iniEscena ();
}

void MyGLWidget::modelTransformCub (float escala, float angle) 
{
    TG = glm::mat4(1.f);
    TG = glm::rotate(TG,angle,glm::vec3(0,1.,0.));
    TG = glm::translate(TG,glm::vec3(5.,0.,0.));
    TG = glm::scale(TG,glm::vec3(escala));
    glUniformMatrix4fv (transLoc, 1, GL_FALSE, &TG[0][0]);
}

void MyGLWidget::modelTransformPatricio ()    // Mètode que has de modificar
{
    TG = glm::mat4(1.f);
    TG = glm::rotate(TG,anglepat,glm::vec3(0.,1.,0.));
    TG = glm::translate(TG, glm::vec3(5.,altura,1.));
    TG = glm::rotate(TG,float(2*M_PI/4 ),glm::vec3(-1.,0.,0.));
    TG = glm::scale(TG, glm::vec3(escala*2));
    TG = glm::translate(TG, -centreBasePat);
  
    glUniformMatrix4fv (transLoc, 1, GL_FALSE, &TG[0][0]);
}

void MyGLWidget::iniCamera ()
{
    camPlanta = false; // tipo camara
    blanco = true;

    //lookat
    obs = centreEsc + glm::vec3(0,2*radiEsc,0);
    vrp = centreEsc;
    up = glm::vec3(1,0,0);

    //ortho
    l = -radiEsc;
    r = radiEsc;
    b = -radiEsc;
    t = radiEsc;

    ExamGLWidget::iniCamera();
}

void MyGLWidget::resizeGL (int w, int h) 
{
  ample = w;
  alt = h;
  ra = float(ample)/float(alt);
  if (camPlanta){
    if (ra > 1.0){
      l = -radiEsc*ra;
      r = radiEsc*ra;
    }
    else{
      b = -radiEsc/ra;
      t = radiEsc/ra;
    }
  }
  projectTransform();
}

void MyGLWidget::viewTransform ()    // Mètode que has de modificar
{
  if (!camPlanta)
    ExamGLWidget::viewTransform();
  else
  {
    // Codi per a la viewMatrix de la Càmera-2
    View = glm::lookAt(obs,vrp,up);

    glUniformMatrix4fv (viewLoc, 1, GL_FALSE, &View[0][0]);
  }
}

void MyGLWidget::projectTransform ()
{
  if (!camPlanta)
    ExamGLWidget::projectTransform();
  else
  {
    // Codi per a la projectMatrix de la Càmera-2
    glm::mat4 Proj;  // Matriu de projecció
    Proj = glm::ortho(l,r,b,t,zn,zf);
    glUniformMatrix4fv (projLoc, 1, GL_FALSE, &Proj[0][0]);
  }
}

void MyGLWidget::keyPressEvent(QKeyEvent* event) {
  makeCurrent();
  switch (event->key()) {
  case Qt::Key_V: {
     ver = (ver+1)%3;
    break;
	}
  case Qt::Key_1: {
      anglepat = 0.0;
      altura = 2.25f;
      emit actualizaDial(0);
    break;
	}
  case Qt::Key_2: {
      anglepat = (2*M_PI/3);
      altura = 2.75f;
      emit actualizaDial(1);
    break;
	}
  case Qt::Key_3: {
      anglepat = 2*(2*M_PI/3);
      altura = 3.25f;
      emit actualizaDial(2);
    break;
	}
  case Qt::Key_F: {
    blanco = not blanco;
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
      camPlanta = not camPlanta;
      viewTransform();
      projectTransform();
    break;
	}
  case Qt::Key_Right: {

    anglecub1 += (2.*M_PI/3);     //cub 1

    anglecub2 += (2.*M_PI/3);     //cub 2

    anglecub3 += (2.*M_PI/3);     //cub 3

    anglepat += (2.*M_PI/3);     //pat
    break;
	}
  case Qt::Key_Left: {
    //cub 1
    anglecub1 -= (2.*M_PI/3);     //cub 1

    anglecub2 -= (2.*M_PI/3);     //cub 2

    anglecub3 -= (2.*M_PI/3);     //cub 3

    anglecub3 -= (2.*M_PI/3);     //pat
    break;
	}
  case Qt::Key_R: {
    anglecub1 = 0.;
    anglecub2 = (2.*M_PI/3);
    anglecub3 = (2.*(2.*M_PI/3));
    anglepat = 0.0;
    altura = 2.25f;
    angleY = 0.65;
    angleX = -1.2;
    camPlanta = false;
    ver = 0;
    projectTransform();
    viewTransform();
    break;
	}
  default: ExamGLWidget::keyPressEvent(event); break;
  }
  update();
}

void MyGLWidget::cambiaCamara()
{
  makeCurrent();
  camPlanta = not camPlanta;
  viewTransform();
  projectTransform();
  update();
}

void MyGLWidget::cambiaPat(int n)
{
  makeCurrent();
  if (n == 0) anglepat = 0.0;
  else if (n == 1) anglepat = (2.*M_PI/3);
  else if (n == 2) anglepat = (2.*(2.*M_PI/3));
  update();
}