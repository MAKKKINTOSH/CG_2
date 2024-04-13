#ifndef GAME_HPP
#define GAME_HPP

#ifndef WINDOW_SIZE
#define WINDOW_SIZE
#define WIDTH 1282
#define HEIGHT 732
#endif // WINDOW_SIZE

#include <string>
#include <iostream>

inline float min(float a, float b) {
    if( a < b ) return a;
    else return b;
}

inline float max(float a, float b) {
    if( a > b ) return a;
    else return b;
}

const std::string TileMap[] = {
    "RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR",
    "R                                      R",
    "R                                      R",
    "R                                      R",
    "R                                      R",
    "R                                      R",
    "R                                      R",
    "R                                      R",
    "R            RRRRRRRRRRRR        RRRRRRR",
    "R                                      R",
    "R       RRRRRRRRR                      R",
    "R                                      R",
    "RRRR                                   R",
    "RRRRRR                                 R",
    "RRRRRRRRRRRRRRR                        R",
    "R                                      R",
    "R                                      R",
    "R           RRRRRRRRRRRRRRR            R",
    "R                                      R",
    "R                             RRRRRRRRRR",
    "R                         RRRRRRRRRRRRRR",
    "RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR"
};

struct Player {
    float x = WIDTH/2 - 80/2;
    float y = 3*32;
    float vx = 0, vy = 0, ax = 0, ay = -1;
    bool onGround = true;
    short direction = 1;
    const short w = 80, h=100;
    float scale = 0.5;

    int animation = 0, frame = 0, itr = 0;

    void iter() {
        move(vx*direction, vy);

        if (vx < 0.001) vx = ax = 0;
        checkGround();

        if (onGround) {
            ++itr %= 40;
            frame = itr/5;
        } else {
            animation = 1;
            if (vy > 13) frame = 1;
            else if (vy > 10) frame = 2;
            else if (vy > 4) frame = 3;
            else if (vy > -4) frame = 4;
            else if (vy > -10) frame = 5;
            else if (vy > -13) frame = 6;
            else frame = 7;
        }
    }

    void move(float dx, float dy) {

        int row=0, col=0;
        bool check;

        check = true;
        if (dy != 0) {
            row = (dy < 0 ? y+dy : min(HEIGHT-32, y+dy+h*scale))/32;
            for (float cx=x; check && cx<min(WIDTH-33, x+w*scale); cx+=16) {
                col = cx/32;
                check = check && (TileMap[21-row][col] == ' ');
            }
        }
        if (check) {
            y += dy;
            vy += ay;
        }
        else {
            y = (dy < 0) ? ((int)y/32)*32 : ((int) min(HEIGHT-32, y+h*scale)/32 + 1)*32 - h*scale - 1;
            vy = 0;
        }


        check = true;
        if (dx != 0) {
            col = (dx < 0 ? x+dx : min(WIDTH-33, x+dx+w*scale))/32;
            for (float cy=y; check && cy<min(HEIGHT-32, y+h*scale); cy+=16) {
                row = cy/32;
                check = check && (TileMap[21-row][col] == ' ');
            }
        }
        if (check) {
            x += dx;
            vx = max(0, vx+ax);
        }
        else {
            x = (dx < 0) ? ((int) x/32)*32 : ((int) min(WIDTH-33, x+w*scale)/32 + 1)*32 - w*scale - 1;
            vx = ax = 0;
        }
    }

    void checkGround() {
        bool check = false;
        for (int mx=x; (!check) && mx<min(WIDTH, x+w*scale); mx+=16) {
            int row = (y-2)/32, col = mx/32;
            check = check || (TileMap[21-row][col] == 'R');
        }

        if (check && !onGround) {
            onGround = true;
            itr=0;
            frame=0;
            ay = 0;
        } else if (!check && onGround) {
            ay = -1;
            onGround = false;
        }
    }

    void jump() {
        if (onGround) {
            onGround = false;
            ay = -1;
            vy = 15;
            ax = 0;
            vx = 0;
        }
    }
};

void Game(GLuint texture);

#endif // GAME_HPP
