//////////////////////////////////////////////////////////////////////////////
//
//  Gouraud.cpp
//
//////////////////////////////////////////////////////////////////////////////

#include <vermilion.h>
#include "vapp.h"
#include "LoadShaders.h"

enum VAO_IDs { Triangles, NumVAOs };
enum Buffer_IDs { ArrayBuffer, NumBuffers };
enum Attrib_IDs { vPosition = 0, vColor = 1 };

GLuint  VAOs[NumVAOs];
GLuint  Buffers[NumBuffers];

const GLuint  NumVertices = 6;

BEGIN_APP_DECLARATION(GouraudExample)
    virtual void Initialize(const char * title);
    virtual void Display(bool auto_redraw);
    virtual void OnKey(int key, int scancode, int action, int mods);
END_APP_DECLARATION()

DEFINE_APP(GouraudExample, "Gouraud Example")

//----------------------------------------------------------------------------
//
// init
//

void GouraudExample::Initialize(const char * title)
{
    base::Initialize(title);

    // very stranger name using  :NumVAOs ,Triangles
    glGenVertexArrays( NumVAOs, VAOs );
    glBindVertexArray( VAOs[Triangles] );

    struct VertexData {
        GLubyte color[4];
        GLfloat position[4];
    };

    VertexData vertices[NumVertices] = {
        {{ 255,   0,   0, 255 }, { -0.90f, -0.90f }},  // Triangle 1
        {{   0, 255,   0, 255 }, {  0.85f, -0.90f }},
        {{   0,   0, 255, 255 }, { -0.90f,  0.85f }},
        {{  10,  10,  10, 255 }, {  -0.40f, -0.85f }},  // Triangle 2
        {{ 100, 100, 100, 255 }, {  0.90f,  0.90f }},
        {{ 255, 255, 255, 255 }, { -0.85f,  0.90f }}
    };

    // same here
    glGenBuffers( NumBuffers, Buffers );
    glBindBuffer( GL_ARRAY_BUFFER, Buffers[ArrayBuffer] );
    glBufferData( GL_ARRAY_BUFFER, sizeof(vertices),
                  vertices, GL_STATIC_DRAW );

    ShaderInfo  shaders[] = {
        { GL_VERTEX_SHADER, "media/shaders/gouraud/gouraud.vert" },
        { GL_FRAGMENT_SHADER, "media/shaders/gouraud/gouraud.frag" },
        { GL_NONE, NULL }
    };

    GLuint program = LoadShaders( shaders );
    glUseProgram( program );

    glVertexAttribPointer( vColor, 4, GL_UNSIGNED_BYTE,
                           GL_TRUE, sizeof(VertexData), BUFFER_OFFSET(0) );
    glVertexAttribPointer( vPosition, 2, GL_FLOAT,
                           GL_FALSE, sizeof(VertexData),
                           BUFFER_OFFSET(sizeof(vertices[0].color)) );

    glEnableVertexAttribArray( vColor );
    glEnableVertexAttribArray( vPosition );

    // Set the stencil's clear value
    glClearStencil(0xFF);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_ALWAYS, 0x1, 0xFF);
    glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE);
}

void GouraudExample::OnKey(int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        switch (key)
        {
            case GLFW_KEY_M:
            {
                static GLenum mode = GL_FILL;

                mode = (mode == GL_FILL ? GL_LINE : GL_FILL);
                glPolygonMode(GL_FRONT_AND_BACK, mode);
                return;
            }
        }
    }

    base::OnKey(key, scancode, action, mods);
}

//----------------------------------------------------------------------------
//
// display
//

void GouraudExample::Display(bool auto_redraw)
{
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |GL_STENCIL_BUFFER_BIT);

    glBindVertexArray( VAOs[Triangles] );

    // draw sphere where the stencil is 1
    glStencilFunc(GL_ALWAYS, 0x1, 0xFF);
//    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glDrawArrays( GL_TRIANGLES, 3, 6 );

    glStencilFunc(GL_EQUAL, 0x1, 0xFF);
    glStencilMask(0xFF);
    glDisable(GL_DEPTH_TEST);
    glDrawArrays( GL_TRIANGLES, 0, 3 );

    // Done
    base::Display();
}
