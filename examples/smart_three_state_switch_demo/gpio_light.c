/*******************************************************
                tianyuan technology
                  2017-12-08 Frd
                      yang
*******************************************************/
#include "gpio_light.h"
#include "gpio_key.h"
#include "user_timer.h"

uint32_t Timer3_value = 0;
uint16_t light_b;

uint8_t key_press = 0;

/******************************************************
                wifiָʾ��״̬����
******************************************************/
void Key_wifi_led_on(void){
  GPIO_ResetBits(GPIOA, WIFI_STATUS_LED);
}
void Key_wifi_led_off(void){
  GPIO_SetBits(GPIOA, WIFI_STATUS_LED);
}

/******************************************************
                ����״ִ̬�к���
******************************************************/
void Key_cc_on(void){
  GPIO_ResetBits(GPIOB, KEY_LIGHT_1);
  GPIO_SetBits(GPIOB, KEY_LIGHT_2);
  GPIO_SetBits(GPIOA, KEY_LIGHT_3);
  key_press = 0x01;
  TIM_Cmd(TIM4, ENABLE);
}
void Key_cc_off(void){
  GPIO_ResetBits(GPIOB, KEY_LIGHT_2);
  GPIO_SetBits(GPIOB, KEY_LIGHT_1);
  GPIO_SetBits(GPIOA, KEY_LIGHT_3);
  key_press = 0x02;
  TIM_Cmd(TIM4, ENABLE);

}
void Key_cc_stop(void){
  GPIO_ResetBits(GPIOA, KEY_LIGHT_3);
  GPIO_SetBits(GPIOB, KEY_LIGHT_2);
  GPIO_SetBits(GPIOB, KEY_LIGHT_1);
  key_press = 0;
  TIM_Cmd(TIM4, DISABLE);
}

/******************************************************
                bָʾ��״̬����
******************************************************/
void Key_RGB_B_on(void){
  GPIO_ResetBits(GPIOA, KEY_RGB_B);
}
void Key_RGB_B_off(void){
  GPIO_SetBits(GPIOA, KEY_RGB_B);
}

/******************************************************
                wifi״ָ̬ʾ����
******************************************************/
void Wifi_Work_state_led(void){
  switch(wifi_work_status){
    case SMART_CONFIG_START:
      if(get_time2_value() % 2)
        Key_wifi_led_on();
      else
        Key_wifi_led_off();
    break;
    case WIFI_CONNECT_SUCCESS:
      Key_wifi_led_on();
    break;
    case WIFI_NOT_CONNECT:
      Key_wifi_led_off();
    break;
  }
}


/******************************************************
                pwm״̬����ָʾ����
******************************************************/
void Set_pwm_light_lv(void){
  if(light_b == 0){
    Key_RGB_B_off();
  }
}


/******************************************************
                PWM���ƺ���
******************************************************/
void RGB_tmp_value(){
  
  if(light_b > 0){
    if(Timer3_value == light_b){
        Key_RGB_B_off();
     }
  }
  if(Timer3_value >= 255){
    Timer3_value = 0;
    if(light_b > 0){
      Key_RGB_B_on();
    }
  }
}


/******************************************************
                Timer3�жϴ�����
******************************************************/
void TIM3_IRQHandler(void){
if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET){
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
    Timer3_value++;
    RGB_tmp_value();
  }
}


/******************************************************
                Timer4�жϴ�����
******************************************************/
void TIM4_IRQHandler(void){
if(TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET){
    TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
    if(key_press & 0x01){
      if(light_b >= 255)
        Key_cc_stop();
      else
        light_b++;
    }
    if(key_press & 0x02){
      if(light_b <= 0)
        Key_cc_stop();
      else
        light_b--;
      Set_pwm_light_lv();
    }
  }
}


/******************************************************
                RGBlight ��ʼ������
******************************************************/

uint8_t light_init(void){
  
  light_b = 255;
  GPIO_InitTypeDef GPIO_InitStruct;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStruct.GPIO_Pin = WIFI_STATUS_LED | KEY_RGB_B | KEY_LIGHT_3;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStruct);
  
  GPIO_InitStruct.GPIO_Pin = KEY_LIGHT_1 | KEY_LIGHT_2;
  GPIO_Init(GPIOB, &GPIO_InitStruct);
  
  GPIO_ResetBits(GPIOA,KEY_LIGHT_3);
  GPIO_SetBits(GPIOB, KEY_LIGHT_1);
  GPIO_SetBits(GPIOB, KEY_LIGHT_2);
  //Key_RGB_B_off();
  
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);                          //ʱ��ʹ��
  TIM_DeInit(TIM3);
  
  //50us
   
  //��ʱ��TIM3��ʼ��
  TIM_TimeBaseStructure.TIM_Period = 35;                                      	
  TIM_TimeBaseStructure.TIM_Prescaler = 99;                                    
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;                       
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;                   
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);                               

  TIM_ClearFlag(TIM3, TIM_FLAG_Update);
  TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);                                     

  //�ж����ȼ�NVIC����
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;                               
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;                     
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;                            
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                               
  NVIC_Init(&NVIC_InitStructure);                                                
  
  Timer3_value = 0;

  TIM_Cmd(TIM3, ENABLE);
  
  // 25ms
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);                          //ʱ��ʹ��
  TIM_DeInit(TIM4);
  TIM_TimeBaseStructure.TIM_Period = 3599;                                      	
  TIM_TimeBaseStructure.TIM_Prescaler = 499;                                    
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;                       
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;                   
  TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);                               

  TIM_ClearFlag(TIM4, TIM_FLAG_Update);
  TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);                                     

  //�ж����ȼ�NVIC����
  NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;                               
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;                     
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;                            
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                               
  NVIC_Init(&NVIC_InitStructure);                                                

  TIM_Cmd(TIM4, DISABLE);
  
  
  return (SUCCESS);
}