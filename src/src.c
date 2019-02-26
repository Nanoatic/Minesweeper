#include <SDL2/SDL.h>
//#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>


#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MARGINLEFT 10
#define MARGINRIGHT 20
#define R_BOUNDARY 16
#define C_BOUNDARY 30
#define MINES_DEPLOYED 99
#define SIZE_OF_BOX 16 // in pixels
#define WIDTH_TIMERBOX 13 //13
#define HIGHT_TIMERBOX  23 //23
#define WIDTH_SMILEY 25 //25
#define HIGHT_SMILEY 23 //23
#define MARGE_FACE 10
#define MARGE_COUNTER 10

static int minestomark = MINES_DEPLOYED;
static int firsttime = 1;
static int numrev = 0;
static int runs = 1;
static int Loss = 0;
static int Win = 0;
static int Counter1 = 0;
static int Counter2 = 0;
static int Counter3 = 0;
static int Counter4 = 0;
static int playonetimeloss = 0;
static int playonetimewin = 0;
static int onetimered = 0;
static int corruptedCol = 0;
static int corruptedRow = 0;

Mix_Chunk *Audexplo, *Audexpanding, *Audwinning, *Audsimpleclick, *Audmiddleclick;
SDL_Event event;
SDL_Texture *texture;
SDL_Renderer *renderer;
SDL_Window *window;
SDL_Surface *screen, *loadimg, *boxes[12], *numbers[10], *happy, *sad, *ow, *victor, *bombmiss, *bombred;
SDL_Rect boxesrec[12], numbersrec[10], happyrec, sadrec, owrec, victorrec, bombmissrec, bombredrec;


int t[R_BOUNDARY][C_BOUNDARY] = {0};
int marking[R_BOUNDARY][C_BOUNDARY] = {0};
char tmask[R_BOUNDARY][C_BOUNDARY];

void update() {
    SDL_UpdateTexture(texture, NULL, screen->pixels, screen->pitch);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);

}

Uint32 NanoCallbackfnc(Uint32 interval, void *param) {
    SDL_Rect rect;
    if (!Win && !Loss && firsttime) {
        if (Counter1 == 10) {
            Counter1 = 0;
            Counter2++;
            if (Counter2 == 10) {
                Counter2 = 0;
                Counter3++;
                if (Counter3 == 10) {
                    Counter3 = 0;
                    Counter4++;
                    if (Counter4 == 10)
                        return 0;
                }

            }


        }


        rect.x = MARGINLEFT;
        rect.y = screen->h - HIGHT_TIMERBOX - MARGE_COUNTER;
        SDL_BlitSurface(numbers[Counter4], NULL, screen, &rect);
        rect.x = MARGINLEFT + WIDTH_TIMERBOX;
        rect.y = screen->h - HIGHT_TIMERBOX - MARGE_COUNTER;
        SDL_BlitSurface(numbers[Counter3], NULL, screen, &rect);
        rect.x = MARGINLEFT + WIDTH_TIMERBOX * 2;
        rect.y = screen->h - HIGHT_TIMERBOX - MARGE_COUNTER;
        SDL_BlitSurface(numbers[Counter2], NULL, screen, &rect);
        rect.x = MARGINLEFT + WIDTH_TIMERBOX * 3;
        rect.y = screen->h - HIGHT_TIMERBOX - MARGE_COUNTER;
        SDL_BlitSurface(numbers[Counter1], NULL, screen, &rect);
        update();
        Counter1++;
    }
    return interval;
}

char numberTochar(int x) {

    switch (x) {
        case 0:
            return '0';
        case 1:
            return '1';
        case 2:
            return '2';
        case 3:
            return '3';
        case 4:
            return '4';
        case 5:
            return '5';
        case 6:
            return '6';
        case 7:
            return '7';
        case 8:
            return '8';
        default:
            break;
    }
    return 'x';
}

