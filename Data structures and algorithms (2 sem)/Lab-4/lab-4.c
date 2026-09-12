#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define N       10
#define SEED    5306
#define N3      0
#define N4      6

#define K1  (1.0f - N3*0.01f - N4*0.01f - 0.30f)

#define K2  (1.0f - N3*0.005f - N4*0.005f - 0.27f)

#define WIN_W   900
#define WIN_H   750
#define CX      450
#define CY      375
#define RADIUS  280
#define VRAD    22
#define ARROW_SIZE 11
#define CURVE_BEND 40

typedef struct { float x, y; } Vec2;
typedef int Matrix[N][N];

Matrix Adir1;
Matrix Aundir1;
Matrix Adir2;

int deg_dir1[N];
int deg_undir1[N];
int indeg1[N];
int outdeg1[N];

int indeg2[N];
int outdeg2[N];

Matrix A2, A3;
Matrix Reach;
Matrix Strong;
int    comp[N];
int    num_comp;

int    cond_n;
Matrix Cond;

Vec2 vpos[N];
Vec2 cpos[N];

int view_mode = 1;

TTF_Font *font13 = NULL;
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

void build_directed(Matrix A, float k) {
    float T[N][N];
    srand(SEED);
    randm(T);
    mulmr(T, k, A);
    int i;
    for (i = 0; i < N; i++) A[i][i] = 0;
}

void build_undirected(Matrix Aundir, Matrix Adir) {
    int i, j;
    for (i = 0; i < N; i++)
        for (j = 0; j < N; j++)
            Aundir[i][j] = (Adir[i][j] || Adir[j][i]) ? 1 : 0;
}

void calc_positions(Vec2 pos[N], int n, int cx, int cy, int r) {
    int i;
    if (n == 1) { pos[0].x = cx; pos[0].y = cy; return; }
    for (i = 0; i < n - 1; i++) {
        float a = (float)(2.0 * M_PI * i) / (n - 1) - (float)(M_PI / 2.0);
        pos[i].x = cx + r * cosf(a);
        pos[i].y = cy + r * sinf(a);
    }
    pos[n - 1].x = cx;
    pos[n - 1].y = cy;
}

//2.1 2.2 обчислення степенів та напівстепенів вершин
void calc_degrees(void) {
    int i, j;
    for (i = 0; i < N; i++) {
        indeg1[i] = outdeg1[i] = 0;
        for (j = 0; j < N; j++) {
            outdeg1[i] += Adir1[i][j];
            indeg1[i]  += Adir1[j][i];
        }
        deg_dir1[i] = indeg1[i] + outdeg1[i];
    }

    for (i = 0; i < N; i++) {
        deg_undir1[i] = 0;
        for (j = 0; j < N; j++) {
            if (Aundir1[i][j])
                deg_undir1[i] += (i == j) ? 2 : 1;
        }
    }
}

void print_degrees(void) {
    int i;
    printf("\n=== П.2: Характеристики графа 1 ===\n");
    printf("\nНапрямлений граф (Adir1):\n");
    printf("%-6s %-8s %-8s %-8s\n", "Верш.", "outdeg", "indeg", "deg");
    for (i = 0; i < N; i++)
        printf("%-6d %-8d %-8d %-8d\n", i+1, outdeg1[i], indeg1[i], deg_dir1[i]);

    printf("\nНенапрямлений граф (Aundir1):\n");
    printf("%-6s %-8s\n", "Верш.", "deg");
    for (i = 0; i < N; i++)
        printf("%-6d %-8d\n", i+1, deg_undir1[i]);

    // 2.3 Перевірка на регулярність (однорідність)
    int reg = 1, d = deg_undir1[0];
    for (i = 1; i < N; i++) if (deg_undir1[i] != d) { reg = 0; break; }
    if (reg)
        printf("\nНенапрямлений граф є регулярним (однорідним) степеня %d.\n", d);
    else
        printf("\nНенапрямлений граф НЕ є регулярним.\n");

    // 2.4 Перелік висячих та ізольованих
    printf("\nВисячі вершини (deg=1) ненапрямленого: ");
    int found = 0;
    for (i = 0; i < N; i++) if (deg_undir1[i] == 1) { printf("%d ", i+1); found=1; }
    if (!found) printf("немає");

    printf("\nІзольовані вершини (deg=0) ненапрямленого: ");
    found = 0;
    for (i = 0; i < N; i++) if (deg_undir1[i] == 0) { printf("%d ", i+1); found=1; }
    if (!found) printf("немає");
    printf("\n");
}

