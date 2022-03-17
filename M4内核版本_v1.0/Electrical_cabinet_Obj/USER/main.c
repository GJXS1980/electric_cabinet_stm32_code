#include "sys.h"
#include "delay.h"
#include "hmi_user_uart.h"
#include "led.h"
#include "adc.h"
#include "dac.h"
#include "cabinet.h"
#include "hmi_driver.h"
#include "hmi_user_uart.h"
#include "cmd_queue.h"
#include "cmd_process.h"
#include "stdio.h"
#include "hw_config.h"
#include "tim9.h"
#include "ch_tips.h"

volatile uint32  timer_tick_count = 0;                                               //��ʱ������
volatile float bat_val1 = 2000.0;
volatile float bat_val2 = 1500.0;
volatile float temperature = 50.0;

uint8  cmd_buffer[CMD_MAX_SIZE];                                                     //ָ���
static uint16 current_screen_id =  0;                                                 //��ǰ����ID
static int32 progress_value = 0;                                                     //����������ֵ
static int32 test_value = 0;                                                         //����ֵ
static uint8 update_en = 0;                                                          //���±��
static int32 meter_flag = 0;                                                         //�Ǳ�ָ��������־λ
static int32 num = 0;                                                                //���߲��������
static int sec = 1;                                                                  //ʱ����
static int32 curves_type = 0;                                                        //���߱�־λ  0Ϊ���Ҳ���1Ϊ��ݲ�                  
static int32 second_flag=0;                                                          //ʱ���־λ
static int32 icon_flag = 0;                                                          //ͼ���־λ
static uint8 Select_H ;                                                              //����ѡ��Сʱ
static uint8 Select_M ;                                                              //����ѡ����� 
static uint8 Last_H ;                                                                //��һ��ѡ��Сʱ
static uint8 Last_M;                                                                 //��һ��ѡ����� 
static int32 Progress_Value = 0;                                                     //��������ֵ 

static u8 UI_DEBUG=0;
static u8 simulation = 0;


void UpdateUI(void);                                                                 //����UI����

void OPERATION_LOCAL_MODLE(void)
{
	
	static u8  energy_t = 1;
	static u8  GATE_STATE = 0;
	
	//����ģʽ���ܷ�բ���ܺ�բ
	if(OVERHAUL==0)
	{
		GATE_LED_ON;
		TIM_Cmd(TIM9, DISABLE); 
		if(CLOSE_GATE==0)	//��բ
		{
				GREED_ON;	
				RED_OFF;
				GATE_STATE = 0;
		}
		else if(OPEN_GATE==0)
		{
				GREED_OFF;
		}		
			
	}
	else
	{
		GATE_LED_OFF;  //δ����
		
		/*
				ִ�и�������
		*/
		if(ENERGY==0)
		{
				TIM_Cmd(TIM9, ENABLE);   
		}
		else
		{
				TIM_Cmd(TIM9, DISABLE); 
		}
		
		if(ENERGY_STATE)
			YELLOW_ON;
		
		if(OPEN_GATE==0)      	//��ʱ�� 5S  ENERGY_STATEΪ��
		{
					DelayMS(200);
				  if((OPEN_GATE==0 && ENERGY==0 && ENERGY_STATE)|| ENERGY_STATE)
					{	
						GATE_STATE = 1;
								 
						YELLOW_OFF;
						//energy_t = 0;
						TIM9->CNT = 0;
					}
		}
		else if(CLOSE_GATE==0)      //��բ
		{
					DelayMS(200);
					if(CLOSE_GATE==0)
						GATE_STATE = 0;
				//ENERGY_STATE = FALSE;
		}		
			
		if(GATE_STATE==1) 	//���ܼ���
		{
				if(ENERGY_STATE && energy_t)
				{	
					
					RED_ON;	
					GREED_OFF;
					ENERGY_STATE = 0;	
					energy_t = 0;
					
				}
		}
		else if(GATE_STATE == 0)      //��բ
		{
				GATE_STATE = 0;
				RED_OFF;
				GREED_ON;
				energy_t = 1;
				//energy_t = 1;
				//ENERGY_STATE = FALSE;
		}

		
	}
}	