void initMaskField() {
    SDL_Rect rect;
    int i, j;
    for (i = 0; i < R_BOUNDARY; i++) {
        for (j = 0; j < C_BOUNDARY; j++) {
            rect.x = SIZE_OF_BOX * j;
            rect.y = SIZE_OF_BOX * i;
            tmask[i][j] = '+';
            SDL_BlitSurface(boxes[10], NULL, screen, &rect);

        }
    }

    rect.x = MARGINLEFT;
    rect.y = screen->h - HIGHT_TIMERBOX - MARGE_COUNTER;
    SDL_BlitSurface(numbers[0], NULL, screen, &rect);

    rect.x = MARGINLEFT + WIDTH_TIMERBOX;
    rect.y = screen->h - HIGHT_TIMERBOX - MARGE_COUNTER;
    SDL_BlitSurface(numbers[0], NULL, screen, &rect);

    rect.x = MARGINLEFT + WIDTH_TIMERBOX * 2;
    rect.y = screen->h - HIGHT_TIMERBOX - MARGE_COUNTER;
    SDL_BlitSurface(numbers[0], NULL, screen, &rect);

    rect.x = MARGINLEFT + WIDTH_TIMERBOX * 3;
    rect.y = screen->h - HIGHT_TIMERBOX - MARGE_COUNTER;
    SDL_BlitSurface(numbers[0], NULL, screen, &rect);

    rect.x = screen->w / 2;
    rect.y = screen->h - HIGHT_TIMERBOX - MARGE_FACE;
    SDL_BlitSurface(happy, NULL, screen, &rect);

    rect.x = screen->w - MARGINRIGHT - WIDTH_TIMERBOX * 2;
    rect.y = screen->h - HIGHT_TIMERBOX - MARGE_COUNTER;
    SDL_BlitSurface(numbers[MINES_DEPLOYED / 100 % 10], NULL, screen, &rect);

    rect.x = screen->w - MARGINRIGHT - WIDTH_TIMERBOX * 1;

    rect.y = screen->h - HIGHT_TIMERBOX - MARGE_COUNTER;

    SDL_BlitSurface(numbers[MINES_DEPLOYED / 10 % 10], NULL, screen, &rect);

    rect.x = screen->w - MARGINRIGHT;

    rect.y = screen->h - HIGHT_TIMERBOX - MARGE_COUNTER;

    SDL_BlitSurface(numbers[MINES_DEPLOYED % 10], NULL, screen, &rect);
}

int isRevealed(int row, int col) {
    if (tmask[row][col] != '+')
        return 1;

    return 0;
}

void reveal(int row, int col) {
    if (!isRevealed(row, col))
        numrev++;
    SDL_Rect rect;
    rect.x = SIZE_OF_BOX * col;
    rect.y = SIZE_OF_BOX * row;
    tmask[row][col] = numberTochar(t[row][col]);
    switch (t[row][col]) {
        case 0:
            SDL_BlitSurface(boxes[0], NULL, screen, &rect);
            break;
        case 1:
            SDL_BlitSurface(boxes[1], NULL, screen, &rect);
            break;
        case 2:
            SDL_BlitSurface(boxes[2], NULL, screen, &rect);
            break;
        case 3:
            SDL_BlitSurface(boxes[3], NULL, screen, &rect);
            break;
        case 4:
            SDL_BlitSurface(boxes[4], NULL, screen, &rect);
            break;
        case 5:
            SDL_BlitSurface(boxes[5], NULL, screen, &rect);
            break;
        case 6:
            SDL_BlitSurface(boxes[6], NULL, screen, &rect);
            break;
        case 7:
            SDL_BlitSurface(boxes[7], NULL, screen, &rect);
            break;
        case 8:
            SDL_BlitSurface(boxes[8], NULL, screen, &rect);
            break;
        default:
            SDL_BlitSurface(boxes[9], NULL, screen, &rect);
            Loss = 1;
            if (onetimered == 0) {
                corruptedCol = col;
                corruptedRow = row;
                onetimered++;
            }
            break;
    }

}

