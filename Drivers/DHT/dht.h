#ifndef DHT_H
#define DHT_H

#include "main.h"

typedef struct {
    GPIO_TypeDef* port;
    uint16_t pin;
} DHT;

void DHT_begin(DHT dht);
float DHT_ReadTemperature(DHT dht, uint8_t force);
float DHT_ReadHumidity(DHT dht);

#endif
