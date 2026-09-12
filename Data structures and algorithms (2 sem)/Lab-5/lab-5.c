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
#define K       (1.0f - N3*0.01f - N4*0.005f - 0.15f)

#define WIN_W        1200
#define WIN_H         700

#define GRAPH_CX     340
#define GRAPH_CY     360
#define GRAPH_R      270

#define TREE_X0      730
#define TREE_Y0       80
#define TREE_DX      80
#define TREE_DY      65

#define VRAD         22
#define ARROW_SIZE   10
#define CURVE_BEND   38

typedef struct { float x, y; } Vec2;
typedef int Matrix[N][N];

typedef enum { WHITE=0, GRAY=1, BLACK=2, CURRENT=3 } VColor;

typedef struct {
    int  vertex;
    int  parent;
    char action[64];
} LogEntry;

typedef struct {
    int     vertex;
    int     parent;
    Vec2    pos;
} TreeNode;

Matrix  A;
Vec2    vpos[N];

VColor  color[N];
int     parent_of[N];
int     tree_edge[N][N];

int     bfs_queue[N*N];
int     bfs_head, bfs_tail;

int     dfs_stack[N*N];
int     dfs_top;

LogEntry log_buf[N*N*2];
int      log_len;
int      log_shown;

TreeNode tree_nodes[N];
int      tree_size;

int      mode_dfs = 0;

int      traversal_done = 0;

int      current_vertex = -1;

TTF_Font *font13 = NULL;
TTF_Font *font16 = NULL;
TTF_Font *font20 = NULL;

void build_matrix(void) {
    float T[N][N];
    float k = K;
    int i, j;
    srand(SEED);
    for (i = 0; i < N; i++)
        for (j = 0; j < N; j++)
            T[i][j] = ((float)rand() / RAND_MAX) * 2.0f;
    for (i = 0; i < N; i++)
        for (j = 0; j < N; j++)
            A[i][j] = (T[i][j] * k >= 1.0f) ? 1 : 0;
    for (i = 0; i < N; i++) A[i][i] = 0;
}

void calc_positions(void) {
    int i;
    for (i = 0; i < N - 1; i++) {
        float a = (float)(2.0*M_PI*i)/(N-1) - (float)(M_PI/2.0);
        vpos[i].x = GRAPH_CX + GRAPH_R * cosf(a);
        vpos[i].y = GRAPH_CY + GRAPH_R * sinf(a);
    }
    vpos[N-1].x = GRAPH_CX;
    vpos[N-1].y = GRAPH_CY;
}

void reset_traversal(void) {
    int i, j;
    for (i = 0; i < N; i++) {
        color[i]    = WHITE;
        parent_of[i] = -1;
        for (j = 0; j < N; j++) tree_edge[i][j] = 0;
    }
    bfs_head = bfs_tail = 0;
    dfs_top  = 0;
    log_len  = log_shown = 0;
    tree_size = 0;
    current_vertex = -1;
    traversal_done = 0;
}

int find_start(void) {
    int i, j;
    for (i = 0; i < N; i++) {
        if (color[i] != WHITE) continue;
        for (j = 0; j < N; j++) if (A[i][j]) return i;
    }
    return -1;
}

void log_add(int v, int par, const char *fmt, ...) {
    LogEntry *e = &log_buf[log_len++];
    e->vertex = v;
    e->parent = par;
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(e->action, sizeof(e->action), fmt, ap);
    va_end(ap);
}

void tree_add(int v, int par_v) {
    int i;
    TreeNode *nd = &tree_nodes[tree_size];
    nd->vertex = v;
    nd->parent = -1;

    if (par_v >= 0) {
        for (i = 0; i < tree_size; i++) {
            if (tree_nodes[i].vertex == par_v) {
                nd->parent = i;
                break;
            }
        }
    }

    int depth = 0, idx = nd->parent;
    while (idx >= 0) { depth++; idx = tree_nodes[idx].parent; }

    int order = 0;
    for (i = 0; i < tree_size; i++) {
        int d2 = 0, idx2 = tree_nodes[i].parent;
        while (idx2 >= 0) { d2++; idx2 = tree_nodes[idx2].parent; }
        if (d2 == depth) order++;
    }

    nd->pos.x = TREE_X0 + depth * TREE_DX;
    nd->pos.y = TREE_Y0 + order * TREE_DY;
    tree_size++;
}

