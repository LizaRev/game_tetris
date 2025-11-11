#include <ncurses.h>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <unistd.h>

using namespace std;

const int width = 10;
const int height = 20;

int field[height][width] = {0};

// фигуры (T, I, O, L, J, S, Z)
vector<vector<vector<int>>> figures = {
    {{1,1,1},{0,1,0}}, {{1},{1},{1},{1}}, {{1,1},{1,1}},
    {{1,0,0},{1,1,1}}, {{0,0,1},{1,1,1}}, {{0,1,1},{1,1,0}}, {{1,1,0},{0,1,1}}
};

struct Point { int x, y; };

bool check(vector<Point> &p) {
    for(auto &pt : p)
        if(pt.x < 0 || pt.x >= width || pt.y >= height || (pt.y >= 0 && field[pt.y][pt.x]))
            return false;
    return true;
}

int clearLines() {
    int lines = 0;
    for(int y = height-1; y >=0; y--) {
        bool full = true;
        for(int x=0; x<width; x++) if(!field[y][x]) full = false;
        if(full) {
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

void draw(vector<Point> &p, int score) {
    clear();
    for(int y=0;y<height;y++){
        printw("|");
        for(int x=0;x<width;x++){
            bool printed = false;
            for(auto &pt: p)
                if(pt.x==x && pt.y==y){
                    printw("[]");
                    printed = true;
                    break;
                }
            if(!printed) printw(field[y][x]?"[]":"  ");
        }
        printw("|\n");
    }
    for(int i=0;i<width*2+2;i++) printw("-");
    printw("\nScore: %d\n", score);
    refresh();
}

int main() {
    srand(time(0));
    initscr();
    noecho();
    cbreak();
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);

    int score = 0;

    while(true) {
        int figType = rand() % figures.size();
        auto &fig = figures[figType];
        int fh = fig.size();
        int fw = fig[0].size();

        vector<Point> p;
        for(int i=0;i<fh;i++)
            for(int j=0;j<fw;j++)
                if(fig[i][j]) p.push_back({width/2-fw/2 + j, i});

        while(true) {
            // движение вниз
            for(auto &pt: p) pt.y++;
            if(!check(p)) {
                for(auto &pt: p) pt.y--;
                for(auto &pt: p) if(pt.y>=0) field[pt.y][pt.x] = 1;
                score += clearLines();
                break;
            }

            // управление
            int ch = getch();
            vector<Point> pNext = p;
            if(ch == 'a') for(auto &pt: pNext) pt.x--;
            if(ch == 'd') for(auto &pt: pNext) pt.x++;
            if(ch == 's') for(auto &pt: pNext) pt.y++;
            if(ch == 'w') {
                auto pRot = rotate(p, fw, fh);
                if(check(pRot)) pNext = pRot;
            }
            if(check(pNext)) p = pNext;

            draw(p, score);
            usleep(300000);
        }

        // Game Over
        for(int x=0;x<width;x++)
            if(field[0][x]){
                draw(p, score);
                printw("GAME OVER!\n");
                refresh();
                nodelay(stdscr, FALSE);
                getch();
                endwin();
                return 0;
            }
    }

    endwin();
    return 0;
}
