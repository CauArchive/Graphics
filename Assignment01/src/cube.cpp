//
// Display a color cube
//
// Colors are assigned to each vertex and then the rasterizer interpolates
//   those colors across the triangles.  We us an orthographic projection
//   as the default projetion.

#include "cube.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"

glm::mat4 projectMat;
glm::mat4 viewMat;

GLuint pvmMatrixID;

float rotAngle = 0.0f;
int isDrawingCar = true;

typedef glm::vec4  color4;
typedef glm::vec4  point4;

const int NumVertices = 36; //(6 faces)(2 triangles/face)(3 vertices/triangle)

point4 points[NumVertices];
color4 colors[NumVertices];

// Vertices of a unit cube centered at origin, sides aligned with axes
point4 vertices[8] = {
	point4(-0.5, -0.5, 0.5, 1.0),
	point4(-0.5, 0.5, 0.5, 1.0),
	point4(0.5, 0.5, 0.5, 1.0),
	point4(0.5, -0.5, 0.5, 1.0),
	point4(-0.5, -0.5, -0.5, 1.0),
	point4(-0.5, 0.5, -0.5, 1.0),
	point4(0.5, 0.5, -0.5, 1.0),
	point4(0.5, -0.5, -0.5, 1.0)
};

// RGBA colors
color4 vertex_colors[8] = {
	color4(0.0, 0.0, 0.0, 1.0),  // black
	color4(0.0, 1.0, 1.0, 1.0),   // cyan
	color4(1.0, 0.0, 1.0, 1.0),  // magenta
	color4(1.0, 1.0, 0.0, 1.0),  // yellow
	color4(1.0, 0.0, 0.0, 1.0),  // red
	color4(0.0, 1.0, 0.0, 1.0),  // green
	color4(0.0, 0.0, 1.0, 1.0),  // blue
	color4(1.0, 1.0, 1.0, 1.0)  // white
};

//----------------------------------------------------------------------------

// quad generates two triangles for each face and assigns colors
//    to the vertices
int Index = 0;
void
quad(int a, int b, int c, int d)
{
	colors[Index] = vertex_colors[a]; points[Index] = vertices[a];  Index++;
	colors[Index] = vertex_colors[b]; points[Index] = vertices[b];  Index++;
	colors[Index] = vertex_colors[c]; points[Index] = vertices[c];  Index++;
	colors[Index] = vertex_colors[a]; points[Index] = vertices[a];  Index++;
	colors[Index] = vertex_colors[c]; points[Index] = vertices[c];  Index++;
	colors[Index] = vertex_colors[d]; points[Index] = vertices[d];  Index++;
}

//----------------------------------------------------------------------------

// generate 12 triangles: 36 vertices and 36 colors
void
colorcube()
{
	quad(1, 0, 3, 2);
	quad(2, 3, 7, 6);
	quad(3, 0, 4, 7);
	quad(6, 5, 1, 2);
	quad(4, 5, 6, 7);
	quad(5, 4, 0, 1);
}

//----------------------------------------------------------------------------

// OpenGL initialization
void
init()
{
	colorcube();

	// Create a vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Create and initialize a buffer object
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors),
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors);

	// Load shaders and use the resulting shader program
	GLuint program = InitShader("src/vshader.glsl", "src/fshader.glsl");
	glUseProgram(program);

	// set up vertex arrays
	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(0));

	GLuint vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(sizeof(points)));

	pvmMatrixID = glGetUniformLocation(program, "mPVM");

	projectMat = glm::perspective(glm::radians(65.0f), 1.0f, 0.1f, 100.0f);
	viewMat = glm::lookAt(glm::vec3(0, 0, 2), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0, 0.0, 0.0, 1.0);
}

//----------------------------------------------------------------------------