void OPERATION_DISTANCE_MODLE(void)
{
		
		u8 key=Button_Scan();
	
		if((Secondary_switch==0) && (ENERGY==0))		// Զ��ģʽ��������
		{
			
        switch(key)
				{
					case KEY2_PRES: 	RED_OFF;GREED_OFF;YELLOW_OFF;	SetTextValue(4,8,close_tip); 	break;
					case KEY3_PRES: 	RED_ON;GREED_OFF;YELLOW_OFF;	SetTextValue(4,8,protect_tip);	break;
					case KEY4_PRES: 	RED_ON;GREED_OFF;YELLOW_OFF;	SetTextValue(4,8,clear_tip);	break;
					default:break;
				}
		}
		else if ((Secondary_switch==1) && (ENERGY==0))
		{
				if(OVERHAUL==0)
				{
				   switch(key)
					{
							case KEY2_PRES: 	RED_OFF;GREED_OFF;YELLOW_ON; GATE_LED_OFF; GATE_LED_OFF;SetTextValue(4,8,overhaul_tip);break;
							case KEY3_PRES: 	RED_ON;GREED_ON;YELLOW_ON;  GATE_LED_OFF;	 GATE_LED_OFF;SetTextValue(4,8,overhaul_tip);		break;
							default:break;
					}		
					
					return;
				}
				
        switch(key)
				{
					case KEY2_PRES: 	RED_OFF;GREED_OFF;YELLOW_ON; SetTextValue(4,8,close_tip);	break;
					case KEY3_PRES: 	RED_ON;GREED_OFF;YELLOW_ON;	 SetTextValue(4,8,protect_tip);	break;
					case KEY4_PRES: 	RED_OFF;GREED_ON;YELLOW_ON;	 SetTextValue(4,8,overhaul_tip);		break;
					default:break;
				}		
		}	

}	


int main(void)
{ 

  qsize  size = 0; 
	u8 t = 0;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  systicket_init();      
	UartInit(19200);		
	Tim9_Init(50000-1,16800 - 1);  // 186000 000 /16800 = 10000 hz = 0.1ms -> 5s 
	cabinet_gpio_init();		 				 				
	Dac_Init();		 			

		//1 	 �ϵ��ȡIO״̬
	  //2 	 ��������ʼ��
		//3 	 ��ȡ����ģʽ
		//4		 ��ʱ�ϴ���ѹ����
		//5		 ��ʱ��ʵʱ��ȡIO��״̬ 
		//6 	 ��ʱ������
		//7 	 IO״̬����
	
	 //��մ��ڽ��ջ�����                                                          
   queue_reset();
    //��ʱ�ȴ���������ʼ�����,����ȴ�300ms                                      
   delay_ms(300); 
		
	
	while(1)
	{
		if(!UI_DEBUG && !simulation)
		{	
			if(OPERATION_LOCAL)   	//�͵�ģʽ
			{
					OPERATION_LOCAL_MODLE();
					
			}
			else 						//Զ��ģʽ
			{
						OPERATION_DISTANCE_MODLE();
			}	
		}
				
			size = queue_find_cmd(cmd_buffer,CMD_MAX_SIZE);                              //�ӻ������л�ȡһ��ָ��         

      if(size>0&&cmd_buffer[1]!=0x07)                                              //���յ�ָ�� �����ж��Ƿ�Ϊ������ʾ
      {                                                                           
            ProcessMessage((PCTRL_MSG)cmd_buffer, size);                             //ָ���  
      }                                                                           
      else if(size>0&&cmd_buffer[1]==0x07)                                         //���Ϊָ��0x07��������STM32  
      {                                                                           
            __disable_fault_irq();                                                   
            NVIC_SystemReset();                                                                                                                                          
      }                                                                            
			
			if(timer_tick_count % 300 ==0 && timer_tick_count)
			{	
				
				switch(t%3)
				{
					case 0: bat_val1=100.00;bat_val2=150.00;temperature=100.53;break;
					case 1: bat_val1=1300.00;bat_val2=2000.00;temperature=50.32;break;
					case 2: bat_val1=1500.00;bat_val2=1800.00;temperature=50.14;break;
					default: break;
				}
				
				UpdateUI(); 
				Dac1_Set_Vol(bat_val1);																													//���õ�ѹ��͵�����
				Dac2_Set_Vol(bat_val2);
				
				if(simulation)   //ģ��ģʽ
				{	
					switch(t%8)
					{
						case 0:  RED_OFF;GREED_OFF;YELLOW_OFF;SetTextValue(4,8,close_tip); 	break;
						case 1:  RED_ON;GREED_OFF;YELLOW_OFF;	SetTextValue(4,8,protect_tip);	break;
						case 2:  RED_ON;GREED_OFF;YELLOW_OFF;	SetTextValue(4,8,clear_tip);	break;
						case 3:  RED_OFF;GREED_OFF;YELLOW_ON; SetTextValue(4,8,close_tip);	break;
						case 4:  RED_ON;GREED_OFF;YELLOW_ON;	SetTextValue(4,8,protect_tip);	break;
						case 5:  RED_OFF;GREED_ON;YELLOW_ON;	SetTextValue(4,8,overhaul_tip);	break;
						case 6:  RED_OFF;GREED_OFF;YELLOW_ON; GATE_LED_OFF;SetTextValue(4,8,overhaul_tip);break;
						case 7:  RED_ON;GREED_ON;YELLOW_ON;  GATE_LED_OFF;SetTextValue(4,8,overhaul_tip);		break;			
						default: break;
					}
					
				}	
				
				t++;
				
      }
		
	}	
}


