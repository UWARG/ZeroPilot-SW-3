#include "main.h"
#include "FreeRTOS.h"
#include "cmsis_os2.h"
#include "task.h"
#include "LOS_Actuators.hpp"

osThreadId blinkyTestHandle;
osThreadId losActuatorsTestHandle;

void StartBlinkyTest(void const * argument);
void StartLosActuatorsTest(void const * argument);


int main()
{
    
/// Attributes structure for thread.
typedef struct {
  const char                   *name;   ///< name of the thread
  uint32_t                 attr_bits;   ///< attribute bits
  void                      *cb_mem;    ///< memory for control block
  uint32_t                   cb_size;   ///< size of provided memory for control block
  void                   *stack_mem;    ///< memory for stack
  uint32_t                stack_size;   ///< size of stack
  osPriority_t              priority;   ///< initial thread priority (default: osPriorityNormal)
  TZ_ModuleId_t            tz_module;   ///< TrustZone module identifier
  uint32_t                  reserved;   ///< reserved (must be 0)
} osThreadAttr_t;osThreadId_t osThreadNew (osThreadFunc_t func, void *argument, const osThreadAttr_t *attr);

    /* Init scheduler */
    osKernelInitialize();

    blinkyTestHandle = osThreadCreate();

    osThreadAttr_t blinkyTest = {
        .name = "start_blinky",
        .stack_size = 128,
        .priority = osPriorityNormal
    };

    osThreadAttr_t losActTest = {
        .name = "los_act",
        .stack_size = 1024,
        .priority = osPriorityNormal
    };

    osThreadNew (blinkyTest, NULL, &blinkyTest);
    osThreadNew (losActuatorsTest, NULL, &losActuatorsTest);

    losInit();

    //should not get here bec losInit() starts the scheduler
    while(1){}

    return 0;
}


void StartBlinkyTest(void const * argument)
{
    for(;;)
    {
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
        HAL_Delay(250);
        osDelay(1);
    }
}

void StartLosActuatorsTest(void const * argument)
{
    for (;;)
    {
        Los_Actuators::getInstance().set(0, 50);
        osDelay(2000);
        Los_Actuators::getInstance().set(0, 100);
        osDelay(2000);
        Los_Actuators::getInstance().set(0, 0);
        osDelay(2000);
    }

}