void InitAllsquare() {
    int i;

    for (i = 0; i < 12; i++)
        boxes[i] = SDL_CreateRGBSurface(0, SIZE_OF_BOX, SIZE_OF_BOX, 32, 0, 0, 0, 0);
    for (i = 0; i < 10; i++)
        numbers[i] = SDL_CreateRGBSurface(0, WIDTH_TIMERBOX, HIGHT_TIMERBOX, 32, 0, 0, 0, 0);

    happy = SDL_CreateRGBSurface(0, WIDTH_SMILEY, HIGHT_SMILEY, 32, 0, 0, 0, 0);
    sad = SDL_CreateRGBSurface(0, WIDTH_SMILEY, HIGHT_SMILEY, 32, 0, 0, 0, 0);
    ow = SDL_CreateRGBSurface(0, WIDTH_SMILEY, HIGHT_SMILEY, 32, 0, 0, 0, 0);
    victor = SDL_CreateRGBSurface(0, WIDTH_SMILEY, HIGHT_SMILEY, 32, 0, 0, 0, 0);
    bombmiss = SDL_CreateRGBSurface(0, SIZE_OF_BOX, SIZE_OF_BOX, 32, 0, 0, 0, 0);
    bombred = SDL_CreateRGBSurface(0, SIZE_OF_BOX, SIZE_OF_BOX, 32, 0, 0, 0, 0);
    for (i = 0; i <= 8; i++) {
        boxesrec[i].x = i * SIZE_OF_BOX;
        boxesrec[i].y = SIZE_OF_BOX;
        boxesrec[i].h = SIZE_OF_BOX;
        boxesrec[i].w = SIZE_OF_BOX;
    }
    boxesrec[10].x = 0;
    boxesrec[10].y = 0;
    boxesrec[10].h = SIZE_OF_BOX;
    boxesrec[10].w = SIZE_OF_BOX;

    boxesrec[9].x = 0;
    boxesrec[9].y = SIZE_OF_BOX * 2;
    boxesrec[9].h = SIZE_OF_BOX;
    boxesrec[9].w = SIZE_OF_BOX;

    boxesrec[11].x = SIZE_OF_BOX;
    boxesrec[11].y = 0;
    boxesrec[11].h = SIZE_OF_BOX;
    boxesrec[11].w = SIZE_OF_BOX;

    for (i = 0; i < 10; i++) {
        numbersrec[i].x = i * WIDTH_TIMERBOX;
        numbersrec[i].y = SIZE_OF_BOX * 3;
        numbersrec[i].w = WIDTH_TIMERBOX;
        numbersrec[i].h = HIGHT_TIMERBOX;
    }

    happyrec.x = 0 ;
    happyrec.y = SIZE_OF_BOX * 3 + HIGHT_TIMERBOX;
    happyrec.h = HIGHT_SMILEY;
    happyrec.w = WIDTH_SMILEY;

    sadrec.x = WIDTH_SMILEY;
    sadrec.y = SIZE_OF_BOX * 3 + HIGHT_TIMERBOX;
    sadrec.h = HIGHT_SMILEY;
    sadrec.w = WIDTH_SMILEY;

    owrec.x = WIDTH_SMILEY * 2;
    owrec.y = SIZE_OF_BOX * 3 + HIGHT_TIMERBOX;
    owrec.h = HIGHT_SMILEY;
    owrec.w = WIDTH_SMILEY;

    victorrec.x = WIDTH_SMILEY * 3;
    victorrec.y = SIZE_OF_BOX * 3 + HIGHT_TIMERBOX;
    victorrec.h = HIGHT_SMILEY;
    victorrec.w = WIDTH_SMILEY;

    bombmissrec.x = SIZE_OF_BOX * 2;
    bombmissrec.y = SIZE_OF_BOX * 2;
    bombmissrec.h = SIZE_OF_BOX;
    bombmissrec.w = SIZE_OF_BOX;

    bombredrec.x = SIZE_OF_BOX;
    bombredrec.y = SIZE_OF_BOX * 2;
    bombredrec.h = SIZE_OF_BOX;
    bombredrec.w = SIZE_OF_BOX;

    for (i = 0; i < 12; i++) {
        SDL_BlitSurface(loadimg, &boxesrec[i], boxes[i], NULL);
        printf("%s", SDL_GetError());
    }
    for (i = 0; i < 10; i++) {
        SDL_BlitSurface(loadimg, &numbersrec[i], numbers[i], NULL);
        printf("%s", SDL_GetError());
    }
    SDL_BlitSurface(loadimg, &happyrec, happy, NULL);
    SDL_BlitSurface(loadimg, &sadrec, sad, NULL);
    SDL_BlitSurface(loadimg, &owrec, ow, NULL);
    SDL_BlitSurface(loadimg, &victorrec, victor, NULL);
    SDL_BlitSurface(loadimg, &bombmissrec, bombmiss, NULL);
    SDL_BlitSurface(loadimg, &bombredrec, bombred, NULL);
}

