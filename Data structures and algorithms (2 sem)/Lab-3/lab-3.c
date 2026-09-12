#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define N           10
#define SEED        5306
#define N3          0
#define N4          6

#define WIN_W       1100
#define WIN_H       700
#define PANEL_W     550
#define CX_LEFT     (PANEL_W / 2)
#define CX_RIGHT    (PANEL_W + PANEL_W / 2)
#define CY          355

#define RADIUS      215
#define VRAD        20
#define ARROW_SIZE  10
#define CURVE_BEND  35

typedef struct { float x, y; } Vec2;
typedef int Matrix[N][N];

Matrix   Adir, Aundir;
Vec2     pos_left[N];
Vec2     pos_right[N];
TTF_Font *font14 = NULL;
TTF_Font *font16 = NULL;
TTF_Font *font20 = NULL;

void randm(float T[N][N]) {
    int i, j;
    for (i = 0; i < N; i++)
        for (j = 0; j < N; j++)
            T[i][j] = ((float)rand() / RAND_MAX) * 2.0f;
}

void mulmr(float T[N][N], float k, Matrix A) {
    int i, j;
    for (i = 0; i < N; i++)
        for (j = 0; j < N; j++)
            A[i][j] = (T[i][j] * k >= 1.0f) ? 1 : 0;
}

void build_Adir(void) {
    float T[N][N];
    float k = 1.0f - N3 * 0.02f - N4 * 0.005f - 0.25f;
    srand(SEED);
    randm(T);
    mulmr(T, k, Adir);
    int i;
    for (i = 0; i < N; i++) Adir[i][i] = 0;
}

void build_Aundir(void) {
    int i, j;
    for (i = 0; i < N; i++)
        for (j = 0; j < N; j++)
            Aundir[i][j] = (Adir[i][j] == 1 || Adir[j][i] == 1) ? 1 : 0;
}

void calc_positions(Vec2 pos[N], int cx, int cy) {
    int i;
    for (i = 0; i < N - 1; i++) {
        float a = (float)(2.0 * M_PI * i) / (N - 1) - (float)(M_PI / 2.0);
        pos[i].x = cx + RADIUS * cosf(a);
        pos[i].y = cy + RADIUS * sinf(a);
    }
    pos[N - 1].x = cx;
    pos[N - 1].y = cy;
}

void print_matrix(const char *name, Matrix A) {
    int i, j;
    printf("\n%s:\n    ", name);
    for (j = 0; j < N; j++) printf("%3d", j + 1);
    printf("\n   +");
    for (j = 0; j < N; j++) printf("---");
    printf("\n");
    for (i = 0; i < N; i++) {
        printf("%2d |", i + 1);
        for (j = 0; j < N; j++)
            printf("%3d", A[i][j]);
        printf("\n");
    }
}

static void set_color(SDL_Renderer *r, Uint8 red, Uint8 g, Uint8 b, Uint8 a) {
    SDL_SetRenderDrawColor(r, red, g, b, a);
}

void draw_circle(SDL_Renderer *rnd, int cx, int cy, int r) {
    int x = r, y = 0, err = 0;
    while (x >= y) {
        SDL_RenderDrawPoint(rnd, cx+x, cy+y); SDL_RenderDrawPoint(rnd, cx+y, cy+x);
        SDL_RenderDrawPoint(rnd, cx-y, cy+x); SDL_RenderDrawPoint(rnd, cx-x, cy+y);
        SDL_RenderDrawPoint(rnd, cx-x, cy-y); SDL_RenderDrawPoint(rnd, cx-y, cy-x);
        SDL_RenderDrawPoint(rnd, cx+y, cy-x); SDL_RenderDrawPoint(rnd, cx+x, cy-y);
        y++;
        err += 1 + 2*y;
        if (2*(err-x)+1 > 0) { x--; err += 1-2*x; }
    }
}

void fill_circle(SDL_Renderer *rnd, int cx, int cy, int r) {
    int y;
    for (y = -r; y <= r; y++) {
        int dx = (int)sqrtf((float)(r*r - y*y));
        SDL_RenderDrawLine(rnd, cx-dx, cy+y, cx+dx, cy+y);
    }
}

