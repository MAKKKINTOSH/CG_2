#include <windows.h>
#include <gl/gl.h>
#include <math.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb-master/stb_image.h"
#include "stb-master/stb_easy_font.h"
#include "menu.h"

#define WINDOW_HEIGHT 1000
#define WINDOW_WIDTH 1600

#include <float.h>




LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);

// ���������� ����������
int button1 = 0;
int button2 = 0;

GLuint Idle_sprite, Walk_sprite, Jump_sprite, background, brickTile, groundTile;

const float blockSize = 40.0f; // ������ ������ �����
#define H 25 //������ ������ � ������ 1000/40
#define W 40 //������ ������ � ������ 1600/40

int currentFrame = 0;
const int totalFrames = 8;
float frameWidth = 1.0f / 8.0f;
int isMoving = 0;
float jumpSpeed = 30.0f;
float gravity = -2.0f;
BOOL isAirborne = TRUE;



char TileMap[H][W] = {
    "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB",
    "B                                      B",
    "B                                      B",
    "B                                      B",
    "BB                                     B",
    "B                                      B",
    "B                                      B",
    "BBB                                    B",
    "B                    BBBBBBB           B",
    "B                                      B",
    "BBBBBBBB                               B",
    "B                                      B",
    "B                                      B",
    "B                                      B",
    "BBBBBBBBBB                             B",
    "B                          BBBBBBBBBBBBB",
    "B                                      B",
    "BBBBBBBBBBBB                           B",
    "                                        ",
    "                     BBBBB              ",
    "                                        ",
    "                                        ",
    "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB",
    "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB",
    "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB",
  };

typedef struct {
    float x, y;
    float dx, dy;
    BOOL isAirborne;
    BOOL isMoving;
    float width, height;
    BOOL faceRight;
    float scale;
    float currentJumpSpeed;
} Hero;

Hero hero = {
    .x = 0.0f,
    .y = 0.0f,
    .dx = 0.0f,
    .dy = 0.0f,
    .isAirborne = TRUE,
    .isMoving = FALSE,
    .width = 80.0f,
    .height = 120.0f,
    .scale = 1.0f,
    .currentJumpSpeed = 0.0f
};

void DrawMap() {
    for (int i = 0; i < H; ++i) {
        for (int j = 0; j < W; ++j) {
            char tile = TileMap[i][j];
            GLuint textureID = 0;

            // ���������� �������� ��� �������� �����
            switch(tile) {
                case 'B': // ������
                    textureID =  brickTile; // ����������� �������
                    break;
                default:
                    continue; // ���������� ������ ������������ ��� ����������� �������
            }

            // ��������� �����
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, textureID);
            glBegin(GL_QUADS);
                glColor3f(1,1,1);
                glTexCoord2f(0.0f, 0.0f); glVertex2f(j * blockSize, i * blockSize);
                glTexCoord2f(1.0f, 0.0f); glVertex2f(j * blockSize + blockSize, i * blockSize);
                glTexCoord2f(1.0f, 1.0f); glVertex2f(j * blockSize + blockSize, i * blockSize + blockSize);
                glTexCoord2f(0.0f, 1.0f); glVertex2f(j * blockSize, i * blockSize + blockSize);
            glEnd();
            glDisable(GL_TEXTURE_2D);
        }
    }
}