// BFS
int bfs_step(void) {
    if (bfs_head == bfs_tail) {
        int s = find_start();
        if (s < 0) { traversal_done = 1; return 0; }
        color[s] = GRAY;
        bfs_queue[bfs_tail++] = s;
        tree_add(s, -1);
        log_add(s, -1, "Старт BFS з вершини %d", s+1);
        current_vertex = s;
        return 1;
    }

    int v = bfs_queue[bfs_head++];
    color[v] = CURRENT;
    current_vertex = v;

    int j, pushed = 0;
    for (j = 0; j < N; j++) {
        if (!A[v][j]) continue;
        if (color[j] == WHITE) {
            color[j]      = GRAY;
            parent_of[j]  = v;
            tree_edge[v][j] = 1;
            bfs_queue[bfs_tail++] = j;
            tree_add(j, v);
            log_add(v, -1, "  BFS: %d → %d  (в чергу)", v+1, j+1);
            pushed = 1;
        }
    }
    if (!pushed)
        log_add(v, -1, "  BFS: %d  (нових сусідів немає)", v+1);

    color[v] = BLACK;

    if (bfs_head == bfs_tail) {
        int s = find_start();
        if (s < 0) traversal_done = 1;
    }
    return 1;
}

// DFS 
int dfs_step(void) {
    if (dfs_top == 0) {
        int s = find_start();
        if (s < 0) { traversal_done = 1; return 0; }
        color[s] = GRAY;
        dfs_stack[dfs_top++] = s;
        tree_add(s, -1);
        log_add(s, -1, "Старт DFS з вершини %d", s+1);
        current_vertex = s;
        return 1;
    }

    int v = dfs_stack[--dfs_top];

    if (color[v] == BLACK) {
        return bfs_step();
    }

    color[v] = CURRENT;
    current_vertex = v;

    int j, pushed = 0;
    for (j = N-1; j >= 0; j--) {
        if (!A[v][j]) continue;
        if (color[j] == WHITE) {
            color[j]     = GRAY;
            parent_of[j] = v;
            tree_edge[v][j] = 1;
            dfs_stack[dfs_top++] = j;
            tree_add(j, v);
            log_add(v, -1, "  DFS: %d → %d  (в стек)", v+1, j+1);
            pushed = 1;
        }
    }
    if (!pushed)
        log_add(v, -1, "  DFS: %d  (нових сусідів немає)", v+1);

    color[v] = BLACK;

    if (dfs_top == 0) {
        int s = find_start();
        if (s < 0) traversal_done = 1;
    }
    return 1;
}

void do_step(void) {
    if (traversal_done) return;
    if (mode_dfs) dfs_step();
    else          bfs_step();
    while (log_shown < log_len) {
        printf("%s\n", log_buf[log_shown].action);
        log_shown++;
    }
}

static void set_color(SDL_Renderer *r, Uint8 red, Uint8 g, Uint8 b, Uint8 a) {
    SDL_SetRenderDrawColor(r, red, g, b, a);
}

void draw_circle_s(SDL_Renderer *rnd, int cx, int cy, int r) {
    int x=r, y=0, err=0;
    while (x>=y) {
        SDL_RenderDrawPoint(rnd,cx+x,cy+y); SDL_RenderDrawPoint(rnd,cx+y,cy+x);
        SDL_RenderDrawPoint(rnd,cx-y,cy+x); SDL_RenderDrawPoint(rnd,cx-x,cy+y);
        SDL_RenderDrawPoint(rnd,cx-x,cy-y); SDL_RenderDrawPoint(rnd,cx-y,cy-x);
        SDL_RenderDrawPoint(rnd,cx+y,cy-x); SDL_RenderDrawPoint(rnd,cx+x,cy-y);
        y++; err+=1+2*y;
        if (2*(err-x)+1>0){x--;err+=1-2*x;}
    }
}