void draw_arrowhead(SDL_Renderer *rnd, float x, float y, float angle) {
    float s = sinf(angle), c = cosf(angle);
    float L = ARROW_SIZE, W = ARROW_SIZE * 0.42f;
    float p1x = x,          p1y = y;
    float p2x = x-L*c+W*s,  p2y = y-L*s-W*c;
    float p3x = x-L*c-W*s,  p3y = y-L*s+W*c;
    SDL_RenderDrawLine(rnd,(int)p1x,(int)p1y,(int)p2x,(int)p2y);
    SDL_RenderDrawLine(rnd,(int)p2x,(int)p2y,(int)p3x,(int)p3y);
    SDL_RenderDrawLine(rnd,(int)p3x,(int)p3y,(int)p1x,(int)p1y);

    int minY=(int)fminf(fminf(p1y,p2y),p3y);
    int maxY=(int)fmaxf(fmaxf(p1y,p2y),p3y);
    float pts[3][2]={{p1x,p1y},{p2x,p2y},{p3x,p3y}};
    int iy, e;
    for (iy=minY; iy<=maxY; iy++) {
        float xs[2]; int cnt=0;
        for (e=0; e<3&&cnt<2; e++) {
            float y0=pts[e][1], y1=pts[(e+1)%3][1];
            float x0=pts[e][0], x1=pts[(e+1)%3][0];
            if ((iy>=y0&&iy<y1)||(iy>=y1&&iy<y0))
                xs[cnt++]=x0+(x1-x0)*(iy-y0)/(y1-y0);
        }
        if (cnt==2) SDL_RenderDrawLine(rnd,(int)xs[0],iy,(int)xs[1],iy);
    }
}

void draw_bezier(SDL_Renderer *rnd,
                 float x0, float y0, float cpx, float cpy,
                 float x1, float y1) {
    float px=x0, py=y0;
    int i;
    for (i=1; i<=60; i++) {
        float t=(float)i/60.0f, mt=1.0f-t;
        float nx=mt*mt*x0+2*mt*t*cpx+t*t*x1;
        float ny=mt*mt*y0+2*mt*t*cpy+t*t*y1;
        SDL_RenderDrawLine(rnd,(int)px,(int)py,(int)nx,(int)ny);
        px=nx; py=ny;
    }
}

void draw_text_centered(SDL_Renderer *rnd, TTF_Font *font,
                        const char *text, int x, int y,
                        Uint8 r, Uint8 g, Uint8 b) {
    if (!font) return;
    SDL_Color col = {r, g, b, 255};
    SDL_Surface *surf = TTF_RenderUTF8_Blended(font, text, col);
    if (!surf) return;
    SDL_Texture *tex = SDL_CreateTextureFromSurface(rnd, surf);
    SDL_Rect dst = {x - surf->w/2, y - surf->h/2, surf->w, surf->h};
    SDL_RenderCopy(rnd, tex, NULL, &dst);
    SDL_DestroyTexture(tex);
    SDL_FreeSurface(surf);
}

void draw_edge(SDL_Renderer *rnd, Vec2 from, Vec2 to,
               int directed, int curved) {
    float dx=to.x-from.x, dy=to.y-from.y;
    float len=sqrtf(dx*dx+dy*dy);
    if (len < 1.0f) return;
    float ux=dx/len, uy=dy/len;
    set_color(rnd, 51, 51, 51, 255);
    if (curved) {
        float mx=(from.x+to.x)/2.0f, my=(from.y+to.y)/2.0f;
        float cpx=mx-uy*CURVE_BEND,   cpy=my+ux*CURVE_BEND;
        float a0=atan2f(cpy-from.y, cpx-from.x);
        float sx=from.x+cosf(a0)*VRAD, sy=from.y+sinf(a0)*VRAD;
        float a1=atan2f(to.y-cpy, to.x-cpx);
        float ex=to.x-cosf(a1)*(VRAD+(directed?ARROW_SIZE:0));
        float ey=to.y-sinf(a1)*(VRAD+(directed?ARROW_SIZE:0));
        draw_bezier(rnd, sx, sy, cpx, cpy, ex, ey);
        if (directed) draw_arrowhead(rnd, ex, ey, a1);
    } else {
        float sx=from.x+ux*VRAD, sy=from.y+uy*VRAD;
        float ex=to.x-ux*(VRAD+(directed?ARROW_SIZE:0));
        float ey=to.y-uy*(VRAD+(directed?ARROW_SIZE:0));
        SDL_RenderDrawLine(rnd,(int)sx,(int)sy,(int)ex,(int)ey);
        if (directed) draw_arrowhead(rnd, ex, ey, atan2f(uy, ux));
    }
}

void draw_selfloop(SDL_Renderer *rnd, Vec2 v, int directed) {
    int lx=(int)v.x, ly=(int)(v.y-VRAD-14);
    set_color(rnd, 51, 51, 51, 255);
    draw_circle(rnd, lx, ly, 13);
    draw_circle(rnd, lx, ly, 14);
    if (directed) {
        float a=atan2f(v.y-(float)(ly+14), v.x-(float)lx);
        draw_arrowhead(rnd, v.x+cosf(a)*VRAD, v.y+sinf(a)*VRAD, a);
    }
}

