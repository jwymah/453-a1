/*
 * CPSC 453 - Introduction to Computer Graphics
 * Assignment 1
 *
 * Renderer - OpenGL widget for drawing scene
 */

#ifndef RENDERER_H
#define RENDERER_H

#include <QWidget>
#include <QOpenGLWidget>
//#include <QOpenGLFunctions>
#include <QOpenGLFunctions_4_2_Core>
#include <QMatrix4x4>
#include <QOpenGLShaderProgram>
#include <QOpenGLShader>
#include <QMouseEvent>
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"

using namespace std;

class Renderer : public QOpenGLWidget, protected QOpenGLFunctions_4_2_Core
{

    // informs the qmake that a Qt moc_* file will need to be generated
    Q_OBJECT

public:
    // constructor
    Renderer(QWidget *parent = 0);

    // destructor
    virtual ~Renderer();

    void drawBorder();
    void setupBorder();
    void rotate10();
protected:

    // override fundamental drawing functions

    // Called when OpenGL is first initialized
    void initializeGL();

    // Called when the window is resized
    void resizeGL(int w, int h);

    // Called when the window needs to be redrawn
    void paintGL();

    // override mouse event functions

    // Called when a mouse button is pressed
    virtual void mousePressEvent(QMouseEvent * event);

    // Called when a mouse button is released
    virtual void mouseReleaseEvent(QMouseEvent * event);

    // Called when the mouse moves
    virtual void mouseMoveEvent(QMouseEvent * event);

private:

    // member variables for shader manipulation
    GLuint m_programID;
    GLuint m_MMatrixUniform; // model matrix
    GLuint m_VMatrixUniform; // view matrix
    GLuint m_PMatrixUniform; // projection matrix

    GLuint m_triangleVbo;
    GLuint m_posAttr;
    GLuint m_colAttr;
    GLuint m_norAttr;

    GLuint m_boxVbo;

    GLuint m_boxVao;
    GLuint m_triangleVao;

    GLuint m_borderUVaos[54];

    QOpenGLShaderProgram *m_program;

    // for storing triangle vertices and colours
    vector<GLfloat> triVertices;
    vector<GLfloat> triColours;
    vector<GLfloat> triNormals;

    vector<GLfloat> vertices;
    vector<GLfloat> colours;
    vector<GLfloat> normals;

    long boxSize;
    long borderSize;

    int rotationOnX;

    // helper function for loading shaders
    GLuint loadShader(GLenum type, const char *source);

    // helper function for drawing bordering triangles
    void generateBorderTriangles();

    void setupBorderTriangles();
    void drawBorderTriangles();

    void setupBox();
    void drawBox();

    void setupUBorder();
    void drawUBorder();

    void bindit();

};

#endif // RENDERER_H
