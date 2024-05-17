#include <windows.h>
#include <gl/gl.h>
#include <math.h>
#include <iostream>

GLfloat light_position[] = { 0.0f, 0.0f, 2.0f, 0.f }; //������� ���������
GLfloat light_spot_direction[] = {0.0, 0.0, -2.0, 1.0}; // ������� ����
GLfloat light_ambient[] = { 0.1f, 0.1f, 0.1f, 1.0f }; //���������
GLfloat light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f }; //���������
GLfloat light_specular[] = { 0.2f, 0.2f, 0.2f, 32.0f }; //���������

bool x = true;

void Init_Light()
{
     glEnable(GL_LIGHTING); //����� ��������� ��� ����� ������������
     glShadeModel(GL_SMOOTH);

     glLightfv(GL_LIGHT0, GL_POSITION, light_position);
     glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 15); // ����� ���
    //������������� ���������
     glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, light_spot_direction);
     glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 8.0); // ����������
    //�������� �������������
    //�������������� �������� ��� ��������� LIGHT0
     glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
     glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
     glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
     glEnable(GL_LIGHT0); // �������� ����� LIGHT0


}

void Init_Material()
{
    glEnable(GL_COLOR_MATERIAL); //���������� �������������
    //���������
     glShadeModel(GL_SMOOTH); // ���������� �������
     GLfloat material_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
     GLfloat material_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
     GLfloat material_specular[] = { 1.0f, 1.0f, 1.0f, 32.0f };
     GLfloat material_shininess[] = { 50.0f }; //����� ���������

     GLfloat mat_diffuse[] = {1.0, 1.0, 1.0, 0.5}; // ������ �����-����� 0.5
     GLfloat mat_ambient[] = {1.0, 1.0, 1.0, 0.5};

     glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
     glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
     glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
     glMaterialfv(GL_FRONT, GL_SHININESS, material_shininess);

     glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void on_light(){
    Init_Light();
    Init_Material();
}

