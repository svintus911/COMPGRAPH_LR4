#include "Render.h"




#include <windows.h>

#include <GL\gl.h>
#include <GL\glu.h>
#include "GL\glext.h"

#include "MyOGL.h"

#include "Camera.h"
#include "Light.h"
#include "Primitives.h"

#include "MyShaders.h"

#include "ObjLoader.h"
#include "GUItextRectangle.h"

#include "Texture.h"
#include <chrono>

GuiTextRectangle rec;

OpenGL *ogl_obj;

//��������� ������ ��� ��������� ����
#define POP glPopMatrix()
#define PUSH glPushMatrix()


enum Mode
{
	PLANE, ORTHO, MODEL, BUTTERFLY
} mode;



//����� ��� ��������� ������
class CustomCamera : public Camera
{
public:
	//��������� ������
	double camDist;
	//���� �������� ������
	double fi1, fi2;

	
	//������� ������ �� ���������
	CustomCamera()
	{
		camDist = 10;
		fi1 = -1;
		fi2 = 1;
	}

	
	//������� ������� ������, ������ �� ����� ��������, ���������� �������
	virtual void SetUpCamera()
	{

		lookPoint.setCoords(0, 0, 0);

		pos.setCoords(camDist*cos(fi2)*cos(fi1),
			camDist*cos(fi2)*sin(fi1),
			camDist*sin(fi2));

		if (cos(fi2) <= 0)
			normal.setCoords(0, 0, -1);
		else
			normal.setCoords(0, 0, 1);

		LookAt();
	}

	void CustomCamera::LookAt()
	{
		gluLookAt(pos.X(), pos.Y(), pos.Z(), lookPoint.X(), lookPoint.Y(), lookPoint.Z(), normal.X(), normal.Y(), normal.Z());
	}
}  camera;   //������� ������ ������


//����� ��� ��������� �����
class CustomLight : public Light
{
public:
	
	CustomLight()
	{
		//��������� ������� �����
		pos = Vector3(1, 1, 3);
	}

	
	//������ ����� � ����� ��� ���������� �����, ���������� �������
	void  DrawLightGhismo()
	{
		if (mode == Mode::ORTHO)
			return;
		Shader::DontUseShaders();
		bool f1 = glIsEnabled(GL_LIGHTING);
		glDisable(GL_LIGHTING);
		bool f2 = glIsEnabled(GL_TEXTURE_2D);
		glDisable(GL_TEXTURE_2D);
		bool f3 = glIsEnabled(GL_DEPTH_TEST);
		
		glDisable(GL_DEPTH_TEST);
		glColor3d(0.9, 0.8, 0);
		Sphere s;
		s.pos = pos;
		s.scale = s.scale*0.08;
		s.Show();

		if (ogl_obj->isKeyPressed2('G'))
		{
			glColor3d(0, 0, 0);
			//����� �� ��������� ����� �� ����������
				glBegin(GL_LINES);
			glVertex3d(pos.X(), pos.Y(), pos.Z());
			glVertex3d(pos.X(), pos.Y(), 0);
			glEnd();

			//������ ���������
			Circle c;
			c.pos.setCoords(pos.X(), pos.Y(), 0);
			c.scale = c.scale*1.5;
			c.Show();
		}
	}

	void SetUpLight()
	{
		GLfloat amb[] = { 0.2, 0.2, 0.2, 0 };
		GLfloat dif[] = { 1.0, 1.0, 1.0, 0 };
		GLfloat spec[] = { .7, .7, .7, 0 };
		GLfloat position[] = { pos.X(), pos.Y(), pos.Z(), 1. };

		// ��������� ��������� �����
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		// �������������� ����������� �����
		// ������� ��������� (���������� ����)
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		// ��������� ������������ �����
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
		// ��������� ���������� ������������ �����
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

		glEnable(GL_LIGHT0);
	}


} light;  //������� �������� �����



//������ ���������� ����
int mouseX = 0, mouseY = 0;


float offsetX = 0, offsetY = 0;
float zoom=1;
int Time = 0;

std::chrono::steady_clock::time_point chrono_time_base = std::chrono::steady_clock::now();


