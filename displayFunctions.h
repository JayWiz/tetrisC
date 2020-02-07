/*
 * displayFunctions.h
 *
 *  Created on: 07.02.2020
 *      Author: Johan
 */

#ifndef DISPLAYFUNCTIONS_H_
#define DISPLAYFUNCTIONS_H_


void moveCursor(uint8_t xPosition, uint8_t yPosition) {
    // cursor {0,0} in upper left corner
    // move cursor to screen location v,h (Esc[Line;ColumnH  )
    serialWrite(0x1B);
    serialWrite(0x5B);

    if (yPosition > 9) {
        char yChar1 = (yPosition / 10) + '0';
        char yChar2 = (yPosition % 10) + '0';
        serialWrite(yChar1);
        serialWrite(yChar2);
    } else {
        char yChar = yPosition + '0';
        serialWrite(yChar);
    }
    serialWrite(0x3B);          //;
    if (xPosition > 9) {
        char xChar1 = (xPosition / 10) + '0';
        char xChar2 = (xPosition % 10) + '0';
        serialWrite(xChar1);
        serialWrite(xChar2);
    } else {
        char xChar = xPosition + '0';
        serialWrite(xChar);
    }
    serialWrite('H');
}

void clearEntireScreen() {
    // Esc[2J
    serialWrite(0x1B);
    serialWrite(0x5B);
    serialWrite(0x32);
    serialWrite(0x4A);
}

void drawFrame(uint8_t maxRows, uint8_t maxColumns) {
    // left
    uint8_t i;
    for (i = 1; i < maxRows + 1; i++) {
        moveCursor(1, i);
        serialPrint("#");
    }
    // right
    for (i = 1; i < maxRows + 1; i++) {
        moveCursor(maxColumns + 2, i);
        serialPrint("#");
    }
}


#endif /* DISPLAYFUNCTIONS_H_ */
