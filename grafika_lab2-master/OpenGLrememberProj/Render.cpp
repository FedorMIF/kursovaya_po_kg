#include "Render.h"

#include <sstream>
#include <iostream>

#include <windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>

#include "MyOGL.h"

#include "Camera.h"
#include "Light.h"
#include "Primitives.h"

#include "GUItextRectangle.h"

bool textureMode = true;
bool lightMode = false;

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
		camDist = 15;
		fi1 = 1;
		fi2 = 1;
	}


	//������� ������� ������, ������ �� ����� ��������, ���������� �������
	void SetUpCamera()
	{
		//�������� �� ������� ������ ������
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
		//������� ��������� ������
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
		glDisable(GL_LIGHTING);


		glColor3d(0.9, 0.8, 0);
		Sphere s;
		s.pos = pos;
		s.scale = s.scale*0.08;
		s.Show();

		if (OpenGL::isKeyPressed('G'))
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
float bazex = 0, bazey = 0, bazez = 0;

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


	//������� ���� �� ���������, � ����� ��� ����
	if (OpenGL::isKeyPressed('G') && !OpenGL::isKeyPressed(VK_LBUTTON))
	{
		LPPOINT POINT = new tagPOINT();
		GetCursorPos(POINT);
		ScreenToClient(ogl->getHwnd(), POINT);
		POINT->y = ogl->getHeight() - POINT->y;

		Ray r = camera.getLookRay(POINT->x, POINT->y);

		double z = light.pos.Z();

		double k = 0, x = 0, y = 0;
		if (r.direction.Z() == 0)
			k = 0;
		else
			k = (z - r.origin.Z()) / r.direction.Z();

		x = k * r.direction.X() + r.origin.X();
		y = k * r.direction.Y() + r.origin.Y();

		light.pos = Vector3(x, y, z);
	}

	if (OpenGL::isKeyPressed('G') && OpenGL::isKeyPressed(VK_LBUTTON))
	{
		light.pos = light.pos + Vector3(0, 0, 0.02*dy);
	}


}

void mouseWheelEvent(OpenGL *ogl, int delta)
{

	if (delta < 0 && camera.camDist <= 1)
		return;
	if (delta > 0 && camera.camDist >= 100)
		return;

	camera.camDist += 0.01*delta;

}

void keyDownEvent(OpenGL *ogl, int key)
{
	if (key == 'L')
	{
		lightMode = !lightMode;
	}

	if (key == 'T')
	{
		textureMode = !textureMode;
	}

	if (key == 'R')
	{
		camera.fi1 = 1;
		camera.fi2 = 1;
		camera.camDist = 15;

		light.pos = Vector3(1, 1, 3);
	}

	if (key == 'F')
	{
		light.pos = camera.pos;
	}

	if (key == 'W') {
		bazex += 0.1;
	}

	if (key == 'S') {
		bazex -= 0.1;
	}

	if (key == 'A') {
		bazey += 0.1;
	}

	if (key == 'D') {
		bazey -= 0.1;
	}
}

void keyUpEvent(OpenGL *ogl, int key)
{

}



GLuint texId;
GLuint skinId;

