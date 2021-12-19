#pragma once

//#include <cmath>
#include <iostream>
#include <vector>
#include "glm/glm.hpp"
//#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/constants.hpp"

using namespace std;
typedef glm::vec4  point4;

const float PI = glm::pi<float>();

class Swimmer {
public:
	const int NumVertices = 36; //(6 faces)(2 triangles/face)(3 vertices/triangle)
	point4 points[36];;
	vector<glm::vec4> verts;
	vector<glm::vec4> normals;
	vector<glm::vec2> texCoords;

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

	Swimmer() { 
		makeUV();
		computeNormals();
	};
	~Swimmer() {
		verts.clear();
		vector<glm::vec4>().swap(verts);
		normals.clear();
		vector<glm::vec4>().swap(normals);
	}
private:
	const float PI = glm::pi<float>();
	int nLongitude;
	int nLatitude;
	void quad(int a, int b, int c, int d);
	void makeUV();
	void computeNormals();
	void computeTexCoordQuad(glm::vec2 texcoord[4], int u, int v, int u2, int v2);
} ;


