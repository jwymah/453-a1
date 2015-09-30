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

// Define the box's geometry (as triangles), normals, and colour
const float unitCube[] = {

    0.,1.,0.,  1.,1.,0.,  1.,0.,0.,  // back
    0.,1.,0.,  0.,0.,0.,  1.,0.,0.,

    0.,0.,0.,  1.,0.,0.,  0.,0.,1., // bottom
    1.,0.,0.,  1.,0.,1.,  0.,0.,1.,

    0.,1.,0.,  0.,0.,1.,  0.,1.,1., // left
    0.,1.,0.,  0.,0.,0.,  0.,0.,1.,

    0.,1.,0.,  0.,0.,0.,  1.,0.,0., // top
    0.,1.,0.,  0.,0.,0.,  1.,0.,0.,

    1.,1.,0.,  1.,0.,1.,  1.,1.,1., // right
    1.,1.,0.,  1.,0.,0.,  1.,0.,1.,

    1.,1.,1.,  1.,0.,1.,  0.,1.,1., // front
    1.,0.,1.,  0.,0.,1.,  0.,1.,1.,
};

const float cubeNorms[] = {
    0,-1,0,  0,-1,0,  0,-1,0,
    0,-1,0,  0,-1,0,  0,-1,0,

    -1,0,0,  -1,0,0,  -1,0,0,
    -1,0,0,  -1,0,0,  -1,0,0,

    0,0,-1,  0,0,-1,  0,0,-1,
    0,0,-1,  0,0,-1,  0,0,-1,

    0,0,1,  0,0,1,  0,0,1,
    0,0,1,  0,0,1,  0,0,1,

    0,0,1,  0,0,1,  0,0,1,
    0,0,1,  0,0,1,  0,0,1,

    0,0,1,  0,0,1,  0,0,1,
    0,0,1,  0,0,1,  0,0,1,
};

const float cubeColors[] = {
    1,1,1,  1,1,1,  1,1,1, // back
    1,1,1,  1,1,1,  1,1,1,

    1,1,0,  1,1,0,  1,1,0, // bottom
    1,1,0,  1,1,0,  1,1,0,

    1,0,1,  1,0,1,  1,0,1, // left
    1,0,1,  1,0,1,  1,0,1,

    1,0,0,  1,0,0,  1,0,0,  // top
    1,0,0,  1,0,0,  1,0,0,

    0,1,0,  0,1,0,  0,1,0,  // right
    0,1,0,  0,1,0,  0,1,0,

    0,0,1,  0,0,1,  0,0,1,  //front
    0,0,1,  0,0,1,  0,0,1,
};

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
    setupBorderTriangles();

//    setupBox();
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    setupUBorder();
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
//    view_matrix.rotate(30, 1.0, 1.0, 0.0);

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

    drawBorderTriangles();

    drawUBorder();

//    drawBox();

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
        1.0, 0.0, 0.0,
        0.0, 1.0, 0.0,

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

void Renderer::setupBorderTriangles()
{
    generateBorderTriangles();
    long cBufferSize = triColours.size() * sizeof(float),
        vBufferSize = triVertices.size() * sizeof(float),
        nBufferSize = triNormals.size() * sizeof(float);
    borderSize = vBufferSize + cBufferSize + nBufferSize;

    // Initialize VBOs
    glGenBuffers(1, &this->m_triangleVbo);
    glBindBuffer(GL_ARRAY_BUFFER, this->m_triangleVbo);

    // Allocate space
    glBufferData(GL_ARRAY_BUFFER, borderSize, NULL, GL_STATIC_DRAW);

    // Upload the data to the GPU
    glBufferSubData(GL_ARRAY_BUFFER, 0, vBufferSize, &triVertices[0]);
    glBufferSubData(GL_ARRAY_BUFFER, vBufferSize, cBufferSize, &triColours[0]);
    glBufferSubData(GL_ARRAY_BUFFER, vBufferSize + cBufferSize, nBufferSize, &triNormals[0]);

    bindit();
}

