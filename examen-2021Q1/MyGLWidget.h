#include "ExamGLWidget.h"

class MyGLWidget:public ExamGLWidget
{
  Q_OBJECT

  public:
    MyGLWidget(QWidget *parent=0) : ExamGLWidget(parent) {}
    ~MyGLWidget();

  protected:
    virtual void paintGL ();
    virtual void keyPressEvent(QKeyEvent* event);
    virtual void modelTransformCub (float escala, float angle);
    virtual void modelTransformPatricio ();
    virtual void projectTransform ();
    virtual void viewTransform ();
    virtual void iniCamera ();
    virtual void iniEscena ();
    virtual void resizeGL (int width, int height);

  private:
    int printOglError(const char file[], int line, const char func[]);
    
    float anglepat;
    float altura = 2.25f;
    int ver = 0;
    float anglecub1, anglecub2, anglecub3;

    glm::vec3 obs,vrp,up;
    float l,r,b,t;
    bool blanco;

    public slots:
      void cambiaCamara();
      void cambiaPat(int n);

    signals:
    void actualizaDial(int n);

};
