#include "dht.h"
#include "../../TFLM_App/Inc/timer.h"




void DHT_begin(DHT dht) {
    // Initialize GPIO for DHT11
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // Enable GPIO clock if not already enabled
    if (dht.port == GPIOA) __HAL_RCC_GPIOA_CLK_ENABLE();
    else if (dht.port == GPIOB) __HAL_RCC_GPIOB_CLK_ENABLE();
    else if (dht.port == GPIOC) __HAL_RCC_GPIOC_CLK_ENABLE();
    // Add more ports if needed

    // Configure GPIO pin
    GPIO_InitStruct.Pin = dht.pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(dht.port, &GPIO_InitStruct);

    // Set initial state
    HAL_GPIO_WritePin(dht.port, dht.pin, GPIO_PIN_SET);

    // Wait for the sensor to stabilize
    HAL_Delay(1000);
}

static int DHT_readData(DHT dht, uint8_t data[5]) {
    uint8_t laststate = GPIO_PIN_SET;
    uint8_t counter = 0;
    uint8_t j = 0, i;
    uint32_t start;

    // Empty data array
    for (i = 0; i < 5; i++) data[i] = 0;

    // Configure pin as output
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = dht.pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(dht.port, &GPIO_InitStruct);

    // Send start signal
    HAL_GPIO_WritePin(dht.port, dht.pin, GPIO_PIN_RESET);
    HAL_Delay(18);  // At least 18ms for DHT11
    HAL_GPIO_WritePin(dht.port, dht.pin, GPIO_PIN_SET);
    delay_us(40);   // Wait 20-40us

    // Configure pin as input
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(dht.port, &GPIO_InitStruct);

    // Read data
    for (i = 0; i < 85; i++) {
        counter = 0;
        while (HAL_GPIO_ReadPin(dht.port, dht.pin) == laststate) {
            counter++;
            delay_us(1);
            if (counter == 100) return -1;  // Timeout
        }
        laststate = HAL_GPIO_ReadPin(dht.port, dht.pin);

        if (i >= 4 && i % 2 == 0) {
            data[j/8] <<= 1;
            if (counter > 16) data[j/8] |= 1;
            j++;
        }
    }

    // Verify checksum
    if (j >= 40 && ((data[0] + data[1] + data[2] + data[3]) & 0xFF) == data[4])
        return 0;  // Success

    return -1;
}

float DHT_ReadTemperature(DHT dht, uint8_t force) {
    uint8_t data[5] = {0};

    if (DHT_readData(dht, data) != 0) return -1;

    float temperature = (float)data[2];  // DHT11 only reports integer values

    return temperature;
}

float DHT_ReadHumidity(DHT dht) {
    uint8_t data[5] = {0};

    if (DHT_readData(dht, data) != 0) return -1;

    float humidity = (float)data[0];  // DHT11 only reports integer values

    return humidity;
}