void expand(int row, int col) {

    if (col < C_BOUNDARY && row < R_BOUNDARY && col >= 0 && row >= 0) {
        if (t[row][col] == 0 && !isRevealed(row, col))
            Mix_PlayChannel(1, Audexpanding, 0);

        if ((t[row][col] > 0 || t[row][col] < 0) && !marking[row][col])
            reveal(row, col);

        if (!isRevealed(row, col) && t[row][col] == 0 && !marking[row][col]) {
            reveal(row, col);


            expand(row, col + 1);

            expand(row + 1, col);
            expand(row - 1, col);
            expand(row, col - 1);


            expand(row + 1, col + 1);

            expand(row + 1, col - 1);
            expand(row - 1, col + 1);
            expand(row - 1, col - 1);

        }
    }


}

void warningMineMarking(int field[][C_BOUNDARY]) {
    int i, j;
    for (i = 0; i < R_BOUNDARY; i++) {
        for (j = 0; j < C_BOUNDARY; j++) {
            if (field[i][j] < 0) {
                //west
                if (j > 0)
                    field[i][j - 1]++;
                //east
                if (j < C_BOUNDARY - 1)
                    field[i][j + 1]++;
                //south
                if (i > 0)
                    field[i - 1][j]++;
                //north
                if (i < R_BOUNDARY - 1)
                    field[i + 1][j]++;

                //north west
                if (i < R_BOUNDARY - 1 && j > 0)
                    field[i + 1][j - 1]++;
                //north east
                if (i < R_BOUNDARY - 1 && j < C_BOUNDARY - 1)
                    field[i + 1][j + 1]++;
                //south west
                if (i > 0 && j > 0)
                    field[i - 1][j - 1]++;
                //south east
                if (i > 0 && j < C_BOUNDARY - 1)
                    field[i - 1][j + 1]++;
            }
        }
    }
}

int testNumbthrdfnc(int row, int col) {
    int i = 0;
    if (col > 0) {
        if (marking[row][col - 1])
            i++;
    }
    //east
    if (col < C_BOUNDARY - 1) {
        if (marking[row][col + 1])
            i++;
    }
    //south
    if (row > 0) {
        if (marking[row - 1][col])
            i++;
    }
    //north
    if (row < R_BOUNDARY - 1) {
        if (marking[row + 1][col])
            i++;
    }

    //north west
    if (row < R_BOUNDARY - 1 && col > 0) {
        if (marking[row + 1][col - 1])
            i++;
    }
    //north east
    if (row < R_BOUNDARY - 1 && col < C_BOUNDARY - 1) {
        if (marking[row + 1][col + 1])
            i++;
    }
    //south west
    if (row > 0 && col > 0) {
        if (marking[row - 1][col - 1])
            i++;
    }
    //south east
    if (row > 0 && col < C_BOUNDARY - 1) {
        if (marking[row - 1][col + 1])
            i++;
    }
    return (i == t[row][col]);
}

void thirdfnc(int row, int col) {
    if (isRevealed(row, col) && t[row][col] > 0) {
        if (testNumbthrdfnc(row, col)) {
            if (col > 0)
                expand(row, col - 1);
            //east
            if (col < C_BOUNDARY - 1)
                expand(row, col + 1);
            //south
            if (row > 0)
                expand(row - 1, col);
            //north
            if (row < R_BOUNDARY - 1)
                expand(row + 1, col);

            //north west
            if (row < R_BOUNDARY - 1 && col > 0)
                expand(row + 1, col - 1);
            //north east
            if (row < R_BOUNDARY - 1 && col < C_BOUNDARY - 1)
                expand(row + 1, col + 1);
            //south west
            if (row > 0 && col > 0)
                expand(row - 1, col - 1);
            //south east
            if (row > 0 && col < C_BOUNDARY - 1)
                expand(row - 1, col + 1);
        }
    }
}

void deployMines(int numberMines) {
    srand((unsigned) time(NULL));
    int bombsdeployed = 0;
    int row;
    int col;
    while (bombsdeployed != numberMines) {
        do {
            row = rand() % R_BOUNDARY;
            col = rand() % C_BOUNDARY;
        } while (t[row][col] < 0);
        t[row][col] = -9;
        bombsdeployed++;
    }
}

