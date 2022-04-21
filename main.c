#include "funcoes.h"

#define sw_1 GPIO_PIN_4
#define sw_2 GPIO_PIN_0
#define LOW 0x00
#define HIGH 0xFF

int estado, lock, tempo;

void trata_Systick(){
    tempo--;
}

int main(void){
    Nokia5110_Init();
    SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
    Set_controls();

    hp = 300; hp_ai = 600; lock = 0; estado = 0; lock = 0;

    while(1){
        if (estado > 0 && estado <= 3 && lock == 0){
            lock = 1;
            SysTickPeriodSet ((SysCtlClockGet() / 3600 )* 10000000);
            SysTickIntEnable();
            SysTickEnable ();
        }

        if (estado == 0){
            start();
        }else if (estado == 1){
            Nokia5110_ClearBuffer();
            aux = hp; aux_ai = hp_ai; stamina = 7;  KO = 0; KO_ai = 0; dmg = 60; dmg_ai = 20; tempo = 100;
            game();                    //glass john
        }else if (estado == 2){
            Nokia5110_ClearBuffer();
            aux = hp; aux_ai = hp_ai; stamina = 7;  KO = 0; KO_ai = 0; dmg = 40; dmg_ai = 25; tempo = 100;
            game();                    //bear kisser
        }else if (estado == 3){
            Nokia5110_ClearBuffer();
            aux = hp; aux_ai = hp_ai; stamina = 7;  KO = 0; KO_ai = 0; dmg = 30; dmg_ai = 30; tempo = 150;
            game();                    //king rhino

        }

    }
}
