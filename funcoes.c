/*
 Created on: Nov 9, 2020
 *      Author: arthu
 */
#include "funcoes.h"
#include "bitmaps.h"

#define red GPIO_PIN_1
#define blue GPIO_PIN_2
#define green GPIO_PIN_3
#define sw_1 GPIO_PIN_4
#define sw_2 GPIO_PIN_0
#define LOW 0x00
#define HIGH 0xFF


void IntPortalF (void){
    GPIOIntDisable(GPIO_PORTF_BASE, sw_1|sw_2);

    if (GPIOPinRead(GPIO_PORTF_BASE, sw_1) ==  LOW && GPIOPinRead(GPIO_PORTF_BASE, sw_2) ==  LOW){
        move = -1;
   }else if(GPIOPinRead(GPIO_PORTF_BASE, sw_2) ==  LOW && GPIOPinRead(GPIO_PORTF_BASE, sw_1) !=  LOW){
       move = 2;
   }else if(GPIOPinRead(GPIO_PORTF_BASE, sw_1) == LOW &&  GPIOPinRead(GPIO_PORTF_BASE, sw_2) !=  LOW)
       move = 1;

    GPIOIntClear(GPIO_PORTF_BASE, sw_1|sw_2);
    GPIOIntEnable(GPIO_PORTF_BASE, sw_1|sw_2);
}

void Timer0IntHandler(void){
    Nokia5110_DisplayBuffer();
    ai_value = 0;
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
}

void Timer1IntHandler(void){
    Nokia5110_DisplayBuffer();
    move = 0;
    TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
}

void Timer2IntHandler(void){
    Nokia5110_DisplayBuffer();
    move = 0;
    TimerIntClear(TIMER2_BASE, TIMER_TIMA_TIMEOUT);
}

void Timer3IntHandler(void){
    Nokia5110_DisplayBuffer();
    move = 0;
    TimerIntClear(TIMER3_BASE, TIMER_TIMA_TIMEOUT);
}

void Set_controls(){

            SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
            SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
            SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
            SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER2);
            SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER3);

            GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, red|blue|green);

            HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
            HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= 0x01;
            HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = 0;
            GPIODirModeSet(GPIO_PORTF_BASE, sw_1|sw_2, GPIO_DIR_MODE_IN);
            GPIOPadConfigSet(GPIO_PORTF_BASE, sw_1|sw_2, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

            GPIOIntTypeSet(GPIO_PORTF_BASE, GPIO_INT_PIN_0|GPIO_INT_PIN_4,GPIO_FALLING_EDGE);
            IntEnable(INT_GPIOF);
            GPIOIntEnable(GPIO_PORTF_BASE, GPIO_INT_PIN_0|GPIO_INT_PIN_4);
            IntMasterEnable();

}

int get_controls(){
    uint32_t ui32Period = (SysCtlClockGet() / 3600 )*100000;

    Nokia5110_PrintBMP2(32, 32, idle, 22, 18);
    Nokia5110_DisplayBuffer();

    if (move == 1){
            //printa a animacao de soco esquerdo
            Nokia5110_PrintBMP2(32, 32, left, 22, 18);
            Nokia5110_DisplayBuffer();

            TimerConfigure(TIMER1_BASE, TIMER_CFG_A_ONE_SHOT);
            TimerLoadSet(TIMER1_BASE, TIMER_A, ui32Period);
            IntEnable(INT_TIMER1A);
            TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);

            TimerEnable(TIMER1_BASE, TIMER_A);

            if(ai_value >= 0){

                return 1; //retorna 1 ou 2 quando o soco da dano, mas faz animacao independente
            }

        }else if(move == 2){
            //printa a animacao de soco direito
            Nokia5110_PrintBMP2(32, 32, right, 22, 18);

            TimerConfigure(TIMER2_BASE, TIMER_CFG_A_ONE_SHOT);
            TimerLoadSet(TIMER2_BASE, TIMER_A, ui32Period);
            IntEnable(INT_TIMER2A);
            TimerIntEnable(TIMER2_BASE, TIMER_TIMA_TIMEOUT);

            TimerEnable(TIMER2_BASE, TIMER_A);

            if(ai_value >= 0){

                return 2;
            }

        }else if(move == -1 && stamina > 0){
            //stamina preisa ser maior que zero pra poder defender

            Nokia5110_PrintBMP2(32, 32, def_user, 22, 18);
            //printa a animacao de defesa

            TimerConfigure(TIMER3_BASE, TIMER_CFG_A_ONE_SHOT);
            TimerLoadSet(TIMER3_BASE, TIMER_A, ui32Period);
            IntEnable(INT_TIMER3A);
            TimerIntEnable(TIMER3_BASE, TIMER_TIMA_TIMEOUT);

            TimerEnable(TIMER3_BASE, TIMER_A);

            if(ai_value == 1){

                return -1;
            }
        }

    return 0;
}