//����������� ����� ������ ��������
void initRender(OpenGL *ogl)
{
	//��������� �������

	//4 ����� �� �������� �������
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//��������� ������ ��������� �������
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//�������� ��������
	glEnable(GL_TEXTURE_2D);


	//������ ����������� ���������  (R G B)
	RGBTRIPLE *texarray;

	//������ ��������, (������*������*4      4, ���������   ����, �� ������� ������������ �� 4 ����� �� ������� �������� - R G B A)
	char *texCharArray;
	int texW, texH;
	OpenGL::LoadBMP("teture.bmp", &texW, &texH, &texarray);
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);



	//���������� �� ��� ��������
	glGenTextures(1, &texId);
	//������ ��������, ��� ��� ����� ����������� � ���������, ����� ����������� �� ����� ��
	glBindTexture(GL_TEXTURE_2D, texId);

	//��������� �������� � �����������, � ���������� ��� ������  ��� �� �����
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);

	//�������� ������
	free(texCharArray);
	free(texarray);

	//������� ����
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


	//2
	glBindTexture(GL_TEXTURE_2D, 0);
	OpenGL::LoadBMP("skin.bmp", &texW, &texH, &texarray);
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);
	glGenTextures(1, &skinId);
	glBindTexture(GL_TEXTURE_2D, skinId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);
	free(texCharArray);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);




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

	camera.fi1 = -1.3;
	camera.fi2 = 0.8;
}
std::vector<double> matr(double x1, double y1, double z1, double x2, double y2, double z2, double x3, double y3, double z3)
{

	double sum = 0;
	//double v1 = 0, v2 = 0, v3 = 0;
	auto ir = (y2 - y1) * (z3 - z1) - (y3 - y1) * (z2 - z1);//���������� ����������� �������
	auto jr = -((x2 - x1) * (z3 - z1) - (z2 - z1) * (x3 - x1));
	auto kr = (x2 - x1) * (y3 - y1) - (y2 - y1) * (x3 - x1);
	sum = sqrt(ir * ir + jr * jr + kr * kr);
	ir /= sum;
	jr /= sum;
	kr /= sum;

	return std::vector<double>({ ir, jr, kr });
}

void glMyNormal9d(double x1, double y1, double z1, double x2, double y2, double z2, double x3, double y3, double z3)
{
	auto n = matr(x1, y1, z1, x2, y2, z2, x3, y3, z3);
	glNormal3d(n[0], n[1], n[2]);
}

void drawTexturedCircle(double x, double y, double z, double r) {
	// x, y, z -- ���������� ������ ����������, r -- ������
	const int triangleAmount = 25; // ���-�� �������������, ������������ ��� ���������� �����
	glBindTexture(GL_TEXTURE_2D, texId);
	glBegin(GL_TRIANGLES);
	glColor3d(1, 1, 1);
	for (int i = 1; i <= triangleAmount; i++) {
		glTexCoord2d(0.5, 0.5);
		glNormal3d(0, 0, 1);
		glVertex3d(x, y, z);
		glTexCoord2d(
			0.5 + 0.5 * cos(((M_PI * 2) / triangleAmount) * (i - 1)),
			0.5 + 0.5 * sin(((M_PI * 2) / triangleAmount) * (i - 1))
		);
		glVertex3d(
			x + r * cos(((M_PI * 2) / triangleAmount) * (i - 1)),
			y + r * sin(((M_PI * 2) / triangleAmount) * (i - 1)),
			z
		);
		glTexCoord2d(
			0.5 + 0.5 * cos(((M_PI * 2) / triangleAmount) * i),
			0.5 + 0.5 * sin(((M_PI * 2) / triangleAmount) * i)
		);
		glVertex3d(
			x + r * cos(((M_PI * 2) / triangleAmount) * i),
			y + r * sin(((M_PI * 2) / triangleAmount) * i),
			z
		);
	}
	glEnd();
}

