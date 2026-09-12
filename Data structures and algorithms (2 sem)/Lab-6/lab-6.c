#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define N        10
#define SEED     5306
#define N3       0
#define N4       6
#define K_VAL    (1.0f - N3*0.01f - N4*0.005f - 0.05f)

#define WIN_W    1300
#define WIN_H     740

#define GCX      310
#define GCY      385
#define GR       220

#define TCX      970
#define TCY      385
#define TR       220

#define VRAD     22
#define ARROW_S  10
#define CBEND    38

#define MAX_EDGES  (N*(N-1)/2)

typedef struct { float x, y; } Vec2;
typedef int   Matrix[N][N];

typedef struct VNode {
    int          id;
    struct VNode *next;
} VNode;

typedef struct ENode {
    int          u, v, w;
    struct ENode *next;
} ENode;

typedef enum { EDGE_NORMAL=0, EDGE_CURRENT, EDGE_ACCEPTED, EDGE_REJECTED } EdgeState;

Matrix  Adir;
Matrix  Aundir;
Matrix  W;

VNode  *vlist      = NULL;
ENode  *elist      = NULL;
ENode  *span_list  = NULL;

Vec2    gpos[N];
Vec2    tpos[N];

EdgeState  estate[N][N];
int        current_u = -1, current_v = -1;

int  dsu_parent[N];
int  dsu_rank[N];

typedef struct { int u,v,w; } Edge;
Edge  sorted_edges[MAX_EDGES];
int   num_edges = 0;
int   kruskal_step_idx = 0;
int   span_edge_count  = 0;
int   algo_done        = 0;

char  log_buf[256][80];
int   log_len = 0;

TTF_Font *font12 = NULL;
TTF_Font *font15 = NULL;
TTF_Font *font19 = NULL;

ENode *elist_remove_vertex(ENode *head, int vid);

VNode *vlist_add(VNode *head, int id) {
    VNode *nd = (VNode*)malloc(sizeof(VNode));
    nd->id = id; nd->next = NULL;
    if (!head) return nd;
    VNode *cur = head;
    while (cur->next) cur = cur->next;
    cur->next = nd;
    return head;
}

VNode *vlist_remove(VNode *head, int id, ENode **elist_ptr) {

    if (elist_ptr)
        *elist_ptr = elist_remove_vertex(*elist_ptr, id);

    VNode *cur = head, *prev = NULL;
    while (cur) {
        if (cur->id == id) {
            if (prev) prev->next = cur->next;
            else      head = cur->next;
            free(cur);
            return head;
        }
        prev = cur; cur = cur->next;
    }
    return head;
}

int vlist_contains(VNode *head, int id) {
    while (head) { if (head->id == id) return 1; head = head->next; }
    return 0;
}

int vlist_count(VNode *head) {
    int n = 0;
    while (head) { n++; head = head->next; }
    return n;
}

void vlist_free(VNode *head) {
    while (head) { VNode *t = head; head = head->next; free(t); }
}

ENode *elist_insert_sorted(ENode *head, int u, int v, int w) {
    ENode *cur = head;
    while (cur) {
        if ((cur->u==u && cur->v==v) || (cur->u==v && cur->v==u))
            return head;
        cur = cur->next;
    }
    ENode *nd = (ENode*)malloc(sizeof(ENode));
    nd->u=u; nd->v=v; nd->w=w; nd->next=NULL;
    if (!head || w < head->w) { nd->next=head; return nd; }
    cur = head;
    while (cur->next && cur->next->w <= w) cur = cur->next;
    nd->next = cur->next; cur->next = nd;
    return head;
}

ENode *elist_remove(ENode *head, int u, int v) {
    ENode *cur = head, *prev = NULL;
    while (cur) {
        if ((cur->u==u && cur->v==v) || (cur->u==v && cur->v==u)) {
            if (prev) prev->next = cur->next;
            else      head = cur->next;
            free(cur);
            return head;
        }
        prev = cur; cur = cur->next;
    }
    return head;
}

