#include <iostream>
#include <GL\glew.h>
#include <GL\freeglut.h>
#include "cTexture.h"
#include "cTimer.h"
#include "glslprogram.h"
#include "vbomesh.h"
#include "defines.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform2.hpp>
#include "Planet.h"
using glm::mat4;
using glm::vec3;
using namespace std;

GLSLProgram prog;
VBOMesh *sphere;

mat4 model;
mat4 view;
mat4 projection;

float angle;
cTimer translationTimer;

Planet *earth;
Planet *moon;

void init(void)
{
    if( !prog.compileShaderFromFile("shader/120/texture.vs",GLSLShader::VERTEX) )
    {
        printf("Vertex shader failed to compile!\n%s",
               prog.log().c_str());
        exit(1);
    }
    if( ! prog.compileShaderFromFile("shader/120/texture.fs",GLSLShader::FRAGMENT))
    {
        printf("Fragment shader failed to compile!\n%s",
               prog.log().c_str());
        exit(1);
    }
	//bind attributes in case of glsl 1.20 fallback
    glBindAttribLocation(prog.getHandle(), 0, "VertexPosition");
    glBindAttribLocation(prog.getHandle(), 1, "VertexNormal");
    glBindAttribLocation(prog.getHandle(), 2, "VertexTexCoord");
    
    
    if( ! prog.link() )
    {
        printf("Shader program failed to link!\n%s",
               prog.log().c_str());
        exit(1);
    }
    
    prog.use();
    
	glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    
    view = glm::lookAt(vec3(-1.5f,0.25f,2.0f), vec3(0.0f,0.0f,0.0f), vec3(0.0f,1.0f,0.0f));
    projection = mat4(1.0f);
    angle = (float)( TO_RADIANS(100.0) );
    
    prog.setUniform("Light.Intensity", vec3(0.9f,0.9f,0.9f) );
    
    glActiveTexture(GL_TEXTURE0);
    
    // Initialize the planets.
    earth = new Planet(1.0f, 0.0f, "texture/earth.jpg");
    moon = new Planet(0.2f, 2.0f, "texture/moon.jpg");
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    
    prog.setUniform("Tex1", 0);
}

void update(int)
{
    translationTimer.AdvanceTime();
    angle -= translationTimer.GetElapsedTime() * 1;
    translationTimer.Reset();
    
    glutTimerFunc(1,update,0);
    glutPostRedisplay();
}

void setMatrices()
{
    mat4 mv = view * model;
    prog.setUniform("ModelViewMatrix", mv);
    prog.setUniform("NormalMatrix", mat3( vec3(mv[0]), vec3(mv[1]), vec3(mv[2]) ));
    prog.setUniform("MVP", projection * mv);
}

void render(void)
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    prog.setUniform("Light.Position", view * vec4(2.0f * cos(angle),1.0f,2.0f * sin(angle),1.0f) );
    prog.setUniform("Material.Ks", 0.05f, 0.05f, 0.05f);  // Specular
	prog.setUniform("Material.Kd", 0.9f, 0.9f, 0.9f);     // Ambient
    prog.setUniform("Material.Ka", 0.5f, 0.5f, 0.5f);     // Diffuse
    prog.setUniform("Material.Shininess", 1.0f);
    
    model = earth->Update(angle);
    setMatrices();
	earth->Render();
    
    model = moon->Update(angle);
    setMatrices();
    moon->Render();
    
	glutSwapBuffers();
}

void reshape(int w, int h)
{
    projection = glm::perspective(100.0f, (float)w/(float)h, 0.1f, 100.0f);
    glViewport(0,0,w,h);
}

void keyboard (unsigned char key, int x, int y)
{
    cout << key << " " << x << " " << y;
	glutPostRedisplay ();
}

int main(int argc, char* argv[])
{
	//GLUT INIT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(1000,600);
	glutCreateWindow("Earth & Moon");
	glutDisplayFunc(render);
	glutReshapeFunc(reshape);	
	glutKeyboardFunc(keyboard);
	glutTimerFunc(1,update,0);

	//GLEW INIT
	glewInit();
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		cout << "glewInit failed" << endl;
		exit (1);
	}
	//INIT GL RESOURCES
	init();
	//ENTER LOOP
	glutMainLoop();
	getchar();
	//system("pause");
	return 0;
}