BOOL CollisionHappened(float x, float y, Hero *hero) {
    for (int i = (int)(y / blockSize); i < round((y + hero->height * hero->scale) / blockSize); i++) {
        for (int j = (int)(x / blockSize); j < round((x + hero->width * hero->scale) / blockSize); j++) {
            char tile = TileMap[i][j];
            if (tile == 'B') {
                return TRUE;
            }
        }
    }
    return FALSE;
}
void isCollisionRight(float x, float y, Hero *hero, int *result_arr){
    int blocks_x = (int)((x + hero->width)/blockSize);
    for (int j = (int)(y/blockSize); j >= (int)((y - hero->height)/blockSize); j--){
        if (TileMap[j][blocks_x] == 'B') {
            result_arr[0] = 1;
            result_arr[1] = blocks_x;
            result_arr[2] = j;
            return;
        }
    }
    result_arr[0] = 0;
}
void isCollisionLeft(float x, float y, Hero *hero, int *result_arr){
    int blocks_x = (int)(x/blockSize);
    for (int j = (int)(y/blockSize); j >= (int)((y - hero->height)/blockSize); j--){
        if (TileMap[j][blocks_x] == 'B') {
            result_arr[0] = 1;
            result_arr[1] = blocks_x;
            result_arr[2] = j;
            return;
        }
    }
    result_arr[0] = 0;
}
void isCollisionDown(float x, float y, Hero *hero, int *result_arr){
    int blocks_y = (int)(y/blockSize);
    for (int i = (int)(x/blockSize); i <= (int)((x + hero->width)/blockSize); i++){
        if (TileMap[blocks_y][i] == 'B') {
            result_arr[0] = 1;
            result_arr[1] = i;
            result_arr[2] = blocks_y;
            return;
        }
    }
    result_arr[0] = 0;
}
void isCollisionUp(float x, float y, Hero *hero, int *result_arr){
    int blocks_y = (int)((y + hero->height)/blockSize);
    for (int i = (int)(x/blockSize); i <= (int)((x + hero->width)/blockSize); i++){
        if (TileMap[blocks_y][i] == 'B') {
            result_arr[0] = 1;
            result_arr[1] = i;
            result_arr[2] = blocks_y;
            return;
        }
    }
    result_arr[0] = 0;
}

void UpdateHeroPositionAndCollisions(Hero *hero) {
    if (hero->y < 0) {
        hero->y = 0;
        hero->dy = 0;
    }
    if (hero->y > 1000.0f - hero->height*hero->scale) {
        hero->y = 1000.0f;
        hero->dy = 0;
    }
    if (hero->x < 0) {
        hero->x = 0;
        hero->dx = 0;
    }
    if (hero->x > 1600 - hero->width) {
        hero->x = 1600 - hero->width;
        hero->dx = 0;
    }

    float old_x = hero->x;
    float old_y = hero->y;

    hero->x += hero->dx;
    hero->y += hero->dy;

    int* left_arr = (int*) malloc(3 * sizeof(int));
    int* right_arr = (int*) malloc(3 * sizeof(int));
    int* up_arr = (int*) malloc(3 * sizeof(int));
    int* down_arr = (int*) malloc(3 * sizeof(int));

    isCollisionRight(hero->x, old_y, hero, right_arr);
    isCollisionLeft(hero->x, old_y, hero, left_arr);
    isCollisionDown(old_x, hero->y, hero, down_arr);
    isCollisionUp(old_x, hero->y, hero, up_arr);

    if(down_arr[0]){
        hero->y = down_arr[2]*blockSize - blockSize*5;
        isAirborne = FALSE;
        hero->dy = 0;
    }
    else{
        isAirborne = TRUE;
    }

    if(hero->isAirborne)
    {
        if (hero->currentJumpSpeed > 0 )
        {
            hero->dy = hero->currentJumpSpeed;
            hero->y -= hero->dy;
            hero->currentJumpSpeed += gravity;
        }
        else
        {
            hero->dy -= gravity;
            hero->y += hero->dy;
        }
    }
}

GLuint LoadTexture(const char *filename)
{
    int width, height, cnt;
    unsigned char *image = stbi_load(filename, &width, &height, &cnt, 0);

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, cnt == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, image);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(image);
    return textureID;
}