/*! 
*  \brief  ��Ϣ��������
*  \param msg ��������Ϣ
*  \param size ��Ϣ����
*/

void ProcessMessage( PCTRL_MSG msg, uint16 size )
{
    uint8 cmd_type = msg->cmd_type;                                                  //ָ������
    uint8 ctrl_msg = msg->ctrl_msg;                                                  //��Ϣ������
    uint8 control_type = msg->control_type;                                          //�ؼ�����
    uint16 screen_id = PTR2U16(&msg->screen_id);                                     //����ID
    uint16 control_id = PTR2U16(&msg->control_id);                                   //�ؼ�ID
    uint32 value = PTR2U32(msg->param);                                              //��ֵ


    switch(cmd_type)
    {  
    case NOTIFY_TOUCH_PRESS:                                                        //����������
    case NOTIFY_TOUCH_RELEASE:                                                      //�������ɿ�
        NotifyTouchXY(cmd_buffer[1],PTR2U16(cmd_buffer+2),PTR2U16(cmd_buffer+4)); 
        break;                                                                    
    case NOTIFY_WRITE_FLASH_OK:                                                     //дFLASH�ɹ�
        NotifyWriteFlash(1);                                                      
        break;                                                                    
    case NOTIFY_WRITE_FLASH_FAILD:                                                  //дFLASHʧ��
        NotifyWriteFlash(0);                                                      
        break;                                                                    
    case NOTIFY_READ_FLASH_OK:                                                      //��ȡFLASH�ɹ�
        NotifyReadFlash(1,cmd_buffer+2,size-6);                                     //ȥ��֡ͷ֡β
        break;                                                                    
    case NOTIFY_READ_FLASH_FAILD:                                                   //��ȡFLASHʧ��
        NotifyReadFlash(0,0,0);                                                   
        break;                                                                    
    case NOTIFY_READ_RTC:                                                           //��ȡRTCʱ��
        NotifyReadRTC(cmd_buffer[2],cmd_buffer[3],cmd_buffer[4],cmd_buffer[5],cmd_buffer[6],cmd_buffer[7],cmd_buffer[8]);
        break;
    case NOTIFY_CONTROL:
        {
            if(ctrl_msg==MSG_GET_CURRENT_SCREEN)                                    //����ID�仯֪ͨ
            {
                NotifyScreen(screen_id);                                            //�����л������ĺ���
            }
            else
            {
                switch(control_type)
                {
                case kCtrlButton:                                                   //��ť�ؼ�
                    NotifyButton(screen_id,control_id,msg->param[1]);                  
                    break;                                                             
                case kCtrlText:                                                     //�ı��ؼ�
                    NotifyText(screen_id,control_id,msg->param);                       
                    break;                                                             
                case kCtrlProgress:                                                 //�������ؼ�
                    NotifyProgress(screen_id,control_id,value);                        
                    break;                                                             
                case kCtrlSlider:                                                   //�������ؼ�
                    NotifySlider(screen_id,control_id,value);                          
                    break;                                                             
                case kCtrlMeter:                                                    //�Ǳ�ؼ�
                    NotifyMeter(screen_id,control_id,value);                           
                    break;                                                             
                case kCtrlMenu:                                                     //�˵��ؼ�
                    NotifyMenu(screen_id,control_id,msg->param[0],msg->param[1]);      
                    break;                                                              
                case kCtrlSelector:                                                 //ѡ��ؼ�
                    NotifySelector(screen_id,control_id,msg->param[0]);                
                    break;                                                              
                case kCtrlRTC:                                                      //����ʱ�ؼ�
                    NotifyTimer(screen_id,control_id);
                    break;
                default:
                    break;
                }
            } 
            break;  
        } 
    case NOTIFY_HandShake:                                                          //����֪ͨ                                                     
        NOTIFYHandShake();
        break;
    default:
        break;
    }
}
/*! 
*  \brief  ����֪ͨ
*/
void NOTIFYHandShake()
{
   SetButtonValue(3,2,1);
}

