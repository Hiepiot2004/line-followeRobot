#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "pwm.h"

void app_main() {
    configure_pwm();
    while (1) {
        set_pwm_duty(MAX_DUTY / 4, MAX_DUTY / 4);  // Chạy cả hai động cơ ở tốc độ 50%
        vTaskDelay(pdMS_TO_TICKS(2000));              // Chạy trong 2 giây
        set_pwm_duty(MAX_DUTY/4, 0);      // Động cơ trái 100%, phải 25%
        vTaskDelay(pdMS_TO_TICKS(1000));              // Chạy trong 2 giây

    }
}
