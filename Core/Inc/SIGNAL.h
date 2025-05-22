/*
 * SIGNAL.h
 *
 *  Created on: Dec 23, 2024
 *      Author: phat
 */

#ifndef INC_SIGNAL_H_
#define INC_SIGNAL_H_

#include "main.h"

#define LEFT_STATE 				1
#define RIGHT_STATE 			2
#define FORWARD_STATE 			3
#define BACKWARD_LEFT_STATE		4
#define BACKWARD_RIGHT_STATE	5
#define FORWARD_LEFT_STATE 		6
#define FORWARD_RIGHT_STATE 	7
#define TEST_SENSORS 			8

extern uint8_t LEFT[8];
extern uint8_t RIGHT[8];
extern uint8_t FORWARD[8];
extern uint8_t BACKWARD_LEFT[8];
extern uint8_t BACKWARD_RIGHT[8];
extern uint8_t FORWARD_LEFT[8];
extern uint8_t FORWARD_RIGHT[8];

extern uint8_t returnSignal;

uint8_t Get_State(int d1, int d2, uint8_t *forceChange, uint8_t state);
uint8_t Get_CounterState(uint8_t state);
#endif /* INC_SIGNAL_H_ */
