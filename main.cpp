#include "main.h"

#include "FreeRTOS.h"
#include "LOS_Actuators.hpp"
#include "SM.hpp"
#include "cmsis_os2.h"
#include "task.h"

void SMOperationTask(void *pvParameters);
const static auto SM_PERIOD_MS = 5;

int main() {
    losInit();

    TaskHandle_t SM_handle = NULL;

    xTaskCreate(SMOperationTask, "SM Thread", 400U, NULL, osPriorityNormal, &SM_handle);

    losKernelStart();

    // should not get here bec losInit() starts the scheduler
    while (1) {
    }

    return 0;
}

void SMOperationTask(void *pvParameters) {
    SM::SystemManager SM_instance;

    TickType_t xNextWakeTime;
    xNextWakeTime = xTaskGetTickCount();
    while (true) {
        SM_instance.execute();
        vTaskDelayUntil(&xNextWakeTime, SM_PERIOD_MS);
    }
}
