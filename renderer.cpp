#include "renderer.h"
#include <QTextStream>
#include <QOpenGLBuffer>
#include <cmath>
#include <iostream>

using namespace std;

// constructor
Renderer::Renderer(QWidget *parent)
    : QOpenGLWidget(parent)
{

}

// constructor
Renderer::~Renderer()
{

}

// called once by Qt GUI system, to allow initialization for OpenGL requirements
void Renderer::initializeGL()
{
    // Qt support for inline GL function calls
	initializeOpenGLFunctions();

    // sets the background clour
    glClearColor(0.7f, 0.7f, 1.0f, 1.0f);

    // links to and compiles the shaders, used for drawing simple objects
    m_program = new QOpenGLShaderProgram(this);
    m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, "per-fragment-phong.vs.glsl");
    m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, "per-fragment-phong.fs.glsl");
    m_program->link();
    m_posAttr = m_program->attributeLocation("position_attr");
    m_colAttr = m_program->attributeLocation("colour_attr");
    m_norAttr = m_program->attributeLocation("normal_attr");
    m_PMatrixUniform = m_program->uniformLocation("proj_matrix");
    m_VMatrixUniform = m_program->uniformLocation("view_matrix");
    m_MMatrixUniform = m_program->uniformLocation("model_matrix");
    m_programID = m_program->programId();

    // Setup the triangles
    generateBorderTriangles();
    long cBufferSize = triColours.size() * sizeof(float),
        vBufferSize = triVertices.size() * sizeof(float),
        nBufferSize = triNormals.size() * sizeof(float);

    // Initialize VBOs
    glGenBuffers(1, &this->m_triangleVbo);
    glBindBuffer(GL_ARRAY_BUFFER, this->m_triangleVbo);

    // Allocate space
    glBufferData(GL_ARRAY_BUFFER,
                 vBufferSize + cBufferSize + nBufferSize,
                 NULL,
                 GL_STATIC_DRAW);

    // Uplaod the data to the PGU
    glBufferSubData(GL_ARRAY_BUFFER, 0, vBufferSize, &triVertices[0]);
    glBufferSubData(GL_ARRAY_BUFFER, vBufferSize, cBufferSize, &triColours[0]);
    glBufferSubData(GL_ARRAY_BUFFER, vBufferSize + cBufferSize, nBufferSize, &triNormals[0]);

    //Enable the attribute arrays
    glEnableVertexAttribArray(this->m_posAttr);
    glEnableVertexAttribArray(this->m_colAttr);
    glEnableVertexAttribArray(this->m_norAttr);

    // Specify where these are in the VBO
    glVertexAttribPointer(this->m_posAttr, 3, GL_FLOAT, 0, GL_FALSE, (const GLvoid*)0);
    glVertexAttribPointer(this->m_colAttr, 3, GL_FLOAT, 0, GL_FALSE, (const GLvoid*)(vBufferSize));
    glVertexAttribPointer(this->m_norAttr, 3, GL_FLOAT, 0, GL_FALSE, (const GLvoid*)(vBufferSize + cBufferSize));

    glDisableVertexAttribArray(this->m_posAttr);
    glDisableVertexAttribArray(this->m_colAttr);
    glDisableVertexAttribArray(this->m_norAttr);

    //documentation glVertexAttribPointer, gl(enable/disable)vertexAttribArray
}

// called by the Qt GUI system, to allow OpenGL drawing commands
void Renderer::paintGL()
{
    // Clear the screen buffers

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set the current shader program

    glUseProgram(m_programID);

    // Modify the current projection matrix so that we move the
    // camera away from the origin.  We'll draw the game at the
    // origin, and we need to back up to see it.

    QMatrix4x4 view_matrix;
    view_matrix.translate(0.0f, 0.0f, -40.0f);
    glUniformMatrix4fv(m_VMatrixUniform, 1, false, view_matrix.data());

    // Not implemented: set up lighting (if necessary)

    // Not implemented: scale and rotate the scene

    QMatrix4x4 model_matrix;

    // You'll be drawing unit cubes, so the game will have width
    // 10 and height 24 (game = 20, stripe = 4).  Let's translate
    // the game so that we can draw it starting at (0,0) but have
    // it appear centered in the window.

    model_matrix.translate(-5.0f, -12.0f, 0.0f);
    glUniformMatrix4fv(m_MMatrixUniform, 1, false, model_matrix.data());

    // Not implemented: actually draw the current game state.
    // Here's some test code that draws red triangles at the
    // corners of the game board.

    glBindBuffer(GL_ARRAY_BUFFER, this->m_triangleVbo);

    // Enable the attribute arrays
    glEnableVertexAttribArray(this->m_posAttr);
    glEnableVertexAttribArray(this->m_colAttr);
    glEnableVertexAttribArray(this->m_norAttr);

    // Draw the triangles
    glDrawArrays(GL_TRIANGLES, 0, triVertices.size()/3);

    glDisableVertexAttribArray(this->m_norAttr);
    glDisableVertexAttribArray(this->m_colAttr);
    glDisableVertexAttribArray(this->m_posAttr);

//    generateBorderTriangles();

//    // draw border
//    if (triVertices.size() > 0)
//    {
//        // pass in the list of vertices and their associated colours
//        // 3 coordinates per vertex, or per colour
//        glVertexAttribPointer(m_posAttr, 3, GL_FLOAT, GL_FALSE, 0, &triVertices[0]);
//        glVertexAttribPointer(m_colAttr, 3, GL_FLOAT, GL_FALSE, 0, &triColours[0]);
//        glVertexAttribPointer(m_norAttr, 3, GL_FLOAT, GL_FALSE, 0, &triNormals[0]);

//        glEnableVertexAttribArray(m_posAttr);
//        glEnableVertexAttribArray(m_colAttr);
//        glEnableVertexAttribArray(m_norAttr);

//        // draw triangles
//        glDrawArrays(GL_TRIANGLES, 0, triVertices.size()/3); // 3 coordinates per vertex

//        glDisableVertexAttribArray(m_norAttr);
//        glDisableVertexAttribArray(m_colAttr);
//        glDisableVertexAttribArray(m_posAttr);
//    }

//    generateCube();

//    glVertexAttribPointer(m_posAttr, 3, GL_FLOAT, GL_FALSE, 0, &vertices[0]);
//    glVertexAttribPointer(m_colAttr, 3, GL_FLOAT, GL_FALSE, 0, &colours[0]);
//    glVertexAttribPointer(m_norAttr, 3, GL_FLOAT, GL_FALSE, 0, &normals[0]);

//    glEnableVertexAttribArray(m_posAttr);
//    glEnableVertexAttribArray(m_colAttr);
//    glEnableVertexAttribArray(m_norAttr);

//    // draw triangles
//    glDrawArrays(GL_TRIANGLES, 0, vertices.size()/3); // 3 coordinates per vertex

//    glDisableVertexAttribArray(m_norAttr);
//    glDisableVertexAttribArray(m_colAttr);
//    glDisableVertexAttribArray(m_posAttr);

    // deactivate the program
    m_program->release();

}

