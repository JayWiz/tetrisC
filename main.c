
#include <templateEMP.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "displayFunctions.h"
#include "button.h"

typedef struct {
    uint8_t column;
    uint8_t row;
    uint8_t components[4][4];
} tBlock;


#define ROWS 24
#define COLUMNS 10


void moveLeft(tBlock *pBlock);
void moveRight(tBlock *pBlock);
void moveDown(tBlock *pBlock);
void rotate(tBlock *pBlock);
void plotBlock(tBlock *pBlock, bool negative);
bool checkBottomCollision(tBlock *pBlock);
void spawnNewBlock(tBlock *pCurrentBlock);
bool checkGameOver(tBlock *pBlock);
void plotTable();
void checkLineClear(tBlock *pBlock);

uint8_t table[ROWS][COLUMNS];

uint8_t blocks[5][4][4] = {
                           {{0, 0, 0, 0}, {0, 1, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}},        // square
                           {{0, 0, 0, 0}, {0, 1, 1, 1}, {0, 0, 1, 0}, {0, 0, 0, 0}},        // T
                           {{0, 0, 0, 0}, {0, 1, 1, 1}, {0, 1, 0, 0}, {0, 0, 0, 0}},        // L
                           {{0, 0, 0, 0}, {1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}},        // I
                           {{0, 0, 0, 0}, {0, 1, 1, 0}, {0, 0, 1, 1}, {0, 0, 0, 0}},        // S


};


tBlock currentBlock;

void initGame() {
    clearEntireScreen();
    uint8_t i, j;
    for (i = 0; i < ROWS; i++) {
        for (j = 0; j < COLUMNS; j++) {
            table[i][j] = 0;
        }
    }
    drawFrame(ROWS, COLUMNS);
    spawnNewBlock(&currentBlock);

    // fill last row debug
    for (i = 0; i < COLUMNS; i++) {
        table[ROWS - 1][i] = 1;
    }
    plotTable();
}


void main(void) {
	initMSP();
	initShiftRegister();
	initGame();


	while(1) {

	    if (!checkGameOver(&currentBlock)) {
	        moveDown(&currentBlock);
            plotBlock(&currentBlock, false);        // DRAW
            __delay_cycles(62500);
            plotBlock(&currentBlock, true);         // DELETE
            uint8_t button = getButton();
            if (button == 1) {
                moveLeft(&currentBlock);
            } else if (button == 4) {
                moveRight(&currentBlock);
            } else if (button == 3) {
                rotate(&currentBlock);
            } else if (button == 2) {
                moveDown(&currentBlock);
            }
	    } else {
	        initGame();
	    }
	}
}



void rotate(tBlock *pBlock) {
    uint8_t i, j;
    uint8_t N_max = 4;

    uint8_t oldState[4][4];
    memcpy(oldState, pBlock->components, sizeof(oldState));

    for (i = 0; i < N_max / 2; i++) {
        for (j = i; j < N_max - i - 1; j++) {
            // thanks to https://www.geeksforgeeks.org/inplace-rotate-square-matrix-by-90-degrees/
            uint8_t temp = pBlock->components[i][j];
            pBlock->components[i][j] = pBlock->components[j][N_max - 1 - i];
            pBlock->components[j][N_max - 1 - i] = pBlock->components[N_max - 1 - i][N_max - 1 - j];
            pBlock->components[N_max - 1 - i][N_max - 1 - j] = pBlock->components[N_max - 1 - j][i];
            pBlock->components[N_max - 1 - j][i] = temp;
        }
    }

    if (checkBorderCollision(pBlock)) {
        // collision detected
        memcpy(pBlock->components, oldState, sizeof(pBlock->components));
    }
}



bool checkBottomCollision(tBlock *pBlock) {
    uint8_t i, j;
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            // check whether component is filled
            if (pBlock->components[i][j] == 1) {
                uint8_t column = pBlock->column;
                uint8_t row = pBlock->row;
                uint8_t componentColumn = column + j;
                uint8_t componentRow = row + i;

                if (table[componentRow + 1][componentColumn] == 1) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool checkBorderCollision(tBlock *pBlock) {
    uint8_t i, j;
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            // check whether component is filled
            if (pBlock->components[i][j] == 1) {
                uint8_t column = pBlock->column;
                uint8_t row = pBlock->row;
                uint8_t componentColumn = column + j;
                uint8_t componentRow = row + i;

                if (table[componentRow][componentColumn + 1] == 1) {
                    return true;
                } else if (table[componentRow][componentColumn - 1] == 1) {
                    return true;
                } else if (componentColumn < 1 || componentColumn > COLUMNS - 2) {
                    return true;
                }

            }
        }
    }
    return false;
}