/*! 
*  \brief  �����л�֪ͨ
*  \details  ��ǰ����ı�ʱ(�����GetScreen)��ִ�д˺���
*  \param screen_id ��ǰ����ID
*/
void NotifyScreen(uint16 screen_id)
{
    //TODO: ����û�����
    current_screen_id = screen_id;                                                   //�ڹ��������п��������л�֪ͨ����¼��ǰ����ID

    //��������3����һ����ť
    if(screen_id == 3)
    {
				UI_DEBUG = 1;
        //SetButtonValue(3,1,1); //����״̬
    }else
		{
			UI_DEBUG = 0;
		}
		
    //���������Զ�����GIF
    if(current_screen_id == 9)
    {
        AnimationStart(9,1);                                                         //������ʼ����
    }
       //���������������ȡ��ǰ�ı�ֵ
    else if(current_screen_id==5)                                   
    {
        GetControlValue(5,1);
    }
    //������ά��ҳ�����ɶ�ά��
    else if(current_screen_id==14)                                   
    {
        //��ά��ؼ���ʾ�����ַ�ʱ����Ҫת��ΪUTF8���룬
        //ͨ����ָ�����֡���ת����www.gz-dc.com�� ���õ��ַ�����������
        uint8 dat[] = {0x77,0x77,0x77,0x2E,0x67,0x7A,0x2D,0x64,0x63,0x2E,0x63,0x6F,0x6D};
        SetTextValue(14,1,dat);                                                      //���Ͷ�ά���ַ�����                     
    }

    //���ݼ�¼��ʾ
    else if(current_screen_id == 15)
    {
        Record_SetEvent(15,1,0,0);  
        Record_SetEvent(15,1,1,0);
        Record_SetEvent(15,1,2,0);
        Record_SetEvent(15,1,3,0);
        Record_SetEvent(15,1,4,0);
        Record_SetEvent(15,1,5,0);
        Record_SetEvent(15,1,6,0);
        Record_SetEvent(15,1,7,0);
        delay_ms(2000);                                                              //��ʱ����
        Record_ResetEvent(15,1,0,0);
        Record_ResetEvent(15,1,1,0);
        Record_ResetEvent(15,1,2,0);

    }
    //�������ֻ����Զ�����
    else if(current_screen_id == 17)   
    {
         uint8 buffer[6] = {0x90,0x01 ,0x00 ,0x01 ,0x01};
         SetButtonValue(17,3,1);
         PlayMusic(buffer);                                                           //��������
    }
}

/*! 
*  \brief  ���������¼���Ӧ
*  \param press 1���´�������3�ɿ�������
*  \param x x����
*  \param y y����
*/
void NotifyTouchXY(uint8 press,uint16 x,uint16 y)
{ 
    //TODO: ����û�����
}


