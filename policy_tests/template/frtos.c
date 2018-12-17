#include <stdarg.h>
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"

#include "utils.h"
#include "test.h"

void main_task(void *argument);
unsigned long sys_GetWallTimestampUs(void);
void printk(const char* s, ...);
/*
 * Test wrapper for dover-os that calls the test case in test.c
 */
extern int test_main(void);
void main_task(void*);
void main_task(void *argument)
{
  test_main();
  vTaskDelay(1);

  for( ;; );

  // this may need changes to portable layer
  vTaskEndScheduler();
}

int main(void){
  xTaskCreate(main_task, "Main task", 1000, NULL, 1, NULL);
  
  vTaskStartScheduler();

  // never reached
  return 0;
}

/*
 * Provides wrapper for printf for all test output
 */
int t_printf(const char *s, ...){
  va_list vl;

  va_start(vl, s);
  printf_uart(s, vl);
  va_end(vl);

  return 0;
}


/* ---------------------------- stuff to make FreeRTOS work ---------------------- */

/* 
 * This function must return uS timestamp with an order of magnitude
 * more resolution than FreeRTOS tick
 */
extern uint32_t uiPortGetWallTimestampUs(void);

unsigned long sys_GetWallTimestampUs(void);

unsigned long sys_GetWallTimestampUs(void)
{
    /* TBD on real FPGA hw */
  return uiPortGetWallTimestampUs();
}

void printk(const char*, ...);
void printk(const char* s, ...)
{
  va_list vl;

  va_start(vl, s);
  printf_uart(s, vl);
  va_end(vl);


}


void vApplicationMallocFailedHook( void );
void vApplicationMallocFailedHook( void )
{
  t_printf("ERROR: Out of memory\n");
  taskDISABLE_INTERRUPTS();
  for( ;; );
}

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName );
void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
  ( void ) pcTaskName;
  ( void ) pxTask;
  t_printf("ERROR: Stack Overflow\n");
  taskDISABLE_INTERRUPTS();
  for( ;; );
}

void vApplicationTickHook( void );
void vApplicationTickHook( void ) { }
void vApplicationIdleHook( void );
void vApplicationIdleHook( void ) { }