void draw_graph(SDL_Renderer *rnd, Matrix A, int directed, Vec2 pos[N]) {
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            if (!A[i][j]) continue;
            if (!directed && j < i) continue;
            if (i == j) { draw_selfloop(rnd, pos[i], directed); continue; }
            int curved = directed && A[j][i];
            draw_edge(rnd, pos[i], pos[j], directed, curved);
        }
    }

    for (i = 0; i < N; i++) {
        int cx=(int)pos[i].x, cy=(int)pos[i].y;

        SDL_SetRenderDrawBlendMode(rnd, SDL_BLENDMODE_BLEND);
        set_color(rnd, 77, 166, 255, 45);
        fill_circle(rnd, cx, cy, VRAD+7);
        SDL_SetRenderDrawBlendMode(rnd, SDL_BLENDMODE_NONE);

        set_color(rnd, 77, 166, 255, 255);
        fill_circle(rnd, cx, cy, VRAD);
        set_color(rnd, 43, 125, 233, 255);
        draw_circle(rnd, cx, cy, VRAD);
        draw_circle(rnd, cx, cy, VRAD-1);

        char buf[4];
        snprintf(buf, sizeof(buf), "%d", i+1);
        draw_text_centered(rnd, font14, buf, cx, cy, 255, 255, 255);
    }
}

void draw_hud(SDL_Renderer *rnd) {
    draw_text_centered(rnd, font20, "Напрямлений граф",
                       CX_LEFT,  26, 30, 30, 30);
    draw_text_centered(rnd, font16, "(Adir)",
                       CX_LEFT,  50, 100, 100, 100);

    draw_text_centered(rnd, font20, "Ненапрямлений граф",
                       CX_RIGHT, 26, 30, 30, 30);
    draw_text_centered(rnd, font16, "(Aundir)",
                       CX_RIGHT, 50, 100, 100, 100);

    set_color(rnd, 190, 205, 220, 255);
    SDL_RenderDrawLine(rnd, PANEL_W,   10, PANEL_W,   WIN_H-10);
    SDL_RenderDrawLine(rnd, PANEL_W+1, 10, PANEL_W+1, WIN_H-10);

    /* Підказка */
    draw_text_centered(rnd, font14, "ESC / Q — вийти",
                       WIN_W/2, WIN_H-16, 160, 160, 160);
}

int main(void) {
    build_Adir();
    build_Aundir();
    calc_positions(pos_left,  CX_LEFT,  CY);
    calc_positions(pos_right, CX_RIGHT, CY);

    float k = 1.0f - N3*0.02f - N4*0.005f - 0.25f;
    printf("=== Лабораторна робота №3 ===\n");
    printf("N=%d  seed=%d  n3=%d  n4=%d  k=%.4f\n", N, SEED, N3, N4, k);
    print_matrix("Adir (напрямлений граф)", Adir);
    print_matrix("Aundir (ненапрямлений граф)", Aundir);
    printf("\n");

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
        return 1;
    }
    if (TTF_Init() != 0)
        fprintf(stderr, "TTF_Init: %s\n", TTF_GetError());

    const char *fpaths[] = {
        "/opt/homebrew/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/Library/Fonts/Arial.ttf",
        "/System/Library/Fonts/Helvetica.ttc",
        "C:/Windows/Fonts/arial.ttf",
        NULL
    };
    int fi;
    for (fi = 0; fpaths[fi] && !font14; fi++) {
        font14 = TTF_OpenFont(fpaths[fi], 14);
        font16 = TTF_OpenFont(fpaths[fi], 16);
        font20 = TTF_OpenFont(fpaths[fi], 20);
    }

    SDL_Window *win = SDL_CreateWindow(
        "Лабораторна робота №3 — Графічне представлення графів",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WIN_W, WIN_H, SDL_WINDOW_SHOWN);
    if (!win) { fprintf(stderr, "Window: %s\n", SDL_GetError()); SDL_Quit(); return 1; }
    SDL_RaiseWindow(win);

    SDL_Renderer *rnd = SDL_CreateRenderer(
        win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!rnd) {
        fprintf(stderr, "Renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(win); SDL_Quit(); return 1;
    }

    int running = 1;
    SDL_Event ev;

    while (running) {
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_QUIT) running = 0;
            if (ev.type == SDL_KEYDOWN)
                if (ev.key.keysym.sym == SDLK_ESCAPE ||
                    ev.key.keysym.sym == SDLK_q)
                    running = 0;
        }

        SDL_SetRenderDrawColor(rnd, 240, 244, 248, 255);
        SDL_RenderClear(rnd);

        draw_graph(rnd, Adir,   1, pos_left);
        draw_graph(rnd, Aundir, 0, pos_right);
        draw_hud(rnd);

        SDL_RenderPresent(rnd);
    }

    if (font14) TTF_CloseFont(font14);
    if (font16) TTF_CloseFont(font16);
    if (font20) TTF_CloseFont(font20);
    TTF_Quit();
    SDL_DestroyRenderer(rnd);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}