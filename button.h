/*
 * button.h
 *
 *  Created on: 07.02.2020
 *      Author: Johan
 */

#ifndef BUTTON_H_
#define BUTTON_H_

void initShiftRegister(void) {
    // Setup Shift Registers
    P2DIR |= 0b01111111;
    P2SEL &= ~(BIT6 | BIT7);

    // Clear
    P2OUT &= ~BIT5;
    P2OUT |= BIT5;

    // Reset Clock
    P2OUT &= ~BIT4;
}


void clock(void) {
    P2OUT |= BIT4;
    P2OUT &= ~BIT4;
}

int getButton(void) {
    // Push Parallel Inputs to Outputs
    P2OUT |= (BIT2 | BIT3);
    clock();

    int numberOfButton = 4;

    while(numberOfButton >= 0) {
        if ((P2IN & BIT7) != 0) {
            return numberOfButton;
        }
        numberOfButton--;

        // Shift Outputs right
        P2OUT |= BIT2;
        P2OUT &= ~BIT3;
        clock();
    }
    return 0;
}


#endif /* BUTTON_H_ */