//���������� �������� ����
void mouseEvent(OpenGL *ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//������ ���� ������ ��� ������� ����� ������ ����
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.01*dx;
		camera.fi2 += -0.01*dy;
	}


	if (OpenGL::isKeyPressed(VK_LBUTTON))
	{
		offsetX -= 1.0*dx/ogl->getWidth()/zoom;
		offsetY += 1.0*dy/ogl->getHeight()/zoom;
	}


	
	//������� ���� �� ���������, � ����� ��� ����
	if (ogl->isKeyPressed2('G') && !OpenGL::isKeyPressed(VK_LBUTTON) && mode!=Mode::ORTHO)
	{
		LPPOINT POINT = new tagPOINT();
		GetCursorPos(POINT);
		ScreenToClient(ogl->getHwnd(), POINT);
		POINT->y = ogl->getHeight() - POINT->y;

		Ray r = camera.getLookRay(POINT->x, POINT->y,60,ogl->aspect);

		double z = light.pos.Z();

		double k = 0, x = 0, y = 0;
		if (r.direction.Z() == 0)
			k = 0;
		else
			k = (z - r.origin.Z()) / r.direction.Z();

		x = k*r.direction.X() + r.origin.X();
		y = k*r.direction.Y() + r.origin.Y();

		light.pos = Vector3(x, y, z);
	}

	if (ogl->isKeyPressed2('G') && OpenGL::isKeyPressed(VK_LBUTTON))
	{
		light.pos = light.pos + Vector3(0, 0, 0.02*dy);
	}

	
}

//���������� �������� ������  ����
void mouseWheelEvent(OpenGL *ogl, int delta)
{
	float _tmpZ = delta*0.003;
	if (ogl->isKeyPressed('Z'))
		_tmpZ *= 10;
	zoom += 0.2*zoom*_tmpZ;


	if (delta < 0 && camera.camDist <= 1)
		return;
	if (delta > 0 && camera.camDist >= 100)
		return;

	camera.camDist += 0.01*delta;
}

//���������� ������� ������ ����������
void keyDownEvent(OpenGL *ogl, int key)
{	
	if (key == 'F')
	{
		light.pos = camera.pos;
	}
	if (key == 49)
	{
		mode = Mode::PLANE;
	}
	if (key == 50)
	{
		mode = Mode::MODEL;
	}
	if (key == 51)
	{
		mode = Mode::ORTHO;
	}
	if (key == 52)
	{
		mode = Mode::BUTTERFLY;
	}
}

void keyUpEvent(OpenGL *ogl, int key)
{
	
}



ObjFile _model;
Shader _shader;
Texture tex[4];
int tick_o = 0;
int tick_n = 0;



extern "C" __declspec(dllexport) int ex_loadModel(LPWSTR filename)
{
	int err = loadModel(filename, &_model);
	
	return 1;
}

//extern "C" __declspec(dllexport) int ex_loadPixShader(void *text, int size)
//{
//	(&_shader)->loadPixShader((char *)text, size, 0);
//	return 1;
//}

extern "C" __declspec(dllexport) int ex_loadPixShader(const char** strings, int* lengths, int count)
{
	(&_shader)->loadPixShader(strings, lengths, count,0);
	return 1;
}

extern "C" __declspec(dllexport) int ex_loadVertShader(const char** strings, int* lengths, int count)
{
	(&_shader)->loadVertShader(strings, lengths, count, 0);
	return 1;
}
extern "C" __declspec(dllexport) void ex_Compile()
{
	(&_shader)->Compile();

	tick_o = GetTickCount();
	tick_n = GetTickCount();
	Time = 0;

	chrono_time_base = std::chrono::steady_clock::now();
}

extern "C" __declspec(dllexport) void loadTextute(int chanel, unsigned char *texture, int w, int h)
{
	Texture *t = tex + chanel;
	t->loadFromBuffer(texture, w, h);
}

extern "C" __declspec(dllexport) void deleteTexture(int chanel)
{
	Texture* t = tex + chanel;
	t->deleteTexture();
}

//����������� ����� ������ ��������
void initRender(OpenGL *ogl)
{
	initShadersFunctions();
	glActiveTexture(GL_TEXTURE0);

	ogl_obj = ogl;
	//��������� �������

	//4 ����� �� �������� �������
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//��������� ������ ��������� �������
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//�������� ��������
	glEnable(GL_TEXTURE_2D);
	
	//������ � ���� ����������� � "������"
	ogl->mainCamera = &camera;
	ogl->mainLight = &light;

	// ������������ �������� : �� ����� ����� ����� 1
	glEnable(GL_NORMALIZE);

	// ���������� ������������� ��� �����
	glEnable(GL_LINE_SMOOTH); 


	//   ������ ��������� ���������
	//  �������� GL_LIGHT_MODEL_TWO_SIDE - 
	//                0 -  ������� � ���������� �������� ���������(�� ���������), 
	//                1 - ������� � ���������� �������������� ������� ��������       
	//                �������������� ������� � ���������� ��������� ����������.    
	//  �������� GL_LIGHT_MODEL_AMBIENT - ������ ������� ���������, 
	//                �� ��������� �� ���������
	// �� ��������� (0.2, 0.2, 0.2, 1.0)

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);
		

	rec.setSize(100, 25);
	rec.setPosition(10, 0);
	rec.setText(L"T - ���/���� �������\nL - ���/���� ���������\nF - ���� �� ������\nG - ������� ���� �� �����������\nG+��� ������� ���� �� ���������",0,0,0);  //T - ���/���� �������\nL - ���/���� ���������\nF - ���� �� ������\nG - ������� ���� �� �����������\nG+��� ������� ���� �� ���������

	tick_o = GetTickCount();
	tick_n = GetTickCount();

	chrono_time_base = std::chrono::steady_clock::now();

	mode = Mode::PLANE;

}