void drawSwimmer(glm::mat4 carMat)
{
	glm::mat4 modelMat, pvmMat;
	glm::vec3 head;
	glm::vec3 arms[2];
	glm::vec3 legs[2];
	head = glm::vec3(-0.5, 0, 0);
	arms[0] = glm::vec3(-0.2, 0.4, 0);
	arms[1] = glm::vec3(-0.2, -0.4, 0);
	legs[0] = glm::vec3(0.4, 0.2, 0);
	legs[1] = glm::vec3(0.4, -0.2, 0);;


	// human body
	modelMat = glm::scale(carMat, glm::vec3(0.6, 0.6, 0.15));
	pvmMat = projectMat * viewMat * modelMat;
	glUniformMatrix4fv(pvmMatrixID, 1, GL_FALSE, &pvmMat[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

	// human head
	modelMat = glm::translate(carMat, head);  //P*V*C*T*S*v
	modelMat = glm::scale(modelMat, glm::vec3(0.3, 0.3, 0.15));
	pvmMat = projectMat * viewMat * modelMat;
	glUniformMatrix4fv(pvmMatrixID, 1, GL_FALSE, &pvmMat[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

	// human arm
	for (int i = 0; i < 2; i++) {
		modelMat = glm::translate(carMat, arms[i]);  //P*V*C*T*S*v
		modelMat = glm::scale(modelMat, glm::vec3(0.2, 0.1, 0.15));

		int Sign = 1;
		if (i == 1) Sign = -1;
		float speed = 5.0f;
		float childRot = 0.0f;
		float maxAngle = 180.0f;
		float newAngle = std::fmod(glm::degrees(rotAngle) * speed, maxAngle);
		int rotIDX = int(glm::degrees(rotAngle) * speed / int(360.0f)) % 2;
		
		int resCal = int(glm::degrees(rotAngle) * speed / (int)maxAngle) % 4;
		if (resCal == 1) {
			newAngle = glm::radians(maxAngle - newAngle);
			if(i == 1) childRot = Sign * newAngle * 0.7;
		}
		else if (resCal == 2) {
			newAngle = glm::radians(0.0f - newAngle);
			if (i == 1) childRot = Sign * newAngle * 0.7;
		}
		else if (resCal == 0) {
			newAngle = glm::radians(0.0f + newAngle);
			if (i == 0) childRot = Sign * newAngle * 0.7;
		}
		else if (resCal == 3) {
			newAngle = glm::radians(-maxAngle + newAngle);
			if (i == 0) childRot = Sign * newAngle * 0.7;
		}

		if(rotIDX == i) modelMat = glm::rotate(modelMat, rotAngle*speed*Sign, glm::vec3(0, 1, 0));
		pvmMat = projectMat * viewMat * modelMat;
		glUniformMatrix4fv(pvmMatrixID, 1, GL_FALSE, &pvmMat[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, NumVertices);

		modelMat = glm::translate(modelMat, glm::vec3(-1.2,0,0));
		if (rotIDX == i) modelMat = glm::rotate(modelMat, childRot, glm::vec3(0, 1, 0));
		pvmMat = projectMat * viewMat * modelMat;
		glUniformMatrix4fv(pvmMatrixID, 1, GL_FALSE, &pvmMat[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, NumVertices);
		}
	// human leg
	for (int i = 0; i < 2; i++) {
		modelMat = glm::translate(carMat, legs[i]);  //P*V*C*T*S*v
		modelMat = glm::scale(modelMat, glm::vec3(0.2, 0.1, 0.15));

		float speed = 5.0f;
		int Sign = 1;
		if (i == 0) Sign = -1;
		float maxAngle = 50.0f;
		float newAngle = std::fmod(glm::degrees(rotAngle) * speed, maxAngle);

		int resCal = int(glm::degrees(rotAngle) * speed / (int)maxAngle) % 4;
		if (resCal == 1) newAngle = glm::radians(maxAngle - newAngle);
		else if (resCal == 2) newAngle = glm::radians(0.0f - newAngle);
		else if (resCal == 0) newAngle = glm::radians(0.0f + newAngle);
		else if (resCal == 3) newAngle = glm::radians(-maxAngle + newAngle);

		modelMat = glm::rotate(modelMat, newAngle * Sign, glm::vec3(0, 1, 0));
		pvmMat = projectMat * viewMat * modelMat;
		glUniformMatrix4fv(pvmMatrixID, 1, GL_FALSE, &pvmMat[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, NumVertices);

		modelMat = glm::translate(modelMat, glm::vec3(1.2, 0, 0));
		modelMat = glm::rotate(modelMat, Sign*newAngle*0.7f, glm::vec3(0, 1, 0));
		pvmMat = projectMat * viewMat * modelMat;
		glUniformMatrix4fv(pvmMatrixID, 1, GL_FALSE, &pvmMat[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	}
}


void display(void)
{
	glm::mat4 worldMat, pvmMat;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	worldMat = glm::rotate(glm::mat4(1.0f), 2.1f, glm::vec3(1.0f, 1.0f, 0.0f));

	if (isDrawingCar)
	{
		drawSwimmer(worldMat);
	}
	else
	{
		pvmMat = projectMat * viewMat * worldMat;
		glUniformMatrix4fv(pvmMatrixID, 1, GL_FALSE, &pvmMat[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	}

	glutSwapBuffers();
}

//----------------------------------------------------------------------------

void idle()
{
	static int prevTime = glutGet(GLUT_ELAPSED_TIME);
	int currTime = glutGet(GLUT_ELAPSED_TIME);

	if (abs(currTime - prevTime) >= 20)
	{
		float t = abs(currTime - prevTime);
		rotAngle += glm::radians(t*360.0f / 10000.0f);
		prevTime = currTime;
		glutPostRedisplay();
	}
}

//----------------------------------------------------------------------------

void
keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'c': case 'C':
		isDrawingCar = !isDrawingCar;
		break;
	case 033:  // Escape key
	case 'q': case 'Q':
		exit(EXIT_SUCCESS);
		break;
	}
}

//----------------------------------------------------------------------------

void resize(int w, int h)
{
	float ratio = (float)w / (float)h;
	glViewport(0, 0, w, h);

	projectMat = glm::perspective(glm::radians(65.0f), ratio, 0.1f, 100.0f);

	glutPostRedisplay();
}

//----------------------------------------------------------------------------

int
main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(512, 512);
	glutInitContextVersion(3, 2);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow("Color Car");

	glewInit();

	init();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(resize);
	glutIdleFunc(idle);

	glutMainLoop();
	return 0;
}
