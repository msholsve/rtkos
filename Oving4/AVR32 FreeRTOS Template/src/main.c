/*This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief FreeRTOS Real Time Kernel example.
 *
 * Creates all the demo application tasks, then starts the scheduler.  The WEB
 * documentation provides more details of the demo application tasks.
 *
 * Main. c also creates a task called "Check".  This only executes every three
 * seconds but has the highest priority so is guaranteed to get processor time.
 * Its main function is to check that all the other tasks are still operational.
 * Each task that does not flash an LED maintains a unique count that is
 * incremented each time the task successfully completes its function.  Should
 * any error occur within such a task the count is permanently halted.  The
 * check task inspects the count of each task to ensure it has changed since
 * the last time the check task executed.  If all the count variables have
 * changed all the tasks are still executing error free, and the check task
 * toggles an LED.  Should any task contain an error at any time the LED toggle
 * will stop.
 *
 * The LED flash and communications test tasks do not maintain a count.
 *
 * - Compiler:           IAR EWAVR32 and GNU GCC for AVR32
 * - Supported devices:  All AVR32 devices with GPIO.
 * - AppNote:
 *
 * \author               Atmel Corporation: http://www.atmel.com \n
 *                       Support and FAQ: http://support.atmel.no/
 *
 *****************************************************************************/

/*
    FreeRTOS V7.0.0 - Copyright (C) 2011 Real Time Engineers Ltd.
	

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS tutorial books are available in pdf and paperback.        *
     *    Complete, revised, and edited pdf reference manuals are also       *
     *    available.                                                         *
     *                                                                       *
     *    Purchasing FreeRTOS documentation will not only help you, by       *
     *    ensuring you get running as quickly as possible and with an        *
     *    in-depth knowledge of how to use FreeRTOS, it will also help       *
     *    the FreeRTOS project to continue with its mission of providing     *
     *    professional grade, cross platform, de facto standard solutions    *
     *    for microcontrollers - completely free of charge!                  *
     *                                                                       *
     *    >>> See http://www.FreeRTOS.org/Documentation for details. <<<     *
     *                                                                       *
     *    Thank you for using FreeRTOS, and thank you for your support!      *
     *                                                                       *
    ***************************************************************************


    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation AND MODIFIED BY the FreeRTOS exception.
    >>>NOTE<<< The modification to the GPL is included to allow you to
    distribute a combined work that includes FreeRTOS without being obliged to
    provide the source code for proprietary components outside of the FreeRTOS
    kernel.  FreeRTOS is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
    or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
    more details. You should have received a copy of the GNU General Public
    License and the FreeRTOS license exception along with FreeRTOS; if not it
    can be viewed here: http://www.freertos.org/a00114.html and also obtained
    by writing to Richard Barry, contact details for whom are available on the
    FreeRTOS WEB site.

    1 tab == 4 spaces!

    http://www.FreeRTOS.org - Documentation, latest information, license and
    contact details.

    http://www.SafeRTOS.com - A version that is certified for use in safety
    critical systems.

    http://www.OpenRTOS.com - Commercial support, development, porting,
    licensing and training services.
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <intc.h>
#include <interrupt.h>
#include <sleepmgr.h>
#include <uc3/sleepmgr.h>
#include <gpio.h>
#include <flashc.h>
#include <power_clocks_lib.h>
#include <sleep.h>
#include <sysclk.h>
#include <tc.h>
#include <usart.h>
#include <udi_cdc.h>
#include <usb_protocol_cdc.h>
#include <udc.h>
#include <udd.h>
#include <compiler.h>
#include <status_codes.h>
#include <board.h>
#include <stdio_usb.h>


// defines for USB UART
#define CONFIG_USART_IF (AVR32_USART2)

// include FreeRTOS header files
#include "FreeRTOS.h"
#include "task.h"

// defines for BRTT interface
#define TEST_A AVR32_PIN_PA31
#define RESPONSE_A AVR32_PIN_PA30
#define TEST_B AVR32_PIN_PA29
#define RESPONSE_B AVR32_PIN_PA28
#define TEST_C AVR32_PIN_PA27
#define RESPONSE_C AVR32_PIN_PB00

void init()
{
	// board init
	board_init();
	
#if UC3L
	// clock frequencies
	#define EXAMPLE_TARGET_DFLL_FREQ_HZ     96000000  // DFLL target frequency, in Hz
	#define EXAMPLE_TARGET_MCUCLK_FREQ_HZ   12000000  // MCU clock target frequency, in Hz
	#define EXAMPLE_TARGET_PBACLK_FREQ_HZ   12000000  // PBA clock target frequency, in Hz

	// parameters to pcl_configure_clocks().
	static scif_gclk_opt_t gc_dfllif_ref_opt = { SCIF_GCCTRL_SLOWCLOCK, 0, false};
	static pcl_freq_param_t pcl_dfll_freq_param = {
		.main_clk_src = PCL_MC_DFLL0,
		.cpu_f        = EXAMPLE_TARGET_MCUCLK_FREQ_HZ,
		.pba_f        = EXAMPLE_TARGET_PBACLK_FREQ_HZ,
		.pbb_f        = EXAMPLE_TARGET_PBACLK_FREQ_HZ,
		.dfll_f       = EXAMPLE_TARGET_DFLL_FREQ_HZ,
		.pextra_params = &gc_dfllif_ref_opt
	};
	pcl_configure_clocks(&pcl_dfll_freq_param);
#else
	pcl_switch_to_osc(PCL_OSC0, FOSC0, OSC0_STARTUP);
#endif	
	
	gpio_configure_pin(RESPONSE_A, GPIO_DIR_OUTPUT);
	gpio_configure_pin(RESPONSE_B, GPIO_DIR_OUTPUT);
	gpio_configure_pin(RESPONSE_C, GPIO_DIR_OUTPUT);
	
	// stdio init
	stdio_usb_init(&CONFIG_USART_IF);

	// Specify that stdout and stdin should not be buffered.

#if defined(__GNUC__) && defined(__AVR32__)
	setbuf(stdout, NULL);
	setbuf(stdin,  NULL);
#endif
}

/*********************************************************************
Helper functions
*********************************************************************/
void toggleResponsPin(int pin)
{
	gpio_set_pin_low(pin);
	vTaskDelay(1);

	gpio_set_pin_high(pin);
	
	//gpio_toggle_pin(LED0_GPIO);
}

