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

MyGLWidget::~MyGLWidget()
{
}

void MyGLWidget::iniEscena()
{
    Pmin = glm::vec3(-10,0,-7);
    Pmax = glm::vec3(10,4,7);

    centreEscena = (Pmax + Pmin)/2.f;
    radiEscena = glm::distance(Pmin,Pmax)/2;
    d = 2.0 * radiEscena;

    //lookat
    vrp = centreEscena;
    obs = vrp + glm::vec3(0,d,0);
    up = glm::vec3(-1,0,0);


    //ortho
    l = -radiEscena;
    r = radiEscena;
    b = -radiEscena;
    t = radiEscena;

    altPorter = 4;
    posPorter = glm::vec3(9.0, 0.0, 0.0);  // posició inicial del porter
    posPilota = glm::vec3(7.0, 0.0, 0.0);  // posició inicial de la pilota
    dirInicialPilota();    // direcció inicial de la pilota

}

void MyGLWidget::iniCamera()
{   
    //euler
    girPsi = 0.0f;
    girTheta = float(0.25*M_PI);

    alfaini = asin(radiEscena/d);
    fov = 2 * alfaini;
    ra  = 1.0;
    znear =  d - radiEscena;
    zfar  = d + radiEscena;

    tipocam = true;
    viewTransform();
    projectTransform();
}
void MyGLWidget::viewTransform()
{
    glm::mat4 View(1.0f);
    if (tipocam){
        View = translate(View,glm::vec3(0,0,-d));
        View = rotate(View,girTheta,glm::vec3(1,0,0));
        View = rotate(View,girPsi,glm::vec3(0,1,0));
        View = translate(View,-centreEscena);
    }
    else{
        View = glm::lookAt (obs, vrp, up);
    }
    glUniformMatrix4fv (viewLoc, 1, GL_FALSE, &View[0][0]);
}

void MyGLWidget::projectTransform()
{
    glm::mat4 Proj(1.0f);
    if (tipocam){
        Proj = glm::perspective (fov, ra, znear, zfar);
    }
    else{
        Proj = glm::ortho(l,r,b,t,znear,zfar);
    }
    glUniformMatrix4fv (projLoc, 1, GL_FALSE, &Proj[0][0]);
}

void MyGLWidget::mouseMoveEvent(QMouseEvent *e)
{
    makeCurrent();

    girTheta += (e->y() - yClick)*0.005;
    girPsi += (e->x() - xClick)*0.005;

    xClick = e->x();
    yClick = e->y();
    viewTransform();
    update ();
}

void MyGLWidget::resizeGL(int w, int h) {
    float raViewport = float(w) / float(h);
    ra = raViewport;
    if (tipocam) {
        if (raViewport < 1.0) {
            fov = 2.0 * atan(tan(alfaini)/raViewport);
        }
    }
    else {
        if (raViewport > 1.0) {
            l = -radiEscena*raViewport;
            r = radiEscena*raViewport;
        }
        else {
            b = -radiEscena/raViewport;
            t = radiEscena/raViewport;
        }
    }
    factorAngleY = M_PI / ample;
    factorAngleX = M_PI / alt;
    projectTransform();
}

void MyGLWidget::keyPressEvent(QKeyEvent* event) 
{
  makeCurrent();
  switch (event->key()) {
    case Qt::Key_Up: { // moviment de la pilota
      if (posPilota[0] == 7.0)
        mourePilota ();
      break;
    }
    case Qt::Key_I: { // reinicia posició pilota
      escalaPil = 0.00851676;
      iniciPilota ();
      break;
    }
    case Qt::Key_Left: { // mueve izquierda
     double limit = 7-(altPorter / 2.0);
      if (posPorter.z < limit){
        posPorter.z += 0.5;
        break;
        patrTransform();
      }
    }
    case Qt::Key_Right: { // mueve derecha
     double limit = 7-(altPorter / 2.0);
      if (posPorter.z > -limit){
        posPorter.z -= 0.5;
        break;
        patrTransform();
      }
    }
    case Qt::Key_C: { // mueve derecha
        tipocam = not tipocam;
        viewTransform();
        projectTransform();
        break;
    }
        case Qt::Key_W: {
        if (altPorter < 6) {
            ++altPorter;
        }
        patrTransform();
        break;
    }
    case Qt::Key_S: {
        if (altPorter > 2) {
            --altPorter;
        }
        patrTransform();
        break;
    }
    case Qt::Key_R: {
        altPorter = 4;
        tipocam = true;
        escalaPil = 0.00851676;
        iniciPilota ();
        mourePilota ();
        viewTransform();
        projectTransform();
        patrTransform();
        break;
    }
    default: event->ignore(); break;
  }
  update();
}

void MyGLWidget::tractamentGol()
{
    escalaPil = 0.0f;
    timer.stop();
} 

void MyGLWidget::rebotaParets() 
{
    if (posPilota[2] >= 5.7){
        dirPilota[2] = -dirPilota[2];
    }
    else if (posPilota[0] <= -8.9){
        dirPilota[0] = -dirPilota[0];
    }
    else if (posPilota[2] <= 5.7){
       dirPilota[2] = -dirPilota[2];
    }
} 

void MyGLWidget::paintGL()
{
    // descomentar per canviar paràmetres
    // glViewport (0, 0, ample, alt);

    // Esborrem el frame-buffer
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Patricio
    glBindVertexArray (VAO_Patr);
    patrTransform();
    glDrawArrays(GL_TRIANGLES, 0, patr.faces().size()*3);

    // Terra
    glBindVertexArray (VAO_Terra);
    identTransform();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    //paret 1
    paretrans(glm::vec3(0,0,-6.9),glm::vec3(20,2,0.2));
    glBindVertexArray (VAO_Cub);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    //paret 2
    paretrans(glm::vec3(0,0,6.9),glm::vec3(20,2,0.2));
    glBindVertexArray (VAO_Cub);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    //paret 3
    paretrans(glm::vec3(-9.9,0,0),glm::vec3(0.2,2,14));
    glBindVertexArray (VAO_Cub);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // Pilota
    glBindVertexArray (VAO_Pil);
    pilTransform();
    glDrawArrays(GL_TRIANGLES, 0, pil.faces().size()*3);

    glBindVertexArray (0);
}

void MyGLWidget::patrTransform()
{
    float escadef = altPorter * escalaPatr;
    glm::mat4 TG(1.0f);
    TG = glm::translate(TG,posPorter);
    TG = glm::rotate(TG,float(-0.5*M_PI),glm::vec3(0.,1.,0.));
    TG = glm::scale(TG,glm::vec3(escadef));
    TG = glm::translate(TG, -centreBasePatr);
    glUniformMatrix4fv(transLoc, 1, GL_FALSE, &TG[0][0]);
}

void MyGLWidget::paretrans(glm::vec3 pospar, glm::vec3 escalapar)
{
    glm::mat4 TG(1.0f);
    TG = translate(TG,pospar);
    TG = scale(TG,escalapar);
    glUniformMatrix4fv(transLoc, 1, GL_FALSE, &TG[0][0]);
}

