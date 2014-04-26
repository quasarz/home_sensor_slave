/*
 * dht.h
 *
 *  Created on: Apr 20, 2014
 *      Author: wasan
 */

#ifndef DHT_H_
#define DHT_H_

#define DHT_OK 0
#define DHT_ERROR_TIMEOUT 1
#define DHT_ERROR_CHECKSUM 2
#define DHT_TIMEOUT 1000

#include "stm32f4xx.h"

typedef struct {
	double humidity;
	double temperature;
} xDHTData;


uint8_t ucDHTRead(xDHTData* pxDHTData, GPIO_TypeDef* pxGPIOx, uint16_t usGPIOPin);


#endif /* DHT_H_ */