/*! 
*  \brief  ��������
*/ 
void UpdateUI()
{
    //�ı����ú���ʾ  ��ʱ20msˢ��һ��
    if(current_screen_id==4)                                              
    {		
			  SetTextFloat(4,5,bat_val1*4.02f/1000.0f,2,1);			
			  SetTextFloat(4,6,bat_val2*4.02f/1000.0f,2,1);		
				SetTextFloat(4,7,temperature,2,1);		
			  //SetTextValue(4,1,"����10");                                                 //�����ı�ֵ
    }                                                                               

    //�Ǳ�ؼ�   ��ʱ20msˢ��һ��                                                   
    if(current_screen_id == 6)                                                                          
    {                                                                               
        if(meter_flag == 0)                                                          //��־λ 0˳ʱ�� ��1��ʱ��
        {   
            //˳ʱ����ת                    
            SetMeterValue(6,1,test_value);                                           //����ͼƬָ����ת�Ƕ�
            test_value +=1;                                                          //ָ����ת��0�ȵ�260��
            if(test_value >= 260)                                                   
            {                                                                        
                test_value = 260;                                                     
                meter_flag = 1;                                                             
            }                                                                        
        }                                                                          
        else if(meter_flag == 1)                                                               
        {    
            //��ʱ����ת              
            Set_picMeterValue(6,1,test_value);                                       //����ͼƬָ����ת�Ƕ�
            test_value -=1;                                                          //ָ����ת��260�ȵ�0��
            if(test_value <= 0)
            {
                test_value = 0;
                meter_flag = 0;    
            }
        }
    }

    //ͼ��1s������ʾ 
    if(current_screen_id == 10)
    {
        if(timer_tick_count %100 == 0 && icon_flag == 0)       
         {        
             SetButtonValue(10,5,0); 
             SetButtonValue(10,2,1);                                                      //������ͼ��
             AnimationPlayFrame(10,1,0);
             icon_flag = 1 ;            
         }
         else if(timer_tick_count %100 == 0 && icon_flag == 1)
         {
             SetButtonValue(10,2,0);                                                      
             SetButtonValue(10,3,1);                                                      //��ͣ��ͼ��
             AnimationPlayFrame(10,1,1);  
             icon_flag = 2;             
         }
         else if(timer_tick_count %100 == 0 && icon_flag == 2)
         {
             SetButtonValue(10,3,0);                                                      
             SetButtonValue(10,4,1);                                                      //ֹͣͼ��
             AnimationPlayFrame(10,1,2);  
             icon_flag = 3 ;            
         }
         else if(timer_tick_count %100 == 0 && icon_flag == 3)
         {        
             SetButtonValue(10,4,0);                                                                                                                  
             SetButtonValue(10,5,1);                                                      //����ͼ��
             AnimationPlayFrame(10,1,3);  
             icon_flag  = 0 ;              
         }
    }


    //ʵʱ���ߣ����Ҳ����顣  ��ʱ20ms��������
    if(current_screen_id == 11)      
    { 
        if(curves_type == 0)
        {
            //��������
            uint8 sin[256] = {1,1,1,1,1,2,3,4,6,8,10,13,15,
                19,22,25,29,33,38,42,47,52,57,62,68,73,79,85,91,97,103,109,115,121,127,134,
                140,146,152,158,164,170,176,182,187,193,198,203,208,213,217,222,226,230,233,
                236,240,242,245,247,249,251,252,253,254,254,254,254,254,253,252,251,249,247,
                245,242,240,236,233,230,226,222,217,213,208,203,198,193,187,182,176,170,164,
                158,152,146,140,134,128,121,115,109,103,97,91,85,79,73,68,62,57,52,47,42,38,
                33,29,25,22,19,15,13,10,8,6,4,3,2,1,1,1,1,1,2,3,4,6,8,10,13,15,
                19,22,25,29,33,38,42,47,52,57,62,68,73,79,85,91,97,103,109,115,121,127,134,
                140,146,152,158,164,170,176,182,187,193,198,203,208,213,217,222,226,230,233,
                236,240,242,245,247,249,251,252,253,254,254,254,254,254,253,252,251,249,247,
                245,242,240,236,233,230,226,222,217,213,208,203,198,193,187,182,176,170,164,
                158,152,146,140,134,128,121,115,109,103,97,91,85,79,73,68,62,57,52,47,42,38,
                33,29,25,22,19,15,13,10,8,6,4,3,2};

            GraphChannelDataAdd(11,1,0,&sin[num],2);                                    //������ݵ�����  һ����������

            num += 2;                                                                        
            if(num >= 255)                                                                
            {                                                                             
                num =0;                                                                  
            }
        } 
        else if(curves_type == 1)
        {
            //��ݲ�����
            uint8 sawtooth[180] = {0,9,18,27,36,45,54,63,72,81,90,99,108,117,126,135,144,153,162,171,180,189,198,207,216,225,234,243,252,
                0,9,18,27,36,45,54,63,72,81,90,99,108,117,126,135,144,153,162,171,180,189,198,207,216,225,234,243,252,
                0,9,18,27,36,45,54,63,72,81,90,99,108,117,126,135,144,153,162,171,180,189,198,207,216,225,234,243,252,
                0,9,18,27,36,45,54,63,72,81,90,99,108,117,126,135,144,153,162,171,180,189,198,207,216,225,234,243,252,
                0,9,18,27,36,45,54,63,72,81,90,99,108,117,126,135,144,153,162,171,180,189,198,207,216,225,234,243,252,
                0,9,18,27,36,45,54,63,72,81,90,99,108,117,126,135,144,153,162,171,180,189,198,207,216,225,234,243,252};


            GraphChannelDataAdd(11,1,0,&sawtooth[num],2);                              //������ݵ�����  һ����������
            num +=2 ;                                                                        
            if(num >= 180)                                                                
            {                                                                             
                num =0;                                                                  
            }
        }          
    }                                                                              

    //��ʱ���ߣ����Ҳ�����                                                         
    if(current_screen_id == 16 && timer_tick_count %100 == 0)                           //��ʷ���߿ؼ���������1sһ���㡣//һ������������                                                    
    {   
        if(curves_type == 0)
        {        
            //��������
            uint8 sin[256] = {1,1,1,1,1,2,3,4,6,8,10,13,15,
                19,22,25,29,33,38,42,47,52,57,62,68,73,79,85,91,97,103,109,115,121,127,134,
                140,146,152,158,164,170,176,182,187,193,198,203,208,213,217,222,226,230,233,
                236,240,242,245,247,249,251,252,253,254,254,254,254,254,253,252,251,249,247,
                245,242,240,236,233,230,226,222,217,213,208,203,198,193,187,182,176,170,164,
                158,152,146,140,134,128,121,115,109,103,97,91,85,79,73,68,62,57,52,47,42,38,
                33,29,25,22,19,15,13,10,8,6,4,3,2,1,1,1,1,1,2,3,4,6,8,10,13,15,
                19,22,25,29,33,38,42,47,52,57,62,68,73,79,85,91,97,103,109,115,121,127,134,
                140,146,152,158,164,170,176,182,187,193,198,203,208,213,217,222,226,230,233,
                236,240,242,245,247,249,251,252,253,254,254,254,254,254,253,252,251,249,247,
                245,242,240,236,233,230,226,222,217,213,208,203,198,193,187,182,176,170,164,
                158,152,146,140,134,128,121,115,109,103,97,91,85,79,73,68,62,57,52,47,42,38,
                33,29,25,22,19,15,13,10,8,6,4,3,2}; 

            HistoryGraph_SetValueInt8(16,1,&sin[num],1);                               //�����ʷ��������         
            num++;
            if(num >= 255)
            {
                num =0;
            }
        } 
        else if(curves_type == 1)
        {          
            //��ݲ�����
            uint8 sawtooth[180] = {0,9,18,27,36,45,54,63,72,81,90,99,108,117,126,135,144,153,162,171,180,189,198,207,216,225,234,243,252,
                0,9,18,27,36,45,54,63,72,81,90,99,108,117,126,135,144,153,162,171,180,189,198,207,216,225,234,243,252,
                0,9,18,27,36,45,54,63,72,81,90,99,108,117,126,135,144,153,162,171,180,189,198,207,216,225,234,243,252,
                0,9,18,27,36,45,54,63,72,81,90,99,108,117,126,135,144,153,162,171,180,189,198,207,216,225,234,243,252,
                0,9,18,27,36,45,54,63,72,81,90,99,108,117,126,135,144,153,162,171,180,189,198,207,216,225,234,243,252,
                0,9,18,27,36,45,54,63,72,81,90,99,108,117,126,135,144,153,162,171,180,189,198,207,216,225,234,243,252};

            HistoryGraph_SetValueInt8(16,1,&sawtooth[num],1);                          //�����ʷ��������        
            num++;
            if(num >= 90)
            {
                num =0;
            }
        } 


    }

}