void DrawBackground(GLuint texture)
{
    static float vertices[] = {0.0f, 0.0f,  1920.0f, 0.0f,  1920.0f, 1080.0f,  0.0f, 1080.0f};

    static float TexCord[] = {0, 0,  1, 0,  1, 1,  0, 1};

    glClearColor(0, 0, 0, 0);

    glEnable(GL_TEXTURE_2D); // �������� 2D ���������������
    glBindTexture(GL_TEXTURE_2D, texture); // ����������� ��������

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glVertexPointer(2, GL_FLOAT, 0, vertices); // ��������� ������ ������
    glTexCoordPointer(2, GL_FLOAT, 0, TexCord); // ��������� ������ ��������� ��������

    glDrawArrays(GL_QUADS, 0, 4); // ������ ��������������� �� 4 ������

    glDisableClientState(GL_VERTEX_ARRAY); // ��������� ������ ������
    glDisableClientState(GL_TEXTURE_COORD_ARRAY); // ��������� ������ ��������� ��������

    glDisable(GL_TEXTURE_2D); // ��������� 2D ���������������
}

// ������� ��� ���������� �������� �����
void UpdateGeneralAnimationFrame() {

        currentFrame = (currentFrame + 1) % totalFrames; // ����������� ���������� �����

}

// ������� ���������� ��������
void RenderSpriteAnimation(GLuint texture, float posX, float posY, float width, float height, float scale, int currentFrame,float frameWidth) {
    float texLeft = currentFrame * frameWidth;
    float texRight = texLeft + frameWidth;

       // �������� ������ �� �����������, ���� ����� ������� �����
    if (hero.faceRight) {
        float temp = texRight;
        texRight = texLeft;
        texLeft = temp;
    }

    // ������������ ������� ������� � ������ ��������
    float scaledWidth = width * scale;
    float scaledHeight = height * scale;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);

    glColor3f(1,1,1);
    glBegin(GL_QUADS);
        glTexCoord2f(texLeft, 0.0f); glVertex2f(posX, posY);                               // ����� ������� ����
        glTexCoord2f(texRight, 0.0f); glVertex2f(posX + scaledWidth, posY);                 // ������ ������� ����
        glTexCoord2f(texRight, 1.0f); glVertex2f(posX + scaledWidth, posY + scaledHeight);  // ������ ������ ����
        glTexCoord2f(texLeft, 1.0f); glVertex2f(posX, posY + scaledHeight);                 // ����� ������ ����
    glEnd();

    glDisable(GL_TEXTURE_2D);
}

