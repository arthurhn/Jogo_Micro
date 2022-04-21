/*
 * funcoes.h
 *
 *  Created on: Nov 9, 2020
 *      Author: arthu
 */

#ifndef FUNCOES_H_
#define FUNCOES_H_

#include <stdint.h>
#include "Nokia5110.h"

#include "inc/tm4c123gh6pm.h"
#include "driverlib/interrupt.h"
#include "driverlib/debug.h"
#include "driverlib/timer.h"
#include "inc/hw_gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"

#include <stdlib.h>
#include<time.h>

int estado, tempo, lock;
int stamina, ai_value, move, hp, hp_ai, aux, aux_ai, KO, KO_ai, dmg, dmg_ai;

void IntPortalF();
void Timer0IntHandler();
void Timer1IntHandler();
void Timer2IntHandler();
void Timer3IntHandler();
void Enemy_ai();
void Set_controls();
int get_controls();
void start();
void game();

#endif /* FUNCOES_H_ */
