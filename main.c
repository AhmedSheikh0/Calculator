/*
 * main.c
 *
 *  Created on: Oct 21, 2023
 *      Author: AHMED
 */

#include <stdbool.h>
#include <avr/delay.h>

#include "LIB/STD_TYPES.h"

#include "MCAL/DIO/DIO_Interface.h"
#include "MCAL/Port/Port_Interface.h"

#include "HAL/LCD2/LCD_Interface.h"
#include "HAL/KeyPad/KPD_Interface.h"

void CheckPriority();
void CalculateResult();

u8 Global_index;
f32 numArr[20] = {0};
u8  opArr[20] = {0};
f32 oldNum = -1;
u8 oldOperation = 0;
u8 currentOperation = 0;
u8 key = 0xFF;
u8 errorFlag = 0;

int main(){

	PORT_voidInit();
	LCD_voidInit();

	LCD_voidWriteString((u8 *)"NTI CALC");
	LCD_voidGoToPosition(1,0);

	while(1){

		key = KPD_u8GetPressedKey();

		if(key != 0xFF){

			if(currentOperation == '=' || errorFlag == 1){
				errorFlag = 0;
				LCD_voidSendCommand(LCD_ClearDisplay);
				LCD_voidWriteString((u8 *)"NTI CALC");
				LCD_voidGoToPosition(1,0);

				currentOperation = 0;
			}
			if(key < 10){
				if(oldNum < 0){
					oldNum = 0;
				}
				LCD_voidWriteIntData(key);
				oldNum = (oldNum * 10) + key;
			}
			else{
				if(oldNum >= 0 || key == 'C'){
					switch(key){
					case '/':
						CheckPriority();
						if(errorFlag != -1){
							LCD_voidWriteChar(key);
							oldOperation = '/';
						}
						break;
					case '*':
						CheckPriority();
						if(errorFlag != -1){
							LCD_voidWriteChar(key);
							oldOperation = '*';
						}
						break;

					case '-':
						currentOperation = '-';
						LCD_voidWriteChar(key);
						CheckPriority();
						break;

					case '+':
						currentOperation = '+';
						LCD_voidWriteChar(key);
						CheckPriority();
						break;

					case '=':
						currentOperation = '=';
						CheckPriority();
						CalculateResult();
						if(errorFlag != 1){
							LCD_voidGoToPosition(3, 0);
							LCD_voidWriteString((u8*)"Ans");
							LCD_voidWriteChar(key);
							LCD_voidWriteFloatData(numArr[Global_index-1]);
							Global_index = 0;
						}
						break;

					case 'C':
						LCD_voidSendCommand(LCD_ClearDisplay);
						LCD_voidWriteString((u8 *)"NTI CALC");
						LCD_voidGoToPosition(1,0);

						Global_index = 0;
						oldNum = -1;
						oldOperation = 0;
						break;

					}
				}
			}
		}
	}
	return 0;
}

void CheckPriority(){
	if(oldOperation == '/'){
		oldOperation = 0;
		if(oldNum > 0){
			oldNum = numArr[Global_index - 1] / oldNum;
			Global_index--;
		}
		else{
			LCD_voidSendCommand(LCD_ClearDisplay);
			LCD_voidWriteString((u8*)"Error: Div. by 0!");
			oldNum = -1;
			Global_index = 0;
			errorFlag = 1;
			return;
		}

	}
	else if(oldOperation == '*'){
		oldOperation = 0;
		oldNum = numArr[Global_index - 1] * oldNum;
		Global_index--;
	}
	if(currentOperation == '+' || currentOperation == '-'){
		opArr[Global_index] = key;
		currentOperation = 0;
	}

	numArr[Global_index] = oldNum;
	Global_index++;
	oldNum = -1;
}

void CalculateResult(){
	u8 counter = 0;
	for(counter = 0; counter+1 < Global_index; counter++){
		if(opArr[counter] == '+'){
			numArr[counter+1] = numArr[counter] + numArr[counter + 1];
		}
		else if(opArr[counter] == '-'){
			numArr[counter+1] = numArr[counter] - numArr[counter + 1];
		}
	}
}
