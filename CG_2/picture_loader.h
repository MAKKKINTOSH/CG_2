#ifndef PICTURE_LOADER_H_INCLUDED
#define PICTURE_LOADER_H_INCLUDED

void LoadPicture(char *filename, GLuint *PictureID, int swarp, int twarp, int filter);
void RenderPicture(GLuint picture);

#endif // PICTURE_LOADER_H_INCLUDED
