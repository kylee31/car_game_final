#include "GL/glew.h"
#include "glm/glm.hpp"
#include <iostream>
#include <fstream>
#include "../BaseCodes/Mesh.h"
#include "CarModel.h"


///////////////////////////////////////////////
// Two-Triangle

static GLuint g_car_vao;
static GLuint g_car_vbo_pos;
static GLuint g_car_vbo_color;
static GLuint g_car_vbo_normal;
static GLuint g_car_index_buf;

static Mesh g_car_mesh;

void InitCarModel()
{
	g_car_mesh.ReadObjFormatFile("../Data/car.obj");
	
	g_car_mesh.TriangulateAndUniformizeBuffers();

	g_car_mesh.ScaleUniformlyVertices(0.01f);

	//3D로 봤을때 서있어서 돌려줌
	//g_car_mesh.RotateVertices(glm::radians(-90.f), glm::vec3(1, 0, 0));
	g_car_mesh.RotateVertices(glm::radians(180.f), glm::vec3(0, 1, 0));
	//전체적인 컬러값을 하나로 통일해서 설정
	g_car_mesh.SetColor(0.45f, 0.f, 0.f, 1);

	//g_car_mesh안에 꼭지점의 정보가 들어가게 되는것

	// Vertex Array Object
	glGenVertexArrays(1, &g_car_vao);
	glBindVertexArray(g_car_vao);


	// Position VBO
	glGenBuffers(1, &g_car_vbo_pos);
	glBindBuffer(GL_ARRAY_BUFFER, g_car_vbo_pos);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*3*g_car_mesh.num_points(), g_car_mesh.points(), GL_STATIC_DRAW);

	// Color VBO
	glGenBuffers(1, &g_car_vbo_color);
	glBindBuffer(GL_ARRAY_BUFFER, g_car_vbo_color);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*4*g_car_mesh.num_points(), g_car_mesh.colors(), GL_STATIC_DRAW);

	// Normal VBO
	glGenBuffers(1, &g_car_vbo_normal);
	glBindBuffer(GL_ARRAY_BUFFER, g_car_vbo_normal);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*3*g_car_mesh.num_points(), g_car_mesh.normals(), GL_STATIC_DRAW);


	// Index Buffer Object
	glGenBuffers(1, &g_car_index_buf);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_car_index_buf);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*3*g_car_mesh.num_triangles(), g_car_mesh.triangle_ids(), GL_STATIC_DRAW);


	// Vertex Attribute
	glBindBuffer(GL_ARRAY_BUFFER, g_car_vbo_pos);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, g_car_vbo_color);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, g_car_vbo_normal);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);
}


void DrawCarModel()
{
	glBindVertexArray(g_car_vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_car_index_buf);
	glDrawElements(GL_TRIANGLES, g_car_mesh.num_triangles()*3, GL_UNSIGNED_INT, NULL);
}


void DeleteCarModel()
{
	glDeleteBuffers(1, &g_car_vbo_pos);
	glDeleteBuffers(1, &g_car_vbo_normal);
	glDeleteBuffers(1, &g_car_index_buf);
	glDeleteVertexArrays(1, &g_car_vao);
}