// called by the Qt GUI system, to allow OpenGL to respond to widget resizing
void Renderer::resizeGL(int w, int h)
{
    // width and height are better variables to use
    Q_UNUSED(w); Q_UNUSED(h);

    // update viewing projections
    glUseProgram(m_programID);

    // Set up perspective projection, using current size and aspect
    // ratio of display
    QMatrix4x4 projection_matrix;
    projection_matrix.perspective(40.0f, (GLfloat)width() / (GLfloat)height(),
                                  0.1f, 1000.0f);
    glUniformMatrix4fv(m_PMatrixUniform, 1, false, projection_matrix.data());

    glViewport(0, 0, width(), height());

}

// computes the vertices and corresponding colours-per-vertex for a quadrilateral
// drawn from (x1, y1) to (x2, y2)
// Note: the magic numbers in the vector insert commands should be better documented
void Renderer::generateBorderTriangles()
{
    // make sure array lists are clear to start with
    triVertices.clear();
    triColours.clear();

    // add vertices to rectangle list
    float vectList [] = {
        0.0, 0.0, 0.0,  // bottom left triangle
        5.0, 0.0, 0.0,
        0.0, 5.0, 0.0,

        9.0, 0.0, 0.0,  // bottom right triangle
        10.0, 0.0, 0.0,
        10.0, 1.0, 0.0,

        0.0, 19.0, 0.0, // top left triangle
        1.0, 20.0, 0.0,
        0.0, 20.0, 0.0,

        10.0, 19.0, 0.0,    // top right triangle
        10.0, 20.0, 0.0,
        9.0, 20.0, 0.0 };
    triVertices.insert(triVertices.end(), vectList, vectList + 3*4*3); // 36 items in array

    // shader supports per-vertex colour; add colour for each vertex add colours to colour list - use current colour
    QColor borderColour = Qt::red;
    float colourList [] = { (float)borderColour.redF(), (float)borderColour.greenF(), (float)borderColour.blueF() };
    float normalList [] = { 0.0f, 0.0f, 1.0f }; // facing viewer
    for (int v = 0; v < 4 * 3; v++)
    {
        triColours.insert(triColours.end(), colourList, colourList + 3); // 3 coordinates per vertex
        triNormals.insert(triNormals.end(), normalList, normalList + 3); // 3 coordinates per vertex
    }

}

void Renderer::generateCube()
{
    // use point (x,y,z) with width w to determine cube
    float x = 0.0;
    float y = 0.0;
    float z = 0.0;
    float w = 1.0;

    vertices.clear();
    colours.clear();

    // board is 20 tall
    // and 10 wide
    // and 1 depth?
    float vectList [] = {
        // back goes counter clockwise
        x+w, y, z-w,
        x, y+w, z-w,
        x,   y, z-w,
        // front vertice order goes clockwise
        x,   y, z,
        x, y+w, z,
        x+w, y, z,

        x+w, y+w, z,
        x+w, y, z,
        x, y+w, z
    };

    vertices.insert(vertices.end(), vectList, vectList + 3*3*3);
    QColor cubeColor = Qt::cyan;

    float colourList [] = { (float)cubeColor.redF(), (float)cubeColor.greenF(), (float)cubeColor.blueF() };
    float normalList [] = { 0.0f, 0.0f, 1.0f }; // facing viewer
    for (int v = 0; v < 3; v++)
    {
        colours.insert(colours.end(), colourList, colourList + 3); // 3 coordinates per vertex
        normals.insert(normals.end(), normalList, normalList + 3); // 3 coordinates per vertex
    }
}

// override mouse press event
void Renderer::mousePressEvent(QMouseEvent * event)
{
    QTextStream cout(stdout);
    cout << "Stub: Button " << event->button() << " pressed.\n";
}

// override mouse release event
void Renderer::mouseReleaseEvent(QMouseEvent * event)
{
    QTextStream cout(stdout);
    cout << "Stub: Button " << event->button() << " pressed.\n";
}

// override mouse move event
void Renderer::mouseMoveEvent(QMouseEvent * event)
{
    QTextStream cout(stdout);
    cout << "Stub: Motion at " << event->x() << ", " << event->y() << ".\n";
}
