#include <gl/gl.h>
#include "texture_manager.h"
#include "stb-master/stb_image.h"

void LoadPicture(char *filename, GLuint *texture)
{
    int width, hight, cnt;
    unsigned char *data = stbi_load(filename, &width, &hight, &cnt, 0);

    glGenTextures(1, texture);
    glBindTexture(GL_TEXTURE_2D, *texture);

        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, hight, 0, cnt == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data);

    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(data);
}

void RenderPicture(GLuint picture)
{
    static float svertix[]= {1080*2,700, 1080*2,0, -1080,0, -1080,700};
    static float TexCord[]= {-1,1, -1,0, 2,0, 2,1 };
    glClearColor(0,0,0,0);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, picture);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glVertexPointer(2,GL_FLOAT,0,svertix);
    glTexCoordPointer(2,GL_FLOAT,0,TexCord);
    glDrawArrays(GL_TRIANGLE_FAN,0,4);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}