void drawMyFigure() {

	glBindTexture(GL_TEXTURE_2D, skinId);
	glBegin(GL_QUADS);
	glMyNormal9d(3, 6, 0, 0, 6, 0, 3, 3, 2);
	glColor3d(1, 1, 1);
	glTexCoord2d(0, 1);
	glVertex3d(3, 6, 0);//1
	glTexCoord2d(0, 0);
	glVertex3d(0, 6, 0);//2
	glTexCoord2d(1, 0);
	glVertex3d(0, 3, 2);//3
	glTexCoord2d(1, 1);
	glVertex3d(3, 3, 2);//4
	glEnd();

	glBegin(GL_QUADS);
	glMyNormal9d(0, 3, 2, 3, 3, 2, 0, 3, 8);
	//glColor3d(0.150, 0.200, 0.050);
	glTexCoord2d(1, 1);
	glVertex3d(0, 3, 2);//3
	glTexCoord2d(0, 1);
	glVertex3d(3, 3, 2);//4
	glTexCoord2d(1, 0);
	glVertex3d(3, 3, 8);//5
	glTexCoord2d(0, 0);
	glVertex3d(0, 3, 8);//6
	glEnd();

	glBegin(GL_QUADS);
	glMyNormal9d(3, 3, 8, 0, 3, 8, 3, 6, 10);
	//glColor3d(0.150, 0.150, 0.050);
	glTexCoord2d(1, 1);
	glVertex3d(3, 3, 8);//5
	glTexCoord2d(1, 0);
	glVertex3d(0, 3, 8);//6
	glTexCoord2d(0, 1);
	glVertex3d(0, 6, 10);//7
	glTexCoord2d(0, 0);
	glVertex3d(3, 6, 10);//8
	glEnd();

	glBegin(GL_QUADS);
	glMyNormal9d(0, 6, 10, 3, 6, 10, 0, 9, 5);
	//glColor3d(0.150, 0.050, 0.100);
	glTexCoord2d(1, 1);
	glVertex3d(0, 6, 10);//7
	glTexCoord2d(1, 0);
	glVertex3d(3, 6, 10);//8
	glTexCoord2d(0, 1);
	glVertex3d(3, 9, 5);//9
	glTexCoord2d(0, 0);
	glVertex3d(0, 9, 5);//10
	glEnd();

	glBegin(GL_QUADS);
	glMyNormal9d(3, 9, 5, 0, 9, 5, 3, 7, 4);
	//glColor3d(0.250, 0.150, 0.050);
	glTexCoord2d(1, 1);
	glVertex3d(3, 9, 5);//9
	glTexCoord2d(1, 0);
	glVertex3d(0, 9, 5);//10
	glTexCoord2d(0, 1);
	glVertex3d(0, 7, 4);//11
	glTexCoord2d(0, 0);
	glVertex3d(3, 7, 4);//12
	glEnd();

	glBegin(GL_QUADS);
	glMyNormal9d(0, 7, 4, 3, 7, 4, 0, 9, 3);
	//glColor3d(0.50, 0.250, 0.100);
	glTexCoord2d(1, 1);
	glVertex3d(0, 7, 4);//11
	glTexCoord2d(1, 0);
	glVertex3d(3, 7, 4);//12
	glTexCoord2d(0, 1);
	glVertex3d(3, 9, 3);//13
	glTexCoord2d(0, 0);
	glVertex3d(0, 9, 3);//14
	glEnd();

	glBegin(GL_QUADS);
	glMyNormal9d(3, 9, 3, 0, 9, 3, 3, 6, 0);
	//glColor3d(0.100, 0.100, 0.250);
	glTexCoord2d(1, 1);
	glVertex3d(3, 9, 3);//13
	glTexCoord2d(1, 0);
	glVertex3d(0, 9, 3);//14
	glTexCoord2d(0, 1);
	glVertex3d(0, 6, 0);//2
	glTexCoord2d(0, 0);
	glVertex3d(3, 6, 0);//1
	glEnd();

	///////////////////////////////

	glBegin(GL_QUADS);
	glMyNormal9d(0, 9, 5, 0, 7, 4, 0, 6, 10);
	//glColor3d(0.00, 0.100, 0.100);
	glTexCoord2d(1, 1);
	glVertex3d(0, 9, 5);//10
	glTexCoord2d(0, 1);
	glVertex3d(0, 7, 4);//11
	glTexCoord2d(1, 0);
	glVertex3d(0, 3, 8);//6
	glTexCoord2d(0, 0);
	glVertex3d(0, 6, 10);//7
	glEnd();

	glBegin(GL_QUADS);
	glMyNormal9d(0, 6, 0, 0, 3, 2, 0, 9, 3);
	//glColor3d(0.00, 0.100, 0.100);
	glTexCoord2d(1, 1);
	glVertex3d(0, 6, 0);//2
	glTexCoord2d(1, 0);
	glVertex3d(0, 3, 2);//3
	glTexCoord2d(0, 1);
	glVertex3d(0, 7, 4);//11
	glTexCoord2d(0, 0);
	glVertex3d(0, 9, 3);//14
	glEnd();

	glBegin(GL_TRIANGLES);
	glMyNormal9d(0, 7, 4, 0, 3, 2, 0, 3, 8);
	//glColor3d(0.00, 0.100, 0.100);
	glTexCoord2d(1, 1);
	glVertex3d(0, 7, 4);//11
	glTexCoord2d(0, 1);
	glVertex3d(0, 3, 8);//6
	glTexCoord2d(1, 0);
	glVertex3d(0, 3, 2);//3
	glEnd();

	glBegin(GL_QUADS);
	glMyNormal9d(3, 6, 10, 3, 3, 8, 3, 9, 5);
	//glColor3d(0.200, 0.00, 0.00);
	glTexCoord2d(1, 1);
	glVertex3d(3, 6, 10);//8
	glTexCoord2d(0, 1);
	glVertex3d(3, 9, 5);//9
	glTexCoord2d(1, 0);
	glVertex3d(3, 7, 4);//12
	glTexCoord2d(0, 0);
	glVertex3d(3, 3, 8);//5
	glEnd();

	glBegin(GL_QUADS);
	glMyNormal9d(3, 7, 4, 3, 3, 2, 3, 9, 3);
	//glColor3d(0.200, 0.00, 0.0);
	glTexCoord2d(1, 1);
	glVertex3d(3, 7, 4);//12
	glTexCoord2d(1, 0);
	glVertex3d(3, 9, 3);//13
	glTexCoord2d(0, 1);
	glVertex3d(3, 6, 0);//1
	glTexCoord2d(0, 0);
	glVertex3d(3, 3, 2);//4
	glEnd();

	glBegin(GL_TRIANGLES);
	glMyNormal9d(3, 7, 4, 3, 3, 8, 3, 2, 3);
	//glColor3d(0.200, 0.00, 0.0);
	glTexCoord2d(1, 1);
	glVertex3d(3, 7, 4);//12
	glTexCoord2d(0, 1);
	glVertex3d(3, 3, 8);//5
	glTexCoord2d(1, 0);
	glVertex3d(3, 3, 2);//4
	glEnd();

	////////////////////////////

	glBegin(GL_TRIANGLES);
	double k = 1.80;
	//glColor3d(0.4, 0.6, 1);
	for (double i = 0; i < 700; i++)
	{
		if ((sin(i) > -0.98) && (cos(i) > -0.923))
		{
			double x = 4.5 + k * cos(i), y = 9 + k * sin(i), x1 = 4.5 + k * cos(i + 1), y1 = 9 + k * sin(i + 1);
			glMyNormal9d(2.995, x, y, 2.995, x1, y1, 2.995, 4.5, 9);
			glTexCoord2d(0, 0);
			glVertex3d(2.995, x, y);
			glTexCoord2d(0, 1);
			glVertex3d(2.995, 4.5, 9);
			glTexCoord2d(1, 0);
			glVertex3d(2.995, x1, y1);
			glMyNormal9d(0.005, x, y, 0.005, 4.5, 9, 0.005, x1, y1);
			glTexCoord2d(0, 0);
			glVertex3d(0.005, x, y);
			glTexCoord2d(0, 1);
			glVertex3d(0.005, 4.5, 9);
			glTexCoord2d(1, 0);
			glVertex3d(0.005, x1, y1);


		}
	}

	glEnd();
	glBegin(GL_QUADS);
	for (double i = 0; i < 700; i++)
	{
		if ((sin(i) > -0.98) && (cos(i) > -0.988))
		{
			double x = 4.5 + k * cos(i), y = 9 + k * sin(i), x1 = 4.5 + k * cos(i + 1), y1 = 9 + k * sin(i + 1);
			double x2 = 4.5 + k * cos(i + 0.5), y2 = 9 + k * sin(i + 0.5);
			glMyNormal9d(2.995, x, y, 0.005, x, y, 2.995, x2, y2);
			glTexCoord2d(1, 1);
			glVertex3d(2.995, x, y);
			glTexCoord2d(1, 0);
			glVertex3d(2.995, x2, y2);
			glTexCoord2d(0, 1);
			glVertex3d(0.005, x2, y2);
			glTexCoord2d(0, 0);
			glVertex3d(0.005, x, y);


		}
	}
}

