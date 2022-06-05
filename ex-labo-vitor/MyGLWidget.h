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

  private:
    int printOglError(const char file[], int line, const char func[]);
    void initializeGL ();
    void iniCamera ();
    void resizeGL (int w, int h) ;
    float raw;
    float fov_ini;
    glm::vec3 obs, vrp, up;
    float left, right, top, bottom, distancia;
    float angle_i, angle_cub[3], anglePat;
    bool pintaCubos, blanco;
    void reset();

    public slots:
      void cambiaCamara();
      void cambiaPat(int n);

    signals:
      void actualizaDial(int n);


};
