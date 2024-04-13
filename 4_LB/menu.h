#ifndef MENU_H_INCLUDED
#define MENU_H_INCLUDED

int MenuAddButton(char *name, float x, float y, float width, float height, float textScale);
void MenuShowMenu();
int Menu_MouseMove(float x, float y);
char isCoordInButton(int buttonId, float x, float y);
int Menu_MouseDown();
void Menu_MouseUp();


#endif // MENU_H_INCLUDED