void fill_circle_s(SDL_Renderer *rnd, int cx, int cy, int r) {
    int y;
    for (y=-r; y<=r; y++) {
        int dx=(int)sqrtf((float)(r*r-y*y));
        SDL_RenderDrawLine(rnd,cx-dx,cy+y,cx+dx,cy+y);
    }
}

void draw_arrowhead_s(SDL_Renderer *rnd, float x, float y, float angle) {
    float s=sinf(angle), c=cosf(angle);
    float L=ARROW_SIZE, W=ARROW_SIZE*0.42f;
    float p1x=x,p1y=y;
    float p2x=x-L*c+W*s,p2y=y-L*s-W*c;
    float p3x=x-L*c-W*s,p3y=y-L*s+W*c;
    SDL_RenderDrawLine(rnd,(int)p1x,(int)p1y,(int)p2x,(int)p2y);
    SDL_RenderDrawLine(rnd,(int)p2x,(int)p2y,(int)p3x,(int)p3y);
    SDL_RenderDrawLine(rnd,(int)p3x,(int)p3y,(int)p1x,(int)p1y);
    int minY=(int)fminf(fminf(p1y,p2y),p3y);
    int maxY=(int)fmaxf(fmaxf(p1y,p2y),p3y);
    float pts[3][2]={{p1x,p1y},{p2x,p2y},{p3x,p3y}};
    int iy,e;
    for (iy=minY;iy<=maxY;iy++){
        float xs[2];int cnt=0;
        for (e=0;e<3&&cnt<2;e++){
            float y0=pts[e][1],y1=pts[(e+1)%3][1];
            float x0=pts[e][0],x1=pts[(e+1)%3][0];
            if((iy>=y0&&iy<y1)||(iy>=y1&&iy<y0))
                xs[cnt++]=x0+(x1-x0)*(iy-y0)/(y1-y0);
        }
        if(cnt==2) SDL_RenderDrawLine(rnd,(int)xs[0],iy,(int)xs[1],iy);
    }
}

void draw_bezier_s(SDL_Renderer *rnd,
                   float x0,float y0,float cpx,float cpy,float x1,float y1) {
    float px=x0,py=y0; int i;
    for (i=1;i<=60;i++){
        float t=(float)i/60.0f,mt=1.0f-t;
        float nx=mt*mt*x0+2*mt*t*cpx+t*t*x1;
        float ny=mt*mt*y0+2*mt*t*cpy+t*t*y1;
        SDL_RenderDrawLine(rnd,(int)px,(int)py,(int)nx,(int)ny);
        px=nx;py=ny;
    }
}

void draw_text_c(SDL_Renderer *rnd, TTF_Font *font, const char *text,
                 int x, int y, Uint8 r, Uint8 g, Uint8 b) {
    if (!font) return;
    SDL_Color col={r,g,b,255};
    SDL_Surface *surf=TTF_RenderUTF8_Blended(font,text,col);
    if (!surf) return;
    SDL_Texture *tex=SDL_CreateTextureFromSurface(rnd,surf);
    SDL_Rect dst={x-surf->w/2,y-surf->h/2,surf->w,surf->h};
    SDL_RenderCopy(rnd,tex,NULL,&dst);
    SDL_DestroyTexture(tex);SDL_FreeSurface(surf);
}

void draw_text_l(SDL_Renderer *rnd, TTF_Font *font, const char *text,
                 int x, int y, Uint8 r, Uint8 g, Uint8 b) {
    if (!font) return;
    SDL_Color col={r,g,b,255};
    SDL_Surface *surf=TTF_RenderUTF8_Blended(font,text,col);
    if (!surf) return;
    SDL_Texture *tex=SDL_CreateTextureFromSurface(rnd,surf);
    SDL_Rect dst={x,y-surf->h/2,surf->w,surf->h};
    SDL_RenderCopy(rnd,tex,NULL,&dst);
    SDL_DestroyTexture(tex);SDL_FreeSurface(surf);
}

