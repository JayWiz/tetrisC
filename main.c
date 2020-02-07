
#include <templateEMP.h>
#include <stdint.h>
#include <stdbool.h>

#include "displayFunctions.h"

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
void plotBlock(tBlock *pBlock, bool negative);
bool checkCollision(tBlock *pBlock, uint8_t **pTable);
void spawnNewBlock(tBlock *pCurrentBlock);

uint8_t table[ROWS][COLUMNS];
uint8_t oBlock[4][4] = {{0, 0, 0, 0}, {0, 1, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}};
uint8_t teBlock[4][4] = {{0, 0, 0, 0}, {0, 1, 1, 1}, {0, 0, 1, 0}, {0, 0, 0, 0}};
tBlock currentBlock;


void main(void) {
	initMSP();
	clearEntireScreen();


	// fill last row debug
	uint8_t i;
	for (i = 0; i < 10; i++) {
	    table[ROWS - 1][i] = 1;
	}

	spawnNewBlock(&currentBlock);

	drawFrame(ROWS, COLUMNS);

	uint8_t debugLeft = 0;
	while(1) {

	    moveDown(&currentBlock);
	    plotBlock(&currentBlock, false);

	    if (debugLeft == 2) {
	        moveLeft(&currentBlock);
	        moveLeft(&currentBlock);
	        plotBlock(&currentBlock, false);
	    }

	    __delay_cycles(125000);
	    plotBlock(&currentBlock, true);
	    debugLeft++;
	}
}

bool checkCollision(tBlock *pBlock, uint8_t **pTable) {
    uint8_t i, j;
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            // check whether component is filled
            if (pBlock->components[i][j] == 1) {
                uint8_t column = pBlock->column;
                uint8_t row = pBlock->row;
                uint8_t componentColumn = column + j;
                uint8_t componentRow = row + i;

                if (pTable[componentRow + 1][componentColumn] == 1) {
                    // Collision
                    serialPrintInt(componentRow);

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

void plotTable() {
    uint8_t i, j;
    for (i = 0; i < ROWS; i++) {
        for (j = 0; j < COLUMNS; j++) {
            if (table[i][j] == 1) {
                // Ausgabe 2 nach rechts, 1 nach unten verschoben
                moveCursor(j + 2, i + 1);
                serialPrint("X");
            }
        }
    }
}


void moveLeft(tBlock *pBlock) {
    pBlock->column -= 1;
}

void moveRight(tBlock *pBlock) {
    pBlock->column += 1;
}

void spawnNewBlock(tBlock *pCurrentBlock) {
    uint8_t i, j;
    pCurrentBlock->column = COLUMNS/2 - 2;
    pCurrentBlock->row = 0;
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            pCurrentBlock->components[i][j] = teBlock[i][j];
        }
    }
}

void moveDown(tBlock *pBlock) {
    uint8_t *pTable[ROWS];
    uint8_t i, j;
    for(i = 0; i < ROWS; i++) {
        pTable[i] = table[i];
    }

    moveCursor(35, 5);
    serialPrint("ROW: ");
    serialPrintInt(pBlock->row);
    serialPrint(" COLUMN: ");
    serialPrintInt(pBlock->column);
    serialPrint("    ");

    if (!checkCollision(pBlock, pTable)) {
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
                    pTable[componentRow][componentColumn] = 1;
                }
            }
        }
        //
        spawnNewBlock(pBlock);
        plotTable();

    }
}