/*! 
*  \brief  ��ť�ؼ�֪ͨ
*  \details  ����ť״̬�ı�(�����GetControlValue)ʱ��ִ�д˺���
*  \param screen_id ����ID
*  \param control_id �ؼ�ID
*  \param state ��ť״̬��0����1����
*/
void NotifyButton(uint16 screen_id, uint16 control_id, uint8  state)
{ 

	  if(screen_id == 3)
    {
			
			 if(state==1) 
			 {	 
					switch(control_id)
					{
						case 1:RED_ON;break;
						case 2:GREED_ON;break;
						case 3:YELLOW_ON;break;
						case 4:RED_ON;GREED_ON;YELLOW_ON;break;
						case 5:RED_ON;GREED_OFF;YELLOW_OFF;break;
						case 6:RED_OFF;GREED_ON;YELLOW_OFF;break;
						case 7:RED_OFF;GREED_OFF;YELLOW_ON;break;
						case 8:GATE_LED_ON;simulation = 1;break;
						case 9:GATE_LED_ON;break;
						default:break;
					}
			}
			else
			{
					switch(control_id)
					{
						case 1:RED_OFF;break;
						case 2:GREED_OFF;break;
						case 3:YELLOW_OFF;break;
						case 4:RED_OFF;GREED_OFF;YELLOW_OFF;break;
						case 8:GATE_LED_ON;simulation = 0;break;
						case 9:GATE_LED_OFF;break;
						default:break;
					}
			}	
						
    }
		
	
    //�����������߲���
    if(screen_id == 11)
    {
        if(control_id==2)                                                            //���Ҳ��ؼ�
        {
            curves_type = 0;                                                                
        }
        else if(control_id==3)                                                       //��ݲ��ؼ�
        {
            curves_type = 1;
        }   
    }
    //ʱ�仭�水ť��ȡRTCʱ��
    if(screen_id == 8)
    {
        if(control_id==8 && state == 1)                                              //��ȡʱ��
        {
            ReadRTC();                                                              
        }
    }
        //ѡ��ؼ���ʾʱ���
     if(screen_id == 13 && control_id==4)
     {
         if(Select_H<=6)                                                       //0~6Сʱ���賿  
        {     
            SetSelectorValue(13,3,0);                                                       //����ѡ��ؼ�ѡ��  
        }                                        

        else if(Select_H>=7&&Select_H<=12)                                                  //7~12Сʱ������
        {
            SetSelectorValue(13,3,1);
        }         

        else if(Select_H>=13&&Select_H<=18)                                                 //13~18Сʱ������
        { 
            SetSelectorValue(13,3,2);
        }    

        else if(Select_H>18&&Select_H<=23)                                                  //19~23Сʱ����ҹ
        {
            SetSelectorValue(13,3,3);
        }   
         Last_H   = Select_H;   
         Last_M   = Select_M;                 
     }
     if(screen_id == 13 && control_id==5)
     {
          SetSelectorValue(13,1,Last_H);
          SetSelectorValue(13,2,Last_M);
     }
     if(screen_id == 5)
    {
        if(control_id==3)                                                            //�ݼ�
        {
            Progress_Value -= 1; 
            if(Progress_Value <= 0)
            {
                Progress_Value = 0;
            }                
            SetProgressValue(5,1,Progress_Value);                                    //���ý�������ֵ
            SetTextInt32(5,2,Progress_Value,0,1);                                    //�����ı����ֵ
        }
        else if(control_id==4)                                                       //�ݼ�
        {
            Progress_Value += 1;
            if(Progress_Value >= 100)
            {
                Progress_Value = 100;
            }
            SetProgressValue(5,1,Progress_Value);
            SetTextInt32(5,2,Progress_Value,0,1);
        }   
    }
     
}

