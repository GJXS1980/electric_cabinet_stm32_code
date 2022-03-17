#ifndef __CABINET_H
#define __CABINET_H
#include "sys.h"
	 		
#define 	TURE 		  1
#define 	FALSE			0

//�ĸ���ʾ�� --- ֻ��ͨ���̵������� ����˿�ȫ������24V 
/*
Y0_A  -- PA8  ���
Y1_A  -- PC9  �̵�
Y2_A  -- PD3  �Ƶ�
Y3_A  -- PD4  �׵�
*/

//�������� 
/*
X13_A -- PA12			���
X14_A -- PA11    	�̼�
X15_A -- PD15    	�Ƽ�
*/

//��������ת���� -- ��5��
/*
X0_A  	-- PC7  	����
X7_A  	-- PD0  	Զ��/�͵�
X2_A  	-- PB15  	����
X10_A  	-- PC12  	��բ
X11_A  	-- PC10  	��բ
X12_A 	-- PA15   Զ����������

*/


//���ư�ť
#define  	OVERHAUL    						GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_7)   	//����
#define   OPERATION_LOCAL  				GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_0)			//����ģʽ
#define   Secondary_switch  			GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_15)		//Զ��ģʽ��������

//����
#define 	ENERGY 									GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_15)


//��բ     
#define  	OPEN_GATE    						GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_12)   //��բ
//��բ
#define  	CLOSE_GATE    					GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_10)   //��բ

//������ȡ
#define 	KEY2 		GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_12)			//���
#define 	KEY3 		GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_11) 			//�̼�
#define 	KEY4 		GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_15)			//�Ƽ�


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