void calc_degrees2(void) {
    int i, j;
    for (i = 0; i < N; i++) {
        indeg2[i] = outdeg2[i] = 0;
        for (j = 0; j < N; j++) {
            outdeg2[i] += Adir2[i][j];
            indeg2[i]  += Adir2[j][i];
        }
    }
}

void print_degrees2(void) {
    int i;
    printf("\n=== П.4.1: Напівстепені графа 2 (Adir2) ===\n");
    printf("%-6s %-8s %-8s\n", "Верш.", "outdeg", "indeg");
    for (i = 0; i < N; i++)
        printf("%-6d %-8d %-8d\n", i+1, outdeg2[i], indeg2[i]);
}

// Шляхи довжинни 2 та 3

void mat_mult(Matrix A, Matrix B, Matrix C) {
    int i, j, k;
    Matrix tmp;
    memset(tmp, 0, sizeof(tmp));
    for (i = 0; i < N; i++)
        for (j = 0; j < N; j++)
            for (k = 0; k < N; k++)
                tmp[i][j] += A[i][k] * B[k][j];
    memcpy(C, tmp, sizeof(tmp));
}

void find_paths(void) {
    mat_mult(Adir2, Adir2, A2);
    mat_mult(A2,    Adir2, A3);

    printf("\n=== П.4.2: Шляхи довжини 2 ===\n");
    int i, j, k, found = 0;
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            if (A2[i][j] == 0) continue;
            for (k = 0; k < N; k++) {
                if (Adir2[i][k] && Adir2[k][j]) {
                    printf("  %d – %d – %d\n", i+1, k+1, j+1);
                    found = 1;
                }
            }
        }
    }
    if (!found) printf("  (шляхів немає)\n");

    printf("\n=== П.4.2: Шляхи довжини 3 ===\n");
    found = 0;
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            if (A3[i][j] == 0) continue;
            int k1, k2;
            for (k1 = 0; k1 < N; k1++) {
                if (!Adir2[i][k1]) continue;
                for (k2 = 0; k2 < N; k2++) {
                    if (Adir2[k1][k2] && Adir2[k2][j]) {
                        printf("  %d – %d – %d – %d\n", i+1, k1+1, k2+1, j+1);
                        found = 1;
                    }
                }
            }
        }
    }
    if (!found) printf("  (шляхів немає)\n");
}

// 4.3 Транзитивне замикання (алгоритм Воршалла)
void warshall(Matrix A, Matrix R) {
    int i, j, k;
    for (i = 0; i < N; i++)
        for (j = 0; j < N; j++)
            R[i][j] = A[i][j] || (i == j);
    for (k = 0; k < N; k++)
        for (i = 0; i < N; i++)
            for (j = 0; j < N; j++)
                if (R[i][k] && R[k][j]) R[i][j] = 1;
}

void calc_reachability(void) {
    warshall(Adir2, Reach);
}

void print_matrix_console(const char *name, Matrix M, int n) {
    int i, j;
    printf("\n%s:\n    ", name);
    for (j = 0; j < n; j++) printf("%3d", j+1);
    printf("\n   +");
    for (j = 0; j < n; j++) printf("---");
    printf("\n");
    for (i = 0; i < n; i++) {
        printf("%2d |", i+1);
        for (j = 0; j < n; j++) printf("%3d", M[i][j]);
        printf("\n");
    }
}

// 4. Матриця сильної зв'язності
void calc_strong(void) {
    int i, j;
    for (i = 0; i < N; i++)
        for (j = 0; j < N; j++)
            Strong[i][j] = (Reach[i][j] && Reach[j][i]) ? 1 : 0;
}

// 5. Компоненти сильної зв'язності
void find_components(void) {
    int i, j;
    memset(comp, -1, sizeof(comp));
    num_comp = 0;
    for (i = 0; i < N; i++) {
        if (comp[i] != -1) continue;
        comp[i] = num_comp; 
        for (j = i+1; j < N; j++) {
            if (comp[j] != -1) continue;
            int same = 1, k;
            for (k = 0; k < N; k++)
                if (Strong[i][k] != Strong[j][k]) { same = 0; break; }
            if (same) comp[j] = num_comp;
        }
        num_comp++;
    }
}

