#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "pwm.h"
#include "driver/adc.h"
#include "esp_adc/adc_oneshot.h"
#include "ldr.h"


void Read_ADC() {
    int max_a =100;
    int base_speed = 20;
    int DK_adc[8] = {2251, 3094, 2620, 2654, 2937, 3289, 2696, 2514};
    int sensorState[8];
    int ldr_values[LDR_COUNT];
    adc_oneshot_read(adc2_handle, LDR_PINS[4], &ldr_values[0]);
    sensorState[0] = (ldr_values[0] > DK_adc[4]) ? 1 : 0; // 1: có line, 0: không có line
    adc_oneshot_read(adc2_handle, LDR_PINS[5], &ldr_values[1]);
    sensorState[1] = (ldr_values[1] > DK_adc[5]) ? 1 : 0; // 1: có line, 0: không có line
    adc_oneshot_read(adc2_handle, LDR_PINS[7], &ldr_values[2]);
    sensorState[2] = (ldr_values[2] > DK_adc[7]) ? 1 : 0; // 1: có line, 0: không có line
    adc_oneshot_read(adc2_handle, LDR_PINS[6], &ldr_values[3]);
    sensorState[3] = (ldr_values[3] > DK_adc[6]) ? 1 : 0; // 1: có line, 0: không có line

    adc_oneshot_read(adc1_handle, LDR_PINS[1], &ldr_values[4]);
    sensorState[4] = (ldr_values[4] > DK_adc[1]) ? 1 : 0; // 1: có line, 0: không có line
    adc_oneshot_read(adc1_handle, LDR_PINS[0], &ldr_values[5]);
    sensorState[5] = (ldr_values[5] > DK_adc[0]) ? 1 : 0; // 1: có line, 0: không có line
    adc_oneshot_read(adc1_handle, LDR_PINS[3], &ldr_values[6]);
    sensorState[6] = (ldr_values[6] > DK_adc[3]) ? 1 : 0; // 1: có line, 0: không có line
    adc_oneshot_read(adc1_handle, LDR_PINS[2], &ldr_values[7]);
    sensorState[7] = (ldr_values[7] > DK_adc[2]) ? 1 : 0; // 1: có line, 0: không có line

    for (int i = 0; i < LDR_COUNT; i++) {
        printf("LDR value %d: %d ", i, ldr_values[i]);
        printf("state: %d\n", sensorState[i]);
    }
    for (int i = 0; i < LDR_COUNT; i++) {
            printf("LDR value %d: %d ", i, ldr_values[i]);
            printf("state: %d\n", sensorState[i]);
        }
        if (sensorState[3] == 1 && sensorState[4] == 1) {
        set_pwm_duty(20, 20);
        }
        else {
            // Không có line -> dừng xe
            set_pwm_duty(0, 0);
        }
    vTaskDelay(pdMS_TO_TICKS(500)); // Đọc giá trị mỗi giây

}
void tinh_trung_binh(int trung_binh[LDR_COUNT]) {
    int tong[LDR_COUNT] = {0};
    int dem = 0;
    unsigned long start_time = xTaskGetTickCount();

    while (xTaskGetTickCount() - start_time < pdMS_TO_TICKS(5000)) {
        int ldr_values[LDR_COUNT];

        // Đọc giá trị từ các kênh của ADC1
        for (int i = 0; i < 4; i++) {
            adc_oneshot_read(adc1_handle, LDR_PINS[i], &ldr_values[i]);
            tong[i] += ldr_values[i];
        }
        // Đọc giá trị từ các kênh của ADC2
        for (int i = 4; i < LDR_COUNT; i++) {
            adc_oneshot_read(adc2_handle, LDR_PINS[i], &ldr_values[i]);
            tong[i] += ldr_values[i];
        }

        dem++;
        vTaskDelay(pdMS_TO_TICKS(10)); // Độ trễ giữa các lần đọc (10ms)
    }

    // Tính giá trị trung bình
    for (int i = 0; i < LDR_COUNT; i++) {
        trung_binh[i] = tong[i] / dem;
    }
}
void app_main() {
    pwm_init();
    adc_init();
    
    while (1) {
       Read_ADC();
    }
}
