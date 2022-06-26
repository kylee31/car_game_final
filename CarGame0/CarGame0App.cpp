#include "GL/glew.h"
#include "GL/freeglut.h"
#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "CarGame0App.h"
#include "../BaseCodes/Camera.h"
#include "../BaseCodes/GroundObj2.h"
#include "../BaseCodes/InitShader.h"
#include "../BaseCodes/BasicShapeObjs.h"
#include "CarModel.h"
#include "TreeModel0.h"
#include "SnowModel.h"
#include "iceModel.h"
#include "deerModel.h"


// Window and User Interface
static bool g_left_button_pushed;
static bool g_right_button_pushed;
static int g_last_mouse_x;
static int g_last_mouse_y;

extern GLuint g_window_w;
extern GLuint g_window_h;

//////////////////////////////////////////////////////////////////////
// Camera 
//////////////////////////////////////////////////////////////////////
static Camera g_camera;
static int g_camera_mode = 0;
static int g_switch = 0;


//light
glm::vec3 pos;
glm::vec3 look;


//////////////////////////////////////////////////////////////////////
//// Define Shader Programs
//////////////////////////////////////////////////////////////////////
GLuint s_program_id;


//////////////////////////////////////////////////////////////////////
//// Animation Parameters
//////////////////////////////////////////////////////////////////////
float g_elaped_time_s = 0.f;	

//////////////////////////////////////////////////////////////////////
//// Car Position, Rotation, Velocity
//// �ڵ��� ���� ������.
//////////////////////////////////////////////////////////////////////
glm::vec3 g_car_poisition(0.f, 0.f, 0.f); //��ġ

float g_car_speed = 0;			          // �ӵ� (�ʴ� �̵� �Ÿ�)
float g_car_rotation_y = 0;		          // ���� ���� (y�� ȸ��)
float g_car_angular_speed = 0;	          // ȸ�� �ӵ� (���ӵ� - �ʴ� ȸ�� ��)

//dayandnight mode
bool g_dayandnight = false ;
int night = 0;


//color change
bool g_change = false;
int color = 0;


//smooth
static float fov;
static glm::vec3 position_s;
static glm::vec3 look_s;

/**
InitOpenGL: ���α׷� �ʱ� �� ������ ���� ���� �� �� ȣ��Ǵ� �Լ�. (main �Լ� ����)
OpenGL�� ������ �ʱ� ���� ���α׷��� �ʿ��� �ٸ� �ʱ� ���� �����Ѵ�.
�������, VAO�� VBO�� ���⼭ ������ �� �ִ�.
*/
void InitOpenGL()
{
	s_program_id = CreateFromFiles("../Shaders/v_shader.glsl", "../Shaders/f_shader.glsl");
	glUseProgram(s_program_id);

	glViewport(0, 0, (GLsizei)g_window_w, (GLsizei)g_window_h);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// Initial State of Camera
	// ī�޶� �ʱ� ��ġ �����Ѵ�.
	g_camera.lookAt(glm::vec3(3.f, 2.f, 3.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));

	// basic meshes
	InitBasicShapeObjs();

	// Tree
	InitTreeModel();

	// Car
	InitCarModel();

	// snowman
	InitSnowModel();

	// snow
	InitIceModel();

	// deer
	InitDeerModel();


	// �ٴ� ���� VAO ����
	InitGround2();	

	//smooth
	fov = 90.f;
	position_s = glm::vec3(0.1f, 0.8f, -0.1f);
	look_s = glm::vec3(0.1f, 0.7f, 1.f);	
}



/**
ClearOpenGLResource: ���α׷��� ������ �޸� ������ ���� �� �� ȣ��Ǵ� �Լ�. (main �Լ� ����)
���α׷����� ����� �޸𸮸� ���⿡�� ������ �� �ִ�.
�������, VAO�� VBO�� ���⼭ ���� �� �ִ�.
*/
void ClearOpenGLResource()
{
	// Delete (VAO, VBO)
	DeleteBasicShapeObjs();
	DeleteTreeModel();
	DeleteCarModel();
	DeleteGround2();
	DeleteSnowModel();
	DeleteIceModel();
	DeleteDeerModel();
}