ENode *elist_remove_vertex(ENode *head, int vid) {
    ENode *cur = head, *prev = NULL;
    while (cur) {
        if (cur->u == vid || cur->v == vid) {
            ENode *del = cur;
            if (prev) prev->next = cur->next;
            else      head = cur->next;
            cur = cur->next;
            free(del);
        } else {
            prev = cur; cur = cur->next;
        }
    }
    return head;
}

int elist_contains(ENode *head, int u, int v) {
    while (head) {
        if ((head->u==u && head->v==v) || (head->u==v && head->v==u))
            return 1;
        head = head->next;
    }
    return 0;
}

int elist_count(ENode *head) {
    int n = 0;
    while (head) { n++; head = head->next; }
    return n;
}

void elist_free(ENode *head) {
    while (head) { ENode *t = head; head = head->next; free(t); }
}

ENode *span_add(ENode *head, int u, int v, int w) {
    ENode *nd = (ENode*)malloc(sizeof(ENode));
    nd->u=u; nd->v=v; nd->w=w; nd->next=head;
    return nd;
}

ENode *span_remove(ENode *head, int u, int v) {
    return elist_remove(head, u, v);
}

void dsu_init(void) {
    int i;
    for (i=0;i<N;i++){dsu_parent[i]=i; dsu_rank[i]=0;}
}

int dsu_find(int x) {
    if (dsu_parent[x]!=x) dsu_parent[x]=dsu_find(dsu_parent[x]);
    return dsu_parent[x];
}

int dsu_union(int x, int y) {
    int rx=dsu_find(x), ry=dsu_find(y);
    if (rx==ry) return 0;
    if (dsu_rank[rx]<dsu_rank[ry]) { int t=rx;rx=ry;ry=t; }
    dsu_parent[ry]=rx;
    if (dsu_rank[rx]==dsu_rank[ry]) dsu_rank[rx]++;
    return 1;
}

void build_matrices(void) {
    float T[N][N], B[N][N];
    float k = K_VAL;
    int i, j;

    srand(SEED);
    for (i=0;i<N;i++)
        for (j=0;j<N;j++)
            T[i][j]=((float)rand()/RAND_MAX)*2.0f;

    for (i=0;i<N;i++)
        for (j=0;j<N;j++)
            Adir[i][j]=(T[i][j]*k>=1.0f)?1:0;
    for (i=0;i<N;i++) Adir[i][i]=0;

    for (i=0;i<N;i++)
        for (j=0;j<N;j++)
            Aundir[i][j]=(Adir[i][j]||Adir[j][i])?1:0;

    for (i=0;i<N;i++)
        for (j=0;j<N;j++)
            B[i][j]=((float)rand()/RAND_MAX)*2.0f;

    float C[N][N];
    for (i=0;i<N;i++)
        for (j=0;j<N;j++)
            C[i][j]=(float)ceil(B[i][j]*100.0f)*(float)Aundir[i][j];

    int D[N][N];
    for (i=0;i<N;i++)
        for (j=0;j<N;j++)
            D[i][j]=(C[i][j]!=0.0f)?1:0;

    int H[N][N];
    for (i=0;i<N;i++)
        for (j=0;j<N;j++)
            H[i][j]=(D[i][j]==D[j][i])?1:0;

    for (i=0;i<N;i++)
        for (j=0;j<N;j++)
            W[i][j]=0;

    for (i=0;i<N;i++) {
        for (j=i+1;j<N;j++) {
            int tr = 1;
            int w  = (int)(D[i][j]*H[i][j]*tr * C[i][j]);
            W[i][j] = w;
            W[j][i] = w;
        }
    }
}