void revealAllmines() {
    int i, j;
    for (i = 0; i < R_BOUNDARY; i++) {
        for (j = 0; j < C_BOUNDARY; j++) {

            if (t[i][j] < 0) {
                if (marking[i][j] == 0)
                    expand(i, j);
            } else {
                SDL_Rect rect;
                rect.x = SIZE_OF_BOX * j;
                rect.y = SIZE_OF_BOX * i;
                if (marking[i][j] == 1)
                    SDL_BlitSurface(bombmiss, NULL, screen, &rect);
            }
        }

    }
    SDL_Rect rect;
    rect.x = corruptedCol * SIZE_OF_BOX;
    rect.y = corruptedRow * SIZE_OF_BOX;
    SDL_BlitSurface(bombred, NULL, screen, &rect);

}

void updateMineCounter(int x) {
    SDL_Rect rect;
    if (x >= 0) {
        rect.x = screen->w - MARGINRIGHT - WIDTH_TIMERBOX * 2;

        rect.y = screen->h - HIGHT_TIMERBOX - MARGE_COUNTER;

        SDL_BlitSurface(numbers[x / 100 % 10], NULL, screen, &rect);

        rect.x = screen->w - MARGINRIGHT - WIDTH_TIMERBOX * 1;

        rect.y = screen->h - HIGHT_TIMERBOX - MARGE_COUNTER;

        SDL_BlitSurface(numbers[x / 10 % 10], NULL, screen, &rect);

        rect.x = screen->w - MARGINRIGHT;

        rect.y = screen->h - HIGHT_TIMERBOX - MARGE_COUNTER;

        SDL_BlitSurface(numbers[x % 10], NULL, screen, &rect);
        update();
    }
}

void transform(int row, int col) {
    SDL_Rect rect;
    if (!marking[row][col]) {
        if (col > 0) {
            if (!isRevealed(row, col - 1) && !marking[row][col - 1]) {
                rect.x = SIZE_OF_BOX * (col - 1);
                rect.y = SIZE_OF_BOX * row;
                SDL_BlitSurface(boxes[0], NULL, screen, &rect);
            }
        }
        //east
        if (col < C_BOUNDARY - 1) {
            if (!isRevealed(row, col + 1) && !marking[row][col + 1]) {
                rect.x = SIZE_OF_BOX * (col + 1);
                rect.y = SIZE_OF_BOX * row;
                SDL_BlitSurface(boxes[0], NULL, screen, &rect);
            }
        }
        //south
        if (row > 0) {
            if (!isRevealed(row - 1, col) && !marking[row - 1][col]) {
                rect.x = SIZE_OF_BOX * col;
                rect.y = SIZE_OF_BOX * (row - 1);
                SDL_BlitSurface(boxes[0], NULL, screen, &rect);
            }
        }
        //north
        if (row < R_BOUNDARY - 1) {
            if (!isRevealed(row + 1, col) && !marking[row + 1][col]) {
                rect.x = SIZE_OF_BOX * col;
                rect.y = SIZE_OF_BOX * (row + 1);
                SDL_BlitSurface(boxes[0], NULL, screen, &rect);
            }
        }
        //north west
        if (row < R_BOUNDARY - 1 && col > 0) {
            if (!isRevealed(row + 1, col - 1) && !marking[row + 1][col - 1]) {
                rect.x = SIZE_OF_BOX * (col - 1);
                rect.y = SIZE_OF_BOX * (row + 1);
                SDL_BlitSurface(boxes[0], NULL, screen, &rect);
            }
        }
        //north east
        if (row < R_BOUNDARY - 1 && col < C_BOUNDARY - 1) {
            if (!isRevealed(row + 1, col + 1) && !marking[row + 1][col + 1]) {
                rect.x = SIZE_OF_BOX * (col + 1);
                rect.y = SIZE_OF_BOX * (row + 1);
                SDL_BlitSurface(boxes[0], NULL, screen, &rect);
            }
        }
        //south west
        if (row > 0 && col > 0) {
            if (!isRevealed(row - 1, col - 1) && !marking[row - 1][col - 1]) {
                rect.x = SIZE_OF_BOX * (col - 1);
                rect.y = SIZE_OF_BOX * (row - 1);
                SDL_BlitSurface(boxes[0], NULL, screen, &rect);
            }
        }
        //south east
        if (row > 0 && col < C_BOUNDARY - 1) {
            if (!isRevealed(row - 1, col + 1) && !marking[row - 1][col + 1]) {
                rect.x = SIZE_OF_BOX * (col + 1);
                rect.y = SIZE_OF_BOX * (row - 1);
                SDL_BlitSurface(boxes[0], NULL, screen, &rect);
            }
        }
    }
}