/**
Display: ������ ȭ���� ������Ʈ �� �ʿ䰡 ���� �� ȣ��Ǵ� callback �Լ�.

������ �� ���� ����� ������ �ϴ� �ڵ�� �� �Լ� ���� �����ؾ��Ѵ�.
�����찡 ó�� ���� ��, ������ ũ�Ⱑ �ٲ� ��, �ٸ� �����쿡 ���� ȭ���� �Ϻ�
�Ǵ� ��ü�� �������ٰ� �ٽ� ��Ÿ�� �� �� �ý����� �ش� ������ ���� �׸��� ����
������Ʈ�� �ʿ��ϴٰ� �Ǵ��ϴ� ��� �ڵ����� ȣ��ȴ�.
���� ȣ���� �ʿ��� ��쿡�� glutPostRedisplay() �Լ��� ȣ���ϸ�ȴ�.

�� �Լ��� �ҽÿ� ����ϰ� ȣ��ȴٴ� ���� ����ϰ�, ������ ���� ��ȭ�� ������
1ȸ�� �ڵ�� �������� �� �Լ� �ۿ� �����ؾ��Ѵ�. Ư�� �޸� �Ҵ�, VAO, VBO ����
���� �ϵ���� ������ �õ��ϴ� �ڵ�� Ư���� ������ ���ٸ� ���� �� �Լ��� ���Խ�Ű��
�ȵȴ�. ���� ���, �޽� ���� �����ϰ� VAO, VBO�� �����ϴ� �κ��� ���� 1ȸ��
�����ϸ�ǹǷ� main() �Լ� �� �ܺο� �����ؾ��Ѵ�. ���ǵ� �޽� ���� ������ ���ۿ�
�׸����� �����ϴ� �ڵ常 �� �Լ��� �����ϸ� �ȴ�.

����, �� �Լ� ������ ���� �޸� �Ҵ��� �ؾ��ϴ� ��찡 �ִٸ� �ش� �޸𸮴� �ݵ��
�� �Լ��� ������ ���� ���� �ؾ��Ѵ�.

ref: https://www.opengl.org/resources/libraries/glut/spec3/node46.html#SECTION00081000000000000000
*/
void Display()
{
	// ��ü ȭ���� �����.
	// glClear�� Display �Լ� ���� �� �κп��� �� ���� ȣ��Ǿ���Ѵ�.

	//���ȭ�� �� �ٲٱ�
	if (g_dayandnight == true) {
		glClearColor(0.1f, 0.1f, 0.1f, 1.f);
	}
	else {
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	// Vertex shader �� matrix �������� location�� �޾ƿ´�.
	int m_proj_loc = glGetUniformLocation(s_program_id, "proj_matrix");
	int m_view_loc = glGetUniformLocation(s_program_id, "view_matrix");
	int m_model_loc = glGetUniformLocation(s_program_id, "model_matrix");
	int m_shadow_loc = glGetUniformLocation(s_program_id, "shadow_matrix");



	glm::mat4 projection_matrix;
	glm::mat4 view_matrix;

	
	//first-person-view
	if (g_camera_mode == 1) {
		
		projection_matrix = glm::perspective(glm::radians(90.f), (float)g_window_w / g_window_h, 0.01f, 10000.f);
		glUniformMatrix4fv(m_proj_loc, 1, GL_FALSE, glm::value_ptr(projection_matrix));
		// Camera Transform Matrix ����.

		pos = glm::rotateY(glm::vec3(0.1f, 0.8f, -0.1f), g_car_rotation_y);
		pos += g_car_poisition;
		look = glm::rotateY(glm::vec3(0.1f, 0.7f, 1.f), g_car_rotation_y);
		look += g_car_poisition;

		fov = 90.f;
		position_s = pos;
		look_s = look;

		view_matrix = glm::lookAt(pos, look, glm::vec3(0.f, 1.f, 0.f));
		glUniformMatrix4fv(m_view_loc, 1, GL_FALSE, glm::value_ptr(view_matrix));
	}

	//bird-view
	else if (g_camera_mode == 2) {
		
		projection_matrix = glm::perspective(glm::radians(45.f), (float)g_window_w / g_window_h, 0.01f, 10000.f);
		glUniformMatrix4fv(m_proj_loc, 1, GL_FALSE, glm::value_ptr(projection_matrix));

		// Camera Transform Matrix ����.
		pos = glm::rotateY(glm::vec3(0.f, 2.f, -3.f), g_car_rotation_y);
		pos += g_car_poisition;
		look = glm::rotateY(glm::vec3(0.f, 0.f, 0.f), g_car_rotation_y);
		look += g_car_poisition;

		fov = 45.f;
		position_s = pos;
		look_s = look;

		view_matrix = glm::lookAt(pos, look, glm::vec3(0.f, 1.f, 0.f));
		glUniformMatrix4fv(m_view_loc, 1, GL_FALSE, glm::value_ptr(view_matrix));
	}

	//first-person-view
	else if (g_camera_mode == 3) {
		projection_matrix = glm::perspective(glm::radians(fov), (float)g_window_w / g_window_h, 0.01f, 10000.f);
		glUniformMatrix4fv(m_proj_loc, 1, GL_FALSE, glm::value_ptr(projection_matrix));
		// Camera Transform Matrix ����.

		pos = glm::rotateY(position_s, g_car_rotation_y);
		pos += g_car_poisition;
		look = glm::rotateY(look_s, g_car_rotation_y);
		look += g_car_poisition;

		view_matrix = glm::lookAt(pos, look, glm::vec3(0.f, 1.f, 0.f));
		glUniformMatrix4fv(m_view_loc, 1, GL_FALSE, glm::value_ptr(view_matrix));
	}

	//bird-view
	else if (g_camera_mode == 4) {
		projection_matrix = glm::perspective(glm::radians(fov), (float)g_window_w / g_window_h, 0.01f, 10000.f);
		glUniformMatrix4fv(m_proj_loc, 1, GL_FALSE, glm::value_ptr(projection_matrix));

		// Camera Transform Matrix ����.
		pos = glm::rotateY(position_s, g_car_rotation_y);
		pos += g_car_poisition;
		look = glm::rotateY(look_s, g_car_rotation_y);
		look += g_car_poisition;

		view_matrix = glm::lookAt(pos, look, glm::vec3(0.f, 1.f, 0.f));
		glUniformMatrix4fv(m_view_loc, 1, GL_FALSE, glm::value_ptr(view_matrix));
	}

	else
	{
		//�̺κ��� �ǵ��� �ʱ�
		// Projection Transform Matrix ����.
		projection_matrix = glm::perspective(glm::radians(45.f), (float)g_window_w / g_window_h, 0.01f, 10000.f);
		glUniformMatrix4fv(m_proj_loc, 1, GL_FALSE, glm::value_ptr(projection_matrix));

		// Camera Transform Matrix ����.
		view_matrix = g_camera.GetGLViewMatrix();
		glUniformMatrix4fv(m_view_loc, 1, GL_FALSE, glm::value_ptr(view_matrix));
	}


	glUniform1i(glGetUniformLocation(s_program_id, "p_shadow_mode"), false);


	//Light ����
	{
		int num_of_lights_loc = glGetUniformLocation(s_program_id, "num_of_lights");
		glUniform1i(num_of_lights_loc, 9);


		//1��. directional light
		{	

			if (g_dayandnight == true) {
			// ���� ���� ���� (0: Directionl Light, 1: Point Light, 2: Spot Light), fshader_MultiLights.glsl ����.
			int type_loc = glGetUniformLocation(s_program_id, "lights[0].type");
			glUniform1i(type_loc, 0);


			// ���� ���� ����.
			glm::vec3 dir(-1.f, -1.f, -1.f);
			dir = glm::normalize(dir);

			////// *** ���� ī�޶� ������ ����ϱ� ���� view transform ����  ***
			//  dir�� ������ ��Ÿ���� �����̹Ƿ� �̵�(Translation)��ȯ�� ���õǵ��� �Ѵ�. (�� ��° ��� 0.f���� ����)
			dir = glm::vec3(view_matrix * glm::vec4(dir, 0.f));

			int dir_loc = glGetUniformLocation(s_program_id, "lights[0].dir");
			glUniform3f(dir_loc, dir[0], dir[1], dir[2]);

			// ���� ���� ����.
			int intensity_loc = glGetUniformLocation(s_program_id, "lights[0].intensity");
			glUniform3f(intensity_loc, 0.1f, 0.1f, 0.1f);


			// Directional Light ���� - �׸���
			{
				// ���� ���� ����.
				glm::vec3 light_dir(0.f, 0.f, 0.f);
				light_dir = glm::normalize(light_dir);

				// fragment shader �� ������ light_dir �� view_matrix �� �̸� �����ؼ� ������.
				glm::vec3 m_light_dir = glm::vec3(view_matrix * glm::vec4(light_dir, 0.f));
				int light_dir_loc = glGetUniformLocation(s_program_id, "light_dir");
				glUniform3f(light_dir_loc, m_light_dir[0], m_light_dir[1], m_light_dir[2]);


				// Shadow Projection Matrix
				glm::mat4 shadow_matrix = glm::mat4(1.f);
				shadow_matrix[1][0] = light_dir.x;
				shadow_matrix[1][1] = 0.f;
				shadow_matrix[3][1] = 0.001f;
				shadow_matrix[1][2] = light_dir.z;

				glUniformMatrix4fv(m_shadow_loc, 1, GL_FALSE, glm::value_ptr(shadow_matrix));
			}


			}
			else {
				int type_loc = glGetUniformLocation(s_program_id, "lights[0].type");
				glUniform1i(type_loc, 0);

				// ���� ���� ����.
				glm::vec3 dir(-1.f, -1.f, -1.f);
				dir = glm::normalize(dir);

				////// *** ���� ī�޶� ������ ����ϱ� ���� view transform ����  ***
				//  dir�� ������ ��Ÿ���� �����̹Ƿ� �̵�(Translation)��ȯ�� ���õǵ��� �Ѵ�. (�� ��° ��� 0.f���� ����)
				dir = glm::vec3(view_matrix * glm::vec4(dir, 0.f));

				int dir_loc = glGetUniformLocation(s_program_id, "lights[0].dir");
				glUniform3f(dir_loc, dir[0], dir[1], dir[2]);

				// ���� ���� ����.
				int intensity_loc = glGetUniformLocation(s_program_id, "lights[0].intensity");
				glUniform3f(intensity_loc, 1.f, 1.f, 1.f);

				// Directional Light ���� - �׸���
				{
					// ���� ���� ����.
					glm::vec3 light_dir(-1.f, -1.f, -1.f);
					light_dir = glm::normalize(light_dir);

					// fragment shader �� ������ light_dir �� view_matrix �� �̸� �����ؼ� ������.
					glm::vec3 m_light_dir = glm::vec3(view_matrix * glm::vec4(light_dir, 0.f));
					int light_dir_loc = glGetUniformLocation(s_program_id, "light_dir");
					glUniform3f(light_dir_loc, m_light_dir[0], m_light_dir[1], m_light_dir[2]);


					// Shadow Projection Matrix
					glm::mat4 shadow_matrix = glm::mat4(1.f);
					shadow_matrix[1][0] = light_dir.x;
					shadow_matrix[1][1] = 0.f;
					shadow_matrix[3][1] = 0.001f;
					shadow_matrix[1][2] = light_dir.z;

					glUniformMatrix4fv(m_shadow_loc, 1, GL_FALSE, glm::value_ptr(shadow_matrix));
				}

			}
		}


		//2��. pointlight
		{
			if (g_dayandnight == true) // Point Light ����
			{	

				//�ݵ�����1
				// ���� ���� ���� (0: Directionl Light, 1: Point Light, 2: Spot Light), fshader_MultiLights.glsl ����.
				int type_loc = glGetUniformLocation(s_program_id, "lights[3].type");
				glUniform1i(type_loc, 1);

				// ���� ����ϴ� ��ġ(����) ����.
				// �ð��� ���� ��ġ�� ���ϵ��� ��.
				glm::vec3 pos(2.f , 0.4f+0.2f*cos(g_elaped_time_s), 2.f + 0.3f* sin(g_elaped_time_s));

				// Apply Camera Matrices
				////// *** ���� ī�޶� ������ ����ϱ� ���� view transform ����  ***
				//  �̶� pos�� ��ġ�� ��Ÿ���� ����Ʈ�̹Ƿ� �̵�(Translation)��ȯ�� ����ǵ��� �Ѵ�. (�� ��° ��� 1.f���� ����)
				pos = glm::vec3(view_matrix * glm::vec4(pos, 1.f));

				int pos_loc = glGetUniformLocation(s_program_id, "lights[3].position");
				glUniform3f(pos_loc, pos[0], pos[1], pos[2]);


				// ���� ���� ����.
				int intensity_loc = glGetUniformLocation(s_program_id, "lights[3].intensity");
				glUniform3f(intensity_loc, 0.4f, 0.75f, 0.8f);


				{
					int shading_mode_loc = glGetUniformLocation(s_program_id, "shading_mode");
					glUniform1i(shading_mode_loc, 1);
						
					// ��ȯ ����� �����Ѵ�.
					glm::mat4 model_T;
					model_T = glm::translate(glm::vec3(2.f, 0.4f + 0.2f*cos(g_elaped_time_s), 2.f + 0.3f* sin(g_elaped_time_s))) * glm::scale(glm::vec3(1.f, 1.f, 1.f));
					glUniformMatrix4fv(m_model_loc, 1, GL_FALSE, glm::value_ptr(model_T));

					// ��ü �������� ����� Color ���� �����Ѵ�. 
					glVertexAttrib4f(2, 0.4f, 0.75f, 0.8f, 1.f);

					// ���� �׸���.
					DrawIceModel();
					glUniform1i(shading_mode_loc, 0);
				}

				//�ݵ�����2
				// ���� ���� ���� (0: Directionl Light, 1: Point Light, 2: Spot Light), fshader_MultiLights.glsl ����.
				int type_loc2 = glGetUniformLocation(s_program_id, "lights[4].type");
				glUniform1i(type_loc2, 1);

				// ���� ����ϴ� ��ġ(����) ����.
				// �ð��� ���� ��ġ�� ���ϵ��� ��.
				glm::vec3 pos2(-2.f + 0.3f* sin(g_elaped_time_s), 0.4f + 0.2f*sin(g_elaped_time_s), -2.f );

				// Apply Camera Matrices
				////// *** ���� ī�޶� ������ ����ϱ� ���� view transform ����  ***
				//  �̶� pos�� ��ġ�� ��Ÿ���� ����Ʈ�̹Ƿ� �̵�(Translation)��ȯ�� ����ǵ��� �Ѵ�. (�� ��° ��� 1.f���� ����)
				pos2 = glm::vec3(view_matrix * glm::vec4(pos2, 1.f));

				int pos_loc2 = glGetUniformLocation(s_program_id, "lights[4].position");
				glUniform3f(pos_loc2, pos2[0], pos2[1], pos2[2]);

				// ���� ���� ����.
				int intensity_loc2 = glGetUniformLocation(s_program_id, "lights[4].intensity");
				glUniform3f(intensity_loc2, 0.4f, 0.75f, 0.8f);
					

				{
					int shading_mode_loc = glGetUniformLocation(s_program_id, "shading_mode");
					glUniform1i(shading_mode_loc, 1);

					// ��ȯ ����� �����Ѵ�.
					glm::mat4 model_T2;
					model_T2 = glm::translate(glm::vec3(-2.f + 0.3f* sin(g_elaped_time_s), 0.4f + 0.2f*sin(g_elaped_time_s), -2.f )) * glm::scale(glm::vec3(1.f, 1.f, 1.f));
					glUniformMatrix4fv(m_model_loc, 1, GL_FALSE, glm::value_ptr(model_T2));

					// ��ü �������� ����� Color ���� �����Ѵ�. 
					glVertexAttrib4f(2, 0.4f, 0.75f, 0.8f, 1.f);

					// ���� �׸���.
					DrawIceModel();
					glUniform1i(shading_mode_loc, 0);
				}

				//�ݵ�����2
				int type_loc4 = glGetUniformLocation(s_program_id, "lights[6].type");
				glUniform1i(type_loc4, 1);
				glm::vec3 pos4(3.5f + 0.3f* sin(g_elaped_time_s), 0.4f + 0.2f*sin(g_elaped_time_s), -4.f);

				pos4 = glm::vec3(view_matrix * glm::vec4(pos4, 1.f));

				int pos_loc4 = glGetUniformLocation(s_program_id, "lights[6].position");
				glUniform3f(pos_loc4, pos4[0], pos4[1], pos4[2]);
			
				int intensity_loc4 = glGetUniformLocation(s_program_id, "lights[6].intensity");
				glUniform3f(intensity_loc4, 0.4f, 0.75f, 0.8f);


				{
					int shading_mode_loc = glGetUniformLocation(s_program_id, "shading_mode");
					glUniform1i(shading_mode_loc, 1);
					
					glm::mat4 model_T4;
					model_T4 = glm::translate(glm::vec3(3.5f + 0.3f* sin(g_elaped_time_s), 0.4f + 0.2f*sin(g_elaped_time_s), -4.f)) * glm::scale(glm::vec3(1.f, 1.f, 1.f));
					glUniformMatrix4fv(m_model_loc, 1, GL_FALSE, glm::value_ptr(model_T4));
					glVertexAttrib4f(2, 0.4f, 0.75f, 0.8f, 1.f);
					DrawIceModel();
					glUniform1i(shading_mode_loc, 0);
				}


				//�ݵ�����2
				int type_loc5= glGetUniformLocation(s_program_id, "lights[7].type");
				glUniform1i(type_loc5, 1);
				glm::vec3 pos5(-3.5f , 0.4f + 0.2f*sin(g_elaped_time_s), 4.f + 0.3f* sin(g_elaped_time_s));

				pos5 = glm::vec3(view_matrix * glm::vec4(pos5, 1.f));

				int pos_loc5 = glGetUniformLocation(s_program_id, "lights[7].position");
				glUniform3f(pos_loc5, pos5[0], pos5[1], pos5[2]);

				int intensity_loc5 = glGetUniformLocation(s_program_id, "lights[7].intensity");
				glUniform3f(intensity_loc5, 0.4f, 0.75f, 0.8f);


				{
					int shading_mode_loc = glGetUniformLocation(s_program_id, "shading_mode");
					glUniform1i(shading_mode_loc, 1);

					glm::mat4 model_T5;
					model_T5 = glm::translate(glm::vec3(-3.5f , 0.4f + 0.2f*sin(g_elaped_time_s), 4.f + 0.3f* sin(g_elaped_time_s))) * glm::scale(glm::vec3(1.f, 1.f, 1.f));
					glUniformMatrix4fv(m_model_loc, 1, GL_FALSE, glm::value_ptr(model_T5));
					glVertexAttrib4f(2, 0.4f, 0.75f, 0.8f, 1.f);
					DrawIceModel();
					glUniform1i(shading_mode_loc, 0);
				}


				//�罿��
				// ���� ���� ���� (0: Directionl Light, 1: Point Light, 2: Spot Light), fshader_MultiLights.glsl ����.
				int type_loc3 = glGetUniformLocation(s_program_id, "lights[5].type");
				glUniform1i(type_loc3, 1);

				// ���� ����ϴ� ��ġ(����) ����.
				glm::vec3 pos3(3.15f,1.1f,4.05f);

				// Apply Camera Matrices
				////// *** ���� ī�޶� ������ ����ϱ� ���� view transform ����  ***
				//  �̶� pos�� ��ġ�� ��Ÿ���� ����Ʈ�̹Ƿ� �̵�(Translation)��ȯ�� ����ǵ��� �Ѵ�. (�� ��° ��� 1.f���� ����)
				pos3 = glm::vec3(view_matrix * glm::vec4(pos3, 1.f));

				int pos_loc3 = glGetUniformLocation(s_program_id, "lights[5].position");
				glUniform3f(pos_loc3, pos3[0], pos3[1], pos3[2]);

				// ���� ���� ����.
				if (g_change == false) {
					int intensity_loc3 = glGetUniformLocation(s_program_id, "lights[5].intensity");
					glUniform3f(intensity_loc3, 0.f, 0.f, 1.f);
				}
				else {
					int intensity_loc3 = glGetUniformLocation(s_program_id, "lights[5].intensity");
					glUniform3f(intensity_loc3, 1.f, 0.f, 0.f);
				}


				{
					int shading_mode_loc = glGetUniformLocation(s_program_id, "shading_mode");
					glUniform1i(shading_mode_loc, 1);

					// ��ȯ ����� �����Ѵ�.
					glm::mat4 model_T3;
					model_T3 = glm::translate(glm::vec3(3.15f,1.1f,4.05f)) * glm::scale(glm::vec3(0.035f, 0.035f, 0.035f));
					glUniformMatrix4fv(m_model_loc, 1, GL_FALSE, glm::value_ptr(model_T3));

					// ��ü �������� ����� Color ���� �����Ѵ�. 
					if (g_change == false) {
						glVertexAttrib4f(2, 0.0f, 0.0f, 1.0f, 1.f);
					}
					else {

						glVertexAttrib4f(2, 1.0f, 0.0f, 0.0f, 1.f);
					}

					// ���� �׸���.
					DrawSphere();
					glUniform1i(shading_mode_loc, 0);
				}



			}
			else {
				int type_loc = glGetUniformLocation(s_program_id, "lights[3].type");
				glUniform1i(type_loc, 3);
				int type_loc2 = glGetUniformLocation(s_program_id, "lights[4].type");
				glUniform1i(type_loc2, 3);
				int type_loc3 = glGetUniformLocation(s_program_id, "lights[5].type");
				glUniform1i(type_loc3, 3);
				int type_loc4 = glGetUniformLocation(s_program_id, "lights[6].type");
				glUniform1i(type_loc4, 3);
				int type_loc5 = glGetUniformLocation(s_program_id, "lights[7].type");
				glUniform1i(type_loc5, 3);
			}
		}

		//3��. spotlight
		{
			if (g_dayandnight == true) {

				//������1
				int type_loc = glGetUniformLocation(s_program_id, "lights[1].type");
				glUniform1i(type_loc, 2);


				// ���� ��� ����, ���� ���� ����.
				glm::vec3 light_position = glm::rotateY(glm::vec3(0.15f, 0.2f, 0.3f), g_car_rotation_y);
				light_position += g_car_poisition;
				glm::vec3 light_dir = glm::rotateY(glm::vec3(0.0f, -0.3f, 2.f), g_car_rotation_y);
				//light_dir += g_car_poisition;
				light_dir = glm::normalize(light_dir);


				// Apply Camera Matrices
				////// *** ���� ī�޶� ������ ����ϱ� ���� view transform ����  ***
				//  light_position�� ��ġ�� ��Ÿ���� ����Ʈ�̹Ƿ� �̵�(Translation)��ȯ�� ����ǵ��� �Ѵ�. (�� ��° ��� 1.f���� ����)
				light_position = glm::vec3(view_matrix * glm::vec4(light_position, 1.f));
				//  light_dir�� ������ ��Ÿ���� �����̹Ƿ� �̵�(Translation)��ȯ�� ������� �ʵ��� �Ѵ�. (�� ��° ��� 0.f���� ����)
				light_dir = glm::vec3(view_matrix * glm::vec4(light_dir, 0.f));


				int light_position_loc = glGetUniformLocation(s_program_id, "lights[1].position");
				glUniform3f(light_position_loc, light_position[0], light_position[1], light_position[2]);

				int light_dir_loc = glGetUniformLocation(s_program_id, "lights[1].dir");
				glUniform3f(light_dir_loc, light_dir[0], light_dir[1], light_dir[2]);


				// Spot Light ���� ����.
				float light_cos_cutoff = cos(glm::radians(20.f));
				glm::vec3 light_indensity(1.0f, 1.0f, 0.0f);	// yellow Light

				int light_cos_cutoff_loc = glGetUniformLocation(s_program_id, "lights[1].cos_cutoff");
				glUniform1f(light_cos_cutoff_loc, light_cos_cutoff);

				int light_intensity_loc = glGetUniformLocation(s_program_id, "lights[1].intensity");
				glUniform3f(light_intensity_loc, light_indensity[0], light_indensity[1], light_indensity[2]);



				//������2
				int type_loc2 = glGetUniformLocation(s_program_id, "lights[2].type");
				glUniform1i(type_loc2, 2);

				// ���� ��� ����, ���� ���� ����.
				glm::vec3 light_position2 = glm::rotateY(glm::vec3(-0.15f, 0.2f, 0.3f), g_car_rotation_y);
				light_position2 += g_car_poisition;
				glm::vec3 light_dir2 = glm::rotateY(glm::vec3(0.0f, -0.3f, 2.f), g_car_rotation_y);
				//light_dir2 += g_car_poisition;
				light_dir2 = glm::normalize(light_dir2);


				// Apply Camera Matrices
				////// *** ���� ī�޶� ������ ����ϱ� ���� view transform ����  ***
				//  light_position�� ��ġ�� ��Ÿ���� ����Ʈ�̹Ƿ� �̵�(Translation)��ȯ�� ����ǵ��� �Ѵ�. (�� ��° ��� 1.f���� ����)
				light_position2 = glm::vec3(view_matrix * glm::vec4(light_position2, 1.f));
				//  light_dir�� ������ ��Ÿ���� �����̹Ƿ� �̵�(Translation)��ȯ�� ������� �ʵ��� �Ѵ�. (�� ��° ��� 0.f���� ����)
				light_dir2 = glm::vec3(view_matrix * glm::vec4(light_dir2, 0.f));


				int light_position_loc2 = glGetUniformLocation(s_program_id, "lights[2].position");
				glUniform3f(light_position_loc2, light_position2[0], light_position2[1], light_position2[2]);

				int light_dir_loc2 = glGetUniformLocation(s_program_id, "lights[2].dir");
				glUniform3f(light_dir_loc2, light_dir2[0], light_dir2[1], light_dir2[2]);


				// Spot Light ���� ����.
				float light_cos_cutoff2 = cos(glm::radians(20.f));
				glm::vec3 light_indensity2(1.0f, 1.0f, 0.0f);	// yellow Light

				int light_cos_cutoff_loc2 = glGetUniformLocation(s_program_id, "lights[2].cos_cutoff");
				glUniform1f(light_cos_cutoff_loc2, light_cos_cutoff2);

				int light_intensity_loc2 = glGetUniformLocation(s_program_id, "lights[2].intensity");
				glUniform3f(light_intensity_loc2, light_indensity2[0], light_indensity2[1], light_indensity2[2]);


				{
					int shading_mode_loc = glGetUniformLocation(s_program_id, "shading_mode");
					glUniform1i(shading_mode_loc, 1);

					// Sphere�� ���� Phong Shading ���� ���� ���� �����Ѵ�.
					glm::vec3 l_position = glm::rotateY(glm::vec3(0.15f, 0.35f, 0.4f), g_car_rotation_y);
					l_position += g_car_poisition;

					// ��ȯ ����� �����Ѵ�.
					glm::mat4 model_T;
					model_T = glm::translate(l_position) * glm::scale(glm::vec3(0.03f, 0.03f, 0.03f));
					glUniformMatrix4fv(m_model_loc, 1, GL_FALSE, glm::value_ptr(model_T));

					// ��ü �������� ����� Color ���� �����Ѵ�. 
					glVertexAttrib4f(2, 1.0f, 1.0f, 0.0f, 1.f);
						
					// ���� �׸���.
					DrawSphere();
					glUniform1i(shading_mode_loc, 0);
				}

				{
					// Sphere�� ���� Phong Shading ���� ���� ���� �����Ѵ�.
					int shading_mode_loc = glGetUniformLocation(s_program_id, "shading_mode");
					glUniform1i(shading_mode_loc, 1);

					glm::vec3 l_position = glm::rotateY(glm::vec3(-0.15f, 0.35f, 0.4f), g_car_rotation_y);
					l_position += g_car_poisition;

					// ��ȯ ����� �����Ѵ�.
					glm::mat4 model_T;
					model_T = glm::translate(l_position) * glm::scale(glm::vec3(0.03f, 0.03f, 0.03f));
					glUniformMatrix4fv(m_model_loc, 1, GL_FALSE, glm::value_ptr(model_T));

					// ��ü �������� ����� Color ���� �����Ѵ�. 
					glVertexAttrib4f(2, 1.0f, 1.0f, 0.0f, 1.f);

					// ���� �׸���.
					DrawSphere();
					glUniform1i(shading_mode_loc, 0);
				}
			}
			else {
				int type_loc = glGetUniformLocation(s_program_id, "lights[1].type");
				glUniform1i(type_loc, 3);
				int type_loc2 = glGetUniformLocation(s_program_id, "lights[2].type");
				glUniform1i(type_loc2, 3);
			}
		}
	}

	// Ground
	{
		// Ground�� ���� Phong Shading ���� ���� ���� �����Ѵ�.
		int shininess_loc = glGetUniformLocation(s_program_id, "shininess_n");
		glUniform1f(shininess_loc, 50.f);

		int K_s_loc = glGetUniformLocation(s_program_id, "K_s");
		glUniform3f(K_s_loc, 0.3f, 0.3f, 0.3f);

		// ī�޶� ��ȯ ����� �����Ѵ�.
		glm::mat4 model_T(1.f);
		glUniformMatrix4fv(m_model_loc, 1, GL_FALSE, glm::value_ptr(model_T));

		// �׸���.
		DrawGround2();
	}

	// �ٴ� ����
	glm::mat4 T0(1.f); // ���� ���
	glUniformMatrix4fv(m_model_loc, 1, GL_FALSE, glm::value_ptr(T0));
	DrawGround2();
	
	//�����
	{
		glm::mat4 snow_T = glm::translate(glm::vec3(-1.3f, -0.05f, -0.6f))* glm::rotate(0.4f, glm::vec3(0.f, 1.f, 0.f)) * glm::scale(glm::vec3(150.f, 150.f, 150.f)) ;
		glUniformMatrix4fv(m_model_loc, 1, GL_FALSE, glm::value_ptr(snow_T));
		DrawSnowModel();
	}
	
	//�罿
	{
		glm::mat4 snow_T = glm::translate(glm::vec3(3.7f, 0.f, 3.7f))* glm::rotate(-1.f, glm::vec3(0.f, 1.f, 0.f)) * glm::scale(glm::vec3(1.05f,1.05f, 1.05f));
		glUniformMatrix4fv(m_model_loc, 1, GL_FALSE, glm::value_ptr(snow_T));
		DrawDeerModel();
	}

	// Moving Car
	{
		glm::mat4 car_T = glm::translate(g_car_poisition) * glm::rotate(g_car_rotation_y, glm::vec3(0.f, 1.f, 0.f))* glm::scale(glm::vec3(0.4f, 0.4f, 0.4f));
		glUniformMatrix4fv(m_model_loc, 1, GL_FALSE,  glm::value_ptr(car_T));
		DrawCarModel();
	}

	// ����
	for (int i = 0; i <= 5; i++)
	{
		for (int j = 0; j <= 5; j++)
		{
			glm::mat4 model_T;
			model_T = glm::translate(glm::vec3(i * 2.f - 5.f, 0.f, j * 2.f - 5.f));
			glUniformMatrix4fv(m_model_loc, 1, GL_FALSE, glm::value_ptr(model_T));
			DrawTreeModel((i + j) % NUM_TREES);
		}
	}


	/////////////////////////////// �Ʒ� ���ʹ� �׸��ڸ� �׸���. ////////////////////////////////////////////////////
	glUniform1i(glGetUniformLocation(s_program_id, "p_shadow_mode"), true);

	// Moving Car
	{
		glm::mat4 car_T = glm::translate(g_car_poisition) * glm::rotate(g_car_rotation_y, glm::vec3(0.f, 1.f, 0.f))* glm::scale(glm::vec3(0.4f, 0.4f, 0.4f));
		glUniformMatrix4fv(m_model_loc, 1, GL_FALSE, glm::value_ptr(car_T));
		DrawCarModel();
	}

	{
		glm::mat4 snow_T = glm::translate(glm::vec3(-1.3f, -0.05f, -0.6f))* glm::rotate(0.4f, glm::vec3(0.f, 1.f, 0.f)) * glm::scale(glm::vec3(150.f, 150.f, 150.f));
		glUniformMatrix4fv(m_model_loc, 1, GL_FALSE, glm::value_ptr(snow_T));
		DrawSnowModel();
	}

	//�罿
	{
		glm::mat4 snow_T = glm::translate(glm::vec3(3.7f, 0.f, 3.7f))* glm::rotate(-1.f, glm::vec3(0.f, 1.f, 0.f)) * glm::scale(glm::vec3(1.05f, 1.05f, 1.05f));
		glUniformMatrix4fv(m_model_loc, 1, GL_FALSE, glm::value_ptr(snow_T));
		DrawDeerModel();
	}


	// ����
	for (int i = 0; i <= 5; i++)
	{
		for (int j = 0; j <= 5; j++)
		{
			glm::mat4 model_T;
			model_T = glm::translate(glm::vec3(i * 2.f - 5.f, 0.f, j * 2.f - 5.f));
			glUniformMatrix4fv(m_model_loc, 1, GL_FALSE, glm::value_ptr(model_T));
			DrawTreeModel((i + j) % NUM_TREES);
		}
	}


	// flipping the double buffers
	// glutSwapBuffers�� �׻� Display �Լ� ���� �Ʒ� �κп��� �� ���� ȣ��Ǿ���Ѵ�.
	glutSwapBuffers();
}


/**
Timer: ������ �ð� �Ŀ� �ڵ����� ȣ��Ǵ� callback �Լ�.
ref: https://www.opengl.org/resources/libraries/glut/spec3/node64.html#SECTION000819000000000000000
*/
void Timer(int value)
{
	// Timer ȣ�� �ð� ������ �����Ͽ�, ���� Timer�� ȣ��� �ĺ��� ������� �귯�� ����Ѵ�.
	g_elaped_time_s += value/1000.f;

	// Turn
	g_car_rotation_y += g_car_angular_speed;

	//smooth
	if (g_camera_mode == 3) {
		if (fov != 90.f) {
			fov += 0.4f;
			if (fov > 90.f) {
				fov = 90.f;
			}
		}
		//��ġ�� ����(���ʹϱ� �Ÿ��� ���� �ؾ���)
		if (position_s != glm::vec3(0.1f, 0.8f, -0.1f)) {
			glm::vec3 pos_s=(glm::vec3(0.1f,0.8f,-0.1f)-position_s)/60.f;
			position_s += pos_s;
		}

		if (look_s != glm::vec3(0.1f, 0.7f, 1.f)) {
			glm::vec3 look_sm = (glm::vec3(0.1f, 0.7f, 1.f) - look_s) / 60.f;
			look_s += look_sm;
		}
	}

	else if (g_camera_mode == 4) {
		if (fov != 45.f) {
			fov -= 0.4f;
			if (fov < 45.f) {
				fov = 45.f;
			}
		}
		//��ġ�� ����(���ʹϱ� �Ÿ��� �����ؾ��� - ���� �߸������ߴ� �κ�(�Ÿ� ��� ����Ʈ�� �����ع���))
		if (position_s != glm::vec3(0.f, 2.f, -3.f)) {
			glm::vec3 pos_s = (glm::vec3(0.f, 2.f, -3.f) - position_s) / 60.f;
			position_s += pos_s;
		}

		if (look_s != glm::vec3(0.0f, 0.0f, 0.0f)) {
			glm::vec3 look_sm = (glm::vec3(0.0f, 0.0f, 0.0f) - look_s) / 60.f;
			look_s += look_sm;
		}
	}


	//perspective

	// Calculate Velocity
	//�ڵ����� ������ ������ �ڷ� ������ ���鶧 xyz�� �����ϸ� �װ� �ڵ����� �����̴°� �ƴ�
	//�׷��� �ڵ����� �����ִ� ���⿡ ���� �յڷ� ���������ϹǷ� �Ʒ��ڵ� ó�� ����
	glm::vec3 speed_v = glm::vec3(0.f, 0.f, g_car_speed);
	glm::vec3 velocity = glm::rotateY(speed_v, g_car_rotation_y);	// speed_v �� y���� �������� g_car_rotation_y ��ŭ ȸ���Ѵ�.

	// Move
	g_car_poisition += velocity;


	// glutPostRedisplay�� ������ ���� �ð� �ȿ� ��ü �׸��� �ٽ� �׸� ���� �ý��ۿ� ��û�Ѵ�.
	// ��������� Display() �Լ��� ȣ�� �ȴ�.
	glutPostRedisplay();

	// 1/60 �� �Ŀ� Timer �Լ��� �ٽ� ȣ��Ƿη� �Ѵ�.
	// Timer �Լ� �� ������ �ð� �������� �ݺ� ȣ��ǰ��Ͽ�,
	// �ִϸ��̼� ȿ���� ǥ���� �� �ִ�
	glutTimerFunc((unsigned int)(1000 / 60), Timer, (1000 / 60));
}



/**
Reshape: �������� ũ�Ⱑ ������ ������ �ڵ����� ȣ��Ǵ� callback �Լ�.

@param w, h�� ���� ������ �������� ���� ũ��� ���� ũ�� (�ȼ� ����).
ref: https://www.opengl.org/resources/libraries/glut/spec3/node48.html#SECTION00083000000000000000
*/
void Reshape(int w, int h)
{
	//  w : window width   h : window height
	g_window_w = w;
	g_window_h = h;

	glViewport(0, 0, (GLsizei)w, (GLsizei)h);

	glutPostRedisplay();
}

/**
Keyboard: Ű���� �Է��� ���� ������ �ڵ����� ȣ��Ǵ� �Լ�.
@param key�� ������ Ű������ ���ڰ�.
@param x,y�� ���� ���콺 �������� ��ǥ��.
ref: https://www.opengl.org/resources/libraries/glut/spec3/node49.html#SECTION00084000000000000000

*/

//������ Ű �� / ���콺�� x,y ��
void Keyboard(unsigned char key, int x, int y)
{
	switch (key)						
	{
	case 's':
		g_car_speed = -0.01f;		// ���� �ӵ� ����
		glutPostRedisplay();
		break;

	case 'w':
		g_car_speed = 0.01f;		// ���� �ӵ� ����
		glutPostRedisplay();
		break;

	case 'a':
		g_car_angular_speed = glm::radians( 1.f );		// ��ȸ�� ���ӵ� ����
		glutPostRedisplay();
		break;

	case 'd':
		g_car_angular_speed = -1 * glm::radians( 1.f );		//  ��ȸ�� ���ӵ� ����
		glutPostRedisplay();
		break;

	case '1':
		g_camera_mode = 0;
		glutPostRedisplay();
		break;

	case '2':
		g_camera_mode = 1;
		glutPostRedisplay();
		break;

	case '3':
		g_camera_mode = 2;
		glutPostRedisplay();
		break;

	case '4':
		g_camera_mode = 3;
		glutPostRedisplay();
		break;

	case '5':
		g_camera_mode = 4;
		glutPostRedisplay();
		break;
	
	case 'n':
		night++;
		if (night % 2 == 1) {
			g_dayandnight = true;
		}
		else if (night % 2 == 0) {
			g_dayandnight = false;
		}
		glutPostRedisplay();
		break;

	case 'c':
		color++;
		if (color % 2 == 1) {
			g_change = true;
		}
		else if (color % 2 == 0) {
			g_change = false;
		}
		glutPostRedisplay();
		break;


	}

}

/**
KeyboardUp: �������� Ű�� ������ ������ �ڵ����� ȣ��Ǵ� �Լ�.
@param key�� �ش� Ű������ ���ڰ�.
@param x,y�� ���� ���콺 �������� ��ǥ��.
ref: https://www.opengl.org/resources/libraries/glut/spec3/node49.html#SECTION00084000000000000000

*/
void KeyboardUp(unsigned char key, int x, int y)
{
	switch (key)						
	{
	case 's':
		g_car_speed = 0.f;		// ���� �ӵ� ����
		glutPostRedisplay();
		break;

	case 'w':
		g_car_speed = 0.f;		// ���� �ӵ� ����
		glutPostRedisplay();
		break;

	case 'a':
		g_car_angular_speed = 0.f;		// ��ȸ�� ���ӵ� ����
		glutPostRedisplay();
		break;

	case 'd':
		g_car_angular_speed = 0.f;		//  ��ȸ�� ���ӵ� ����
		glutPostRedisplay();
		break;

	}

}



/**
Mouse: ���콺 ��ư�� �Էµ� ������ �ڵ����� ȣ��Ǵ� �Լ�.
�Ķ������ �ǹ̴� ������ ����.
@param button: ���� ��ư�� ����
  GLUT_LEFT_BUTTON - ���� ��ư
  GLUT_RIGHT_BUTTON - ������ ��ư
  GLUT_MIDDLE_BUTTON - ��� ��ư (���� �������� ��)
  3 - ���콺 �� (���� ���� ���� ����).
  4 - ���콺 �� (���� �Ʒ��� ���� ����).
@param state: ���� ����
  GLUT_DOWN - ���� ����
  GLUT_UP - ��������
@param x,y: ������ �Ͼ�� ��, ���콺 �������� ��ǥ��.
*/
void Mouse(int button, int state, int x, int y)
{
	float mouse_xd = (float)x / g_window_w;
	float mouse_yd = 1 - (float)y / g_window_h;
	float last_mouse_xd = (float)g_last_mouse_x / g_window_w;
	float last_mouse_yd = 1 - (float)g_last_mouse_y / g_window_h;


	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		g_left_button_pushed = true;

	else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
		g_left_button_pushed = false;

	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
		g_right_button_pushed = true;

	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP)
		g_right_button_pushed = false;
	else if (button == 3)
	{
		g_camera.inputMouse(Camera::IN_TRANS_Z, 0, -1, 0.01f);
		glutPostRedisplay();
	}
	else if (button == 4)
	{
		g_camera.inputMouse(Camera::IN_TRANS_Z, 0, 1, 0.01f);
		glutPostRedisplay();
	}

	g_last_mouse_x = x;
	g_last_mouse_y = y;
}




/**
MouseMotion: ���콺 �����Ͱ� ������ ������ �ڵ����� ȣ��Ǵ� �Լ�.
@prarm x,y�� ���� ���콺 �������� ��ǥ���� ��Ÿ����.
*/
void MouseMotion(int x, int y)
{
	float mouse_xd = (float)x / g_window_w;
	float mouse_yd = 1 - (float)y / g_window_h;
	float last_mouse_xd = (float)g_last_mouse_x / g_window_w;
	float last_mouse_yd = 1 - (float)g_last_mouse_y / g_window_h;

	if (g_left_button_pushed)
	{
		g_camera.inputMouse(Camera::IN_ROTATION_Y_UP, last_mouse_xd, last_mouse_yd, mouse_xd, mouse_yd);
		glutPostRedisplay();
	}
	else if (g_right_button_pushed)
	{
		g_camera.inputMouse(Camera::IN_TRANS, last_mouse_xd, last_mouse_yd, mouse_xd, mouse_yd, 0.01f);
		glutPostRedisplay();
	}

	g_last_mouse_x = x;
	g_last_mouse_y = y;
}