/*********************************************************************
User decelerations
*********************************************************************/

// Assignment A
static void TaskA(void *pvParameters);
static void TaskB(void *pvParameters);

// Assignment B
static void TestA(void *pvParameters);
static void TestB(void *pvParameters);
static void TestC(void *pvParameters);
static void YourBoss(void *pvParameters);

/*********************************************************************
Functions
*********************************************************************/
int main()
{
	// initialize
	init();
	
	// start code from here
	
	// Assignment A
	/*xTaskCreate( TaskA, (signed char * ) "TaskA", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
	xTaskCreate( TaskB, (signed char * ) "TaskB", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);*/
	
	// Assignment B
	xTaskCreate( TestA, (signed char * ) "TestA", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 2, NULL);
	xTaskCreate( TestB, (signed char * ) "TestB", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 2, NULL);
	xTaskCreate( TestC, (signed char * ) "TestC", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 2, NULL);
	xTaskCreate( YourBoss, (signed char * ) "YourBoss", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);

	// Start the scheduler, anything after this will not run.
	vTaskStartScheduler();
}

/************************************************************************/
/* Assignment A                                                         */
/************************************************************************/

static void TaskA(void *pvParameters)
{
	const portTickType xDelay = 200 / portTICK_RATE_MS;
	
	while(1)
	{
		gpio_toggle_pin(LED0_GPIO);
		printf("tick\n");
		
		vTaskDelay(xDelay);
	}
}

static void TaskB(void *pvParameters)
{
	const volatile portTickType xDelay = 500 / portTICK_RATE_MS;
	
	while(1)
	{
		gpio_toggle_pin(LED1_GPIO);
		printf("tick\n");
		
		vTaskDelay(xDelay);
	}
}

/************************************************************************/
/* Assignment B                                                         */
/************************************************************************/

/*
static void TestA(void *pvParameters)
{
	while(1)
	{
		if (gpio_pin_is_low(TEST_A))
		{
			toggleResponsPin(RESPONSE_A);
		}
	}
}

static void TestB(void *pvParameters)
{
	while(1)
	{
		if (gpio_pin_is_low(TEST_B))
		{
			toggleResponsPin(RESPONSE_B);
		}
	}
}

static void TestC(void *pvParameters)
{
	while(1)
	{
		if (gpio_pin_is_low(TEST_C))
		{
			toggleResponsPin(RESPONSE_C);
		}
	}
}*/

static void YourBoss(void *pvParameters)
{
	int i;
	double volatile dummy;
	
	while(1)
	{
		for (i = 0; i < 10000; i++)
		{
			dummy = 2.5 * i;
		}
		gpio_toggle_pin(LED0_GPIO);
	}
}

/************************************************************************/
/* Assignment C                                                         */
/************************************************************************/


static void TestA(void *pvParameters)
{
	portTickType xLastWakeTime = xTaskGetTickCount();
	while(1)
	{
		vTaskDelayUntil(&xLastWakeTime, 5/portTICK_RATE_MS);
		if (gpio_pin_is_low(TEST_A))
		{
			toggleResponsPin(RESPONSE_A);
		}
	}
}

static void TestB(void *pvParameters)
{
	portTickType xLastWakeTime = xTaskGetTickCount();
	while(1)
	{
		vTaskDelayUntil(&xLastWakeTime, 5/portTICK_RATE_MS);
		if (gpio_pin_is_low(TEST_B))
		{
			toggleResponsPin(RESPONSE_B);
		}
	}
}

static void TestC(void *pvParameters)
{
	portTickType xLastWakeTime = xTaskGetTickCount();
	
	while(1)
	{
		vTaskDelayUntil(&xLastWakeTime, 5/portTICK_RATE_MS);
		if (gpio_pin_is_low(TEST_C))
		{
			toggleResponsPin(RESPONSE_C);
		}
	}
}
