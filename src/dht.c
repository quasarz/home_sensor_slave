/*
 * dht.c
 *
 *  Created on: Apr 20, 2014
 *      Author: wasan
 */

#include "FreeRTOS.h"
#include "task.h"
#include "dht.h"

uint8_t static ucReadSensor(uint8_t* pucData, GPIO_TypeDef* pxGPIOx, uint16_t usGPIOPin);
uint8_t static ucTranslateSensorData(uint8_t* pucData, xDHTData* pxDHTData);

uint8_t ucDHTRead(xDHTData* pxDHTData, GPIO_TypeDef* pxGPIOx, uint16_t usGPIOPin) {
	uint8_t ucData[5];

	uint8_t ucStatus = ucReadSensor(ucData, pxGPIOx, usGPIOPin);
	if(ucStatus != DHT_OK) {
		ucStatus = ucTranslateSensorData(ucData, pxDHTData);
	}
	return ucStatus;
}


uint8_t ucReadSensor(uint8_t* pucData, GPIO_TypeDef* pxGPIOx, uint16_t usGPIOPin) {
	uint8_t bitPos = 7;
	uint8_t byteCount = 0;

	uint8_t i;
	for(i = 0; i < 5; i++) {
		pucData[i] = 0;
	}

	GPIO_InitTypeDef xGPIOxInit;

	GPIO_StructInit(&xGPIOxInit);
	xGPIOxInit.GPIO_Pin = usGPIOPin;
	xGPIOxInit.GPIO_Speed = GPIO_Speed_2MHz;
	xGPIOxInit.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(pxGPIOx, &xGPIOxInit);

	GPIO_ResetBits(pxGPIOx, usGPIOPin);
	vTaskDelay(20);
	GPIO_SetBits(pxGPIOx, usGPIOPin);

	GPIO_StructInit(&xGPIOxInit);
	xGPIOxInit.GPIO_Pin = usGPIOPin;
	xGPIOxInit.GPIO_Speed = GPIO_Speed_2MHz;
	xGPIOxInit.GPIO_Mode = GPIO_Mode_IN;
	GPIO_Init(pxGPIOx, &xGPIOxInit);

	uint16_t ucCount = 0;
	while(GPIO_ReadInputDataBit(pxGPIOx, usGPIOPin)) {
		ucCount++;
		if(ucCount > DHT_TIMEOUT) return DHT_ERROR_TIMEOUT;
	}

	ucCount = 0;
	while(!GPIO_ReadInputDataBit(pxGPIOx, usGPIOPin)) {
		ucCount++;
		if(ucCount > DHT_TIMEOUT) return DHT_ERROR_TIMEOUT;
	}

	ucCount = 0;
	while(GPIO_ReadInputDataBit(pxGPIOx, usGPIOPin)) {
		ucCount++;
		if(ucCount > DHT_TIMEOUT) return DHT_ERROR_TIMEOUT;
	}

/*	begin read data 40 bits */
	for(i = 0; i < 40; i++) {
		uint16_t ucStartCount = 0;
		while(!GPIO_ReadInputDataBit(pxGPIOx, usGPIOPin)) {
			ucStartCount++;
			if(ucStartCount > DHT_TIMEOUT) return DHT_ERROR_TIMEOUT;
		}
		uint16_t ucDataCount = 0;
		while(GPIO_ReadInputDataBit(pxGPIOx, usGPIOPin)) {
			ucDataCount++;
			if(ucDataCount > DHT_TIMEOUT) return DHT_ERROR_TIMEOUT;
		}
		if(ucStartCount < ucDataCount) {
			pucData[byteCount] |= (1 << bitPos);
		}
		if(bitPos == 0) {
			bitPos = 7;
			byteCount++;
		}
		else {
			bitPos--;
		}
	}

	GPIO_StructInit(&xGPIOxInit);
	xGPIOxInit.GPIO_Pin = usGPIOPin;
	xGPIOxInit.GPIO_Speed = GPIO_Speed_2MHz;
	xGPIOxInit.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(pxGPIOx, &xGPIOxInit);
	return DHT_OK;
}


uint8_t ucTranslateSensorData(uint8_t* pucData, xDHTData* pxDHTData) {
	pxDHTData->humidity = pucData[0];
	pxDHTData->temperature = pucData[2];

	uint8_t ucCheckSum = pucData[0] + pucData[2];
	if(ucCheckSum != pucData[4]) {
		return DHT_ERROR_CHECKSUM;
	}
	return DHT_OK;
}