void Enemy_ai(){
    int i, j, k;
    uint32_t ui32Period = (SysCtlClockGet() / 3600 )*1000000/2;

    srand(time(NULL));
    i = rand()%2;

    if(i == 1){
        //move sempre antes de atacar, guarda aberta
        //animacoes de movimento, outro rand pra escolher a animacao se sera soco esquerdo ou direito etc

        srand(time(NULL));
        j = rand()%2;   //qual das 2 animacoes

        //ifs pra decidir qual animacao de qual inimigo printar
        if(estado == 1){            //print lutador1
            if(j == 0){
                //animacao esq
                Nokia5110_PrintBMP2(30, 8, enemy1L, 27, 24);           //esse é o lutador 1
                k = 1;
            }else{
                //animacao dir
                Nokia5110_PrintBMP2(30, 8, enemy1R, 27, 24);
                k = 2;
            }
        }else if(estado == 2){                 //print lutador2
            if(j == 0){
                //animacao esq
                Nokia5110_PrintBMP2(30, 8, enemy2L, 27, 24);
                k = 1;
            }else{
                //animacao dir
                Nokia5110_PrintBMP2(30, 8, enemy2R, 27, 24);
                k = 2;
            }
        }else if(estado == 3){                 //print lutador3
            if(j == 0){
                //animacao esq
                Nokia5110_PrintBMP2(30, 8, enemy3L, 27, 24);
                k = 1;
            }else{
                //animacao dir
                Nokia5110_PrintBMP2(30, 8, enemy3R, 27, 24);
                k = 2;
            }
        }

        TimerConfigure(TIMER0_BASE, TIMER_CFG_A_ONE_SHOT);
        TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period);
        IntEnable(INT_TIMER0A);
        TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

        TimerEnable(TIMER0_BASE, TIMER_A);
        //manter a pose

        if (get_controls() <= 0){
            if(estado == 1){            //print lutador1
                if(k == 1){
                    //print soco esquerdo
                    Nokia5110_PrintBMP2(30, 8, punch1L, 27, 24);
                    Nokia5110_DisplayBuffer();
                }else{
                    //print soco direito
                    Nokia5110_PrintBMP2(30, 8, punch1R, 27, 24);
                    Nokia5110_DisplayBuffer();
                }
            }else if(estado == 2){  //lutador 2
                if(k == 1){
                    //print soco esquerdo
                    Nokia5110_PrintBMP2(30, 8, punch2L, 27, 24);
                    Nokia5110_DisplayBuffer();
                }else{
                    //print soco direito
                    Nokia5110_PrintBMP2(30, 8, punch2R, 27, 24);
                    Nokia5110_DisplayBuffer();
                }
            }else if(estado == 3){      //lutador3
                if(k == 1){
                    //print soco esquerdo
                    Nokia5110_PrintBMP2(30, 8, punch3L, 27, 24);
                    Nokia5110_DisplayBuffer();
                }else{
                    //print soco direito
                    Nokia5110_PrintBMP2(30, 8, punch3R, 27, 24);
                    Nokia5110_DisplayBuffer();
                }
            }

            ai_value = 1;       //ataca

        }

    }else if(i == 0){
    //defende
        if(estado == 1){            //lutador 1
            Nokia5110_PrintBMP2(30, 8, def1, 27, 24);
            Nokia5110_DisplayBuffer();
        }else if(estado == 2){      //lutador 2
            Nokia5110_PrintBMP2(30, 8, def2, 27, 24);
            Nokia5110_DisplayBuffer();
        }else if(estado == 3){      //lutador3
            Nokia5110_PrintBMP2(30, 8, def3, 27, 24);
            Nokia5110_DisplayBuffer();
        }

    ai_value = -1;

    }
}

