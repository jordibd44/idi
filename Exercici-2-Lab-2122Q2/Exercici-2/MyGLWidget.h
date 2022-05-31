#include "LL2GLWidget.h"

#include "model.h"

class MyGLWidget : public LL2GLWidget {
  Q_OBJECT

  public:
    MyGLWidget(QWidget *parent=0) : LL2GLWidget(parent) {}
    ~MyGLWidget();

  private:
    int printOglError(const char file[], int line, const char func[]);
    virtual void paintGL();
    virtual void patrTransform();
    virtual void iniEscena ();
    virtual void iniCamera ();
    virtual void viewTransform ();
    virtual void projectTransform ();
    void paretrans(glm::vec3 pospar, glm::vec3 escalapar);
    virtual void resizeGL(int w, int h);

    virtual void mouseMoveEvent (QMouseEvent *event);
    virtual void keyPressEvent (QKeyEvent *event);
    virtual void rebotaParets ();
    virtual void tractamentGol ();

    glm::vec3 Pmin, Pmax;
    float d,alfaini;

     float l,r,b,t;

    float girPsi, girTheta;
    bool tipocam;
};