bool checkGameOver(tBlock *pBlock) {
    uint8_t i, j;
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            // check whether component is filled
            if (pBlock->components[i][j] == 1) {
                uint8_t column = pBlock->column;
                uint8_t row = pBlock->row;
                uint8_t componentColumn = column + j;
                uint8_t componentRow = row + i;

                if (table[componentRow + 1][componentColumn] == 1 && componentRow == 1) {
                    return true;
                }

            }
        }
    }
    return false;
}

void plotBlock(tBlock *pBlock, bool negative) {
    uint8_t column = pBlock->column;
    uint8_t row = pBlock->row;

    uint8_t i, j;
    for (i =  0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            moveCursor(column + 2 + j, row + 1 + i);
            if (pBlock->components[i][j] == 1) {
                if (negative) {
                    serialPrint(" ");
                } else {
                    serialPrint("O");
                }
            }
        }
    }
}

uint8_t getStartIndex() {
    uint8_t i, j;
    for (i = 0; i < ROWS; i++) {
        for (j = 0; j < COLUMNS; j++) {
            if (table[i][j] == 1) {
                return i - 1;
            }
        }
    }
    return 0;
}

void plotLineClear() {
    uint8_t i, j;
    for (i = getStartIndex(); i < ROWS; i++) {
        for (j = 0; j < COLUMNS; j++) {
            moveCursor(j + 2, i + 1);
            if (table[i][j] == 1) {
                // Ausgabe 2 nach rechts, 1 nach unten verschoben
                serialPrint("X");
            } else {
                serialPrint(" ");
            }
        }
    }
}

void plotTable() {
    uint8_t i, j;
    for (i = 0; i < ROWS; i++) {
        for (j = 0; j < COLUMNS; j++) {
            if (table[i][j] == 1) {
                moveCursor(j + 2, i + 1);
                // Ausgabe 2 nach rechts, 1 nach unten verschoben
                serialPrint("X");
            }
        }
    }
}




void spawnNewBlock(tBlock *pCurrentBlock) {
    uint8_t i, j;
    pCurrentBlock->column = COLUMNS/2 - 2;
    pCurrentBlock->row = 0;
    uint8_t randomNumber = rand() % 5;
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            pCurrentBlock->components[i][j] = blocks[randomNumber][i][j];
        }
    }
}

void moveLeft(tBlock *pBlock) {
    if (!checkBorderCollision(pBlock)) {
        pBlock->column -= 1;
    }
}

void moveRight(tBlock *pBlock) {
    if (!checkBorderCollision(pBlock)) {
        pBlock->column += 1;
    }
}



void checkLineClear(tBlock *pBlock) {
    bool lineCleared = false;
    uint8_t i, j, k;
    uint8_t sum = 0;
    for (i = ROWS - 2; i > 0; i--) {
        sum = 0;
        for (j = 0; j < COLUMNS; j++) {
            sum += table[i][j];
        }
        if (sum == COLUMNS) {
            lineCleared = true;

            for (k = i; k > 0; k--) {
                for (j = 0; j < COLUMNS; j++) {
                    table[k][j] = table[k - 1][j];
                }
            }
        }
    }

    if (lineCleared) {
        plotLineClear();
    }
}

void moveDown(tBlock *pBlock) {

    uint8_t i, j;

    if (!checkBottomCollision(pBlock)) {
        pBlock->row += 1;
    } else {
        // fill table
        for (i = 0; i < 4; i++) {
            for (j = 0; j < 4; j++) {
                // check whether component is filled
                if (pBlock->components[i][j] == 1) {
                    uint8_t column = pBlock->column;
                    uint8_t row = pBlock->row;
                    uint8_t componentColumn = column + j;
                    uint8_t componentRow = row + i;
                    table[componentRow][componentColumn] = 1;
                }
            }
        }
        //
        checkLineClear(pBlock);
        spawnNewBlock(pBlock);
        plotTable();

    }
}