void print_components(void) {
    int c, i;
    printf("\n=== П.4.5: Компоненти сильної зв'язності ===\n");
    for (c = 0; c < num_comp; c++) {
        printf("  Компонента %d: { ", c+1);
        for (i = 0; i < N; i++)
            if (comp[i] == c) printf("%d ", i+1);
        printf("}\n");
    }
}

// 6. Граф конденсації
void build_condensation(void) {
    int i, j;
    cond_n = num_comp;
    memset(Cond, 0, sizeof(Cond));
    for (i = 0; i < N; i++)
        for (j = 0; j < N; j++)
            if (Adir2[i][j] && comp[i] != comp[j])
                Cond[comp[i]][comp[j]] = 1;

    calc_positions(cpos, cond_n, CX, CY, (cond_n > 1 ? 200 : 0));
}

void print_all_console(void) {
    printf("\n========================================\n");
    printf(" seed=%d  n3=%d  n4=%d\n", SEED, N3, N4);
    printf(" k1=%.4f  k2=%.4f\n", K1, K2);
    printf("========================================\n");

    print_matrix_console("П.1: Adir1 (напрямлений, k1)", Adir1, N);
    print_matrix_console("П.1: Aundir1 (ненапрямлений, k1)", Aundir1, N);

    print_degrees();

    print_matrix_console("П.3: Adir2 (напрямлений, k2)", Adir2, N);

    print_degrees2();

    find_paths();

    print_matrix_console("П.4.3: Матриця досяжності (Reach)", Reach, N);

    print_matrix_console("П.4.4: Матриця сильної зв'язності (Strong)", Strong, N);

    print_components();

    printf("\n=== П.4.6: Граф конденсації (%d вузлів) ===\n", cond_n);
    print_matrix_console("Матриця суміжності конденсації", Cond, cond_n);
    printf("\nВідповідність вузлів конденсації → вершини:\n");
    int c, i;
    for (c = 0; c < cond_n; c++) {
        printf("  C%d = { ", c+1);
        for (i = 0; i < N; i++)
            if (comp[i] == c) printf("%d ", i+1);
        printf("}\n");
    }
}

static void set_color(SDL_Renderer *r, Uint8 red, Uint8 g, Uint8 b, Uint8 a) {
    SDL_SetRenderDrawColor(r, red, g, b, a);
}

void draw_circle_sdl(SDL_Renderer *rnd, int cx, int cy, int r) {
    int x = r, y = 0, err = 0;
    while (x >= y) {
        SDL_RenderDrawPoint(rnd, cx+x,cy+y); SDL_RenderDrawPoint(rnd, cx+y,cy+x);
        SDL_RenderDrawPoint(rnd, cx-y,cy+x); SDL_RenderDrawPoint(rnd, cx-x,cy+y);
        SDL_RenderDrawPoint(rnd, cx-x,cy-y); SDL_RenderDrawPoint(rnd, cx-y,cy-x);
        SDL_RenderDrawPoint(rnd, cx+y,cy-x); SDL_RenderDrawPoint(rnd, cx+x,cy-y);
        y++; err += 1+2*y;
        if (2*(err-x)+1 > 0) { x--; err += 1-2*x; }
    }
}

void fill_circle_sdl(SDL_Renderer *rnd, int cx, int cy, int r) {
    int y;
    for (y = -r; y <= r; y++) {
        int dx = (int)sqrtf((float)(r*r-y*y));
        SDL_RenderDrawLine(rnd, cx-dx, cy+y, cx+dx, cy+y);
    }
}

