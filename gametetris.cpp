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

// Перевірка, чи можна розташувати фігуру
bool check(vector<Point> &p) {
    for(auto &pt : p)
        if(pt.x < 0 || pt.x >= width || pt.y >= height || (pt.y >= 0 && field[pt.y][pt.x]))
            return false;
    return true;
}

// Очищення повних ліній
int clearLines() {
    int lines = 0;
    for(int y = height - 1; y >= 0; y--) {
        bool full = true;
        for(int x = 0; x < width; x++) {
            if(field[y][x] == 0) {
                full = false;
                break;
            }
        }
        if(full) {
            lines++;
            for(int yy = y; yy > 0; yy--)
                for(int x = 0; x < width; x++)
                    field[yy][x] = field[yy-1][x];
            for(int x = 0; x < width; x++) field[0][x] = 0;
            y++; // перевірити цю лінію знову
        }
    }
    return lines;
}

// Обертання фігури на місці
vector<Point> rotate(vector<Point> &p) {
    int cx = 0, cy = 0;
    for(auto &pt : p) { cx += pt.x; cy += pt.y; }
    cx /= p.size();
    cy /= p.size();

    vector<Point> r;
    for(auto &pt : p) {
        int x = pt.x - cx;
        int y = pt.y - cy;
        r.push_back({cx - y, cy + x}); 
    }
    return r;
}


void draw(vector<Point> &p, int score, int shadowY, int colorID) {
    clear();
    for(int y = 0; y < height; y++) {
        printw("|");
        for(int x = 0; x < width; x++) {
            bool printed = false;

            // Тінь
            for(auto &pt : p) {
                if(pt.x == x && pt.y + shadowY == y) {
                    attron(COLOR_PAIR(8));
                    printw("[]");
                    attroff(COLOR_PAIR(8));
                    printed = true;
                    break;
                }
            }

            // Фігура
            for(auto &pt : p) {
                if(pt.x == x && pt.y == y) {
                    attron(COLOR_PAIR(colorID));
                    printw("[]");
                    attroff(COLOR_PAIR(colorID));
                    printed = true;
                    break;
                }
            }

            if(!printed) printw(field[y][x] ? "[]" : "  ");
        }
        printw("|\n");
    }

    for(int i = 0; i < width * 2 + 2; i++) printw("-");
    printw("\nScore: %d\n", score);
    refresh();
}

int main() {
    srand(time(0));
    initscr();
    start_color();

    for(int i = 1; i <= 7; i++) init_pair(i, i, 0);
    init_pair(8, COLOR_MAGENTA, 0);
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
        int colorID = (figType % 7) + 1;

        vector<Point> p;
        for(int i = 0; i < fh; i++)
            for(int j = 0; j < fw; j++)
                if(fig[i][j]) p.push_back({width / 2 - fw / 2 + j, i});

        while(true) {
            // Тінь
            int shadowY = 0;
            vector<Point> shadow = p;
            while(check(shadow)) {
                for(auto &pt : shadow) pt.y++;
                shadowY++;
            }
            shadowY--;

            draw(p, score, shadowY, colorID);

            int ch = getch();
            vector<Point> pNext = p;

            if(ch == KEY_LEFT)
                for(auto &pt : pNext) pt.x--;
            if(ch == KEY_RIGHT)
                for(auto &pt : pNext) pt.x++;
            if(ch == KEY_DOWN) {
                while(check(pNext))
                    for(auto &pt : pNext) pt.y++;
                for(auto &pt : pNext) pt.y--;
                for(auto &pt : pNext)
                    if(pt.y >= 0) field[pt.y][pt.x] = 1;
                score += clearLines();
                break;
            }
            if(ch == KEY_UP) {
                auto pRot = rotate(p);
                if(check(pRot)) pNext = pRot;
            }

            if(check(pNext)) p = pNext;

            // Автоматичне падіння
            static int counter = 0;
            if(counter++ > 10) {
                counter = 0;
                for(auto &pt : p) pt.y++;
                if(!check(p)) {
                    for(auto &pt : p) pt.y--;
                    for(auto &pt : p)
                        if(pt.y >= 0) field[pt.y][pt.x] = 1;
                    score += clearLines();
                    break;
                }
            }

            usleep(50000);
        }

       
        for(int x = 0; x < width; x++) {
            if(field[0][x]) {
                draw(p, score, 0, colorID);
                printw("GAME OVER!\n");
                refresh();
                nodelay(stdscr, FALSE);
                getch();
                endwin();
                return 0;
            }
        }
    }

    endwin();
    return 0;
}