void build_lists(void) {
    int i, j;

    vlist_free(vlist); vlist = NULL;
    for (i = 0; i < N; i++)
        vlist = vlist_add(vlist, i);

    printf("\n--- Демонстрація операцій зі списками ---\n");
    printf("Вершин у списку: %d\n", vlist_count(vlist));

    vlist = vlist_add(vlist, 99);
    printf("Після vlist_add(99): %d вершин, contains(99)=%d\n",
           vlist_count(vlist), vlist_contains(vlist, 99));
    vlist = vlist_remove(vlist, 99, NULL);
    printf("Після vlist_remove(99): %d вершин, contains(99)=%d\n",
           vlist_count(vlist), vlist_contains(vlist, 99));

    elist_free(elist); elist = NULL;
    num_edges = 0;

    for (i = 0; i < N; i++)
        for (j = i+1; j < N; j++)
            if (W[i][j] != 0)
                elist = elist_insert_sorted(elist, i, j, W[i][j]);

    printf("Ребер у списку: %d\n", elist_count(elist));

    if (elist) {
        int du = elist->u, dv = elist->v;
        printf("elist_contains(%d-%d)=%d\n",
               du+1, dv+1, elist_contains(elist, du, dv));

        int dw = elist->w;
        elist = elist_remove(elist, du, dv);
        printf("Після elist_remove(%d-%d): %d ребер\n",
               du+1, dv+1, elist_count(elist));
        elist = elist_insert_sorted(elist, du, dv, dw);
        printf("Після elist_insert_sorted(%d-%d,w=%d): %d ребер\n",
               du+1, dv+1, dw, elist_count(elist));
    }
    printf("-----------------------------------------\n");

    ENode *cur = elist;
    while (cur && num_edges < MAX_EDGES) {
        sorted_edges[num_edges].u = cur->u;
        sorted_edges[num_edges].v = cur->v;
        sorted_edges[num_edges].w = cur->w;
        num_edges++;
        cur = cur->next;
    }
}

void reset_kruskal(void) {
    int i, j;
    dsu_init();
    kruskal_step_idx = 0;
    span_edge_count  = 0;
    algo_done        = 0;
    current_u = current_v = -1;
    log_len   = 0;

    for (i=0;i<N;i++)
        for (j=0;j<N;j++)
            estate[i][j] = EDGE_NORMAL;

    elist_free(span_list); span_list=NULL;
}

void kruskal_step(void) {
    if (algo_done) return;

    if (span_edge_count == N-1) {
        algo_done=1;
        snprintf(log_buf[log_len++],80,"Кістяк побудовано! (%d ребер)", N-1);
        return;
    }

    if (kruskal_step_idx >= num_edges) {
        algo_done=1;
        snprintf(log_buf[log_len++],80,
            "Ребра вичерпано. Кістяк: %d ребер (граф незв'язний?)",
            span_edge_count);
        return;
    }

    int u = sorted_edges[kruskal_step_idx].u;
    int v = sorted_edges[kruskal_step_idx].v;
    int w = sorted_edges[kruskal_step_idx].w;
    kruskal_step_idx++;

    current_u = u; current_v = v;
    estate[u][v] = estate[v][u] = EDGE_CURRENT;

    if (dsu_union(u,v)) {
        estate[u][v] = estate[v][u] = EDGE_ACCEPTED;
        span_list = span_add(span_list,u,v,w);
        span_edge_count++;
        snprintf(log_buf[log_len++],80,
            "Крок %d: (%d-%d, w=%d) → ДОДАНО",
            kruskal_step_idx, u+1, v+1, w);
    } else {
        estate[u][v] = estate[v][u] = EDGE_REJECTED;
        snprintf(log_buf[log_len++],80,
            "Крок %d: (%d-%d, w=%d) → ЦИКЛ, відхилено",
            kruskal_step_idx, u+1, v+1, w);
    }

    if (span_edge_count == N-1) {
        algo_done=1;
        snprintf(log_buf[log_len++],80,"Кістяк побудовано! (%d ребер)", N-1);
    }
}

void print_console(void) {
    int i,j;
    printf("\n=== Лабораторна робота №6 ===\n");
    printf("seed=%d  n3=%d  n4=%d  k=%.4f\n",SEED,N3,N4,K_VAL);

    printf("\nМатриця суміжності Aundir:\n    ");
    for (j=0;j<N;j++) printf("%3d",j+1);
    printf("\n   +");for(j=0;j<N;j++)printf("---");printf("\n");
    for (i=0;i<N;i++){
        printf("%2d |",i+1);
        for(j=0;j<N;j++) printf("%3d",Aundir[i][j]);
        printf("\n");
    }

    printf("\nМатриця ваг W:\n    ");
    for(j=0;j<N;j++) printf("%5d",j+1);
    printf("\n   +");for(j=0;j<N;j++)printf("-----");printf("\n");
    for(i=0;i<N;i++){
        printf("%2d |",i+1);
        for(j=0;j<N;j++) printf("%5d",W[i][j]);
        printf("\n");
    }

    printf("\nВідсортовані ребра (%d):\n",num_edges);
    int k;
    for(k=0;k<num_edges;k++)
        printf("  (%d-%d, w=%d)\n",
            sorted_edges[k].u+1, sorted_edges[k].v+1, sorted_edges[k].w);

    printf("\nN4=%d (парне) → алгоритм Краскала\n",N4);
    printf("Керування: SPACE/Enter/→=крок  R=скинути  ESC=вийти\n\n");
}