void draw_arrowhead_sdl(SDL_Renderer *rnd, float x, float y, float angle) {
    float s=sinf(angle), c=cosf(angle);
    float L=ARROW_SIZE, W=ARROW_SIZE*0.42f;
    float p1x=x, p1y=y;
    float p2x=x-L*c+W*s, p2y=y-L*s-W*c;
    float p3x=x-L*c-W*s, p3y=y-L*s+W*c;
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

void draw_bezier_sdl(SDL_Renderer *rnd,
                     float x0,float y0,float cpx,float cpy,float x1,float y1) {
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
    SDL_Color col={r,g,b,255};
    SDL_Surface *surf=TTF_RenderUTF8_Blended(font, text, col);
    if (!surf) return;
    SDL_Texture *tex=SDL_CreateTextureFromSurface(rnd, surf);
    SDL_Rect dst={x-surf->w/2, y-surf->h/2, surf->w, surf->h};
    SDL_RenderCopy(rnd, tex, NULL, &dst);
    SDL_DestroyTexture(tex); SDL_FreeSurface(surf);
}

void draw_edge_sdl(SDL_Renderer *rnd, Vec2 from, Vec2 to,
                   int directed, int curved, int vrad) {
    float dx=to.x-from.x, dy=to.y-from.y;
    float len=sqrtf(dx*dx+dy*dy);
    if (len < 1.0f) return;
    float ux=dx/len, uy=dy/len;
    set_color(rnd, 51,51,51,255);
    if (curved) {
        float mx=(from.x+to.x)/2.0f, my=(from.y+to.y)/2.0f;
        float cpx=mx-uy*CURVE_BEND, cpy=my+ux*CURVE_BEND;
        float a0=atan2f(cpy-from.y, cpx-from.x);
        float sx=from.x+cosf(a0)*vrad, sy=from.y+sinf(a0)*vrad;
        float a1=atan2f(to.y-cpy, to.x-cpx);
        float ex=to.x-cosf(a1)*(vrad+(directed?ARROW_SIZE:0));
        float ey=to.y-sinf(a1)*(vrad+(directed?ARROW_SIZE:0));
        draw_bezier_sdl(rnd, sx,sy, cpx,cpy, ex,ey);
        if (directed) draw_arrowhead_sdl(rnd, ex,ey, a1);
    } else {
        float sx=from.x+ux*vrad, sy=from.y+uy*vrad;
        float ex=to.x-ux*(vrad+(directed?ARROW_SIZE:0));
        float ey=to.y-uy*(vrad+(directed?ARROW_SIZE:0));
        SDL_RenderDrawLine(rnd,(int)sx,(int)sy,(int)ex,(int)ey);
        if (directed) draw_arrowhead_sdl(rnd, ex,ey, atan2f(uy,ux));
    }
}

void draw_selfloop_sdl(SDL_Renderer *rnd, Vec2 v, int directed, int vrad) {
    int lx=(int)v.x, ly=(int)(v.y-vrad-14);
    set_color(rnd, 51,51,51,255);
    draw_circle_sdl(rnd, lx, ly, 13);
    draw_circle_sdl(rnd, lx, ly, 14);
    if (directed) {
        float a=atan2f(v.y-(float)(ly+14), v.x-(float)lx);
        draw_arrowhead_sdl(rnd, v.x+cosf(a)*vrad, v.y+sinf(a)*vrad, a);
    }
}

void draw_graph_sdl(SDL_Renderer *rnd, Matrix A, int n,
                    int directed, Vec2 pos[], int vrad,
                    char labels[][8]) {
    int i, j;

    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            if (!A[i][j]) continue;
            if (!directed && j < i) continue;
            if (i == j) { draw_selfloop_sdl(rnd, pos[i], directed, vrad); continue; }
            int curved = directed && A[j][i];
            draw_edge_sdl(rnd, pos[i], pos[j], directed, curved, vrad);
        }
    }

    for (i = 0; i < n; i++) {
        int cx=(int)pos[i].x, cy=(int)pos[i].y;
        SDL_SetRenderDrawBlendMode(rnd, SDL_BLENDMODE_BLEND);
        set_color(rnd, 77,166,255, 45);
        fill_circle_sdl(rnd, cx, cy, vrad+7);
        SDL_SetRenderDrawBlendMode(rnd, SDL_BLENDMODE_NONE);
        set_color(rnd, 77,166,255, 255);
        fill_circle_sdl(rnd, cx, cy, vrad);
        set_color(rnd, 43,125,233, 255);
        draw_circle_sdl(rnd, cx, cy, vrad);
        draw_circle_sdl(rnd, cx, cy, vrad-1);

        char buf[8];
        if (labels) strncpy(buf, labels[i], 7);
        else snprintf(buf, sizeof(buf), "%d", i+1);
        draw_text_centered(rnd, font13, buf, cx, cy, 255,255,255);
    }
}