void Render(OpenGL* ogl)
{



	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);
	if (textureMode)
		glEnable(GL_TEXTURE_2D);

	if (lightMode)
		glEnable(GL_LIGHTING);


	//��������������
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	//��������� ���������
	GLfloat amb[] = { 0.2, 0.2, 0.1, 1. };
	GLfloat dif[] = { 0.4, 0.65, 0.5, 1. };
	GLfloat spec[] = { 0.9, 0.8, 0.3, 1. };
	GLfloat sh = 0.1f * 256;


	//�������
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//��������
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//����������
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec); \
		//������ �����
		glMaterialf(GL_FRONT, GL_SHININESS, sh);

	//���� ���� �������, ��� ����������� (����������� ���������)
	glShadeModel(GL_SMOOTH);
	//===================================
	//������� ��� 

	////������ ��������� ���������� ��������
	//double A[2] = { -4, -4 };
	//double B[2] = { 4, -4 };
	//double C[2] = { 4, 4 };
	//double D[2] = { -4, 4 };

	//glBindTexture(GL_TEXTURE_2D, texId);

	//glColor3d(0.6, 0.6, 0.6);
	//glBegin(GL_QUADS);

	//glNormal3d(0, 0, 1);
	//glTexCoord2d(0, 0);
	//glVertex2dv(A);
	//glTexCoord2d(1, 0);
	//glVertex2dv(B);
	//glTexCoord2d(1, 1);
	//glVertex2dv(C);
	//glTexCoord2d(0, 1);
	//glVertex2dv(D);

	//glEnd();
	//����� ��������� ���������� ��������

	drawTexturedCircle(0, 0, 0, 10);

	glTranslatef(bazex, bazey, bazez);
	drawMyFigure();

	//��������� ������ ������


	glMatrixMode(GL_PROJECTION);	//������ �������� ������� ��������. 
									//(���� ��������� ��������, ����� �� ������������.)
	glPushMatrix();   //��������� ������� ������� ������������� (������� ��������� ������������� ��������) � ���� 				    
	glLoadIdentity();	  //��������� ��������� �������
	glOrtho(0, ogl->getWidth(), 0, ogl->getHeight(), 0, 1);	 //������� ����� ������������� ��������

	glMatrixMode(GL_MODELVIEW);		//������������� �� �����-��� �������
	glPushMatrix();			  //��������� ������� ������� � ���� (��������� ������, ����������)
	glLoadIdentity();		  //���������� �� � ������

	glDisable(GL_LIGHTING);



	GuiTextRectangle rec;		   //������� ����� ��������� ��� ������� ������ � �������� ������.
	rec.setSize(300, 150);
	rec.setPosition(10, ogl->getHeight() - 150 - 10);


	std::stringstream ss;
	ss << "T - ���/���� �������" << std::endl;
	ss << "L - ���/���� ���������" << std::endl;
	ss << "F - ���� �� ������" << std::endl;
	ss << "G - ������� ���� �� �����������" << std::endl;
	ss << "G+��� ������� ���� �� ���������" << std::endl;
	ss << "�����. �����: (" << light.pos.X() << ", " << light.pos.Y() << ", " << light.pos.Z() << ")" << std::endl;
	ss << "�����. ������: (" << camera.pos.X() << ", " << camera.pos.Y() << ", " << camera.pos.Z() << ")" << std::endl;
	ss << "��������� ������: R=" << camera.camDist << ", fi1=" << camera.fi1 << ", fi2=" << camera.fi2 << std::endl;

	rec.setText(ss.str().c_str());
	rec.Draw();

	glMatrixMode(GL_PROJECTION);	  //��������������� ������� �������� � �����-��� �������� �� �����.
	glPopMatrix();


	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}