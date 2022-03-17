#include "cabinet.h" 
#include "delay.h"
#include "stdio.h"

volatile u8  ENERGY_STATE=0;  

//�ĸ���ʾ�� --- ֻ��ͨ���̵������� ����˿�ȫ������24V 
/*
Y0_A  -- PA8  ���
Y1_A  -- PC9  �̵�
Y2_A  -- PD3  �Ƶ�
Y3_A  -- PD4  �׵�
*/

void led_init(void)
{

	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD,ENABLE);//ʹ��PORTA,PORTEʱ��
 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 ; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4 ; 
	GPIO_Init(GPIOD, &GPIO_InitStructure);
}	


//�������� 
/*
X13_A -- PA12			���
X14_A -- PA11    	�̼�
X15_A -- PD15    	�Ƽ�
*/

void control_key_init(void)
{

	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOD,ENABLE);
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15; 
  GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	
}	


     
//��������ת���� -- ��5��
/*
X0_A  	-- PC7  	����
X7_A  	-- PD0  	Զ��/�͵�
X2_A  	-- PB15  	����
X10_A  	-- PC12  	��բ
X11_A  	-- PC10  	��բ
X12_A 	-- PA15   Զ����������

*/

void knob_sw_init(void)
{

	GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB| RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD  , ENABLE);//ʹ��GPIOA,GPIOEʱ��
 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15; 
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_12 |GPIO_Pin_10; 
  GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; 
  GPIO_Init(GPIOD, &GPIO_InitStructure);
	
}	


void cabinet_gpio_init(void)
{
		led_init();						//led��ʼ��
		knob_sw_init();     	//��ť��ʼ��
		control_key_init();		//���ư�����ʼ��
}


void cabinet_parameter_init(void)
{


}


u8 Button_Scan()
{
	static u8 key_up=1;//�������ɿ���־	  
	if(key_up&&(KEY2==0||KEY3==0||KEY4==0))
	{
		delay_ms(10);//ȥ���� 
		key_up=0;
		if(KEY2==0)return 2;
		else if(KEY3==0)return 3;
		else if(KEY4==0)return 4;
	}else if(KEY2==1&&KEY3==1&&KEY4==1)key_up=1; 	    
 	
	return 0;// �ް�������
}