void draw_edge_s(SDL_Renderer *rnd, Vec2 from, Vec2 to,
                 int is_tree, int curved) {
    float dx=to.x-from.x, dy=to.y-from.y;
    float len=sqrtf(dx*dx+dy*dy);
    if (len<1.0f) return;
    float ux=dx/len, uy=dy/len;

    if (is_tree) set_color(rnd, 220, 80, 80, 255);
    else         set_color(rnd, 180,180,180, 255);

    if (curved) {
        float mx=(from.x+to.x)/2.0f, my=(from.y+to.y)/2.0f;
        float cpx=mx-uy*CURVE_BEND, cpy=my+ux*CURVE_BEND;
        float a0=atan2f(cpy-from.y,cpx-from.x);
        float sx=from.x+cosf(a0)*VRAD, sy=from.y+sinf(a0)*VRAD;
        float a1=atan2f(to.y-cpy,to.x-cpx);
        float ex=to.x-cosf(a1)*(VRAD+ARROW_SIZE);
        float ey=to.y-sinf(a1)*(VRAD+ARROW_SIZE);
        draw_bezier_s(rnd,sx,sy,cpx,cpy,ex,ey);
        draw_arrowhead_s(rnd,ex,ey,a1);
    } else {
        float sx=from.x+ux*VRAD, sy=from.y+uy*VRAD;
        float ex=to.x-ux*(VRAD+ARROW_SIZE);
        float ey=to.y-uy*(VRAD+ARROW_SIZE);
        SDL_RenderDrawLine(rnd,(int)sx,(int)sy,(int)ex,(int)ey);
        draw_arrowhead_s(rnd,ex,ey,atan2f(uy,ux));
    }
}

void vertex_fill_color(VColor vc, int is_current,
                       Uint8 *r, Uint8 *g, Uint8 *b) {
    if (is_current) { *r=60;  *g=180; *b=60;  return; }
    switch (vc) {
        case WHITE:   *r=190; *g=195; *b=205; return;
        case GRAY:    *r=240; *g=200; *b=50;  return;
        case BLACK:   *r=77;  *g=166; *b=255; return;
        default:      *r=190; *g=195; *b=205; return;
    }
}

void draw_graph_panel(SDL_Renderer *rnd) {
    int i, j;

    for (i=0;i<N;i++) {
        for (j=0;j<N;j++) {
            if (!A[i][j]) continue;
            if (i==j) {
                int lx=(int)vpos[i].x, ly=(int)(vpos[i].y-VRAD-14);
                int is_t = tree_edge[i][j];
                if (is_t) set_color(rnd,220,80,80,255);
                else      set_color(rnd,180,180,180,255);
                draw_circle_s(rnd,lx,ly,13);
                draw_circle_s(rnd,lx,ly,14);
                continue;
            }
            int is_tree = tree_edge[i][j];
            int curved  = A[j][i];
            draw_edge_s(rnd, vpos[i], vpos[j], is_tree, curved);
        }
    }

    for (i=0;i<N;i++) {
        int cx=(int)vpos[i].x, cy=(int)vpos[i].y;
        int is_cur = (i == current_vertex);
        Uint8 fr,fg,fb;
        vertex_fill_color(color[i], is_cur, &fr,&fg,&fb);

        SDL_SetRenderDrawBlendMode(rnd, SDL_BLENDMODE_BLEND);
        set_color(rnd,fr,fg,fb,50);
        fill_circle_s(rnd,cx,cy,VRAD+8);
        SDL_SetRenderDrawBlendMode(rnd, SDL_BLENDMODE_NONE);

        set_color(rnd,fr,fg,fb,255);
        fill_circle_s(rnd,cx,cy,VRAD);

        set_color(rnd,(Uint8)(fr*0.7f),(Uint8)(fg*0.7f),(Uint8)(fb*0.7f),255);
        draw_circle_s(rnd,cx,cy,VRAD);
        draw_circle_s(rnd,cx,cy,VRAD-1);

        char buf[4];
        snprintf(buf,sizeof(buf),"%d",i+1);
        draw_text_c(rnd,font13,buf,cx,cy,255,255,255);
    }
}

