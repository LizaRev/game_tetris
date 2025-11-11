#include <ncurses.h>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <unistd.h>

using namespace std;

const int width = 10;
const int height = 20;

int field[height][width] = {0};

struct Point { int x, y; };


vector<vector<vector<int>>> figures = {
    {{1,1,1},{0,1,0}},   // T
    {{1},{1},{1},{1}},   // I
    {{1,1},{1,1}},       // O
    {{1,0,0},{1,1,1}},   // L
    {{0,0,1},{1,1,1}},   // J
    {{0,1,1},{1,1,0}},   // S
    {{1,1,0},{0,1,1}}    // Z
};

bool check(vector<Point> &p) {
    for(auto &pt : p)
        if(pt.x < 0 || pt.x >= width || pt.y >= height || (pt.y >=0 && field[pt.y][pt.x]))
            return false;
    return true;
}

int clearLines() {
    int lines = 0;
    for(int y=height-1;y>=0;y--){
        bool full=true;
        for(int x=0;x<width;x++) if(!field[y][x]) full=false;
        if(full){
            lines++;
            for(int i=y;i>0;i--) for(int j=0;j<width;j++) field[i][j]=field[i-1][j];
            for(int j=0;j<width;j++) field[0][j]=0;
            y++;
        }
    }
    return lines;
}

vector<Point> rotate(vector<Point> &p, int fw, int fh) {
    vector<Point> r;
    for(auto &pt : p) r.push_back({pt.y, fw-1-pt.x});
    return r;
}

void draw(vector<Point> &p, int score, int shadowY) {
    clear();
    for(int y=0;y<height;y++){
        printw("|");
        for(int x=0;x<width;x++){
            bool printed=false;
          
            for(auto &pt: p){
                if(pt.x==x && pt.y+shadowY==y){
                    attron(COLOR_PAIR(8));
                    printw("[]");
                    attroff(COLOR_PAIR(8));
                    printed=true;
                    break;
                }
            }
            
            for(auto &pt: p){
                if(pt.x == x && pt.y == y){
                    attron(COLOR_PAIR(1+(rand() % 7)));
                    printw("[]");
                    attroff(COLOR_PAIR(1));
                    printed=true;
                    break;
                }
            }
            // поле
            if(!printed) printw(field[y][x]?"[]":"  ");
        }
        printw("|\n");
    }
    for(int i=0;i<width*2+2;i++) printw("-");
    printw("\nScore: %d\n", score);
    refresh();
}

int main(){
    srand(time(0));
    initscr();
    start_color();
    for(int i=1;i<=7;i++) init_pair(i,i,0);
    init_pair(8,COLOR_MAGENTA,0); // тінь
    noecho();
    cbreak();
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);

    int score=0;

    while(true){
        int figType=rand()%figures.size();
        auto &fig=figures[figType];
        int fh=fig.size();
        int fw=fig[0].size();

        vector<Point> p;
        for(int i=0;i<fh;i++)
            for(int j=0;j<fw;j++)
                if(fig[i][j]) p.push_back({width/2-fw/2+j, i});

        while(true){
            // тінь
            int shadowY=0;
            vector<Point> shadow=p;
            while(check(shadow)){
                for(auto &pt: shadow) pt.y++;
                shadowY++;
            }
            shadowY--;

            draw(p, score, shadowY);

            int ch=getch();
            vector<Point> pNext=p;
            if(ch==KEY_LEFT) for(auto &pt: pNext) pt.x--;
            if(ch==KEY_RIGHT) for(auto &pt: pNext) pt.x++;
            if(ch==KEY_DOWN) for(auto &pt: pNext) pt.y++;
            if(ch==KEY_UP){
                auto pRot=rotate(p,fw,fh);
                if(check(pRot)) pNext=pRot;
            }

            if(check(pNext)) p=pNext;

            // вниз
            static int counter=0;
            if(counter++>10){
                counter=0;
                for(auto &pt: p) pt.y++;
                if(!check(p)){
                    for(auto &pt: p) pt.y--;
                    for(auto &pt: p) if(pt.y>=0) field[pt.y][pt.x]=1;
                    score+=clearLines();
                    break;
                }
            }
            usleep(50000);
        }

        
        for(int x=0;x<width;x++)
            if(field[0][x]){
                draw(p, score, 0);
                printw("GAME OVER!\n");
                refresh();
                nodelay(stdscr,FALSE);
                getch();
                endwin();
                return 0;
            }
    }

    endwin();
    return 0;
}


        



            



    



    







