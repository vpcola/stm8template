#include "stm8s.h"
#include "stdio.h"

void clock_setup(void);
void delay_us(signed int us);
void delay_ms(signed int ms);

void main(void)
{

  int i = 0;

  clock_setup();
  UART1_DeInit();


  /* UART1 needs some settings:
  115200 as BAUD, 8 bit wordlength, 1 stopbit and no parity.
  Syncmode clock disabled, and let's enable both RX and TX so printf and getchar works
  */
  UART1_Init((uint32_t)115200, UART1_WORDLENGTH_8D, UART1_STOPBITS_1, UART1_PARITY_NO,
              UART1_SYNCMODE_CLOCK_DISABLE, UART1_MODE_TXRX_ENABLE);

  printf("Printf with an STM8S is remarkably easy :D\r\n");
  printf("Now with the added bonus of GDB debugging :D\r\n");
  printf("Enter text now and it will be echoed with a newline added\r\n");

  while (1)
  {

    printf("%d\r\n", i);
    i++;
    delay_ms(1000); // Delay for 1 second
  }

}

void clock_setup(void)
{
	CLK_DeInit();

	CLK_HSECmd(DISABLE);
	CLK_LSICmd(DISABLE);
	CLK_HSICmd(ENABLE);
	while(CLK_GetFlagStatus(CLK_FLAG_HSIRDY) == FALSE);

	CLK_ClockSwitchCmd(ENABLE);
	CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
	CLK_SYSCLKConfig(CLK_PRESCALER_CPUDIV1);

	CLK_ClockSwitchConfig(CLK_SWITCHMODE_AUTO, CLK_SOURCE_HSI, DISABLE, CLK_CURRENTCLOCKSTATE_ENABLE);

	CLK_PeripheralClockConfig(CLK_PERIPHERAL_UART1, ENABLE);
	CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER4, ENABLE);
}


void delay_us(signed int us)
{
       TIM4_DeInit();
       if((us <= 200) && (us >= 0))
       {
              TIM4_TimeBaseInit(TIM4_PRESCALER_16, 200);
              TIM4_Cmd(ENABLE);
       }
       else if((us <= 400) && (us > 200))
       {
              us >>= 1;
              TIM4_TimeBaseInit(TIM4_PRESCALER_32, 200);
              TIM4_Cmd(ENABLE);
       }
       else if((us <= 800) && (us > 400))
       {
              us >>= 2;
              TIM4_TimeBaseInit(TIM4_PRESCALER_64, 200);
              TIM4_Cmd(ENABLE);
       }
       else if((us <= 1600) && (us > 800))
       {
              us >>= 3;
              TIM4_TimeBaseInit(TIM4_PRESCALER_128, 200);
              TIM4_Cmd(ENABLE);
       }
       while(TIM4_GetCounter() < us);

       TIM4_ClearFlag(TIM4_FLAG_UPDATE);
       TIM4_Cmd(DISABLE);
}

void delay_ms(signed int ms)
{
       while(ms--)
       {
              delay_us(1000);
       };

}
int putchar(int c) 
{
  //Write a character and wait till it's done
  UART1_SendData8(c);
  while (UART1_GetFlagStatus(UART1_FLAG_TXE) == RESET);
  return 0;
}

int getchar(void)
{

  int c = 0;
  //Loop until a character can be read
  while (UART1_GetFlagStatus(UART1_FLAG_RXNE) == RESET);
    c = UART1_ReceiveData8();
  return (c);
}