void draw_tree_panel(SDL_Renderer *rnd) {
    int i;

    draw_text_c(rnd,font16,
        mode_dfs ? "Дерево DFS" : "Дерево BFS",
        TREE_X0+180, 40, 30,30,30);

    if (tree_size == 0) {
        draw_text_c(rnd,font13,"(ще не розпочато)",
                    TREE_X0+180, 70, 150,150,150);
        return;
    }

    for (i=1; i<tree_size; i++) {
        if (tree_nodes[i].parent < 0) continue;
        Vec2 ppos = tree_nodes[tree_nodes[i].parent].pos;
        Vec2 cpos2 = tree_nodes[i].pos;
        set_color(rnd,220,80,80,255);
        SDL_RenderDrawLine(rnd,
            (int)ppos.x,  (int)ppos.y,
            (int)cpos2.x, (int)cpos2.y);
    }

    for (i=0; i<tree_size; i++) {
        int v   = tree_nodes[i].vertex;
        int cx  = (int)tree_nodes[i].pos.x;
        int cy  = (int)tree_nodes[i].pos.y;
        int is_cur = (v == current_vertex);
        Uint8 fr,fg,fb;
        vertex_fill_color(color[v], is_cur, &fr,&fg,&fb);

        set_color(rnd,fr,fg,fb,255);
        fill_circle_s(rnd,cx,cy,18);
        set_color(rnd,(Uint8)(fr*0.7f),(Uint8)(fg*0.7f),(Uint8)(fb*0.7f),255);
        draw_circle_s(rnd,cx,cy,18);

        char buf[4];
        snprintf(buf,sizeof(buf),"%d",v+1);
        draw_text_c(rnd,font13,buf,cx,cy,255,255,255);
    }
}

void draw_log_panel(SDL_Renderer *rnd) {
    int MAX_LINES = 6;
    int start = log_len - MAX_LINES;
    if (start < 0) start = 0;
    int y = WIN_H - 20 - MAX_LINES * 18;
    int i;
    for (i = start; i < log_len; i++) {
        Uint8 alpha = (i == log_len-1) ? 220 : 120;
        draw_text_l(rnd, font13, log_buf[i].action,
                    10, y, alpha, alpha, alpha);
        y += 18;
    }
    if (traversal_done)
        draw_text_c(rnd, font16, "Обхід завершено!", 340, WIN_H-16,
                    80, 160, 80);
}

void draw_hud(SDL_Renderer *rnd) {
    char title[64];
    snprintf(title, sizeof(title),
        "Лабораторна робота №5 — %s",
        mode_dfs ? "DFS (обхід вглиб)" : "BFS (обхід вшир)");
    draw_text_c(rnd, font20, title, 340, 26, 30,30,30);

    int lx = 10, ly = 60;
    struct { Uint8 r,g,b; const char *label; } legend[] = {
        {190,195,205, "WHITE — не відвідана"},
        {240,200, 50, "GRAY  — в черзі / стеку"},
        { 77,166,255, "BLACK — оброблена"},
        { 60,180, 60, "GREEN — поточна"},
    };
    int li;
    for (li=0; li<4; li++) {
        set_color(rnd, legend[li].r, legend[li].g, legend[li].b, 255);
        SDL_Rect rc = {lx, ly-8, 14, 14};
        SDL_RenderFillRect(rnd, &rc);
        draw_text_l(rnd, font13, legend[li].label, lx+18, ly, 60,60,60);
        ly += 20;
    }

    set_color(rnd,200,210,220,255);
    SDL_RenderDrawLine(rnd, 700, 10, 700, WIN_H-10);
    SDL_RenderDrawLine(rnd, 701, 10, 701, WIN_H-10);

    /* Підказки клавіш */
    draw_text_c(rnd, font13,
        "B-BFS  D-DFS  SPACE/→-крок  R-скинути  ESC-вийти",
        340, WIN_H-36, 140,140,140);
}

