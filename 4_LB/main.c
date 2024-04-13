#include <windows.h>
#include <gl/gl.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb-master/stb_image.h"
#include "stb-master/stb_easy_font.h"
#include "menu.h"

#define WINDOW_HEIGHT 720
#define WINDOW_WIDTH 1024


int button1 = 0;
int button2 = 0;
GLuint Idle_sprite, Walk_sprite, Jump_sprite, background;
int currentFrame = 0;
const int totalFrames = 16;
float frameWidth = 1.0f / 8.0f;
int isMoving = 0;
float jumpSpeed = 50.0f;
float gravity = -5.0f;
float verticalVelocity = 0.0f;
BOOL isJumping = FALSE;
float groundLevel = 0.0f;
BOOL isAirborne = FALSE;

typedef struct {
    float x, y;
    float dx, dy;
} Hero;

Hero hero = {0.0f, 0.0f, 0.0f, 0.0f};

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);

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


void UpdateAnimationFrame() {

        currentFrame = (currentFrame + 1) % totalFrames;

}


void RenderSpriteAnimation(GLuint texture, float posX, float posY, float width, float height, float scale) {
    float texLeft = currentFrame * frameWidth;
    float texRight = texLeft + frameWidth;


    float scaledWidth = width * scale;
    float scaledHeight = height * scale;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);

    glColor3f(1,1,1);
    glBegin(GL_QUADS);
        glTexCoord2f(texLeft, 0.0f); glVertex2f(posX, posY);
        glTexCoord2f(texRight, 0.0f); glVertex2f(posX + scaledWidth, posY);
        glTexCoord2f(texRight, 1.0f); glVertex2f(posX + scaledWidth, posY + scaledHeight);
        glTexCoord2f(texLeft, 1.0f); glVertex2f(posX, posY + scaledHeight);
    glEnd();

    glDisable(GL_TEXTURE_2D);
}

void Init(HWND hwnd)
{
    Menu_AddButton("Load texture",10,10,200,60,2);
    Menu_AddButton("Delete texture",10,90,200,60,2);
    Menu_AddButton("Exit",10,170,200,60,2);

    Idle_sprite = LoadTexture("stay.png");
    Walk_sprite = LoadTexture("run.png");
    Jump_sprite = LoadTexture("jump.png");
    background = LoadTexture("background.jpg");

    RECT rct;
    GetClientRect(hwnd, &rct);
    groundLevel = rct.bottom - 300;

    hero.x = 400.0f;
    hero.y = groundLevel;
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
    float theta = 0.0f;

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
                          WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX,
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

    RECT rct;

    glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            GetClientRect(hwnd, &rct);
            glOrtho(0, rct.right, rct.bottom, 0, 1, -1);
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();


    Init(hwnd);

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
            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            RenderPicture(background);
            Menu_ShowMenu();

            float centerX = rct.right / 2.0f;
            float posY = 150.0f;
            float spriteWidth = 640.0f;
            float spriteHeight = 100.0f;

           if (button1)
            {
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

                float spriteAspectRatio = (float)80 / (float)100;
                float renderedSpriteWidth = spriteHeight * spriteAspectRatio;
                float scale = 2.0f;

                if (!isAirborne && !isMoving)
                {
                    glPushMatrix();
                    UpdateAnimationFrame();
                    RenderSpriteAnimation(Idle_sprite, hero.x, hero.y, renderedSpriteWidth, spriteHeight, scale);
                    glPopMatrix();
                }
                else if (isAirborne)
                    {
                        glPushMatrix();
                        UpdateAnimationFrame();
                        RenderSpriteAnimation(Jump_sprite, hero.x, hero.y, renderedSpriteWidth, spriteHeight, scale);
                        hero.y -= verticalVelocity;
                        verticalVelocity += gravity;
                        hero.x += hero.dx;
                        if (hero.x < 0) hero.x = 0;
                        if (hero.x > rct.right - renderedSpriteWidth) hero.x = rct.right - renderedSpriteWidth;

                        if (hero.y >= groundLevel)
                        {
                            hero.y = groundLevel;
                            isAirborne = FALSE;
                            verticalVelocity = 0;
                        }
                        glPopMatrix();
                    }
                else if (isMoving)
                {
                    glPushMatrix();
                    UpdateAnimationFrame();
                    hero.x += hero.dx;
                    hero.y += hero.dy;
                    if (hero.x < 0) hero.x = 0;
                    if (hero.x > rct.right - renderedSpriteWidth) hero.x = rct.right - renderedSpriteWidth;
                    RenderSpriteAnimation(Walk_sprite, hero.x, hero.y, renderedSpriteWidth, spriteHeight, scale);
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
                    isMoving = 1;
                    break;
                case VK_RIGHT:
                    hero.dx = 15.0f;
                    isMoving = 1;
                    break;
                case VK_UP:
                case VK_SPACE:
                    if (!isAirborne)
                    {
                        isAirborne = TRUE;
                        verticalVelocity = jumpSpeed;
                    }
                    break;
            }
            break;

        case WM_KEYUP:
            switch(wParam) {
                case VK_LEFT:
                case VK_RIGHT:
                    hero.dx = 0.0f;
                    isMoving = 0;
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
