/**
 * AVR32 UC3 template for TTK4147
 */

// include header files for all drivers that have been imported from AVR Software Framework (ASF).
#include <asf.h>
#include <board.h>
#include <gpio.h>
#include <sysclk.h>
#include "busy_delay.h"
#include <stdio.h>
#include "stdio_usb.h"

// defines for USB UART
#define CONFIG_USART_IF (AVR32_USART2)

// defines for BRTT interface
#define TEST_A AVR32_PIN_PA31
#define RESPONSE_A AVR32_PIN_PA30
#define TEST_B AVR32_PIN_PA29
#define RESPONSE_B AVR32_PIN_PA28
#define TEST_C AVR32_PIN_PA27
#define RESPONSE_C AVR32_PIN_PB00

void toggleResponsPin(int pin);

void init()
{
	// board init
	sysclk_init();
	board_init();
	busy_delay_init(BOARD_OSC0_HZ);
	
	
	// interrupts init
	cpu_irq_disable();
    INTC_init_interrupts();
	cpu_irq_enable();
	
	gpio_configure_pin(RESPONSE_A, GPIO_DIR_OUTPUT);
	gpio_configure_pin(RESPONSE_B, GPIO_DIR_OUTPUT);
	gpio_configure_pin(RESPONSE_C, GPIO_DIR_OUTPUT);

	//  stdio init
	stdio_usb_init(&CONFIG_USART_IF);

	// Specify that stdout and stdin should not be buffered.

#if defined(__GNUC__) && defined(__AVR32__)
	setbuf(stdout, NULL);
	setbuf(stdin,  NULL);
#endif
}

/*********************************************************************
User decelerations
*********************************************************************/
int volatile TestA = 0, TestB = 0, TestC = 0;

/*********************************************************************
Interrupt routines
*********************************************************************/
__attribute__((__interrupt__)) 
static void interrupt_AssC(void) 
{ 
	if(gpio_get_pin_interrupt_flag(TEST_A))
	{
		toggleResponsPin(RESPONSE_A);
		gpio_clear_pin_interrupt_flag(TEST_A);
	} 
	if(gpio_get_pin_interrupt_flag(TEST_B))
	{
		toggleResponsPin(RESPONSE_B);
		gpio_clear_pin_interrupt_flag(TEST_B);
	}
	if(gpio_get_pin_interrupt_flag(TEST_C))
	{
		toggleResponsPin(RESPONSE_C);
		gpio_clear_pin_interrupt_flag(TEST_C);
	}		
}

__attribute__((__interrupt__))
static void interrupt_AssD(void)
{
	if(gpio_get_pin_interrupt_flag(TEST_A))
	{
		gpio_clear_pin_interrupt_flag(TEST_A);
		TestA = 1;
	} 
	if(gpio_get_pin_interrupt_flag(TEST_B))
	{
		gpio_clear_pin_interrupt_flag(TEST_B);
		TestB = 1;
	}
	if(gpio_get_pin_interrupt_flag(TEST_C))
	{
		gpio_clear_pin_interrupt_flag(TEST_C);
		TestC = 1;
	}
}

/*********************************************************************
Helper functions
*********************************************************************/
void toggleResponsPin(int pin)
{
	gpio_set_pin_low(pin);
	busy_delay_us(5);

	gpio_set_pin_high(pin);
			
	gpio_toggle_pin(LED0_GPIO);
}

/*********************************************************************
Assignments
*********************************************************************/
void assignmentA()
{
	gpio_set_pin_high(RESPONSE_A);
	
	while (1)
	{
		while (gpio_pin_is_high(TEST_A));
	
		toggleResponsPin(RESPONSE_A);
	}
}

void assignmentB()
{
	gpio_set_pin_high(RESPONSE_A);
	gpio_set_pin_high(RESPONSE_B);
	gpio_set_pin_high(RESPONSE_C);
	
	while (1)
	{
		if (gpio_pin_is_low(TEST_A))
		{
			toggleResponsPin(RESPONSE_A);
		}
		
		if (gpio_pin_is_low(TEST_B))
		{
			toggleResponsPin(RESPONSE_B);
		}
		
		if (gpio_pin_is_low(TEST_C))
		{
			toggleResponsPin(RESPONSE_C);
		}
	}
}

void assignmentC()
{
	gpio_set_pin_high(RESPONSE_A);
	gpio_set_pin_high(RESPONSE_B);
	gpio_set_pin_high(RESPONSE_C);
	
	cpu_irq_disable();
	INTC_register_interrupt(&interrupt_AssC,   AVR32_GPIO_IRQ_3, AVR32_INTC_INT1);
	gpio_enable_pin_interrupt(TEST_A, GPIO_FALLING_EDGE);
	gpio_enable_pin_interrupt(TEST_B, GPIO_FALLING_EDGE);
	gpio_enable_pin_interrupt(TEST_C, GPIO_FALLING_EDGE);
	cpu_irq_enable();
	
	while (1);
}

void assignmentD()
{
	gpio_set_pin_high(RESPONSE_A);
	gpio_set_pin_high(RESPONSE_B);
	gpio_set_pin_high(RESPONSE_C);
	
	cpu_irq_disable();
	INTC_register_interrupt(&interrupt_AssD,   AVR32_GPIO_IRQ_3, AVR32_INTC_INT1);
	gpio_enable_pin_interrupt(TEST_A, GPIO_FALLING_EDGE);
	gpio_enable_pin_interrupt(TEST_B, GPIO_FALLING_EDGE);
	gpio_enable_pin_interrupt(TEST_C, GPIO_FALLING_EDGE);
	cpu_irq_enable();
	
	while (1)
	{
		if (TestA)
		{
			toggleResponsPin(RESPONSE_A);
			TestA = 0;
		}
		if (TestB)
		{
			toggleResponsPin(RESPONSE_B);
			TestB = 0;
		}
		if (TestC)
		{
			toggleResponsPin(RESPONSE_C);
			TestC = 0;
		}
	}
}


/*********************************************************************
Functions
*********************************************************************/
int main (void)
{
	init();
	
	assignmentD();
}

