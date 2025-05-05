#include "stm32f10x.h"    


int main(void)
{
	// Enable the Alternate Function for PINs and enable GPIO A
	RCC->APB2ENR |= 5;

	// Enable UART2 clock
	RCC->APB1ENR |=0x20000;
	
		GPIOA->CRL &= ~(0xFFFFFFFF);   // Clear bits for PA0 - PA7
    GPIOA->CRL |=  (0x33330033);   // Set mode and confuguration for PA0 - PA7
	
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

while(1){
	
		char chat = 'Z';
		
	// wait until data transmission is complete
		while(!(USART2->SR & (1<<6)))
		{};
		
		// send data to data register
		USART2->DR = chat ;
			
	}
}


