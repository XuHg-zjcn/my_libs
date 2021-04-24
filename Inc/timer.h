/*
 * timer.h
 *
 *  Created on: Apr 17, 2021
 *      Author: xrj
 */

#ifndef INC_TIMER_H_
#define INC_TIMER_H_

uint32_t GetTimerFreq(TIM_HandleTypeDef *htim);
void TIM_OCMode(TIM_HandleTypeDef *htim, uint32_t Channel, uint32_t OCMode);

#endif /* INC_TIMER_H_ */
