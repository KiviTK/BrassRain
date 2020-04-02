#include <iostream>
#include <SDL2/SDL.h>
#include <chrono>
#include <thread>
#include <random>
#include <vector>

using namespace std::chrono_literals;

struct brass {
    float x = 0, y = 0;
    float vx = 0, vy = 0;
    int lifetime = 0;
    bool visible = false;

    brass(int x, int y) : x(x), y(y) {}
    brass() {}

    void reset(int xn, int yn) {
        lifetime = 0;
        vx = 0;
        vy = 0;
        visible = true;
        x = xn;
        y = yn;
    }

    void update() {
        if (!visible)
            return;

        lifetime++;
        vy += .1;
        y += vy;

        x += vx;

        if (y > 400) {
            vy *= -0.85;
            y = 399;

            float xspeed = rand() % 200;
            xspeed -= 100;
            xspeed /= 300;

            if (abs(vy) > 1) {
                xspeed *= vy / 4;
                vx += xspeed;
            } else {
                vy *= .8f;
                vx *= .5f;
            }
        }

        if (lifetime > 300)
            visible = false;
    }

    void draw(SDL_Renderer* r) {
        if (!visible)
            return;

        SDL_SetRenderDrawColor(r,120, 90, 60, 255);
        SDL_Rect rect = {(int)x - 3, (int) y-3, 5,5 };
        SDL_RenderFillRect(r, &rect);
    }
};

struct ent {
    SDL_Rect model = {0, 300, 30, 100};

    int h;ddd
    int vely;

    void kick(std::vector<brass>& shells, int dir) {
        float x = model.x + 15;
        float y = model.y + 103;


        for (auto& b : shells) {
            float dist = sqrt(pow(x - b.x, 2) + pow(y - b.y, 2));
            float vecx, vecy;
            vecx = x - b.x;
            vecy = y - b.y;

            if (dist < 15) {
                vecx /= -40;
                vecy /= -10;

                b.vx += vecx;
                b.vy += vecy;
            }
        }

        for (auto& b : shells) {
            if (b.y > 380 && b.x > model.x && b.x < model.x + 30)
                if (dir == 0)
                    b.vx += 1;
                else
                    b.vx -= 1;

        }
    }

    void update(const Uint8* kb, std::vector<brass>& shells, unsigned long long& shellidx) {
        if (kb[SDL_SCANCODE_SPACE]) {
            shells[shellidx % 1000].reset(model.x + 15 + rand() % 5, model.y + 25 + rand() % 7);
            shellidx++;
        }

        if (kb[SDL_SCANCODE_W]) {
            if (model.y == 300)
                vely = -15;
        }


        if (kb[SDL_SCANCODE_D]) {
            kick(shells, 0);
            model.x += 5;
        }


        if (kb[SDL_SCANCODE_A]) {
            kick(shells, 1);
            model.x -= 5;
        }



        vely += 1;
        model.y += vely;
        h = 600 - (model.y + model.h);

        if (model.y > 300)
            model.y = 300;

    }

    void draw(SDL_Renderer* r) {
        SDL_SetRenderDrawColor(r,20, 255, 30, 255);
        SDL_RenderDrawRect(r, &model);
    }
};


int main() {
    srand(time(NULL));

    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window* w;
    SDL_Renderer* r;
    int a = 3;

    std::vector<brass> shells;
    shells.resize(1000);
    unsigned long long shellinx = 0;

    SDL_Rect floor = {0, 400, 800, 200};

    SDL_CreateWindowAndRenderer(800, 600, NULL, &w, &r);

    int x, y;
    const Uint8* kbstate = SDL_GetKeyboardState(NULL);

    ent player;

    while (a > 2) {

        a++;
        SDL_PumpEvents();
        auto state = SDL_GetMouseState(&x,&y);

        if (state & SDL_BUTTON(SDL_BUTTON_LEFT)) {


            for (auto& b : shells) {
                float dist = sqrt(pow(x - b.x, 2) + pow(y - b.y, 2));
                float vecx, vecy;
                vecx = x - b.x;
                vecy = y - b.y;

                if (dist < 100) {
                    vecx /= -40;
                    vecy /= -10;

                    b.vx += vecx;
                    b.vy += vecy;
                }
            }
        }

        player.update(kbstate, shells, shellinx);

        for (auto& b : shells)
            b.update();


        SDL_SetRenderDrawColor(r,0, 0, 0, 255);
        SDL_RenderClear(r);

        SDL_SetRenderDrawColor(r,120, 120, 120, 255);
        SDL_RenderFillRect(r, &floor);

        player.draw(r);

        for (auto& b : shells)
            b.draw(r);

        SDL_RenderPresent(r);

        std::this_thread::sleep_for(0.016s);
    }

    return 0;
}
