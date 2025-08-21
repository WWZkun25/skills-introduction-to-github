#include"sys.h"
#include"delay.h"
#include"usart.h"
#include"lcd.h"

int main(void)
{ 
    delay_init();
    LED_Init();
    while(1)
    {
        LED0=0;
        LED1=1;
        LED2=1;
        LED3=1;

        delay_ms(500);
        LED0=1;
        LED1=0;
        LED2=1;
        LED3=1;

        delay_ms(500);
        LED0=1;
        LED1=1;
        LED2=0;
        LED3=1;

        delay_ms(500);
        LED0=1;
        LED1=1;
        LED2=1;
        LED3=0;

        delay_ms(500);

    }
}
