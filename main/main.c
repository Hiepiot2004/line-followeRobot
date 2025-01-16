#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "pwm.h"
#include "driver/adc.h"
#include "esp_adc/adc_oneshot.h"
#include "ldr.h"

// hàm tính giá trị trung bình của quang trở trong 5s để tìm ngưỡng có line và không có line 
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
void line_follower() {
    int max_speed = 100;
    int base_speed = 20;
    int DK_adc[8] = {2537, 3247, 2125, 2245, 2633, 2198, 2383, 2583};
    int sensorState[8];
    int ldr_values[LDR_COUNT];
    static int last_turn_direction = 0; // 0: không rẽ, -1: rẽ trái, 1: rẽ phải

    // Đọc giá trị ADC từ cảm biến
    adc_oneshot_read(adc2_handle, LDR_PINS[4], &ldr_values[0]);
    sensorState[0] = (ldr_values[0] > DK_adc[0]) ? 1 : 0;
    adc_oneshot_read(adc2_handle, LDR_PINS[5], &ldr_values[1]);
    sensorState[1] = (ldr_values[1] > DK_adc[1]) ? 1 : 0;
    adc_oneshot_read(adc2_handle, LDR_PINS[7], &ldr_values[2]);
    sensorState[2] = (ldr_values[2] > DK_adc[2]) ? 1 : 0;
    adc_oneshot_read(adc2_handle, LDR_PINS[6], &ldr_values[3]);
    sensorState[3] = (ldr_values[3] > DK_adc[3]) ? 1 : 0;
    adc_oneshot_read(adc1_handle, LDR_PINS[1], &ldr_values[4]);
    sensorState[4] = (ldr_values[4] > DK_adc[4]) ? 1 : 0;
    adc_oneshot_read(adc1_handle, LDR_PINS[0], &ldr_values[5]);
    sensorState[5] = (ldr_values[5] > DK_adc[5]) ? 1 : 0;
    adc_oneshot_read(adc1_handle, LDR_PINS[3], &ldr_values[6]);
    sensorState[6] = (ldr_values[6] > DK_adc[6]) ? 1 : 0;
    adc_oneshot_read(adc1_handle, LDR_PINS[2], &ldr_values[7]);
    sensorState[7] = (ldr_values[7] > DK_adc[7]) ? 1 : 0;

    // for (int i = 0; i < LDR_COUNT; i++) {
    //     printf("LDR value %d: %d ", i, ldr_values[i]);
    //     printf("state: %d\n", sensorState[i]);
    // }
    // Kiểm tra cảm biến trung tâm (3 và 4)
    if (sensorState[3] == 1 || sensorState[4] == 1) {
        last_turn_direction = 0; // Đặt lại trạng thái rẽ
        if (sensorState[3] == 1 && sensorState[4] == 0) { // Nhích sang trái
            set_pwm_duty(base_speed - 5, base_speed + 5);
        } else if (sensorState[4] == 1 && sensorState[3] == 0) { // Nhích sang phải
            set_pwm_duty(base_speed + 5, base_speed - 5);
        } else if (sensorState[4] == 1 && sensorState[3] == 1) { // Đi thẳng
            set_pwm_duty(base_speed, base_speed);
        }
    } else {
        int a = 0; // Hệ số tăng tốc
        int b = 0; // Hệ số giảm tốc

        if (sensorState[0] || sensorState[1] || sensorState[2]) { // Line ở bên trái
            last_turn_direction = -1; // Ghi nhớ trạng thái rẽ trái
            if (sensorState[2]) {
                a = max_speed / 2;
                b = base_speed / 2;
            } else if (sensorState[1]) {
                a = (2 * max_speed) / 3;
                b = base_speed;
            } else if (sensorState[0]) {
                a = (3*max_speed) /4;
                b = base_speed;
            }
            set_pwm_duty(base_speed - b, base_speed + a); // Quay trái
        } else if (sensorState[5] || sensorState[6] || sensorState[7]) { // Line ở bên phải
            last_turn_direction = 1; // Ghi nhớ trạng thái rẽ phải
            if (sensorState[5]) {
                a = max_speed / 2;
                b = base_speed / 2;
            } else if (sensorState[6]) {
                a = (2 * max_speed) / 3;
                b = base_speed ;
            } else if (sensorState[7]) {
                a = (3*max_speed) /4;
                b = base_speed;
            }
            set_pwm_duty(base_speed + a, base_speed - b); // Quay phải
        } else { 
            // Không thấy line, tiếp tục rẽ theo trạng thái trước đó
            if (last_turn_direction == -1) { // Rẽ trái
                set_pwm_duty(0, 50);
            } else if (last_turn_direction == 1) { // Rẽ phải
                set_pwm_duty(50,0);
            } else {
                set_pwm_duty(max_speed / 4, 0); // Tiếp tục chạy chậm
            }
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
       line_follower();
    //    Read_ADC();
        // PID_COPILOT();
        // PID();
    //   set_pwm_duty(254,254);
    //   vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
  