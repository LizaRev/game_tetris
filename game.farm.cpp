#include <ncurses.h>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
using namespace std;

const int WIDTH = 40;
const int HEIGHT = 20;

struct Cell {
    bool isTree = false;
    int treeType = 0; 
    int appleStage = 0; 
};

vector<vector<Cell>> field(HEIGHT, vector<Cell>(WIDTH));
int score = 0;

struct Player { int x, y; } player;


void initField() {
    srand(time(0));

    int gridSizeX = 8; 
    int gridSizeY = 6; 

    for (int i = 2; i < HEIGHT-3; i += gridSizeY) {
        for (int j = 2; j < WIDTH-3; j += gridSizeX) {
            int offsetX = rand() % 3; 
            int offsetY = rand() % 3;
            int x = min(j + offsetX, WIDTH-3);
            int y = min(i + offsetY, HEIGHT-4);
            field[y][x].isTree = true;
            field[y][x].treeType = (rand() % 2) + 1;
            field[y][x].appleStage = rand() % 3;
        }
    }

    player = {WIDTH / 2, HEIGHT / 2};
}


void drawTreeType1(int x, int y, int glowPhase, int appleStage){
    if(y-1>=0 && appleStage>0){
        move(y-1, x);
        if(appleStage==1) attron(COLOR_PAIR(4));
        else if(appleStage==2){ if(glowPhase%2==0) attron(A_BOLD); attron(COLOR_PAIR(5)); }
        addch(appleStage==2?'O':'o');
        attroff(A_BOLD); attroff(COLOR_PAIR(4)); attroff(COLOR_PAIR(5));
    }
    move(y, x);     attron(COLOR_PAIR(3)); addch('^'); attroff(COLOR_PAIR(3));
    move(y+1, x-1); attron(COLOR_PAIR(3)); addch('^'); attroff(COLOR_PAIR(3));
    move(y+1, x+1); attron(COLOR_PAIR(3)); addch('^'); attroff(COLOR_PAIR(3));
    move(y+2, x-2); attron(COLOR_PAIR(3)); addch('^'); attroff(COLOR_PAIR(3));
    move(y+2, x);   attron(COLOR_PAIR(3)); addch('^'); attroff(COLOR_PAIR(3));
    move(y+2, x+2); attron(COLOR_PAIR(3)); addch('^'); attroff(COLOR_PAIR(3));
    move(y+3, x);   attron(COLOR_PAIR(2)); addch('|'); attroff(COLOR_PAIR(2));
}


void drawTreeType2(int x, int y, int glowPhase, int appleStage){
    if(y-1>=0 && appleStage>0){
        move(y-1, x);
        if(appleStage==1) attron(COLOR_PAIR(4));
        else if(appleStage==2){ if(glowPhase%2==0) attron(A_BOLD); attron(COLOR_PAIR(5)); }
        addch(appleStage==2?'O':'o');
        attroff(A_BOLD); attroff(COLOR_PAIR(4)); attroff(COLOR_PAIR(5));
    }
    move(y, x);     attron(COLOR_PAIR(3)); addch('^'); attroff(COLOR_PAIR(3));
    move(y+1, x);   attron(COLOR_PAIR(3)); addch('^'); attroff(COLOR_PAIR(3));
    move(y+2, x);   attron(COLOR_PAIR(2)); addch('|'); attroff(COLOR_PAIR(2));
}


void drawField(int glowPhase){
    clear();
    
    for(int i=0;i<HEIGHT;i++){
        for(int j=0;j<WIDTH;j++){
            addch('.');
        }
        addch('\n');
    }

    
    for(int i=0;i<HEIGHT;i++){
        for(int j=0;j<WIDTH;j++){
            if(field[i][j].isTree){
                if(field[i][j].treeType==1) drawTreeType1(j,i,glowPhase,field[i][j].appleStage);
                else drawTreeType2(j,i,glowPhase,field[i][j].appleStage);
            }
        }
    }

 
    move(player.y, player.x);
    attron(COLOR_PAIR(1));
    addch('@');
    attroff(COLOR_PAIR(1));

    mvprintw(HEIGHT+1,0,"Score: %d",score);
    mvprintw(HEIGHT+2,0,"Use WASD to move, Q to quit.");
    refresh();
}


void growApples(){
    static int growCounter = 0;
    growCounter++;
    if(growCounter>=3){ 
        for(int i=1;i<HEIGHT-1;i++){
            for(int j=1;j<WIDTH-1;j++){
                if(field[i][j].isTree && field[i][j].appleStage<2){
                    if(rand()%2==0) field[i][j].appleStage++; 
                }
            }
        }
        growCounter = 0;
    }
}


void movePlayer(int dx,int dy){
    int nx=player.x+dx, ny=player.y+dy;
    if(nx<0||nx>=WIDTH||ny<0||ny>=HEIGHT) return;

    for(int i=-1;i<=1;i++){
        for(int j=-1;j<=1;j++){
            int tx=nx+j, ty=ny+i;
            if(tx>=0 && tx<WIDTH && ty>=0 && ty<HEIGHT){
                if(field[ty][tx].isTree && field[ty][tx].appleStage==2){
                    score++;
                    field[ty][tx].appleStage=0;
                }
            }
        }
    }

    player.x=nx;
    player.y=ny;
}

int main(){
    initscr(); noecho(); curs_set(FALSE);
    keypad(stdscr,TRUE); nodelay(stdscr,TRUE);
    start_color();

  
    init_pair(1,COLOR_CYAN,COLOR_BLACK);   
    init_pair(2,COLOR_YELLOW,COLOR_BLACK); 
    init_pair(3,COLOR_GREEN,COLOR_BLACK); 
    init_pair(4,COLOR_YELLOW,COLOR_BLACK); 
    init_pair(5,COLOR_RED,COLOR_BLACK);    

    initField();
    int glowPhase=0;

    while(true){
        drawField(glowPhase);
        growApples();
        glowPhase=(glowPhase+1)%4;

        int ch=getch();
        switch(ch){
            case 'w': case 'W': movePlayer(0,-1); break;
            case 's': case 'S': movePlayer(0,1); break;
            case 'a': case 'A': movePlayer(-1,0); break;
            case 'd': case 'D': movePlayer(1,0); break;
            case 'q': case 'Q': endwin(); return 0;
        }
        usleep(200000);
    }

    endwin();
    return 0;
}

 