void retransform(int row, int col) {
    SDL_Rect rect;
    if (!marking[row][col]) {
        if (col > 0) {
            if (!isRevealed(row, col - 1) && !marking[row][col - 1]) {
                rect.x = SIZE_OF_BOX * (col - 1);
                rect.y = SIZE_OF_BOX * row;
                SDL_BlitSurface(boxes[10], NULL, screen, &rect);
            }
        }
        //east
        if (col < C_BOUNDARY - 1) {
            if (!isRevealed(row, col + 1) && !marking[row][col + 1]) {
                rect.x = SIZE_OF_BOX * (col + 1);
                rect.y = SIZE_OF_BOX * row;
                SDL_BlitSurface(boxes[10], NULL, screen, &rect);
            }
        }
        //south
        if (row > 0) {
            if (!isRevealed(row - 1, col) && !marking[row - 1][col]) {
                rect.x = SIZE_OF_BOX * col;
                rect.y = SIZE_OF_BOX * (row - 1);
                SDL_BlitSurface(boxes[10], NULL, screen, &rect);
            }
        }
        //north
        if (row < R_BOUNDARY - 1) {
            if (!isRevealed(row + 1, col) && !marking[row + 1][col]) {
                rect.x = SIZE_OF_BOX * col;
                rect.y = SIZE_OF_BOX * (row + 1);
                SDL_BlitSurface(boxes[10], NULL, screen, &rect);
            }
        }
        //north west
        if (row < R_BOUNDARY - 1 && col > 0) {
            if (!isRevealed(row + 1, col - 1) && !marking[row + 1][col - 1]) {
                rect.x = SIZE_OF_BOX * (col - 1);
                rect.y = SIZE_OF_BOX * (row + 1);
                SDL_BlitSurface(boxes[10], NULL, screen, &rect);
            }
        }
        //north east
        if (row < R_BOUNDARY - 1 && col < C_BOUNDARY - 1) {
            if (!isRevealed(row + 1, col + 1) && !marking[row + 1][col + 1]) {
                rect.x = SIZE_OF_BOX * (col + 1);
                rect.y = SIZE_OF_BOX * (row + 1);
                SDL_BlitSurface(boxes[10], NULL, screen, &rect);
            }
        }
        //south west
        if (row > 0 && col > 0) {
            if (!isRevealed(row - 1, col - 1) && !marking[row - 1][col - 1]) {
                rect.x = SIZE_OF_BOX * (col - 1);
                rect.y = SIZE_OF_BOX * (row - 1);
                SDL_BlitSurface(boxes[10], NULL, screen, &rect);
            }
        }
        //south east
        if (row > 0 && col < C_BOUNDARY - 1) {
            if (!isRevealed(row - 1, col + 1) && !marking[row - 1][col + 1]) {
                rect.x = SIZE_OF_BOX * (col + 1);
                rect.y = SIZE_OF_BOX * (row - 1);
                SDL_BlitSurface(boxes[10], NULL, screen, &rect);
            }
        }
    }
}

