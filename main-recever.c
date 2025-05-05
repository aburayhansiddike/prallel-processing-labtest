#include "stm32f10x.h"    

void systick_init();
void init();
void display(char *message);
void LCD_EnablePulse(void);
void delay_ms(uint32_t delay);

// ASCII values of A to Z
uint32_t lsb[] = {0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF, 0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xA};
uint32_t msb[] = {0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5};

	
int main(void)
{
	// Timer init
	systick_init();

	// Enable the Alternate Function for PINs and Enable GPIOA
		RCC->APB2ENR |= 5;
	
		GPIOA->CRL &= ~(0xFFFFFFFF);   // Clear bits for PA0 - PA7
    GPIOA->CRL |=  (0x33330033);   // Set mode and confuguration for PA0 - PA7
	
		// Enable UART2
		RCC->APB1ENR |=0x20000;
	
		// Enable the related PINs. Set GPIOA 2 pin as TX and GRIOA 3 as RX
		GPIOA->CRL |= (0xB00);
		GPIOA->CRL |= (0x8000);
		
	
		// Setup the baude rate for 9600 bps
    uint32_t system_clock = 36000000; 
    uint32_t baud_rate = 9600;
    USART2->BRR = (system_clock + (baud_rate / 2)) / baud_rate; // Correct BRR calculation
		
		// Enable Uart Transmit
		USART2->CR1 |= 8;
		
		// Enable Uart Recive
		USART2->CR1 |= 4;
		
		// Enable Uart
		USART2->CR1 |= 0x2000;
		
		//set word length
		USART2->CR1 |= 0x1000;

		init();
	
while(1)
	{
		char chat;
		
		// wait until data transmission is complete
		while(!(USART2->SR & (1<<6)))
		{};
			
		//Receive data
		chat = USART2->DR;
			
		char message[2];
			
		message[0]=chat;
		message[1]='\0';

		//Display data
		display(message);

		
	}
}
//Enable systick timer
void systick_init(void)
{
		SysTick->CTRL = 0;
		SysTick->LOAD = 0x00FFFFFF;
		SysTick->VAL = 0;
		SysTick->CTRL |= 5;
}

// Delay function
void delay_ms(uint32_t delay) {
    SysTick->LOAD = (SystemCoreClock / 1000) * delay - 1; // Set reload register
    SysTick->VAL = 0;  // Clear the current value register
    SysTick->CTRL = 5; // Enable SysTick (CLKSOURCE = Processor Clock)

    while (!(SysTick->CTRL & 0x10000)); // Wait for COUNTFLAG to be set

    SysTick->CTRL = 0; // Disable SysTick
}

// Enable pulse function for lcd display
void LCD_EnablePulse(void) {
		delay_ms(2);    			// Short delay
		GPIOA->ODR |= (0x2); // Enable high
    delay_ms(2);         // Short delay
    GPIOA->ODR &= ~(0x2); // Enable low
		delay_ms(2); 					// Short delay
}

// LCD display initialization function 
void init() {
    GPIOA->ODR = 0x02<<4;
    LCD_EnablePulse();
    
    GPIOA->ODR = 0x00<<4;
    LCD_EnablePulse();
    
    GPIOA->ODR = 0x01<<4;
    LCD_EnablePulse();
    
    GPIOA->ODR = 0x00<<4;
    LCD_EnablePulse();
    
    GPIOA->ODR = 0x02<<4;
    LCD_EnablePulse();
    
    GPIOA->ODR = 0x00<<4;
    LCD_EnablePulse();
    
    GPIOA->ODR = 0x0C<<4;
    LCD_EnablePulse();
    
   
}

// display message on LCD display function
void display(char *message){
		while (*message) {
			if(*message == ' '){
					GPIOA->ODR = (0x2<<4)|1;
					LCD_EnablePulse();
    
					GPIOA->ODR = 0x1;
					LCD_EnablePulse();
          *message++;
			}else{
          GPIOA->ODR = (msb[*message-'A']<<4)|1;
					LCD_EnablePulse();
    
					GPIOA->ODR = (lsb[*message-'A']<<4)|1;
					LCD_EnablePulse();
          *message++;
			}
    }
		
}