void print_matrix(void) {
    int i, j;
    printf("\nМатриця суміжності Adir (seed=%d, k=%.4f):\n    ", SEED, K);
    for (j=0;j<N;j++) printf("%3d",j+1);
    printf("\n   +");
    for (j=0;j<N;j++) printf("---");
    printf("\n");
    for (i=0;i<N;i++){
        printf("%2d |",i+1);
        for (j=0;j<N;j++) printf("%3d",A[i][j]);
        printf("\n");
    }
}

int main(void) {
    build_matrix();
    calc_positions();
    reset_traversal();
    print_matrix();
    printf("\nКерування: B=BFS  D=DFS  SPACE/Enter/→=крок  R=скинути\n\n");

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr,"SDL_Init: %s\n",SDL_GetError()); return 1;
    }
    if (TTF_Init() != 0)
        fprintf(stderr,"TTF_Init: %s\n",TTF_GetError());

    const char *fpaths[] = {
        "/opt/homebrew/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/Library/Fonts/Arial.ttf",
        "/System/Library/Fonts/Helvetica.ttc",
        "C:/Windows/Fonts/arial.ttf",
        NULL
    };
    int fi;
    for (fi=0; fpaths[fi]&&!font13; fi++){
        font13 = TTF_OpenFont(fpaths[fi],13);
        font16 = TTF_OpenFont(fpaths[fi],16);
        font20 = TTF_OpenFont(fpaths[fi],20);
    }

    SDL_Window *win = SDL_CreateWindow(
        "Лабораторна робота №5 — Обхід графа",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WIN_W, WIN_H, SDL_WINDOW_SHOWN);
    if (!win){fprintf(stderr,"Window: %s\n",SDL_GetError());SDL_Quit();return 1;}
    SDL_RaiseWindow(win);

    SDL_Renderer *rnd = SDL_CreateRenderer(
        win,-1,SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
    if (!rnd){fprintf(stderr,"Renderer: %s\n",SDL_GetError());
              SDL_DestroyWindow(win);SDL_Quit();return 1;}

    int running=1;
    SDL_Event ev;

    while (running) {
        while (SDL_PollEvent(&ev)) {
            if (ev.type==SDL_QUIT) running=0;
            if (ev.type==SDL_KEYDOWN) {
                switch(ev.key.keysym.sym){
                    case SDLK_ESCAPE: case SDLK_q:
                        running=0; break;
                    case SDLK_SPACE: case SDLK_RETURN:
                    case SDLK_RIGHT:
                        do_step(); break;
                    case SDLK_b:
                        mode_dfs=0;
                        reset_traversal();
                        printf("\n--- BFS ---\n");
                        break;
                    case SDLK_d:
                        mode_dfs=1;
                        reset_traversal();
                        printf("\n--- DFS ---\n");
                        break;
                    case SDLK_r:
                        reset_traversal();
                        printf("\n--- Скинуто ---\n");
                        break;
                }
            }
        }

        SDL_SetRenderDrawColor(rnd,240,244,248,255);
        SDL_RenderClear(rnd);

        draw_graph_panel(rnd);
        draw_tree_panel(rnd);
        draw_log_panel(rnd);
        draw_hud(rnd);

        SDL_RenderPresent(rnd);
    }

    if(font13) TTF_CloseFont(font13);
    if(font16) TTF_CloseFont(font16);
    if(font20) TTF_CloseFont(font20);
    TTF_Quit();
    SDL_DestroyRenderer(rnd);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}