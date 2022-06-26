#include "GL/glew.h"
#include "glm/glm.hpp"
#include "../BaseCodes/BasicShapeObjs.h"
#include "../BaseCodes/Mesh.h"
#include <iostream>
#include <string>
#include <stack>

static float g_d = 0.05f;
static float g_delta = glm::radians(30.f);
static std::string g_initial_str = "X";


std::string Rule(char in)
{
	std::string out;


	//F[+X]F[-X]+X
	if (in == 'X') out = "F[+X][-X][^^^X-[X]]FL[&X][-[-@][+@][+++@]]";
	else if (in == 'L') out = "[&+L][-/L]";
	else if (in == 'F') out = "FF";
	else                  out = in;

	return out;
}

std::string Reproduce(std::string input)
{
	std::string output;

	for (unsigned int i = 0; i<input.size(); i++)
	{
		output = output + Rule(input[i]);
	}

	return output;
}

void CreateLSystemTree(int iteration, Mesh &out_mash)
{


	// ³ª¹µ°¡Áö
	Mesh trunk = glm::scale(glm::vec3(0.03f, g_d, 0.03f))
		* glm::translate(glm::vec3(0.f, 0.5f, 0.f))
		* g_cylinder_mesh;
	trunk.SetColor(1, 1, 1, 1);


	// ³ª¹µÀÙ
	Mesh leaf = glm::rotate(glm::pi<float>() / 2.f, glm::vec3(0.f, 1.f, 0.f))
		* glm::rotate(-glm::pi<float>() / 6.f, glm::vec3(1.f, 0.f, 0.f))
		* glm::scale(glm::vec3(0.03f, 0.001f, 0.1f))
		* glm::translate(glm::vec3(0.f, 0.f, 1.f))
		* g_cylinder_mesh;
	leaf.SetColor(1.f, 1.f, 1.f, 1.f);


	// ´«°áÁ¤
	Mesh snow = glm::scale(glm::vec3(0.005f,0.06f, 0.005f))
		* g_cylinder_mesh;
	snow.SetColor(0.3f, 0.7f, 0.9f, 1.f);



	// String Reproduction
	std::string str = g_initial_str;
	for (int i = 0; i<iteration; i++)
		str = Reproduce(str);


	std::stack<glm::mat4> stack_T;
	glm::mat4 T(1);
	for (unsigned int i = 0; i<str.size(); i++)
	{
		if (str[i] == 'F')
		{
			out_mash += T*trunk;
			T = T * glm::translate(glm::vec3(0, g_d, 0));
		}
		if (str[i] == 'L')
		{
			out_mash += T*leaf;
			T = T * glm::translate(glm::vec3(0, g_d, 0));
		}
		else if (str[i] == '@') {
			out_mash += T*snow;
			T = T * glm::translate(glm::vec3(0, g_d, 0));
		}
		else if (str[i] == '+')
		{
			glRotated(g_delta, 1, 0, 0);
			T = T * glm::rotate(g_delta, glm::vec3(1, 0, 0));
		}
		else if (str[i] == '-')
		{
			glRotated(-g_delta, 1, 0, 0);
			T = T * glm::rotate(-g_delta, glm::vec3(1, 0, 0));
		}
		else if (str[i] == '^') {
			glRotated(g_delta, 0, 1, 0);
			T = T * glm::rotate(g_delta, glm::vec3(0, 1, 0));
		}
		else if (str[i] == '&')
		{
			glRotated(-g_delta, 0, 1, 0);
			T = T * glm::rotate(-g_delta, glm::vec3(0, 1, 0));
		}
		else if (str[i] == '\n') {
			glRotated(g_delta, 0, 0, 1);
			T = T *glm::rotate(g_delta, glm::vec3(0, 0, 1));
		}
		else if (str[i] == '/') {
			glRotated(-g_delta, 0, 0, 1);
			T = T *glm::rotate(-g_delta, glm::vec3(0, 0, 1));
		}
		else if (str[i] == '[')
		{
			stack_T.push(T);
		}
		else if (str[i] == ']')
		{
			T = stack_T.top();
			stack_T.pop();
		}
	}
}

