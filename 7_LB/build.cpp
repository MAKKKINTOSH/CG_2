#include <windows.h>
#include <gl/gl.h>
#include <math.h>
#include <iostream>

float vert[] = {1,1,0, 1,-1,0, -1,-1,0, -1,1,0};
float vert1[] = {1,1,5, 1,-1,5, -1,-1,5, -1,1,5};
float positionLight[] = {-1,-1,1,0};

void InitSurface(){
    glNormal3f(0,0,1);

    glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, 0, &vert);
        for (int i = -10; i < 10; i++){
            for(int j = -5; j < 5; j++){
                glPushMatrix();
                    if((i+j)%2 == 0) glColor3f(0.41,0.41,0.41);
                    else glColor3f(1,1,1);
                    glTranslatef(i*2, j*2, 0);
                    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
                glPopMatrix();
            }
        }
    glDisableClientState(GL_VERTEX_ARRAY);
}

void draw(){
    glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, 0, &vert1);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glDisableClientState(GL_VERTEX_ARRAY);
}

void drawPrisma(float weight, float height, int sidesQuantity, float x, float y, float z, float transparency) {
    float angle = (2 * 3.14) / sidesQuantity;
    float halfHeight = height / 2.0f;

    glPushMatrix();

        glTranslatef(x, y, z);
        glEnable(GL_NORMALIZE);
        glEnable(GL_COLOR_MATERIAL);

        glTranslatef(0, 0, 0.1);

        for (int i = 0; i < sidesQuantity; ++i) {
            GLfloat mat_diffuse[] = { 1.0f, 1.0f, 1.0f, transparency};
            GLfloat mat_ambient[] = { 1.0f, 1.0f, 1.0f, transparency};
            glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
            glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
            glColor4f(0.5, 0.22, 0.74, transparency);//

            float x1 = (weight * cos(i * angle));
            float y1 = (weight * sin(i * angle));
            float z1 = 0;

            float x2 = (weight * cos((i + 1) * angle));
            float y2 = (weight * sin((i + 1) * angle));
            float z2 = 0;

            //ÏÐÈÇÌÀ

            glBegin(GL_TRIANGLES);
            glNormal3f(x1, y1, 0.0f);
            glVertex3f(x1/1.5, y1/1.5, z1 + height);
            glNormal3f(x2, y2, 0.0f);
            glVertex3f(x2*2, y2*2, z2);
            glNormal3f(x2, y2, 0.0f);
            glVertex3f(x2/1.5, y2/1.5, z2 + height);
            glEnd();

            glBegin(GL_TRIANGLES);
            glNormal3f(x1, y1, 0.0f);
            glVertex3f(x1*2, y1*2, z1);
            glNormal3f(x2, y2, 0.0f);
            glVertex3f(x2*2, y2*2, z2);
            glNormal3f(x1, y1, 0.0f);
            glVertex3f(x1/1.5, y1/1.5, z1 + height);
            glEnd();

            glBegin(GL_TRIANGLES);
            glNormal3f(0.0f, 0.0f, 1.0f);
            glVertex3f(0.0f, 0.0f, z1);
            glVertex3f(x2*2, y2*2, z2);
            glVertex3f(x1*2, y1*2, z1);
            glEnd();

            glBegin(GL_TRIANGLES);
            glNormal3f(0.0f, 0.0f, 1.0f);
            glVertex3f(0.0f, 0.0f, z1 + height);
            glVertex3f(x1/1.5, y1/1.5, z1 + height);
            glVertex3f(x2/1.5, y2/1.5, z2 + height);
            glEnd();

            glColor4f(0, 1, 0, transparency);

            glBegin(GL_LINE_LOOP);
            glVertex3f(x1*2, y1*2, z1);
            glVertex3f(x2*2, y2*2, z2);
            glVertex3f(x1/1.5, y1/1.5, z1 + height);
            glEnd();

            glBegin(GL_LINE_LOOP);
            glVertex3f(x1/1.5, y1/1.5, z1 + height);
            glVertex3f(x2*2, y2*2, z2);
            glVertex3f(x2/1.5, y2/1.5, z2 + height);
            glEnd();

            glBegin(GL_LINE_LOOP);
            glVertex3f(0.0f, 0.0f, z1);
            glVertex3f(x2*2, y2*2, z2);
            glVertex3f(x1*2, y1*2, z1);
            glEnd();

            glBegin(GL_LINE_LOOP);
            glVertex3f(0.0f, 0.0f, z1 + height);
            glVertex3f(x1/1.5, y1/1.5, z1 + height);
            glVertex3f(x2/1.5, y2/1.5, z2 + height);
            glEnd();
        }
    glPopMatrix();
}

void DrawLine(){
    glLineWidth(5);
    glBegin(GL_LINES);
        glColor3f(1,0,0);
        glVertex3f(1,1,1);
        glVertex3f(1,1,10);

        glColor3f(0,1,0);
        glVertex3f(1,1,1);
        glVertex3f(1,10,1);

        glColor3f(0,0,1);
        glVertex3f(1,1,1);
        glVertex3f(10,1,1);
    glEnd();
}