int main(int argc, char *argv[]) {


    if (SDL_Init(SDL_INIT_EVERYTHING) == -1) {
        fprintf(stderr, " %s ", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024) == -1) //Initialisation de l'API Mixer
    {
        printf("%s", Mix_GetError());
    }
    Mix_AllocateChannels(32);
    Mix_VolumeMusic(10);

    Audexplo = Mix_LoadWAV("../resources/Explosion.wav");
    Audexpanding = Mix_LoadWAV("../resources/0Grid.wav");
    Audwinning = Mix_LoadWAV("../resources/winning.wav");
    Audsimpleclick = Mix_LoadWAV("../resources/middle.wav");
    Audmiddleclick = Mix_LoadWAV("../resources/simple.wav");

    printf("%s", Mix_GetError());
    window = SDL_CreateWindow("NanoSweeper ",
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              C_BOUNDARY * SIZE_OF_BOX, R_BOUNDARY * SIZE_OF_BOX + 50,
                              SDL_WINDOW_OPENGL);

    screen = SDL_CreateRGBSurface(0, C_BOUNDARY * SIZE_OF_BOX, R_BOUNDARY * SIZE_OF_BOX + 50, 32,
                                  0,
                                  0,
                                  0,
                                  0xFF000000);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED  | SDL_RENDERER_PRESENTVSYNC);

    texture = SDL_CreateTexture(renderer,
                                SDL_PIXELFORMAT_ARGB8888,
                                SDL_TEXTUREACCESS_STREAMING,
                                C_BOUNDARY * SIZE_OF_BOX, R_BOUNDARY * SIZE_OF_BOX + 50);

    loadimg = SDL_LoadBMP("../resources/mine2.bmp");

    if (loadimg == NULL) {
        fprintf(stderr, " failed to load image ");
        exit(EXIT_FAILURE);
    }


    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 192, 192, 192));
    InitAllsquare();
    deployMines(MINES_DEPLOYED);
    warningMineMarking(t);
    initMaskField();
    SDL_TimerID timer = SDL_AddTimer(1000, NanoCallbackfnc, NULL);
    SDL_Rect rect;
    int row, col;
    while (runs) {


        SDL_WaitEvent(&event);


        switch (event.type) {


            case SDL_MOUSEBUTTONDOWN :

                row = event.button.y / SIZE_OF_BOX;
                col = event.button.x / SIZE_OF_BOX;
                if (event.button.button == SDL_BUTTON_LEFT && !Loss && !Win) {

                    rect.x = screen->w / 2;
                    rect.y = screen->h - HIGHT_TIMERBOX - MARGE_FACE;
                    SDL_BlitSurface(ow, NULL, screen, &rect);


                }
                if (event.button.button == SDL_BUTTON_MIDDLE && !Loss && !Win) {
                    rect.x = screen->w / 2;
                    rect.y = screen->h - HIGHT_TIMERBOX - MARGE_FACE;
                    SDL_BlitSurface(ow, NULL, screen, &rect);

                    transform(row, col);


                }
                break;
            case SDL_MOUSEBUTTONUP :
                firsttime = 1;
                rect.x = screen->w / 2;
                rect.y = screen->h - HIGHT_TIMERBOX - MARGE_FACE;
                SDL_BlitSurface(happy, NULL, screen, &rect);
                retransform(row, col);
                row = event.button.y / SIZE_OF_BOX;
                col = event.button.x / SIZE_OF_BOX;
                rect.x = SIZE_OF_BOX * col;
                rect.y = SIZE_OF_BOX * row;
                if (event.button.button == SDL_BUTTON_LEFT && !Loss && !Win) {
                    expand(row, col);
                    Mix_PlayChannel(4, Audsimpleclick, 0);
                }
                if (event.button.button == SDL_BUTTON_RIGHT && !Loss && !Win) {

                    if (!isRevealed(row, col)) {
                        if (marking[row][col] == 0) {
                            SDL_BlitSurface(boxes[11], NULL, screen, &rect);
                            marking[row][col] = 1;
                            minestomark--;
                        } else {

                            SDL_BlitSurface(boxes[10], NULL, screen, &rect);
                            marking[row][col] = 0;
                            minestomark++;
                        }

                        updateMineCounter(minestomark);
                    }
                }
                if (event.button.button == SDL_BUTTON_MIDDLE && !Loss && !Win) {
                    thirdfnc(row, col);
                    Mix_PlayChannel(5, Audmiddleclick, 0);
                }

                break;
            case SDL_QUIT:
                runs = 0;
            default:
                break;
        }


        if (Win) {

            rect.x = screen->w / 2;
            rect.y = screen->h - HIGHT_TIMERBOX - MARGE_FACE;
            SDL_BlitSurface(victor, NULL, screen, &rect);
            if (playonetimewin == 0) {
                Mix_PlayChannel(2, Audwinning, 0);
                playonetimewin++;
            }
        }
        if (Loss) {

            revealAllmines();
            rect.x = screen->w / 2;
            rect.y = screen->h - HIGHT_TIMERBOX - MARGE_FACE;
            SDL_BlitSurface(sad, NULL, screen, &rect);
            if (playonetimeloss == 0) {
                Mix_PlayChannel(0, Audexplo, 0);
                playonetimeloss++;
            }

        }
        if ((C_BOUNDARY * R_BOUNDARY - numrev) == MINES_DEPLOYED)
            Win = 1;


        update();

    }

    Mix_CloseAudio();
    SDL_RemoveTimer(timer);
    SDL_Quit();
    return 0;
}