static void setcol(SDL_Renderer *r,Uint8 red,Uint8 g,Uint8 b,Uint8 a){
    SDL_SetRenderDrawColor(r,red,g,b,a);
}

void draw_circ(SDL_Renderer *r,int cx,int cy,int rad){
    int x=rad,y=0,err=0;
    while(x>=y){
        SDL_RenderDrawPoint(r,cx+x,cy+y);SDL_RenderDrawPoint(r,cx+y,cy+x);
        SDL_RenderDrawPoint(r,cx-y,cy+x);SDL_RenderDrawPoint(r,cx-x,cy+y);
        SDL_RenderDrawPoint(r,cx-x,cy-y);SDL_RenderDrawPoint(r,cx-y,cy-x);
        SDL_RenderDrawPoint(r,cx+y,cy-x);SDL_RenderDrawPoint(r,cx+x,cy-y);
        y++;err+=1+2*y;
        if(2*(err-x)+1>0){x--;err+=1-2*x;}
    }
}

void fill_circ(SDL_Renderer *r,int cx,int cy,int rad){
    int y;
    for(y=-rad;y<=rad;y++){
        int dx=(int)sqrtf((float)(rad*rad-y*y));
        SDL_RenderDrawLine(r,cx-dx,cy+y,cx+dx,cy+y);
    }
}

void draw_arr(SDL_Renderer *r,float x,float y,float angle){
    float s=sinf(angle),c=cosf(angle);
    float L=ARROW_S,W2=ARROW_S*0.42f;
    float p1x=x,p1y=y;
    float p2x=x-L*c+W2*s,p2y=y-L*s-W2*c;
    float p3x=x-L*c-W2*s,p3y=y-L*s+W2*c;
    SDL_RenderDrawLine(r,(int)p1x,(int)p1y,(int)p2x,(int)p2y);
    SDL_RenderDrawLine(r,(int)p2x,(int)p2y,(int)p3x,(int)p3y);
    SDL_RenderDrawLine(r,(int)p3x,(int)p3y,(int)p1x,(int)p1y);
    int minY=(int)fminf(fminf(p1y,p2y),p3y);
    int maxY=(int)fmaxf(fmaxf(p1y,p2y),p3y);
    float pts[3][2]={{p1x,p1y},{p2x,p2y},{p3x,p3y}};
    int iy,e;
    for(iy=minY;iy<=maxY;iy++){
        float xs[2];int cnt=0;
        for(e=0;e<3&&cnt<2;e++){
            float y0=pts[e][1],y1=pts[(e+1)%3][1];
            float x0=pts[e][0],x1=pts[(e+1)%3][0];
            if((iy>=y0&&iy<y1)||(iy>=y1&&iy<y0))
                xs[cnt++]=x0+(x1-x0)*(iy-y0)/(y1-y0);
        }
        if(cnt==2)SDL_RenderDrawLine(r,(int)xs[0],iy,(int)xs[1],iy);
    }
}

void draw_bez(SDL_Renderer *r,float x0,float y0,float cpx,float cpy,float x1,float y1){
    float px=x0,py=y0;int i;
    for(i=1;i<=60;i++){
        float t=(float)i/60.0f,mt=1.0f-t;
        float nx=mt*mt*x0+2*mt*t*cpx+t*t*x1;
        float ny=mt*mt*y0+2*mt*t*cpy+t*t*y1;
        SDL_RenderDrawLine(r,(int)px,(int)py,(int)nx,(int)ny);
        px=nx;py=ny;
    }
}

