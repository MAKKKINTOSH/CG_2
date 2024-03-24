#include <windows.h>
#include <gl/gl.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb-master/stb_image.h"
#define MESSAGE 0
#define RENDER 1
#define TERMINATE 2
LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);

int width, height;
unsigned int texture;
BOOL IsImageOnScreen = FALSE;

typedef struct {
    int id;
    char name[20];
    float vert[8];
    float color[3];
    BOOL isActive;
} Button;

Button buttons[] = {
    {MESSAGE, "message", {0,0, 100,0, 100,30, 0,30}, {0.1f,0.1f,1}, FALSE},
    {RENDER, "render", {0,40, 100,40, 100,70, 0,70}, {0.1f,1,0.1f}, FALSE},
    {TERMINATE, "terminate", {0,80, 100,80, 100,110, 0,110}, {1,0.1f,0.1f}, FALSE},
};

int buttonCounter = sizeof(buttons) / sizeof(buttons[0]);

BOOL IsCursorOnButton(int x, int y, Button button)
{
    return (x > button.vert[0]) && (x < button.vert[4]) && (y > button.vert[1]) && (y < button.vert[5]);
}

void ButtonShow(Button button)
{
    glEnableClientState(GL_VERTEX_ARRAY);
    glColor3f(button.color[0], button.color[1], button.color[2]);
    if (button.isActive) glColor3f(button.color[0]/2, button.color[1]/2, button.color[2]/2);
    glVertexPointer(2, GL_FLOAT, 0, button.vert);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glDisableClientState(GL_VERTEX_ARRAY);
}

void ShowMenu()
{
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0,width, height,0, -1,1);
    for(int i = 0; i < buttonCounter; i++){
        ButtonShow(buttons[i]);
    }
    glPopMatrix();
}

void ButtonEventHandler(Button button)
{
    switch (button.id)
    {
        case MESSAGE:
            printf("Hello World!\n");
        break;
        case RENDER:
            IsImageOnScreen = TRUE;
        break;
        case TERMINATE:
            PostQuitMessage(0);
        break;
    }
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
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT,
                          CW_USEDEFAULT,
                          500,
                          500,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);

    ShowWindow(hwnd, nCmdShow);
    /* enable OpenGL for the window */
    EnableOpenGL(hwnd, &hDC, &hRC);

    LoadPicture("manul.jpg", &texture);

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

            if (!IsImageOnScreen)
            {
                ShowMenu();
            }
            else
            {
                RenderPicture(texture);
            }
            SwapBuffers(hDC);
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
            for (int i = 0; i < buttonCounter; i++){
                if (IsCursorOnButton(LOWORD(lParam), HIWORD(lParam), buttons[i])){
                    buttons[i].isActive = TRUE;
                    ButtonShow(buttons[i]);
                }
                else{
                    buttons[i].isActive = FALSE;
                }
            }
        break;
        case WM_LBUTTONDOWN:
            for (int i = 0; i < buttonCounter; i++){
                if (IsCursorOnButton(LOWORD(lParam), HIWORD(lParam), buttons[i])){
                    ButtonEventHandler(buttons[i]);
                }
            }
        break;

        case WM_SIZE:
            width = LOWORD(lParam);
            height = HIWORD(lParam);
            glViewport(0,0, LOWORD(lParam), HIWORD(lParam));
            glLoadIdentity();
            float k = LOWORD(lParam) / (float)HIWORD(lParam);
            glOrtho(-k,k, -1,1, -1,1);
        case WM_DESTROY:
            return 0;

        case WM_KEYDOWN:
        {
            switch (wParam)
            {
                case VK_ESCAPE:
                    PostQuitMessage(0);
                break;
            }
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

