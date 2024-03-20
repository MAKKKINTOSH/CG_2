#include <gl/gl.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb-master/stb_image.h"
#include "picture_loader.h"

void LoadPicture(char *filename, GLuint *PictureID, int swarp, int twarp, int filter)
{
    int width, hight, cnt;
    unsigned char *data = stbi_load(filename, &width, &hight, &cnt, 0);
    glGenTextures(1, PictureID);
    glBindTexture(GL_TEXTURE_2D, *PictureID);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, swarp);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, twarp);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, hight, 0, cnt == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data);
    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(data);
}

void RenderPicture(GLuint picture)
{
    static float svertix[] = {0,0, 256,0, 256,256, 0,256};
    static float TexCord[] = {0,0, 1,0, 1,1, 0,1};
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, picture);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, svertix);
    glTexCoordPointer(2, GL_FLOAT, 0, TexCord);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}