/*! 
*  \brief  �ı��ؼ�֪ͨ
*  \details  ���ı�ͨ�����̸���(�����GetControlValue)ʱ��ִ�д˺���
*  \details  �ı��ؼ����������ַ�����ʽ�·���MCU������ı��ؼ������Ǹ���ֵ��
*  \details  ����Ҫ�ڴ˺����н��·��ַ�������ת�ظ���ֵ��
*  \param screen_id ����ID
*  \param control_id �ؼ�ID
*  \param str �ı��ؼ�����
*/
void NotifyText(uint16 screen_id, uint16 control_id, uint8 *str)
{
    if(screen_id==4)                                                                 //����ID2���ı����ú���ʾ
    {                                                                            
        int32 value=0;                                                            

			sscanf((char*)str,"%ld",&value);                                                    //���ַ���ת��Ϊ���� 
        if(control_id==2)                                                            //��ߵ�ѹ
        {                                                                         
            //�޶���ֵ��Χ��Ҳ�������ı��ؼ����������ã�                             
            if(value<0)                                                              
            {                                                                        
                value = 0;                                                            
            }                                                                        
            else if(value>380)                                                       
            {                                                                        
                value = 380;                                                           
            }                                                                        
            SetTextInt32(4,2,value,0,1);                                             //������ߵ�ѹ
            SetTextInt32(4,5,value/2,1,1);                                           //������ߵ�ѹ/2
        }                                                                         
    }                                                                            
}                                                                                

/*!                                                                              
*  \brief  �������ؼ�֪ͨ                                                       
*  \details  ����GetControlValueʱ��ִ�д˺���                                  
*  \param screen_id ����ID                                                      
*  \param control_id �ؼ�ID                                                     
*  \param value ֵ                                                              
*/                                                                              
void NotifyProgress(uint16 screen_id, uint16 control_id, uint32 value)           
{  
    if(screen_id == 5)
    {
        Progress_Value = value;                                  
        SetTextInt32(5,2,Progress_Value,0,1);                                        //�����ı����ֵ     
    }    
}                                                                                

/*!                                                                              
*  \brief  �������ؼ�֪ͨ                                                       
*  \details  ���������ı�(�����GetControlValue)ʱ��ִ�д˺���                  
*  \param screen_id ����ID                                                      
*  \param control_id �ؼ�ID                                                     
*  \param value ֵ                                                              
*/                                                                              
void NotifySlider(uint16 screen_id, uint16 control_id, uint32 value)             
{                                                             
    uchar back[1] = {0};
    if(screen_id==7&&control_id==2)                                                  //�������
    {            
        if(value<100||value>0)                                                       
        {                                                                            
            SetProgressValue(7,1,value);                                             //���½�������ֵ
            SetTextInt32(7,3,value,0,1); 
            sprintf((char*)back,"%c",(255-value*2));                                        //���ñ������� ����ֵ��Χ 0~255��0������255�
            SetBackLight(back[0]);
        }
    }
    if(screen_id==7&&control_id==5)                                                  //�������
    {                                                                              
        if(value<100||value>0)                                                       
        {                                                                            
            SetProgressValue(7,4,value);                                             //���½�������ֵ
            SetTextInt32(7,6,value,0,1);                                             
        }
    }
}