void txt_c(SDL_Renderer *r,TTF_Font *f,const char *s,int x,int y,Uint8 R,Uint8 G,Uint8 B){
    if(!f)return;
    SDL_Color col={R,G,B,255};
    SDL_Surface *surf=TTF_RenderUTF8_Blended(f,s,col);
    if(!surf)return;
    SDL_Texture *tex=SDL_CreateTextureFromSurface(r,surf);
    SDL_Rect dst={x-surf->w/2,y-surf->h/2,surf->w,surf->h};
    SDL_RenderCopy(r,tex,NULL,&dst);
    SDL_DestroyTexture(tex);SDL_FreeSurface(surf);
}

void txt_l(SDL_Renderer *r,TTF_Font *f,const char *s,int x,int y,Uint8 R,Uint8 G,Uint8 B){
    if(!f)return;
    SDL_Color col={R,G,B,255};
    SDL_Surface *surf=TTF_RenderUTF8_Blended(f,s,col);
    if(!surf)return;
    SDL_Texture *tex=SDL_CreateTextureFromSurface(r,surf);
    SDL_Rect dst={x,y-surf->h/2,surf->w,surf->h};
    SDL_RenderCopy(r,tex,NULL,&dst);
    SDL_DestroyTexture(tex);SDL_FreeSurface(surf);
}

void draw_undir_edge(SDL_Renderer *rnd, Vec2 from, Vec2 to,
                     int weight, EdgeState es, int show_weight) {
    float dx=to.x-from.x, dy=to.y-from.y;
    float len=sqrtf(dx*dx+dy*dy);
    if(len<1.0f) return;
    float ux=dx/len, uy=dy/len;
    float sx=from.x+ux*VRAD, sy=from.y+uy*VRAD;
    float ex=to.x  -ux*VRAD, ey=to.y  -uy*VRAD;

    switch(es){
        case EDGE_CURRENT:  setcol(rnd,240,190, 20,255); break;
        case EDGE_ACCEPTED: setcol(rnd, 50,180, 80,255); break;
        case EDGE_REJECTED: setcol(rnd,220, 60, 60,255); break;
        default:            setcol(rnd,160,165,175,255); break;
    }

 SDL_RenderDrawLine(rnd,(int)sx,(int)sy,(int)ex,(int)ey);
if(es==EDGE_ACCEPTED){
    SDL_RenderDrawLine(rnd,(int)sx+1,(int)sy,(int)ex+1,(int)ey);
    SDL_RenderDrawLine(rnd,(int)sx,(int)sy+1,(int)ex,(int)ey+1);
}

if(show_weight && weight>0){
    char buf[8];
    snprintf(buf,sizeof(buf),"%d",weight);
    int mx=(int)((sx+ex)/2), my=(int)((sy+ey)/2);
    int ox=(int)(-uy*12), oy=(int)(ux*12);
    
    SDL_SetRenderDrawBlendMode(rnd,SDL_BLENDMODE_BLEND);
    setcol(rnd,240,244,248,200);
    SDL_Rect bg={mx+ox-10,my+oy-8,20,16};
    SDL_RenderFillRect(rnd,&bg);
    SDL_SetRenderDrawBlendMode(rnd,SDL_BLENDMODE_NONE);

    Uint8 tr,tg,tb;
    switch(es){
        case EDGE_CURRENT:  tr=180;tg=130;tb=0;   break;
        case EDGE_ACCEPTED: tr=30; tg=140;tb=60;  break;
        case EDGE_REJECTED: tr=180;tg=40; tb=40;  break;
        default:            tr=90; tg=90; tb=100; break;
    }
    txt_c(rnd,font12,buf,mx+ox,my+oy,tr,tg,tb);
}
}

