#include <cstdio>
#include <cstring>
#include <cmath>
#include "../Inc/tflm_app.h"
/* FreeRTOS Includes */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* TensorFlow Lite Micro Includes */
#include "tflm-tree/tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tflm-tree/tensorflow/lite/micro/cortex_m_generic/debug_log_callback.h"
#include "tflm-tree/tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/schema/schema_generated.h"

/* Custom Driver Includes */
#include "timer.h"
#include "dht.h"
#include "gpio.h"
#include "lcd.h"
#include "main.h"
#include "model/model.hh"

/* UART handle for debugging */
extern UART_HandleTypeDef huart2;

/* FreeRTOS Semaphores */
SemaphoreHandle_t xSamplingDone;
SemaphoreHandle_t xProcessingDone;

/* Global Variable to store data and access sensor */
DHT dht = {GPIOA, GPIO_PIN_1};  // Using PA1 for DHT11, adjust as needed
float temp;
char currentClass[10] = "Unknown";

/* TensorFlow Lite Micro objects */
tflite::MicroErrorReporter micro_error_reporter;
tflite::ErrorReporter *error_reporter = &micro_error_reporter;

/* UART Debug Callback */
void DebugCallback(const char *s) {
    HAL_UART_Transmit(&huart2, (uint8_t *)s, strlen(s), 1000);
}

/* Function Declarations for FreeRTOS Tasks */
void pvDataReadingTask(void *params);
void pvDataProcessingTask(void *params);
void pvDisplayTask(void *params);

/* Initialize TensorFlow Lite for the temperature model */
void TFLM_Init(void) {
    /* Register debug callback for error reporting */
    RegisterDebugLogCallback(DebugCallback);

    /* Create model from the flatbuffer */
    const tflite::Model *model = ::tflite::GetModel(temperature_quantized_tflite);

    /* Check model compatibility */
    if (model->version() != TFLITE_SCHEMA_VERSION) {
        TF_LITE_REPORT_ERROR(error_reporter,
                           "Model provided is schema version %d not equal "
                           "to supported version %d.\n",
                           model->version(), TFLITE_SCHEMA_VERSION);
    } else {
        TF_LITE_REPORT_ERROR(error_reporter,
                           "Model Version %d and "
                           "Schema Version %d. Matched!\n",
                           model->version(), TFLITE_SCHEMA_VERSION);
    }
}

int TFLM_App_Init(void) {
    /* Initialize hardware peripherals */
    TimerInit();
    LCD_Init();
    DHT_begin(dht);

    /* Initialize TFLM */
    TFLM_Init();

    /* Create FreeRTOS Semaphores */
    xSamplingDone = xSemaphoreCreateBinary();
    xProcessingDone = xSemaphoreCreateBinary();

    if (xSamplingDone == NULL || xProcessingDone == NULL) {
        return -1; // Failed to create semaphores
    }

    /* Create FreeRTOS Tasks */
    xTaskCreate(pvDataReadingTask, "Sampling Task", 512, NULL, 3, NULL);
    xTaskCreate(pvDataProcessingTask, "Processing Task", 1024, NULL, 2, NULL);
    xTaskCreate(pvDisplayTask, "Display Task", 512, NULL, 1, NULL);

    /* Start the scheduler */
    vTaskStartScheduler();

    /* Should never get here if scheduler started properly */
    return 0;
}

/* Task for reading temperature from DHT11 */
void pvDataReadingTask(void *params) {
    while (true) {
        temp = DHT_ReadTemperature(dht, 0);

        if (temp > 0) {  // Valid reading
            printf("Temperature: %.2f C\r\n", temp);
            xSemaphoreGive(xSamplingDone);
        } else {
            printf("Error reading temperature!\r\n");
        }

        vTaskDelay(pdMS_TO_TICKS(2000));  // Read every 2 seconds
    }
}

/* Task for processing temperature with TFLM */
void pvDataProcessingTask(void *params) {
    /* Setup the model */
    const tflite::Model *model = ::tflite::GetModel(temperature_quantized_tflite);

    /* Define operations needed for this model */
    using TempOpResolver = tflite::MicroMutableOpResolver<2>;
    TempOpResolver resolver;
    resolver.AddFullyConnected();
    resolver.AddSoftmax();

    /* Set up tensor arena for model tensors */
    const int tensor_arena_size = 4096;
    uint8_t tensor_arena[tensor_arena_size];

    /* Set up interpreter */
    tflite::MicroInterpreter interpreter(model, resolver, tensor_arena, tensor_arena_size);
    interpreter.AllocateTensors();

    /* Get pointers to input and output tensors */
    TfLiteTensor *input = interpreter.input(0);
    TfLiteTensor *output = interpreter.output(0);

    /* Model parameters - adjust these based on your training! */
    const float input_mean = 25.0f;    // Approximate mean of your training data
    const float input_std = 6.0f;      // Approximate std dev of your training data

    while (true) {
        xSemaphoreTake(xSamplingDone, portMAX_DELAY);

        if (input != nullptr) {
            /* Normalize and quantize the input */
            float scaled_temp = (temp - input_mean) / input_std;
            input->data.int8[0] = static_cast<int8_t>(scaled_temp / input->params.scale + input->params.zero_point);

            /* Run inference */
            if (interpreter.Invoke() == kTfLiteOk) {
                /* Process the output */
                float predictions[3];

                for (int i = 0; i < 3; i++) {
                    predictions[i] = (output->data.int8[i] - output->params.zero_point) * output->params.scale;
                }

                /* Determine class with highest probability */
                const char *labels[] = {"Cold", "Warm", "Hot"};
                int class_index = 0;
                float max_prob = predictions[0];

                for (int i = 1; i < 3; i++) {
                    if (predictions[i] > max_prob) {
                        max_prob = predictions[i];
                        class_index = i;
                    }
                }

                /* Store result and notify display task */
                strcpy(currentClass, labels[class_index]);
                printf("Predicted: %s (Confidence: %.2f)\r\n", currentClass, max_prob);
                printf("--------------------------------------------------\r\n");

                xSemaphoreGive(xProcessingDone);
            } else {
                printf("[ERROR] Inference failed!\r\n");
            }
        } else {
            printf("[ERROR] Input tensor is NULL!\r\n");
        }
    }
}

/* Task for updating the LCD display */
void pvDisplayTask(void *params) {
    while (true) {
        xSemaphoreTake(xProcessingDone, portMAX_DELAY);

        /* Update the LCD with temperature and classification */
        LCD_DisplayTemp(temp, currentClass);

        vTaskDelay(pdMS_TO_TICKS(500));  // Short delay to stabilize display
    }
}

/* Required FreeRTOS hook for stack overflow detection */
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {
    /* Print error message and halt */
    printf("Stack overflow in task: %s\r\n", pcTaskName);
    for (;;);
}
