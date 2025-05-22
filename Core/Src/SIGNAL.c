#include "SIGNAL.h"

uint8_t LEFT[] 			 = { 1, 0, 0, 0, 0, 0, 0, 0 };
uint8_t RIGHT[] 		 = { 2, 0, 0, 0, 0, 0, 0, 0 };
uint8_t FORWARD[] 		 = { 3, 0, 0, 0, 0, 0, 0, 0 };
uint8_t BACKWARD_LEFT[]	 = { 4, 1, 0, 0, 0, 0, 0, 0 };
uint8_t BACKWARD_RIGHT[] = { 4, 2, 0, 0, 0, 0, 0, 0 };
uint8_t FORWARD_LEFT[]   = { 3, 1, 0, 0, 0, 0, 0, 0 };
uint8_t FORWARD_RIGHT[]  = { 3, 2, 0, 0, 0, 0, 0, 0 };

uint8_t returnSignal = 0;
uint8_t Get_State(int distanceLeft, int distanceRight, uint8_t *forceChange, uint8_t state)
{

	if (*forceChange == 0) {

		if (distanceRight < distanceLeft) {
			if (distanceRight <= 100) return BACKWARD_RIGHT_STATE;
			else if (distanceRight <= 750) return LEFT_STATE;
		} else if (distanceLeft < distanceRight) {
			if (distanceLeft <= 100) return BACKWARD_LEFT_STATE;
			else if (distanceLeft <= 750) return RIGHT_STATE;
		}

	}
	else if (*forceChange == 1) {
		*forceChange = 0;
		return Get_CounterState(state);
	}
	return FORWARD_STATE;
}

uint8_t Get_CounterState(uint8_t state)
{
	if (state == RIGHT_STATE) {
		return LEFT_STATE;
	} else if (state == LEFT_STATE) {
		return RIGHT_STATE;
	} else {
		return LEFT_STATE;
	}
}