void draw_graph_panel(SDL_Renderer *rnd) {
    int i,j;

    txt_c(rnd,font19,"Зважений граф",GCX,30,30,30,30);

    for(i=0;i<N;i++){
        for(j=i+1;j<N;j++){
            if(W[i][j]==0) continue;
            draw_undir_edge(rnd,gpos[i],gpos[j],W[i][j],estate[i][j],1);
        }
    }

    for(i=0;i<N;i++){
        int cx=(int)gpos[i].x, cy=(int)gpos[i].y;

        SDL_SetRenderDrawBlendMode(rnd,SDL_BLENDMODE_BLEND);
        setcol(rnd,77,166,255,50);
        fill_circ(rnd,cx,cy,VRAD+7);
        SDL_SetRenderDrawBlendMode(rnd,SDL_BLENDMODE_NONE);

        setcol(rnd,77,166,255,255);
        fill_circ(rnd,cx,cy,VRAD);
        setcol(rnd,43,125,233,255);
        draw_circ(rnd,cx,cy,VRAD);
        draw_circ(rnd,cx,cy,VRAD-1);

        char buf[4];
        snprintf(buf,sizeof(buf),"%d",i+1);
        txt_c(rnd,font12,buf,cx,cy,255,255,255);
    }
}

void draw_spanning_panel(SDL_Renderer *rnd) {
    txt_c(rnd,font19,"Мінімальний кістяк (Краскал)",TCX,30,30,30,30);

    if(!span_list){
        txt_c(rnd,font12,"(ще порожній)",TCX,70,150,150,150);
    }

    ENode *e=span_list;
    while(e){
        draw_undir_edge(rnd,tpos[e->u],tpos[e->v],e->w,EDGE_ACCEPTED,1);
        e=e->next;
    }

    if(current_u>=0 && current_v>=0 && estate[current_u][current_v]==EDGE_CURRENT){
        draw_undir_edge(rnd,tpos[current_u],tpos[current_v],
                        W[current_u][current_v],EDGE_CURRENT,1);
    }

    int i;
    for(i=0;i<N;i++){
        int cx=(int)tpos[i].x, cy=(int)tpos[i].y;

        int in_span=0;
        ENode *se=span_list;
        while(se){ if(se->u==i||se->v==i){in_span=1;break;} se=se->next; }

        SDL_SetRenderDrawBlendMode(rnd,SDL_BLENDMODE_BLEND);
        setcol(rnd,in_span?50:77, in_span?180:166, in_span?80:255, 50);
        fill_circ(rnd,cx,cy,VRAD+7);
        SDL_SetRenderDrawBlendMode(rnd,SDL_BLENDMODE_NONE);

        if(in_span) setcol(rnd,50,180,80,255);
        else        setcol(rnd,77,166,255,255);
        fill_circ(rnd,cx,cy,VRAD);

        if(in_span) setcol(rnd,30,130,50,255);
        else        setcol(rnd,43,125,233,255);
        draw_circ(rnd,cx,cy,VRAD);
        draw_circ(rnd,cx,cy,VRAD-1);

        char buf[4];
        snprintf(buf,sizeof(buf),"%d",i+1);
        txt_c(rnd,font12,buf,cx,cy,255,255,255);
    }

    if(algo_done){
        int total=0;
        ENode *se=span_list;
        while(se){total+=se->w;se=se->next;}
        char info[48];
        snprintf(info,sizeof(info),"Загальна вага: %d",total);
        txt_c(rnd,font15,info,TCX,WIN_H-50,50,150,70);
    }
}

void draw_log(SDL_Renderer *rnd){
    int MAX=8;
    int start=log_len-MAX; if(start<0)start=0;
    int y=WIN_H-20-MAX*17;
    int i;
    for(i=start;i<log_len;i++){
        Uint8 a=(i==log_len-1)?220:110;
        txt_l(rnd,font12,log_buf[i],10,y,a,a,a);
        y+=17;
    }
}

void draw_hud(SDL_Renderer *rnd){
    setcol(rnd,200,210,220,255);
    SDL_RenderDrawLine(rnd,WIN_W/2,10,WIN_W/2,WIN_H-10);
    SDL_RenderDrawLine(rnd,WIN_W/2+1,10,WIN_W/2+1,WIN_H-10);

    struct{Uint8 r,g,b;const char*lbl;} leg[]={
        {240,190, 20,"поточне ребро"},
        { 50,180, 80,"додано до кістяка"},
        {220, 60, 60,"відхилено (цикл)"},
        {160,165,175,"не розглянуте"},
    };
    int li, lx=10, ly=WIN_H-20-8*17-80;
    for(li=0;li<4;li++){
        setcol(rnd,leg[li].r,leg[li].g,leg[li].b,255);
        SDL_Rect rc={lx,ly-7,14,14};
        SDL_RenderFillRect(rnd,&rc);
        txt_l(rnd,font12,leg[li].lbl,lx+18,ly,60,60,60);
        ly+=19;
    }

    char info[64];
    snprintf(info,sizeof(info),"Крок: %d / %d   Ребер у кістяку: %d / %d",
        kruskal_step_idx, num_edges, span_edge_count, N-1);
    txt_c(rnd,font12,info,WIN_W/2,WIN_H-8,100,100,100);

    txt_c(rnd,font12,
        "SPACE / → / Enter — крок     R — скинути     ESC — вийти",
        WIN_W/2,WIN_H-22,140,140,140);

    if(algo_done)
        txt_c(rnd,font15,"Алгоритм завершено!",WIN_W/2,WIN_H-40,50,150,70);
}