void start(){

    if (move == 2){
    //press sw_2 for rules
        Nokia5110_ClearBuffer();
        Nokia5110_PrintBMP2(0, 0, rules, 84, 48);
        Nokia5110_DisplayBuffer();
    }else{
        Nokia5110_ClearBuffer();
        Nokia5110_PrintBMP2(0, 0, main_menu, 84, 48);
        Nokia5110_DisplayBuffer();
    }

    //press sw_1+sw_2 to start!
    if(move == -1){
        move = 0;
       estado++;
    }

}

void game(){
    uint32_t ui32Period = (SysCtlClockGet() / 3600 )*1000000, i = 0, j = 0;
    int u , d , c;

    Nokia5110_PrintBMP2(0, 0, estadio, 84, 48);
    Nokia5110_DisplayBuffer();

    if(estado == 1){
      Nokia5110_PrintBMP2(56, 0, enemy1, 26, 4);
      Nokia5110_DisplayBuffer();
    }else if(estado == 2){
      Nokia5110_PrintBMP2(56, 0, enemy2, 26, 4);
      Nokia5110_DisplayBuffer();
    }else if (estado == 3){
      Nokia5110_PrintBMP2(56, 0, enemy3, 26, 4);
      Nokia5110_DisplayBuffer();
    }

      while(tempo > 0){
              u = tempo%10; d = (tempo%100)/10; c = tempo/100;

              Nokia5110_PrintBMP2(36, 0, numeros[c], 3, 5);     //centena
              Nokia5110_DisplayBuffer();

              Nokia5110_PrintBMP2(40, 0, numeros[d], 3, 5);     //dezena
              Nokia5110_DisplayBuffer();

              Nokia5110_PrintBMP2(44, 0, numeros[u], 3, 5);     //unidade
              Nokia5110_DisplayBuffer();

              Nokia5110_PrintBMP2(25, 0, mini_num[stamina], 3, 4);     //centena
              Nokia5110_DisplayBuffer();

              Enemy_ai();

                if(get_controls() == -1){
                    stamina--;
                }else if(get_controls() > 0){
                    aux_ai -= dmg;
                    if(estado == 1){        //barra de hp ia
                        Nokia5110_SetPxl(5, 53+j);
                        Nokia5110_DisplayBuffer();
                        Nokia5110_SetPxl(5, 54+j);
                        Nokia5110_DisplayBuffer();
                        Nokia5110_SetPxl(5, 55+j);
                        Nokia5110_DisplayBuffer();
                        j+=3;
                    }else if(estado == 2){
                        Nokia5110_SetPxl(5, 53+j);
                        Nokia5110_DisplayBuffer();
                        Nokia5110_SetPxl(5, 54+j);
                        Nokia5110_DisplayBuffer();
                        j+=2;
                    }else{
                        Nokia5110_SetPxl(5, 53+j);
                        Nokia5110_DisplayBuffer();
                        j++;
                    }
                    move = 0;
                }else if (ai_value > 0 && get_controls() >= 0){
                    aux -= dmg_ai;
                    if(estado == 1){        //barra de hp user
                        Nokia5110_SetPxl(5, 30-i);
                        Nokia5110_DisplayBuffer();
                        Nokia5110_SetPxl(5, 31-i);
                        Nokia5110_DisplayBuffer();
                        i+=2;
                    }else if(estado == 2){
                        Nokia5110_SetPxl(5, 30-i);
                        Nokia5110_DisplayBuffer();
                        Nokia5110_SetPxl(5, 31-i);
                        Nokia5110_DisplayBuffer();
                        Nokia5110_SetPxl(5, 32-i);
                        Nokia5110_DisplayBuffer();
                        i+=2;
                    }else{
                        Nokia5110_SetPxl(5, 30-i);
                        Nokia5110_DisplayBuffer();
                        Nokia5110_SetPxl(5, 31-i);
                        Nokia5110_DisplayBuffer();
                        Nokia5110_SetPxl(5, 32-i);
                        Nokia5110_DisplayBuffer();
                        i+=3;
                    }
                    ai_value = 0;
                }

           if (aux_ai == 0){
                KO++;
                if(KO == 1){
                    Nokia5110_SetPxl(2,30);
                    Nokia5110_DisplayBuffer();
                }else if(KO == 2){
                    Nokia5110_SetPxl(2,32);
                    Nokia5110_DisplayBuffer();
                }else if(KO == 3){
                    Nokia5110_SetPxl(2,34);
                    Nokia5110_DisplayBuffer();
                }
                //reiniciar barras de hp
                Nokia5110_PrintBMP2(0, 4, hp_bar, 32, 3);
                Nokia5110_DisplayBuffer();
                Nokia5110_PrintBMP2(52, 4, hp_bar, 32, 3);
                Nokia5110_DisplayBuffer();

                j=0;
                i=0;
                aux_ai = hp_ai;
                aux = hp;
          }

          if (aux == 0){
                KO_ai++;
                if(KO_ai == 1){
                    Nokia5110_SetPxl(2,49);
                    Nokia5110_DisplayBuffer();
                }if(KO_ai == 2){
                    Nokia5110_SetPxl(2,51);
                    Nokia5110_DisplayBuffer();
                }else if(KO_ai == 3){
                    Nokia5110_SetPxl(2,53);
                    Nokia5110_DisplayBuffer();
                }

                Nokia5110_PrintBMP2(0, 4, hp_bar, 32, 3);
                Nokia5110_DisplayBuffer();
                Nokia5110_PrintBMP2(52, 4, hp_bar, 32, 3);
                Nokia5110_DisplayBuffer();

                j=0;
                i=0;
                aux_ai = hp_ai;
                aux = hp;
          }

          if(KO == 3 || KO_ai == 3){
              SysCtlDelay(ui32Period/1000);
              break;
          }

      }

      if(KO_ai == 3 || tempo == 0){
          //game over
          Nokia5110_ClearBuffer();
          Nokia5110_PrintBMP2(0, 0, lose, 84, 48);
          Nokia5110_DisplayBuffer();

          SysCtlDelay(ui32Period/100);

          estado = 0;
          move = 0;
          lock = 0;
          SysTickDisable();
      }

      if(KO == 3){
          if(estado == 3){
          //you win
          Nokia5110_ClearBuffer();
          Nokia5110_PrintBMP2(0, 0, win, 84, 48);
          Nokia5110_DisplayBuffer();

          SysCtlDelay(ui32Period/100);

          estado = 0;
          move = 0;
          lock = 0;
          SysTickDisable();

          }else{
              //next fight
              Nokia5110_ClearBuffer();
              Nokia5110_PrintBMP2(0, 0, next_fight, 84, 48);
              Nokia5110_DisplayBuffer();

              SysCtlDelay(ui32Period/100);

              estado++;
              move = 0;
              lock = 0;
              SysTickDisable();
          }
      }
}