void Init(HWND hwnd)
{
    Menu_AddButton("Load texture",10,10,200,60,2);
    Menu_AddButton("Delete texture",10,90,200,60,2);
    Menu_AddButton("Exit",10,170,200,60,2);

    Idle_sprite = LoadTexture("white.png");
    Walk_sprite = LoadTexture("white.png");
    Jump_sprite = LoadTexture("white.png");
    background = LoadTexture("Background.jpg");
    brickTile = LoadTexture("ground.jpg");

    RECT rct;
    GetClientRect(hwnd, &rct);

    // ������������� ������� �����
    hero.x = 500;  // ��������� ������� �� X
    hero.y = 600;  // ��������� ������� �� Y
    hero.isAirborne = TRUE;
    hero.faceRight = TRUE;
}

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
    WNDCLASSEX wcex;
    HWND hwnd;
    HDC hDC;
    HGLRC hRC;
    MSG msg;
    BOOL bQuit = FALSE;

    /* register window class */
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_OWNDC;
    wcex.lpfnWndProc = WindowProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = "GLSample";
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);;


    if (!RegisterClassEx(&wcex))
        return 0;

    /* create main window */
    hwnd = CreateWindowEx(0,
                          "GLSample",
                          "OpenGL Sample",
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT,
                          CW_USEDEFAULT,
                          WINDOW_WIDTH,
                          WINDOW_HEIGHT,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);

    ShowWindow(hwnd, nCmdShow);
    /* enable OpenGL for the window */
    EnableOpenGL(hwnd, &hDC, &hRC);
    RECT rct; //�������� ���������� � ������������ �������������


    glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            glOrtho(0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, 1, -1);
            GetClientRect(hwnd, &rct);
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();


    Init(hwnd);
    DWORD lastUpdateTime = GetTickCount();
    /* program main loop */
    while (!bQuit)
    {
          /* check for messages */
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            /* handle or dispatch messages */
            if (msg.message == WM_QUIT)
            {
                bQuit = TRUE;
            }
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {
            /* OpenGL animation code goes here */


            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            DrawBackground(background);
            Menu_ShowMenu();

            if (button1)
            {
                DrawMap();
                Menu_ShowMenu();
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

                UpdateHeroPositionAndCollisions(&hero);

                if (!hero.isAirborne && !hero.isMoving)
                {
                    glPushMatrix();
                    UpdateGeneralAnimationFrame();
                    RenderSpriteAnimation(Idle_sprite, hero.x, hero.y, hero.width, hero.height, hero.scale, currentFrame, frameWidth);
                    glPopMatrix();
                }

                else if (hero.isAirborne)
                    {
                        glPushMatrix();
                        UpdateGeneralAnimationFrame();
                        RenderSpriteAnimation(Jump_sprite, hero.x, hero.y, hero.width, hero.height, hero.scale, currentFrame, frameWidth);
                        glPopMatrix();
                }

                else if (hero.isMoving)
                {
                    glPushMatrix();
                    UpdateGeneralAnimationFrame();
                    RenderSpriteAnimation(Walk_sprite, hero.x, hero.y, hero.width, hero.height, hero.scale, currentFrame, frameWidth);
                    glPopMatrix();
                }

            }
            glDisable(GL_BLEND);
            SwapBuffers(hDC);
            Sleep (40);
        }

    }


    /* shutdown OpenGL */
    DisableOpenGL(hwnd, hDC, hRC);

    /* destroy the window explicitly */
    DestroyWindow(hwnd);

    return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_CLOSE:
            PostQuitMessage(0);
            break;

        case WM_MOUSEMOVE:
            Menu_MouseMove(LOWORD(lParam), HIWORD(lParam));
            break;

        case WM_LBUTTONDOWN:
            {
                int buttonId = Menu_MouseDown();
                if (buttonId == 0)
                {
                    button1 = 1;
                    button2 = 0;
                }
                else if (buttonId == 1)
                {
                    button1 = 0;
                    button2 = 1;
                }
                else if (buttonId == 2)
                    PostQuitMessage(0);
            }
            break;

        case WM_LBUTTONUP:
            Menu_MouseUp();
            break;

        case WM_KEYDOWN:
            switch(wParam) {
                case VK_LEFT:
                    hero.dx = -15.0f;
                    hero.isMoving = TRUE;
                    hero.faceRight = FALSE; // ����� ������� �����
                    break;
                case VK_RIGHT:
                    hero.dx = 15.0f;
                    hero.isMoving = TRUE;
                    hero.faceRight = TRUE;
                    break;
                case VK_UP:
                case VK_SPACE:
                    if (!hero.isAirborne)
                    {
                        hero.isAirborne = TRUE;
                        hero.currentJumpSpeed = jumpSpeed;
                    }
                    break;
            }
            break;

        case WM_KEYUP:
            switch(wParam) {
                case VK_LEFT:
                case VK_RIGHT:
                    hero.dx = 0.0f;
                    hero.isMoving = FALSE;
                    break;
            }
            break;

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}

void EnableOpenGL(HWND hwnd, HDC* hDC, HGLRC* hRC)
{
    PIXELFORMATDESCRIPTOR pfd;

    int iFormat;

    /* get the device context (DC) */
    *hDC = GetDC(hwnd);

    /* set the pixel format for the DC */
    ZeroMemory(&pfd, sizeof(pfd));

    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW |
                  PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;

    iFormat = ChoosePixelFormat(*hDC, &pfd);

    SetPixelFormat(*hDC, iFormat, &pfd);

    /* create and enable the render context (RC) */
    *hRC = wglCreateContext(*hDC);

    wglMakeCurrent(*hDC, *hRC);
}

void DisableOpenGL (HWND hwnd, HDC hDC, HGLRC hRC)
{
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hRC);
    ReleaseDC(hwnd, hDC);
}
