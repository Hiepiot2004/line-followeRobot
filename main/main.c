#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "pwm.h"
#include "driver/adc.h"
#include "esp_adc/adc_oneshot.h"
#include "ldr.h"

void read_adc() {
    int DK_adc[8] = {2022, 2384, 2388, 2559, 2233, 2254, 2559, 2553};
    int sensorState[8];
    int max_a =100;
    int base_speed = 20;
    int ldr_values[LDR_COUNT];
        // Đọc giá trị từ các kênh của ADC1
        for (int i = 0; i < 4; i++) {
            adc_oneshot_read(adc1_handle, LDR_PINS[i], &ldr_values[i]);
            sensorState[i] = (ldr_values[i] > DK_adc[i]) ? 1 : 0; // 1: có line, 0: không có line
        }
        // Đọc giá trị từ các kênh của ADC2
        for (int i = 4; i < LDR_COUNT; i++) {
            adc_oneshot_read(adc2_handle, LDR_PINS[i], &ldr_values[i]);
            sensorState[i] = (ldr_values[i] > DK_adc[i]) ? 1 : 0; // 1: có line, 0: không có line
        }

        // Hiển thị giá trị lên monitor
        for (int i = 0; i < LDR_COUNT; i++) {
            printf("LDR value %d: %d\n", i, ldr_values[i]);
            printf("state: %d\n", sensorState[i]);
        }
        if (sensorState[3] == 1 && sensorState[4] == 1) {
        set_pwm_duty(20, 20);
        }
         else {
        int a = 0; // Hệ số tăng tốc
        int slow_down = 0; // Hệ số giảm tốc

        if (sensorState[0] || sensorState[1] || sensorState[2]) {
            // Line nằm ở bên trái
            if (sensorState[2]) {
                a = max_a / 3;
                slow_down = base_speed / 3; // Giảm nhẹ bánh không rẽ
            } else if (sensorState[1]) {
                a = (2 * max_a) / 3;
                slow_down = base_speed / 3;
            } else if (sensorState[0]) {
                a = max_a;
                slow_down = 2 * base_speed / 3; // Giảm nhiều hơn để hỗ trợ rẽ gấp
            }
            set_pwm_duty(base_speed - a - slow_down, base_speed + a); // Quay trái
        }
        else if (sensorState[5] || sensorState[6] || sensorState[7]) {
            // Line nằm ở bên phải
            if (sensorState[5]) {
                a = max_a / 3;
                slow_down = base_speed / 3; // Giảm nhẹ bánh không rẽ
            } else if (sensorState[6]) {
                a = (2 * max_a) / 3;
                slow_down = base_speed / 3;
            } else if (sensorState[7]) {
                a = max_a;
                slow_down = 2 * base_speed / 3; // Giảm nhiều hơn để hỗ trợ rẽ gấp
            }
            set_pwm_duty(base_speed + a, base_speed - a - slow_down); // Quay phải
        }
        else {
            // Không có line -> dừng xe
            set_pwm_duty(0, 0);
        }
        vTaskDelay(pdMS_TO_TICKS(200)); // Đọc giá trị mỗi giây
    
        }
        
}

void app_main() {
    pwm_init();
    adc_init();

    while (1) {
        read_adc();
    }
}