bool canRender = false;
bool isRendering = false;



void Render(OpenGL *ogl)
{   
	
	auto chrono_time_interval = std::chrono::steady_clock::now() - chrono_time_base;
	auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(chrono_time_interval);
	int ms_int = ms.count();
	
	//���� ����
	tick_o = tick_n;
	tick_n = GetTickCount();
	Time += (tick_n - tick_o);

	rec.setText(std::to_wstring(ms_int).data());

	glActiveTexture(GL_TEXTURE0);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);	
	glEnable(GL_DEPTH_TEST);


	//
	_shader.UseShader();
	int location = glGetUniformLocationARB(_shader.program, "iTexture0");
	if (location > -1)
	{
		glActiveTexture(GL_TEXTURE0);
		tex[0].bindTexture();
		glUniform1iARB(location, 0);
		
	}
	location = glGetUniformLocationARB(_shader.program, "iTexture1");
	if (location > -1)
	{
		glActiveTexture(GL_TEXTURE1);
		tex[1].bindTexture();
		glUniform1iARB(location, 1);

	}
	location = glGetUniformLocationARB(_shader.program, "iTexture2");
	if (location > -1)
	{
		glActiveTexture(GL_TEXTURE2);
		tex[2].bindTexture();
		glUniform1iARB(location, 2);

	}
	location = glGetUniformLocationARB(_shader.program, "iTexture3");
	if (location > -1)
	{
		glActiveTexture(GL_TEXTURE3);
		tex[3].bindTexture();
		glUniform1iARB(location, 3);
	}

	location = glGetUniformLocationARB(_shader.program, "iTime");
	if (location > -1)
	{
		glUniform1iARB(location, ms_int);
	}
	location = glGetUniformLocationARB(_shader.program, "iLightPos");
	if (location > -1)
	{
		glUniform3fARB(location, light.pos.X(), light.pos.Y(), light.pos.Z());
	}

	location = glGetUniformLocationARB(_shader.program, "iCamPos");
	if (location > -1)
	{
		glUniform3fARB(location, camera.pos.X(), camera.pos.Y(), camera.pos.Z());
	}

	location = glGetUniformLocationARB(_shader.program, "iResolution");
	if (location > -1)
	{
		glUniform2fARB(location,ogl_obj->getWidth(), ogl_obj->getHeight());
	}



	location = glGetUniformLocationARB(_shader.program, "iModelViewMatrix");
	if (location > -1)
	{
		float mv_matrix[16];
		glGetFloatv(GL_MODELVIEW_MATRIX, mv_matrix);
		glUniformMatrix4fv(location,1, false, mv_matrix);
	}

	glNormal3d(0, 0, 1);

	if (mode == Mode::PLANE)
	{
		glBegin(GL_QUADS);
		
		glTexCoord2d(1, 1);
		glVertex3d(3, 3, 0);
		
		glTexCoord2d(1, 0);
		glVertex3d(3, -3, 0);
		
		glTexCoord2d(0, 0);
		glVertex3d(-3, -3, 0);
		
		glTexCoord2d(0, 1);
		glVertex3d(-3, 3, 0);
		glEnd();

	}
	if (mode == Mode::MODEL)
	{
		//glScaled(0.1, 0.1, 0.1);
		_model.DrawObj();
	}
	if (mode == Mode::ORTHO)
	{
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glOrtho(0, 1, 0, 1, 0, 1);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();


		glBegin(GL_QUADS);
		
		glTexCoord2d(1, 1);
		glVertex3d(1, 1, 0);

		glTexCoord2d(1, 0);
		glVertex3d(1, 0, 0);
		
		glTexCoord2d(0, 0);
		glVertex3d(0, 0, 0);
		
		glTexCoord2d(0, 1);
		glVertex3d(0, 1, 0);
		
		glEnd();

		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();

	}

	if (mode == Mode::BUTTERFLY)
	{
		/*PUSH;*/

		glColor3b(0.5, 0.2, 0.2);

		glBegin(GL_QUADS);
		//glBegin(GL_POLYGON);

		glVertex3d(5, 0.5, 0.5);
		glVertex3d(5, -0.5, 0.5);
		glVertex3d(5, -0.5, -0.5);
		glVertex3d(5, 0.5, -0.5);

		/*glEnd;

		glBegin(GL_POLYGON);*/

		glVertex3d(-10, 0.5, 0.5);
		glVertex3d(-10, -0.5, 0.5);
		glVertex3d(-10, -0.5, -0.5);
		glVertex3d(-10, 0.5, -0.5);

		glEnd;

		//glBegin(GL_POLYGON);
		glBegin(GL_QUAD_STRIP);

		glVertex3d(5, 0.5, 0.5);
		glVertex3d(-10, 0.5, 0.5);
		glVertex3d(-10, -0.5, 0.5);
		glVertex3d(5, -0.5, 0.5);

		glVertex3d(-10, -0.5, 0.5);
		glVertex3d(5, -0.5, 0.5);
		glVertex3d(5, 0.5, -0.5);
		glVertex3d(-10, 0.5, -0.5);

		glVertex3d(5, 0.5, -0.5);
		glVertex3d(-10, 0.5, -0.5);
		glVertex3d(-10, -0.5, -0.5);
		glVertex3d(5, -0.5, -0.5);

		glVertex3d(-10, -0.5, -0.5);
		glVertex3d(5, -0.5, -0.5);
		glVertex3d(5, 0.5, 0.5);
		glVertex3d(-10, 0.5, 0.5);

		glEnd;
		/*POP;*/
		glBegin(GL_QUADS);
		glEnd();


		double delta = 0.01;




		PUSH;

		glRotated((sin(Time * 0.005) + 1) * 30, 1, 0, 0);
		glScaled(-10, -10, -10);
		glBegin(GL_QUADS);

		for (double i = 0; i < 1; i += delta)
		{
			glTexCoord2d(0.5 - 0.5 * i, 1 - i * i);
			glVertex3d(0.5-0.5*i, 1-i*i, 0);

			glTexCoord2d(0.5 + 0.5 * i, 1 - i * i);
			glVertex3d(0.5 + 0.5 * i, 1-i*i, 0);

			glTexCoord2d(0.5 + 0.5 * (i + delta), 1 - (i + delta) * (i + delta));
			glVertex3d(0.5 + 0.5 * (i + delta), 1-(i + delta)* (i + delta), 0);

			glTexCoord2d(0.5 - 0.5 * (i + delta), 1 - (i + delta) * (i + delta));
			glVertex3d(0.5 - 0.5 * (i + delta), 1-(i + delta)*(i + delta), 0);

		}


		glEnd();

		POP;


		PUSH;
		glRotated(-(sin(Time * 0.005) + 1) * 30, 1, 0, 0);
		glScaled(-10, -10, -10);
		glBegin(GL_QUADS);

		for (double i = 0; i > -1; i -= delta)
		{
			glTexCoord2d(0.5 - 0.5 * (i + delta), 1 - (i + delta) * (i + delta));
			glVertex3d(0.5 - 0.5 * i, -1 + i * i, 0);

			glTexCoord2d(0.5 + 0.5 * (i + delta), 1 - (i + delta) * (i + delta));
			glVertex3d(0.5 + 0.5 * i, -1 + i * i, 0);

			glTexCoord2d(0.5 + 0.5 * i, 1 - i * i);
			glVertex3d(0.5 + 0.5 * (i - delta), -1 + (i - delta) * (i - delta), 0);

			glTexCoord2d(0.5 - 0.5 * i, 1 - i * i);
			glVertex3d(0.5 - 0.5 * (i - delta), -1 + (i - delta) * (i - delta), 0);
		}

		glEnd();
		POP;


		
	}
	
	glActiveTexture(GL_TEXTURE0);
	RenderGUI(ogl_obj);
}   //����� ���� �������


bool gui_init = false;

//������ ���������, ��������� ����� �������� �������
void RenderGUI(OpenGL *ogl)
{
	
	Shader::DontUseShaders();

	glMatrixMode(GL_PROJECTION);
	PUSH;
	glLoadIdentity();
	glOrtho(0, ogl->getWidth(), 0, ogl->getHeight(), 0, 1);
	glMatrixMode(GL_MODELVIEW);
	PUSH;
	glLoadIdentity();

	glDisable(GL_LIGHTING);
	

	
	rec.Draw();

	glMatrixMode(GL_PROJECTION);
	POP;
	glMatrixMode(GL_MODELVIEW);
	POP;

	Shader::DontUseShaders(); 

}

void resizeEvent(OpenGL *ogl, int newW, int newH)
{
	rec.setPosition(10, 0);
}

