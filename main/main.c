#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "pwm.h"
#include "driver/adc.h"
#include "esp_adc/adc_oneshot.h"
#include "ldr.h"

void app_main() {
    configure_pwm();
    configure_adc();

    while (1) {
        int ldr_values[LDR_COUNT];
        // Đọc giá trị từ các kênh của ADC1
        for (int i = 0; i < 4; i++) {
            adc_oneshot_read(adc1_handle, LDR_PINS[i], &ldr_values[i]);
        }
        // Đọc giá trị từ các kênh của ADC2
        for (int i = 4; i < LDR_COUNT; i++) {
            adc_oneshot_read(adc2_handle, LDR_PINS[i], &ldr_values[i]);
        }

        // Hiển thị giá trị lên monitor
        for (int i = 0; i < LDR_COUNT; i++) {
            printf("LDR value %d: %d\n", i, ldr_values[i]);
        }

        vTaskDelay(pdMS_TO_TICKS(1000)); // Đọc giá trị mỗi giây
    }
    
}
