//
// Display a illuminated swimmer
//

#include "initShader.h"
#include "swimmer.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include "texture.hpp"

enum eShadeMode { NO_LIGHT, GOURAUD, PHONG, NUM_LIGHT_MODE };

glm::mat4 projectMat;
glm::mat4 viewMat;
glm::mat4 modelMat = glm::mat4(1.0f);

float rotAngle = 0.0f;

int shadeMode = NO_LIGHT;
int isTexture = false;
int isRotate = false;
const int NumVertices = 36; //(6 faces)(2 triangles/face)(3 vertices/triangle)

GLuint projectMatrixID;
GLuint viewMatrixID;
GLuint modelMatrixID;
GLuint shadeModeID;
GLuint textureModeID;

GLuint pvmMatrixID;

Swimmer swimmer;

//----------------------------------------------------------------------------

// OpenGL initialization
void init()
{
	// Create a vertex array object
	GLuint vao[1];
	glGenVertexArrays(1, vao);
	glBindVertexArray(vao[0]);

	// Create and initialize a buffer object
	GLuint buffer[1];
	glGenBuffers(1, buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);

	int vertSize = sizeof(swimmer.verts[0]) * swimmer.verts.size();
	int normalSize = sizeof(swimmer.normals[0]) * swimmer.normals.size();
	int texSize = sizeof(swimmer.texCoords[0]) * swimmer.texCoords.size();
	glBufferData(GL_ARRAY_BUFFER, vertSize + normalSize + texSize,
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, vertSize, swimmer.verts.data());
	glBufferSubData(GL_ARRAY_BUFFER, vertSize, normalSize, swimmer.normals.data());
	glBufferSubData(GL_ARRAY_BUFFER, vertSize + normalSize, texSize, swimmer.texCoords.data());

	// Load shaders and use the resulting shader program
	GLuint program = InitShader("src/vshader.glsl", "src/fshader.glsl");
	glUseProgram(program);

	// set up vertex arrays
	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(0));

	GLuint vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(vertSize));

	GLuint vTexCoord = glGetAttribLocation(program, "vTexCoord");
	glEnableVertexAttribArray(vTexCoord);
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(vertSize + normalSize));

	projectMatrixID = glGetUniformLocation(program, "mProject");
	projectMat = glm::perspective(glm::radians(65.0f), 1.0f, 0.1f, 100.0f);
	glUniformMatrix4fv(projectMatrixID, 1, GL_FALSE, &projectMat[0][0]);

	viewMatrixID = glGetUniformLocation(program, "mView");
	viewMat = glm::lookAt(glm::vec3(0, 0, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	glUniformMatrix4fv(viewMatrixID, 1, GL_FALSE, &viewMat[0][0]);

	modelMatrixID = glGetUniformLocation(program, "mModel");
	modelMat = glm::mat4(1.0f);
	glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &modelMat[0][0]);

	shadeModeID = glGetUniformLocation(program, "shadeMode");
	glUniform1i(shadeModeID, shadeMode);

	textureModeID = glGetUniformLocation(program, "isTexture");
	glUniform1i(textureModeID, isTexture);

	// Load the texture using any two methods
	GLuint Texture = loadBMP_custom("brick.bmp");
	//GLuint Texture = loadDDS("uvtemplate.DDS");

	// Get a handle for our "myTextureSampler" uniform
	GLuint TextureID = glGetUniformLocation(program, "swimmerTexture");

	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Texture);

	// Set our "myTextureSampler" sampler to use Texture Unit 0
	glUniform1i(TextureID, 0);

	projectMat = glm::perspective(glm::radians(65.0f), 1.0f, 0.1f, 100.0f);
	viewMat = glm::lookAt(glm::vec3(0, 0, 2), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0, 0.0, 0.0, 1.0);
}

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
	glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &modelMat[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

	// human head
	modelMat = glm::translate(carMat, head);  //P*V*C*T*S*v
	modelMat = glm::scale(modelMat, glm::vec3(0.3, 0.3, 0.15));
	pvmMat = projectMat * viewMat * modelMat;
	glUniformMatrix4fv(pvmMatrixID, 1, GL_FALSE, &pvmMat[0][0]);
	glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &modelMat[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

	// human arm
	for (int i = 0; i < 2; i++) {
		modelMat = glm::translate(carMat, arms[i]);  //P*V*C*T*S*v
		modelMat = glm::scale(modelMat, glm::vec3(0.2, 0.1, 0.15));

		//glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &modelMat[0][0]);

		int Sign = 1;
		if (i == 1) Sign = -1;
		float speed = 7.0f;
		int rotIDX = int(glm::degrees(rotAngle) * speed / int(360.0f)) % 2;
		if (rotIDX == i) modelMat = glm::rotate(modelMat, rotAngle * speed * Sign, glm::vec3(0, 1, 0));
		pvmMat = projectMat * viewMat * modelMat;
		glUniformMatrix4fv(pvmMatrixID, 1, GL_FALSE, &pvmMat[0][0]);
		glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &modelMat[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, NumVertices);

		modelMat = glm::translate(modelMat, glm::vec3(-1.2, 0, 0));
		//modelMat = glm::rotate(modelMat, Sign*newAngle, glm::vec3(0, 1, 0));
		pvmMat = projectMat * viewMat * modelMat;
		glUniformMatrix4fv(pvmMatrixID, 1, GL_FALSE, &pvmMat[0][0]);
		glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &modelMat[0][0]);
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
		glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &modelMat[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, NumVertices);

		modelMat = glm::translate(modelMat, glm::vec3(1.2, 0, 0));
		modelMat = glm::rotate(modelMat, Sign * newAngle * 0.7f, glm::vec3(0, 1, 0));
		pvmMat = projectMat * viewMat * modelMat;
		glUniformMatrix4fv(pvmMatrixID, 1, GL_FALSE, &pvmMat[0][0]);
		glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &modelMat[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	}
}

//----------------------------------------------------------------------------

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	drawSwimmer(glm::rotate(modelMat, 2.1f, glm::vec3(1.0f, 1.0f, 0.0f)));
    glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &modelMat[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, swimmer.verts.size());

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
		rotAngle += glm::radians(t * 360.0f / 10000.0f);
		prevTime = currTime;
		glutPostRedisplay();
	}
}

//----------------------------------------------------------------------------

void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'l': case 'L':
		shadeMode = (++shadeMode % NUM_LIGHT_MODE);
		glUniform1i(shadeModeID, shadeMode);
		glutPostRedisplay();
		break;
	case 'r': case 'R':
		isRotate = !isRotate;
		glutPostRedisplay();
		break;
	case 't': case 'T':
		isTexture = !isTexture;
		glUniform1i(textureModeID, isTexture);
		glutPostRedisplay();
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
	glUniformMatrix4fv(projectMatrixID, 1, GL_FALSE, &projectMat[0][0]);
	glutPostRedisplay();
}

//----------------------------------------------------------------------------

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(512, 512);
	glutInitContextVersion(3, 2);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow("Swimmer");

	glewInit();

	init();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(resize);
	glutIdleFunc(idle);

	glutMainLoop();
	return 0;
}