void calc_positions(void){
    int i;
    for(i=0;i<N-1;i++){
        float a=(float)(2.0*M_PI*i)/(N-1)-(float)(M_PI/2.0);
        gpos[i].x=GCX+GR*cosf(a); gpos[i].y=GCY+GR*sinf(a);
        tpos[i].x=TCX+TR*cosf(a); tpos[i].y=TCY+TR*sinf(a);
    }
    gpos[N-1].x=GCX; gpos[N-1].y=GCY;
    tpos[N-1].x=TCX; tpos[N-1].y=TCY;
}

int main(void){
    build_matrices();
    calc_positions();
    build_lists();
    reset_kruskal();
    print_console();

    if(SDL_Init(SDL_INIT_VIDEO)!=0){
        fprintf(stderr,"SDL_Init: %s\n",SDL_GetError()); return 1;
    }
    if(TTF_Init()!=0)
        fprintf(stderr,"TTF_Init: %s\n",TTF_GetError());

    const char *fp[]={
        "/opt/homebrew/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/Library/Fonts/Arial.ttf",
        "/System/Library/Fonts/Helvetica.ttc",
        "C:/Windows/Fonts/arial.ttf",
        NULL
    };
    int fi;
    for(fi=0;fp[fi]&&!font12;fi++){
        font12=TTF_OpenFont(fp[fi],12);
        font15=TTF_OpenFont(fp[fi],15);
        font19=TTF_OpenFont(fp[fi],19);
    }

    SDL_Window *win=SDL_CreateWindow(
        "Лабораторна робота №6 — Мінімальний кістяк (Краскал)",
        SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,
        WIN_W,WIN_H,SDL_WINDOW_SHOWN);
    if(!win){fprintf(stderr,"Window: %s\n",SDL_GetError());SDL_Quit();return 1;}
    SDL_RaiseWindow(win);

    SDL_Renderer *rnd=SDL_CreateRenderer(
        win,-1,SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
    if(!rnd){fprintf(stderr,"Renderer: %s\n",SDL_GetError());
             SDL_DestroyWindow(win);SDL_Quit();return 1;}

    int running=1;
    SDL_Event ev;

    while(running){
        while(SDL_PollEvent(&ev)){
            if(ev.type==SDL_QUIT) running=0;
            if(ev.type==SDL_KEYDOWN){
                switch(ev.key.keysym.sym){
                    case SDLK_ESCAPE: case SDLK_q: running=0; break;
                    case SDLK_SPACE: case SDLK_RETURN: case SDLK_RIGHT:
                        kruskal_step();
                        if(log_len>0) printf("%s\n",log_buf[log_len-1]);
                        break;
                    case SDLK_r:
                        reset_kruskal();
                        printf("\n--- Скинуто ---\n");
                        break;
                }
            }
        }

        SDL_SetRenderDrawColor(rnd,240,244,248,255);
        SDL_RenderClear(rnd);

        draw_graph_panel(rnd);
        draw_spanning_panel(rnd);
        draw_log(rnd);
        draw_hud(rnd);

        SDL_RenderPresent(rnd);
    }

    vlist_free(vlist);
    elist_free(elist);
    elist_free(span_list);
    if(font12) TTF_CloseFont(font12);
    if(font15) TTF_CloseFont(font15);
    if(font19) TTF_CloseFont(font19);
    TTF_Quit();
    SDL_DestroyRenderer(rnd);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}