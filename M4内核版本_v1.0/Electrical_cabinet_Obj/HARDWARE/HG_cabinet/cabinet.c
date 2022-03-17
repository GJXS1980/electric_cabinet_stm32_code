#include "cabinet.h" 
#include "delay.h"
#include "stdio.h"

volatile u8  ENERGY_STATE=0;  

//四个显示灯 --- 只能通过继电器控制 输入端口全部都是24V 
/*
Y0_A  -- PA6  红灯
Y1_A  -- PC6  绿灯
Y2_A  -- PC7  黄灯
Y3_A  -- PA8  白灯
*/

void led_init(void)
{

	GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOC, ENABLE);
 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_8; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;		
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7 ; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;		
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

	
}	


//三个按键 
/*
X13_A -- PA12			红键
X14_A -- PA11    	绿键
X15_A -- PD15    	黄键
*/

void control_key_init(void)
{

	GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOD, ENABLE);//使能GPIOA,GPIOE时钟
 
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15; 
  GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	
}	


     
//检修与旋转开关 -- 共5个
/*
X0_A  	-- PA0  	检修
X7_A  	-- PD0  	远方/就地
X2_A  	-- PB15  	储能
X10_A  	-- PC12  	合闸
X11_A  	-- PC8  	分闸
X12_A 	-- PC9   	远方二级开关

*/

void knob_sw_init(void)
{

	GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB| RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD , ENABLE);//使能GPIOA,GPIOE时钟
 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15; 
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_8 | GPIO_Pin_9; 
  GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 ; 
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  //上拉
  GPIO_Init(GPIOD, &GPIO_InitStructure);


}	


void cabinet_gpio_init(void)
{
		led_init();						//led初始化
		knob_sw_init();     	//旋钮初始化
		control_key_init();		//控制按键初始化
}


void cabinet_parameter_init(void)
{


}


u8 Button_Scan()
{
	static u8 key_up=1;//按键按松开标志	  
	if(key_up&&(KEY2==0||KEY3==0||KEY4==0))
	{
		delay_ms(10);//去抖动 
		key_up=0;
		if(KEY2==0)return 2;
		else if(KEY3==0)return 3;
		else if(KEY4==0)return 4;
	}else if(KEY2==1&&KEY3==1&&KEY4==1)key_up=1; 	    
 	
	return 0;// 无按键按下
}