void Renderer::drawBorderTriangles()
{
    long cBufferSize = triColours.size() * sizeof(float),
        vBufferSize = triVertices.size() * sizeof(float);

    glBindBuffer(GL_ARRAY_BUFFER, this->m_triangleVbo);

    // Enable the attribute arrays
    glEnableVertexAttribArray(this->m_posAttr);
    glEnableVertexAttribArray(this->m_colAttr);
    glEnableVertexAttribArray(this->m_norAttr);

    glVertexAttribPointer(this->m_posAttr, 3, GL_FLOAT, 0, GL_FALSE, (const GLvoid*)(0));
    glVertexAttribPointer(this->m_colAttr, 3, GL_FLOAT, 0, GL_FALSE, (const GLvoid*)(vBufferSize));
    glVertexAttribPointer(this->m_norAttr, 3, GL_FLOAT, 0, GL_FALSE, (const GLvoid*)(vBufferSize + cBufferSize));

    // Draw the triangles
    glDrawArrays(GL_TRIANGLES, 0, triVertices.size()/3);

    glDisableVertexAttribArray(this->m_posAttr);
    glDisableVertexAttribArray(this->m_colAttr);
    glDisableVertexAttribArray(this->m_norAttr);
}

// override mouse press event
void Renderer::mousePressEvent(QMouseEvent * event)
{
    QTextStream cout(stdout);
    cout << "Stub: Button " << event->button() << " pressed.\n";
    QMatrix4x4 view_matrix;
    view_matrix.translate(0.0f, 0.0f, -40.0f);
    view_matrix.rotate(30, 1.0, 1.0, 0.0);

    glUniformMatrix4fv(m_VMatrixUniform, 1, false, view_matrix.data());

    paintGL();

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

void Renderer::setupBox()
{
    long cBufferSize = sizeof(cubeColors) * sizeof(float),
        vBufferSize = sizeof(unitCube) * sizeof(float),
        nBufferSize = sizeof(cubeNorms) * sizeof(float);

    glGenBuffers(1, &this->m_boxVbo);
    glBindBuffer(GL_ARRAY_BUFFER, this->m_boxVbo);

    // Allocate buffer
    glBufferData(GL_ARRAY_BUFFER, vBufferSize + cBufferSize + nBufferSize, NULL,
        GL_STATIC_DRAW);


//    glm::mat4 trans = glm::rotate(trans, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));

//    glm::mat4 trans = glm::translate(glm::mat4(), glm::vec3(2.0f, 2.0f, 2.0f));

//    for (int i=0; i<108; i+=3)
//    {
//        glm::vec4 result = trans * glm::vec4(unitCube[i], unitCube[i+1], unitCube[i+2], 1.0f);
//        unitCube[i] = result.x;
//        unitCube[i+1] = result.y;
//        unitCube[i+2] = result.z;
//        cout << unitCube[i] << "," << unitCube[i+1] << "," << unitCube[i+2] << endl;
//    }

    // Upload the data to the GPU
    glBufferSubData(GL_ARRAY_BUFFER, 0, vBufferSize, &unitCube[0]);
    glBufferSubData(GL_ARRAY_BUFFER, vBufferSize, cBufferSize, &cubeColors[0]);
    glBufferSubData(GL_ARRAY_BUFFER, vBufferSize + cBufferSize, nBufferSize, &cubeNorms[0]);

    bindit();
}

void Renderer::drawBox()
{
    long cBufferSize = sizeof(cubeColors) * sizeof(float),
        vBufferSize = sizeof(unitCube) * sizeof(float),
        nBufferSize = sizeof(cubeNorms) * sizeof(float);
    boxSize = vBufferSize + cBufferSize + nBufferSize;

    // Bind to the correct context
    glBindBuffer(GL_ARRAY_BUFFER, this->m_boxVbo);

    // Enable the attribute arrays
    glEnableVertexAttribArray(this->m_posAttr);
    glEnableVertexAttribArray(this->m_colAttr);
    glEnableVertexAttribArray(this->m_norAttr);

    // Specifiy where these are in the VBO
    glVertexAttribPointer(this->m_posAttr, 3, GL_FLOAT, 0, GL_FALSE, (const GLvoid*)(0));
    glVertexAttribPointer(this->m_colAttr, 3, GL_FLOAT, 0, GL_FALSE, (const GLvoid*)(vBufferSize));
    glVertexAttribPointer(this->m_norAttr, 3, GL_FLOAT, 0, GL_FALSE, (const GLvoid*)(vBufferSize + cBufferSize));

    // Draw the triangles
    glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices

    glDisableVertexAttribArray(this->m_posAttr);
    glDisableVertexAttribArray(this->m_colAttr);
    glDisableVertexAttribArray(this->m_norAttr);
}

void Renderer::setupUBorder()
{
    long cBufferSize = sizeof(cubeColors) * sizeof(float),
        vBufferSize = sizeof(unitCube) * sizeof(float),
        nBufferSize = sizeof(cubeNorms) * sizeof(float);

    float translatedCube[108];
    for (int k=0; k<22; k++)
    {
        glGenBuffers(1, &this->m_borderUVaos[k]);
        glBindBuffer(GL_ARRAY_BUFFER, this->m_borderUVaos[k]);

            // Allocate buffer
            glBufferData(GL_ARRAY_BUFFER, vBufferSize + cBufferSize + nBufferSize, NULL, GL_STATIC_DRAW);

            // translation matrix
            glm::mat4 trans = glm::translate(glm::mat4(), glm::vec3(-1.0f, (float) k-1, 0.0f));

            for (int i=0; i<108; i+=3)
            {
                glm::vec4 result = trans * glm::vec4(unitCube[i], unitCube[i+1], unitCube[i+2], 1.0f);
                translatedCube[i] = result.x;
                translatedCube[i+1] = result.y;
                translatedCube[i+2] = result.z;
            }

            // Upload the data to the GPU
            glBufferSubData(GL_ARRAY_BUFFER, 0, vBufferSize, &translatedCube[0]);
            glBufferSubData(GL_ARRAY_BUFFER, vBufferSize, cBufferSize, &cubeColors[0]);
            glBufferSubData(GL_ARRAY_BUFFER, vBufferSize + cBufferSize, nBufferSize, &cubeNorms[0]);

            bindit();
    }
    for (int k=0; k<10; k++)
    {
        glGenBuffers(1, &this->m_borderUVaos[k+22]);
        glBindBuffer(GL_ARRAY_BUFFER, this->m_borderUVaos[k+22]);

            // Allocate buffer
            glBufferData(GL_ARRAY_BUFFER, vBufferSize + cBufferSize + nBufferSize, NULL, GL_STATIC_DRAW);

            // translation matrix
            glm::mat4 trans = glm::translate(glm::mat4(), glm::vec3((float) k, -1.0f, 0.0f));

            for (int i=0; i<108; i+=3)
            {
                glm::vec4 result = trans * glm::vec4(unitCube[i], unitCube[i+1], unitCube[i+2], 1.0f);
                translatedCube[i] = result.x;
                translatedCube[i+1] = result.y;
                translatedCube[i+2] = result.z;
            }

            // Upload the data to the GPU
            glBufferSubData(GL_ARRAY_BUFFER, 0, vBufferSize, &translatedCube[0]);
            glBufferSubData(GL_ARRAY_BUFFER, vBufferSize, cBufferSize, &cubeColors[0]);
            glBufferSubData(GL_ARRAY_BUFFER, vBufferSize + cBufferSize, nBufferSize, &cubeNorms[0]);

            bindit();
    }
    for (int k=0; k<22; k++)
    {
        glGenBuffers(1, &this->m_borderUVaos[k+32]);
        glBindBuffer(GL_ARRAY_BUFFER, this->m_borderUVaos[k+32]);

            // Allocate buffer
            glBufferData(GL_ARRAY_BUFFER, vBufferSize + cBufferSize + nBufferSize, NULL, GL_STATIC_DRAW);

            // translation matrix
            glm::mat4 trans = glm::translate(glm::mat4(), glm::vec3(10.0f, (float) k-1, 0.0f));

            for (int i=0; i<108; i+=3)
            {
                glm::vec4 result = trans * glm::vec4(unitCube[i], unitCube[i+1], unitCube[i+2], 1.0f);
                translatedCube[i] = result.x;
                translatedCube[i+1] = result.y;
                translatedCube[i+2] = result.z;
            }

            // Upload the data to the GPU
            glBufferSubData(GL_ARRAY_BUFFER, 0, vBufferSize, &translatedCube[0]);
            glBufferSubData(GL_ARRAY_BUFFER, vBufferSize, cBufferSize, &cubeColors[0]);
            glBufferSubData(GL_ARRAY_BUFFER, vBufferSize + cBufferSize, nBufferSize, &cubeNorms[0]);

            bindit();
    }
}

void Renderer::drawUBorder()
{
    long cBufferSize = sizeof(cubeColors) * sizeof(float),
        vBufferSize = sizeof(unitCube) * sizeof(float),
        nBufferSize = sizeof(cubeNorms) * sizeof(float);
    boxSize = vBufferSize + cBufferSize + nBufferSize;

    for (int k=0; k<54; k++)
    {
        // Bind to the correct context
        glBindBuffer(GL_ARRAY_BUFFER, this->m_borderUVaos[k]);

        // Enable the attribute arrays
        glEnableVertexAttribArray(this->m_posAttr);
        glEnableVertexAttribArray(this->m_colAttr);
        glEnableVertexAttribArray(this->m_norAttr);

        // Specifiy where these are in the VBO
        glVertexAttribPointer(this->m_posAttr, 3, GL_FLOAT, 0, GL_FALSE, (const GLvoid*)(0));
        glVertexAttribPointer(this->m_colAttr, 3, GL_FLOAT, 0, GL_FALSE, (const GLvoid*)(vBufferSize));
        glVertexAttribPointer(this->m_norAttr, 3, GL_FLOAT, 0, GL_FALSE, (const GLvoid*)(vBufferSize + cBufferSize));

        // Draw the triangles
        glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices

        glDisableVertexAttribArray(this->m_posAttr);
        glDisableVertexAttribArray(this->m_colAttr);
        glDisableVertexAttribArray(this->m_norAttr);
    }
}

void Renderer::bindit()
{
    long cBufferSize = sizeof(cubeColors) * sizeof(float),
        vBufferSize = sizeof(unitCube) * sizeof(float);

    //Enable the attribute arrays
    glEnableVertexAttribArray(this->m_posAttr);
    glEnableVertexAttribArray(this->m_colAttr);
    glEnableVertexAttribArray(this->m_norAttr);

    // Specifiy where these are in the VBO
    glVertexAttribPointer(this->m_posAttr, 3, GL_FLOAT, 0, GL_FALSE, (const GLvoid*)(0));
    glVertexAttribPointer(this->m_colAttr, 3, GL_FLOAT, 0, GL_FALSE, (const GLvoid*)(vBufferSize));
    glVertexAttribPointer(this->m_norAttr, 3, GL_FLOAT, 0, GL_FALSE, (const GLvoid*)(vBufferSize + cBufferSize));

    glDisableVertexAttribArray(this->m_posAttr);
    glDisableVertexAttribArray(this->m_colAttr);
    glDisableVertexAttribArray(this->m_norAttr);
}