/*! 
*  \brief  �Ǳ�ؼ�֪ͨ
*  \details  ����GetControlValueʱ��ִ�д˺���
*  \param screen_id ����ID
*  \param control_id �ؼ�ID
*  \param value ֵ
*/
void NotifyMeter(uint16 screen_id, uint16 control_id, uint32 value)
{
    //TODO: ����û�����
}

/*! 
*  \brief  �˵��ؼ�֪ͨ
*  \details  ���˵���»��ɿ�ʱ��ִ�д˺���
*  \param screen_id ����ID
*  \param control_id �ؼ�ID
*  \param item �˵�������
*  \param state ��ť״̬��0�ɿ���1����
*/
void NotifyMenu(uint16 screen_id, uint16 control_id, uint8 item, uint8 state)
{
    //TODO: ����û�����
}

/*! 
*  \brief  ѡ��ؼ�֪ͨ
*  \details  ��ѡ��ؼ��仯ʱ��ִ�д˺���
*  \param screen_id ����ID
*  \param control_id �ؼ�ID
*  \param item ��ǰѡ��
*/
void NotifySelector(uint16 screen_id, uint16 control_id, uint8  item)
{
    if(screen_id == 13&&control_id == 1)                                //��ȡ��ǰѡ��ؼ���ֵ
    {
        Select_H =  item;
    } 
    if(screen_id == 13&&control_id == 2)                                //��ȡ��ǰѡ��ؼ���ֵ
    {
        Select_M =  item;
    } 

}

/*! 
*  \brief  ��ʱ����ʱ֪ͨ����
*  \param screen_id ����ID
*  \param control_id �ؼ�ID
*/
void NotifyTimer(uint16 screen_id, uint16 control_id)
{
    if(screen_id==8&&control_id == 7)
    {
        SetBuzzer(100);
    } 
}

/*! 
*  \brief  ��ȡ�û�FLASH״̬����
*  \param status 0ʧ�ܣ�1�ɹ�
*  \param _data ��������
*  \param length ���ݳ���
*/
void NotifyReadFlash(uint8 status,uint8 *_data,uint16 length)
{
    //TODO: ����û�����
}

/*! 
*  \brief  д�û�FLASH״̬����
*  \param status 0ʧ�ܣ�1�ɹ�
*/
void NotifyWriteFlash(uint8 status)
{
    //TODO: ����û�����
}

/*! 
*  \brief  ��ȡRTCʱ�䣬ע�ⷵ�ص���BCD��
*  \param year �꣨BCD��
*  \param month �£�BCD��
*  \param week ���ڣ�BCD��
*  \param day �գ�BCD��
*  \param hour ʱ��BCD��
*  \param minute �֣�BCD��
*  \param second �루BCD��
*/
void NotifyReadRTC(uint8 year,uint8 month,uint8 week,uint8 day,uint8 hour,uint8 minute,uint8 second)
{
    int years,months,weeks,days,hours,minutes;
       
    sec    =(0xff & (second>>4))*10 +(0xf & second);                                    //BCD��תʮ����
    years   =(0xff & (year>>4))*10 +(0xf & year);                                      
    months  =(0xff & (month>>4))*10 +(0xf & month);                                     
    weeks   =(0xff & (week>>4))*10 +(0xf & week);                                      
    days    =(0xff & (day>>4))*10 +(0xf & day);                                      
    hours   =(0xff & (hour>>4))*10 +(0xf & hour);                                       
    minutes =(0xff & (minute>>4))*10 +(0xf & minute);                                      
   
    SetTextInt32(8,1,years,1,1);
    SetTextInt32(8,2,months,1,1);
    SetTextInt32(8,3,days,1,1);
    SetTextInt32(8,4,hours,1,1);
    SetTextInt32(8,5,minutes,1,1);
    SetTextInt32(8,6,sec,1,1);
    if(weeks == 0)
    {
         SetTextValue(8,7,"������");
    }
    else if(weeks == 1)
    {
         SetTextValue(8,7,"����һ");
    }
    else if(weeks == 2)
    {
         SetTextValue(8,7,"���ڶ�");
    }
    else if(weeks == 3)
    {
         SetTextValue(8,7,"������");
    }
    else if(weeks == 4)
    {
         SetTextValue(8,7,"������"); 
    }
    else if(weeks == 5)
    { 
         SetTextValue(8,7,"������");
    }
    else if(weeks == 6)
    {
         SetTextValue(8,7,"������");
    }
}
