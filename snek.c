
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>

#define WIDTH 60
#define HEIGHT 20

#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[38;5;34m"
#define BLUE "\033[38;5;39m" // or 21, 26, 51

#define BG_GREEN "\033[48;5;28m" // or 34, 22
#define BG_WHITE "\033[48;5;15m"

#define BACKGROUND "\033[92m"
#define BORDER "\033[38;5;22m"

#define INVERT "\033[7m"
#define BLOCK "\u2588"
#define HALF_BLOCK "\u2591"

typedef struct {
    int x;
    int y;
} Point;

typedef struct {
    Point body[WIDTH * HEIGHT];
    int dir;
    int length;
} Snake;

typedef struct {
    int x;
    int y;
} Apple;

typedef struct {
    char name[4];
    int highScore;
} Player;

int maxPlayers = 10;
Player players[10];

int score = 0;
int alive = 1;
int exitGame = 1;

int level = 1;
int levelDelay[9] = {300, 250, 200, 160, 120, 90, 70, 50, 30};

int selectedMap = 0;
Point walls[500];
int wallCount = 0;

void setCursorPos(int x,  int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void initScreen() {
    printf(BORDER);
    system("cls");
    printf(RESET);
    int y;
    for (y = 0; y < HEIGHT; y++) {
        int x;
        for (x = 0; x < WIDTH; x++) {
            if ((x == 0 || x == WIDTH - 1) && (y == 0 || y == HEIGHT - 1)) {
                printf(BORDER BLOCK RESET);
            } else if (y == 0 || y == HEIGHT - 1 || x == 0 || x == WIDTH - 1) {
                printf(BORDER BLOCK RESET);
            } else {
                printf(BACKGROUND BLOCK RESET);
            }
        }
        if (y != HEIGHT - 1) printf("\n");
    }
    int i;
    for (i = 0; i < wallCount; i++) {
        setCursorPos(walls[i].x, walls[i].y);
        printf(BORDER BLOCK RESET);
    }
}

void hideCursor() {
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = FALSE;
    SetConsoleCursorInfo(consoleHandle, &info);
}

void restoreCursor() {
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = TRUE;
    SetConsoleCursorInfo(consoleHandle, &info);
}

void spawnApple(Apple* apple, Snake* snake) {
    int validSpawn = 0;
    while (validSpawn == 0) {
        validSpawn = 1;
        apple->x = (rand() % (WIDTH - 2) + 1);
        apple->y = (rand() % (HEIGHT - 2) + 1);
        int i;
        for (i = 0; i < snake->length; i++) {
            if ((apple->x == snake->body[i].x) && (apple->y == snake->body[i].y)) {
                validSpawn = 0;
                break;
            }
        }
        for (i = 0; i < wallCount; i++) {
            if ((apple->x == walls[i].x) && (apple->y == walls[i].y)) {
                validSpawn = 0;
                break;
            }
        }
    }
    setCursorPos(apple->x, apple->y);
    printf(RED BLOCK RESET);
    setCursorPos(0, 0);
}

void initSnake(Snake* snake) {
    memset(snake->body, 0, sizeof(snake->body));
    snake->dir = 1;
    snake->length = 3;
    int i;
    for (i = 0; i < snake->length; i++) {
        snake->body[i].x = 1;
        snake->body[i].y = HEIGHT / 2;
    }
}

void moveSnake(Snake* snake) {
    setCursorPos(snake->body[snake->length - 1].x, snake->body[snake->length - 1].y);
    printf(BACKGROUND BLOCK RESET);
    int i;
    for (i = snake->length - 1; i > 0; i--) {
        snake->body[i] = snake->body[i - 1];
    }
    switch (snake->dir) {
        case 1: snake->body[0].x++; break;
        case 2: snake->body[0].y--; break;
        case 3: snake->body[0].x--; break;
        case 4: snake->body[0].y++; break;
    }
}

void checkCollision(Snake* snake, Apple* apple) {
    if ((snake->body[0].x == apple->x) && (snake->body[0].y == apple->y)) {

        snake->body[snake->length].x = snake->body[snake->length - 1].x;
        snake->body[snake->length].y = snake->body[snake->length - 1].y;

        snake->length++;
        score += ((snake->length - 3) * 10) * level * (selectedMap + 1);
        spawnApple(apple, snake);
    } else if (snake->body[0].y == 0 || snake->body[0].y == HEIGHT - 1 || snake->body[0].x == 0 || snake->body[0].x == WIDTH - 1) {
        alive = 0;
    } else {
        int i;
        for (i = 1; i < snake->length; i++) {
            if ((snake->body[0].x == snake->body[i].x) && (snake->body[0].y == snake->body[i].y)) {
                alive = 0;
                break;
            }
        }
        for (i = 0; i < wallCount; i++) {
            if ((snake->body[0].x == walls[i].x) && (snake->body[0].y == walls[i].y)) {
                alive = 0;
                break;
            }
        }
    }
}

int chooseLevel() {
    int choosing = 1;
    int levelSelected = level;
    int selected = 1;
    int numOptions = 9;
    system("cls");
    while (choosing) {
        setCursorPos(WIDTH / 2 - 12, 5);
        printf(GREEN "==== CHOOSE A LEVEL ====" RESET);
        setCursorPos(WIDTH / 2 - 12, 7);
        int i;
        if (selected == 1) {
            printf(BG_GREEN "[");
            for (i = 0; i < numOptions; i++) {
                if (levelSelected - i > 0) printf(BLOCK);
                else printf(HALF_BLOCK);
            }
            printf("]");
            printf("    Level: %d" RESET, levelSelected);
            setCursorPos(WIDTH / 2 - 12, 9);
            printf("[ Return To Main Menu ]");
        } else {
            printf("[");
            for (i = 0; i < numOptions; i++) {
                if (levelSelected - i > 0) printf(BLOCK);
                else printf(HALF_BLOCK);
            }
            printf("]");
            printf("    Level: %d", levelSelected);
            setCursorPos(WIDTH / 2 - 12, 9);
            printf(BG_GREEN "[ Return To Main Menu ]" RESET);
        }
        int ch = _getch();
        if (ch == 0 || ch == 224) {
            ch = _getch();
            if (selected == 1 && ch == 77) {
                levelSelected++;
                if (levelSelected > 9) levelSelected = 9;
            }
            else if (selected == 1 && ch == 75) {
                levelSelected--;
                if (levelSelected < 1) levelSelected = 1;
            }
            else if (ch == 80 || ch == 72) {
                if (selected == 1) selected = 2;
                else selected = 1;
            }
        }
        else {
            if (selected == 1 && (ch == 'd' || ch == 'D')) {
                levelSelected++;
                if (levelSelected > 9) levelSelected = 9;
            }
            else if (selected == 1 && (ch == 'a' || ch == 'A')) {
                levelSelected--;
                if (levelSelected < 1) levelSelected = 1;
            }
            else if (selected == 3 && (ch == 'w' || ch == 'W' || ch == 's' || ch == 'S')) {
                if (selected == 1) selected = 2;
                else selected = 1;
            }
            else if (ch == 13) {
                if (selected == 1) choosing = 0;
                else if (selected == 2) return 0;
            }
            else if (ch == 27) {
                return 0;
            }
        }
    }
    level = levelSelected;
    return 1;
}

void helperMenu() {
    system("cls");
    
    setCursorPos(WIDTH / 2 - 10, 3);
    printf(GREEN "==== HOW TO PLAY ====" RESET);
    
    setCursorPos(WIDTH / 2 - 20, 6);
    printf(INVERT " CONTROLS " RESET);
    setCursorPos(WIDTH / 2 - 20, 8);
    printf("Use " BLUE "W A S D" RESET " or " BLUE "Arrow Keys" RESET " to change directions.");

    setCursorPos(WIDTH / 2 - 20, 11);
    printf(INVERT " OBJECTIVE & RULES " RESET);
    setCursorPos(WIDTH / 2 - 20, 13);
    printf("1. Eat the " RED BLOCK RESET " " RED "Apples" RESET " to grow and score points.");
    setCursorPos(WIDTH / 2 - 20, 14);
    printf("2. Do " RED "NOT" RESET " hit the " BORDER BLOCK RESET " " BORDER BG_WHITE INVERT "walls" RESET " or your own " GREEN BLOCK RESET " " GREEN "tail." RESET);
    setCursorPos(WIDTH / 2 - 20, 15);
    printf("3. The faster the speed, the higher your score will be!");

    Sleep(1500);

    setCursorPos(WIDTH / 2 - 13, 18);
    printf("Press any key to return...");
    
    while (_kbhit()) _getch();
    _getch();
}

void loadMap() {
    wallCount = 0;
    if (selectedMap == 0) {
        
    }
    else if (selectedMap == 1) {
        int x;
        for (x = 20; x < WIDTH - 20; x++) {
            walls[wallCount].x = x;
            walls[wallCount].y = HEIGHT / 2;
            wallCount++;
        }
    }
    else if (selectedMap == 2) {
        int x;
        for (x = 15; x < WIDTH - 15; x++) {
            walls[wallCount].x = x;
            walls[wallCount].y = HEIGHT / 3;
            wallCount++;

            walls[wallCount].x = x;
            walls[wallCount].y = (HEIGHT / 3) * 2;
            wallCount++;
        }
    }
    else if (selectedMap == 3) {
        int x;
        for (x = 12; x < 48; x++) {
            if (x < 26 || x > 33) {
                walls[wallCount].x = x;
                walls[wallCount].y = 10;
                wallCount++;
            }
        }
        int y;
        for (y = 3; y < 17; y++) {
            if (y < 18 || y > 11) {
                walls[wallCount].x = 30;
                walls[wallCount].y = y;
                wallCount++;
            }
        }
    }
}

void chooseMap() {
    int choosing = 1;
    Point selected;
    selected.x = 1;
    selected.y = 1;
    if (selectedMap == 0)  {
        selected.x = 1;
        selected.y = 1;
    }
    else if (selectedMap == 1) {
        selected.x = 2;
        selected.y = 1;
    }
    else if (selectedMap == 2) {
        selected.x = 1;
        selected.y = 2;
    }
    else if (selectedMap == 3) {
        selected.x = 2;
        selected.y = 2;
    }
    int numOptions = 4;
    system("cls");
    while (choosing) {
        setCursorPos(WIDTH / 2 - 11, 5);
        printf(GREEN "==== CHOOSE A MAP ====" RESET);
        
        setCursorPos(WIDTH / 3 - 5, 8);
        if ((selected.x == 1) && (selected.y == 1)) printf(GREEN BLOCK RESET);
        else printf(" ");
        printf(" Normal");
        
        setCursorPos((WIDTH / 3) * 2 - 4, 8);
        if ((selected.x == 2) && (selected.y == 1)) printf(GREEN BLOCK RESET);
        else printf(" ");
        printf(" Divider");
        
        setCursorPos(WIDTH / 3 - 5, 11);
        if ((selected.x == 1) && (selected.y == 2)) printf(GREEN BLOCK RESET);
        else printf(" ");
        printf(" Corridors");
        
        setCursorPos((WIDTH / 3) * 2 - 4, 11);
        if ((selected.x == 2) && (selected.y == 2)) printf(GREEN BLOCK RESET);
        else printf(" ");
        printf(" The Cross");

        int ch = _getch();
        if (ch == 0 || ch == 224) {
            ch = _getch();
            if (ch == 77 || ch == 75) {
                if (selected.x == 1) selected.x = 2;
                else selected.x = 1;
            }
            else if (ch == 80 || ch == 72) {
                if (selected.y == 1) selected.y = 2;
                else selected.y = 1;
            }
        }
        else {
            if (ch == 'd' || ch == 'D' || ch == 'a' || ch == 'A') {
                if (selected.x == 1) selected.x = 2;
                else selected.x = 1;
            }
            else if (ch == 'w' || ch == 'W' || ch == 's' || ch == 'S') {
                if (selected.y == 1) selected.y = 2;
                else selected.y = 1;
            }
            else if (ch == 13) {
                if ((selected.x == 1) && (selected.y == 1)) selectedMap = 0;
                if ((selected.x == 2) && (selected.y == 1)) selectedMap = 1;
                if ((selected.x == 1) && (selected.y == 2)) selectedMap = 2;
                if ((selected.x == 2) && (selected.y == 2)) selectedMap = 3;
                break;
            }
        }
    }
}

void showLeaderboard() {
    setCursorPos(WIDTH / 2 - 10, 4);
    printf(GREEN "==== LEADERBOARD ====" RESET);
    
    int i;
    for (i = 0; i < maxPlayers; i++) {
        setCursorPos(WIDTH / 2 - 5, 6 + i);
        printf("%3s - %d", players[i].name, players[i].highScore);
    }
    
    Sleep(1500);
    
    setCursorPos(WIDTH / 2 - 13, 18);
    printf("Press any key to return...");
    
    while (_kbhit()) _getch();
    _getch();
}

void menu() {
    int inMenu = 1;
    int selected = 0;
    int numOptions = 5;
    char* options[] = {"Play Game", "Choose Map", "Leaderboard", "How to Play", "Exit Game"};
    while (inMenu) {
        int choosing = 1;
        int spacesNeeded = 0;
        int i;
        for (i = 0; i < numOptions; i++) {
            if (strlen(options[i]) > spacesNeeded) {
                spacesNeeded = strlen(options[i]);
            }
        }
        spacesNeeded += 4;
        system("cls");
        while (choosing) {
            setCursorPos(WIDTH / 2 - 7, 5);
            printf(GREEN "==== SNAKE ====" RESET);
            for (i = 0; i < numOptions; i++) {
                int startX = (WIDTH / 2) - (spacesNeeded / 2);
                setCursorPos(startX, i + 8);
                int wordLen = strlen(options[i]);
                int leftPad = (spacesNeeded - wordLen) / 2;
                int rightPad = spacesNeeded - wordLen - leftPad;
                if (i == selected) {
                    printf(INVERT "%*s%s%*s" RESET, leftPad, "", options[i], rightPad, "");
                } else {
                    printf("%*s%s%*s", leftPad, "", options[i], rightPad, "");
                }
            }
            int ch = _getch();
            if (ch == 0 || ch == 224) {
                ch = _getch();
                if (ch == 72) {
                    selected--;
                    if (selected < 0) selected = numOptions - 1;
                } else if (ch == 80) {
                    selected++;
                    if (selected >= numOptions) selected = 0;
                }
            } else {
                if (ch == 'w' || ch == 'W') {
                    selected--;
                    if (selected < 0) selected = numOptions - 1;
                } else if (ch == 's' || ch == 'S') {
                    selected++;
                    if (selected >= numOptions) selected = 0;
                } else if (ch == 13) {
                    choosing = 0;
                }
            }
        }
        system("cls");
        switch (selected) {
            case 0: 
            if (chooseLevel() == 1) {
                inMenu = 0;
            }
            break;
            case 1: chooseMap(); break;
            case 2: showLeaderboard(); break;
            case 3: helperMenu(); break;
            case 4: exitGame = 0; inMenu = 0; break;
        }
    }
}

void getName() {
    setCursorPos(WIDTH / 2 - 12, 5);
    printf(GREEN "==== NEW HIGH SCORE ====" RESET);
    
    setCursorPos(WIDTH / 2 - 10, 8);
    printf("Enter Your Initials:");
    setCursorPos(WIDTH / 2 - 3, 11);
    printf("> ");
    
    char name[4];
    int naming = 1;
    int letters = 0;
    memset(name, '_', 4);
    name[3] = '\0';
    
    while (naming) {
        int i;
        setCursorPos(WIDTH / 2 - 1, 11);
        for (i = 0; i < 3; i++) printf("%c", name[i]);
        int ch = _getch();
        if (ch == 13) { // enter
            if (letters == 3) break;
        }
        else if (ch == 8) { // backspace
            if (letters > 0) {
                letters--;
                name[letters] = '_';
            }
        }
        else if (isalpha(ch)) {
            if (letters < 3) {
                name[letters] = toupper(ch);
                letters++;
            }
        }
    }
    strcpy(players[maxPlayers - 1].name, name);
}

void swap(Player *x, Player *y) {
    Player temp = *x;
    *x = *y;
    *y = temp;
}

void sortArray(Player* player, int playerCount) {
    int i;
    int flag = 1;
    while (flag) {
        flag = 0;
        for (i = 0; i < playerCount - 1; i++) {
            if (player[i].highScore < player[i + 1].highScore) {
                swap(&player[i], &player[i + 1]);
                flag = 1;
            }
        }
    }
}

void saveHighScore() {
    FILE *file = fopen("C://Tools//snakeHS.txt", "w");
    if (file != NULL) {
        int i;
        for (i = 0; i < maxPlayers; i++) fprintf(file, "%s %d\n", players[i].name, players[i].highScore);
        fclose(file);
    }
}

void loadHighScore() {
    FILE *file = fopen("C://Tools//snakeHS.txt", "r");
    if (file == NULL) {
        FILE *refile = fopen("C://Tools//snakeHS.txt", "w");
        int i;
        for (i = 0; i < maxPlayers; i++) fprintf(refile, "CPU 67\n");
        fclose(refile);

        for (i = 0; i < maxPlayers; i++) {
            strcpy(players[i].name, "CPU");
            players[i].highScore = 67;
        }
    }
    else {
        int i;
        for (i = 0; i < maxPlayers; i++) {
            if (fscanf(file, "%3s %d", players[i].name, &players[i].highScore) != 2) {
                break;
            }
        }
        fclose(file);
    }
}

int main(int argc, char const *argv[]) {
    SetConsoleOutputCP(CP_UTF8);
    srand(time(NULL));
    hideCursor();
    printf("\033[8;%d;%dt", HEIGHT, WIDTH);
    loadHighScore();
    while (exitGame) {
        menu();
        if (exitGame == 0) break;
        
        alive = 1;
        score = 0;
        loadMap();
        initScreen();
        Apple apple;
        Snake snake;
        initSnake(&snake);
        spawnApple(&apple, &snake);
        while (alive) {
            if (_kbhit()) {
                int ch = _getch();
                if (ch == 0 || ch == 224) {
                    ch = _getch();
                    switch (ch) {
                        case 77: if ((snake.dir != 3)) snake.dir = 1; break;
                        case 72: if ((snake.dir != 4)) snake.dir = 2; break;
                        case 75: if ((snake.dir != 1)) snake.dir = 3; break;
                        case 80: if ((snake.dir != 2)) snake.dir = 4; break;
                    }
                } else {
                    switch (ch) {
                        case 'd': case 'D': if ((snake.dir != 3)) snake.dir = 1; break;
                        case 'w': case 'W': if ((snake.dir != 4)) snake.dir = 2; break;
                        case 'a': case 'A': if ((snake.dir != 1)) snake.dir = 3; break;
                        case 's': case 'S': if ((snake.dir != 2)) snake.dir = 4; break;
                    }
                }
            }
            moveSnake(&snake);
            checkCollision(&snake, &apple);
            if (alive == 1) {
                setCursorPos(snake.body[0].x, snake.body[0].y);
                printf(GREEN BLOCK RESET);
                setCursorPos(0, 0);
            }
            Sleep(levelDelay[level - 1]);
        }
        
        setCursorPos(WIDTH / 2 - 9, HEIGHT / 2 - 4);
        printf(RED "G A M E  O V E R !" RESET);
        
        char scoreText[50];
        sprintf(scoreText, "S C O R E:  %d", score);
        int scoreLen = strlen(scoreText);
        int scoreX = (WIDTH / 2) - (scoreLen / 2);
        
        setCursorPos(scoreX, HEIGHT / 2 - 2);
        printf(BLUE "%s" RESET, scoreText);

        if (score > players[maxPlayers - 1].highScore) {
            players[maxPlayers - 1].highScore = score;
            Sleep(1500);
            while (_kbhit()) _getch();
            system("cls");
            getName();
            sortArray(players, maxPlayers);
            saveHighScore();
            setCursorPos(WIDTH / 2 - 4, 14);
            printf("SAVED!");
            Sleep(1000);
        }
        else {
            Sleep(1500);
            while (_kbhit()) _getch();
    
            setCursorPos(WIDTH / 2 - 12, HEIGHT / 2 + 2);
            printf("Press any key to exit...");
            
            _getch();
            while (_kbhit()) _getch();
        }
        system("cls");
    }
    restoreCursor();
    system("cls");
    return 0;
}