void draw_hud_sdl(SDL_Renderer *rnd) {
    const char *titles[] = {
        "",
        "Граф 1 — напрямлений  (Adir1, k1=0.64)",
        "Граф 1 — ненапрямлений  (Aundir1, k1=0.64)",
        "Граф 2 — напрямлений  (Adir2, k2=0.70)",
        "Граф конденсації"
    };
    draw_text_centered(rnd, font20, titles[view_mode], WIN_W/2, 26, 30,30,30);

    draw_text_centered(rnd, font13,
        "1-Adir1   2-Aundir1   3-Adir2   4-Конденсація   ESC/Q-вийти",
        WIN_W/2, WIN_H-18, 140,140,140);
}

void render(SDL_Renderer *rnd) {
    SDL_SetRenderDrawColor(rnd, 240,244,248,255);
    SDL_RenderClear(rnd);

    switch (view_mode) {
        case 1:
            draw_graph_sdl(rnd, Adir1, N, 1, vpos, VRAD, NULL);
            break;
        case 2:
            draw_graph_sdl(rnd, Aundir1, N, 0, vpos, VRAD, NULL);
            break;
        case 3:
            draw_graph_sdl(rnd, Adir2, N, 1, vpos, VRAD, NULL);
            break;
        case 4: {
            char lbl[N][8];
            int c, i;
            for (c = 0; c < cond_n; c++) {
                snprintf(lbl[c], 8, "C%d", c+1);
            }
            draw_graph_sdl(rnd, Cond, cond_n, 1, cpos, VRAD+4, lbl);

            int y_off = 65;
            for (c = 0; c < cond_n; c++) {
                char line[64] = "";
                char tmp[8];
                snprintf(line, sizeof(line), "C%d: {", c+1);
                for (i = 0; i < N; i++) {
                    if (comp[i] == c) {
                        snprintf(tmp, sizeof(tmp), " %d", i+1);
                        strncat(line, tmp, sizeof(line)-strlen(line)-1);
                    }
                }
                strncat(line, " }", sizeof(line)-strlen(line)-1);
                draw_text_centered(rnd, font16, line, 110, y_off, 30,30,30);
                y_off += 24;
            }
            break;
        }
    }

    draw_hud_sdl(rnd);
    SDL_RenderPresent(rnd);
}

int main(void) {

    build_directed(Adir1, K1);
    build_undirected(Aundir1, Adir1);
    build_directed(Adir2, K2);

    calc_degrees();
    calc_degrees2();
    calc_reachability();
    calc_strong();
    find_components();
    build_condensation();

    calc_positions(vpos, N, CX, CY, RADIUS);

    print_all_console();

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
    for (fi = 0; fpaths[fi] && !font13; fi++) {
        font13 = TTF_OpenFont(fpaths[fi], 13);
        font16 = TTF_OpenFont(fpaths[fi], 16);
        font20 = TTF_OpenFont(fpaths[fi], 20);
    }

    SDL_Window *win = SDL_CreateWindow(
        "Лабораторна робота №4 — Характеристики та зв'язність графа",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WIN_W, WIN_H, SDL_WINDOW_SHOWN);
    if (!win) { fprintf(stderr, "Window: %s\n", SDL_GetError()); SDL_Quit(); return 1; }
    SDL_RaiseWindow(win);

    SDL_Renderer *rnd = SDL_CreateRenderer(
        win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!rnd) { fprintf(stderr, "Renderer: %s\n", SDL_GetError());
                SDL_DestroyWindow(win); SDL_Quit(); return 1; }

    int running = 1;
    SDL_Event ev;

    while (running) {
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_QUIT) running = 0;
            if (ev.type == SDL_KEYDOWN) {
                switch (ev.key.keysym.sym) {
                    case SDLK_ESCAPE: case SDLK_q: running = 0; break;
                    case SDLK_1: view_mode = 1; break;
                    case SDLK_2: view_mode = 2; break;
                    case SDLK_3: view_mode = 3; break;
                    case SDLK_4: view_mode = 4; break;
                }
            }
        }
        render(rnd);
    }

    if (font13) TTF_CloseFont(font13);
    if (font16) TTF_CloseFont(font16);
    if (font20) TTF_CloseFont(font20);
    TTF_Quit();
    SDL_DestroyRenderer(rnd);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}