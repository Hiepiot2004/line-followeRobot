#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "pwm.h"
#include "driver/adc.h"
#include "esp_adc/adc_oneshot.h"
#include "ldr.h"


void Read_ADC() {
    int max_speed =255;
    int base_speed = 20;
    // int DK_adc[8] = {2937, 3289, 2514, 2696, 3094, 2251, 2654, 2620};
    int DK_adc[8] = {2537, 3247, 2125, 2245, 2633, 2198, 2383, 2583};
    int sensorState[8];
    int ldr_values[LDR_COUNT];
    // đọc giá trị ADC1
    adc_oneshot_read(adc2_handle, LDR_PINS[4], &ldr_values[0]);
    sensorState[0] = (ldr_values[0] > DK_adc[0]) ? 1 : 0; // 1: có line, 0: không có line
    adc_oneshot_read(adc2_handle, LDR_PINS[5], &ldr_values[1]);
    sensorState[1] = (ldr_values[1] > DK_adc[1]) ? 1 : 0; // 1: có line, 0: không có line
    adc_oneshot_read(adc2_handle, LDR_PINS[7], &ldr_values[2]);
    sensorState[2] = (ldr_values[2] > DK_adc[2]) ? 1 : 0; // 1: có line, 0: không có line
    adc_oneshot_read(adc2_handle, LDR_PINS[6], &ldr_values[3]);
    sensorState[3] = (ldr_values[3] > DK_adc[3]) ? 1 : 0; // 1: có line, 0: không có line
    // đọc giá trị ADC2
    adc_oneshot_read(adc1_handle, LDR_PINS[1], &ldr_values[4]);
    sensorState[4] = (ldr_values[4] > DK_adc[4]) ? 1 : 0; // 1: có line, 0: không có line
    adc_oneshot_read(adc1_handle, LDR_PINS[0], &ldr_values[5]);
    sensorState[5] = (ldr_values[5] > DK_adc[5]) ? 1 : 0; // 1: có line, 0: không có line
    adc_oneshot_read(adc1_handle, LDR_PINS[3], &ldr_values[6]);
    sensorState[6] = (ldr_values[6] > DK_adc[6]) ? 1 : 0; // 1: có line, 0: không có line
    adc_oneshot_read(adc1_handle, LDR_PINS[2], &ldr_values[7]);
    sensorState[7] = (ldr_values[7] > DK_adc[7]) ? 1 : 0; // 1: có line, 0: không có line

    // for (int i = 0; i < LDR_COUNT; i++) {
    //     printf("LDR value %d: %d ", i, ldr_values[i]);
    //     printf("state: %d\n", sensorState[i]);
    // }
    if (sensorState[3] == 1 || sensorState[4] == 1) {
        if(sensorState[3] == 1 && sensorState[4] == 0){ // nhích sang trái
            set_pwm_duty(base_speed - 5,base_speed+5);
        }
        else if(sensorState[4] == 1 && sensorState[3] == 0){ // nhích sang phải 
            set_pwm_duty(base_speed+5,base_speed-5);
        }
        else if(sensorState[4] == 1 && sensorState[3] == 1){ // đi thẳng 
            set_pwm_duty(base_speed,base_speed);
        }
    }
    else { 
    int a = 0; // Hệ số tăng tốc
        int slow_down = 0; // Hệ số giảm tốc

        if (sensorState[0] || sensorState[1] || sensorState[2]) {
            // Line nằm ở bên trái
            if (sensorState[2]) {
                a = 2 * max_speed;
                slow_down = base_speed/2; // Giảm nhẹ bánh không rẽ
            } else if (sensorState[1]) {
                a = (2 * max_speed) / 3;
                slow_down = base_speed ;
            } else if (sensorState[0]) {
                a = max_speed;
                slow_down = base_speed; // Giảm nhiều hơn để hỗ trợ rẽ gấp
            }
            set_pwm_duty(base_speed - slow_down, base_speed + a); // Quay trái
        
        }
        else{
            set_pwm_duty(0,max_speed / 3);
        }
    }
vTaskDelay(pdMS_TO_TICKS(10)); // Đọc giá trị mỗi giây

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
