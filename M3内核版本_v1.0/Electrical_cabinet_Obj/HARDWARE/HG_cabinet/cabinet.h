#ifndef __CABINET_H
#define __CABINET_H
#include "sys.h"
	 		
#define 	TURE 		  1
#define 	FALSE			0

//四个显示灯 --- 只能通过继电器控制 输入端口全部都是24V 
/*
Y0_A  -- PA8  红灯
Y1_A  -- PC9  绿灯
Y2_A  -- PD3  黄灯
Y3_A  -- PD4  白灯
*/

//三个按键 
/*
X13_A -- PA12			红键
X14_A -- PA11    	绿键
X15_A -- PD15    	黄键
*/

//检修与旋转开关 -- 共5个
/*
X0_A  	-- PC7  	检修
X7_A  	-- PD0  	远方/就地
X2_A  	-- PB15  	储能
X10_A  	-- PC12  	合闸
X11_A  	-- PC10  	分闸
X12_A 	-- PA15   远方二级开关

*/


//控制按钮
#define  	OVERHAUL    						GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_7)   	//检修
#define   OPERATION_LOCAL  				GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_0)			//本地模式
#define   Secondary_switch  			GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_15)		//远地模式二级开发

//储能
#define 	ENERGY 									GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_15)


//合闸     
#define  	OPEN_GATE    						GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_12)   //合闸
//分闸
#define  	CLOSE_GATE    					GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_10)   //分闸

//按键读取
#define 	KEY2 		GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_12)			//红键
#define 	KEY3 		GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_11) 			//绿键
#define 	KEY4 		GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_15)			//黄键


#define KEY2_PRES		2
#define KEY3_PRES   3
#define KEY4_PRES   4

#define 	RED_ON        GPIO_SetBits(GPIOA,GPIO_Pin_8)
#define 	RED_OFF				GPIO_ResetBits(GPIOA,GPIO_Pin_8)


#define 	GREED_ON      GPIO_SetBits(GPIOC,GPIO_Pin_9)
#define 	GREED_OFF     GPIO_ResetBits(GPIOC,GPIO_Pin_9)


#define 	YELLOW_ON     GPIO_SetBits(GPIOD,GPIO_Pin_3)
#define 	YELLOW_OFF		GPIO_ResetBits(GPIOD,GPIO_Pin_3)

#define 	GATE_LED_ON    GPIO_SetBits(GPIOD,GPIO_Pin_4)
#define 	GATE_LED_OFF	 GPIO_ResetBits(GPIOD,GPIO_Pin_4)


void cabinet_gpio_init(void);
void cabinet_parameter_init(void);
u8 Button_Scan(void);

extern volatile u8  ENERGY_STATE;  

#endif
