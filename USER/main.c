
#include "stm32f4xx.h"

#include "sdio_sdcard.h"
#include "ff.h"
#include "exfuns.h"
#include "malloc.h"

#include "Timer_Driver.h"
#include "Led_Driver.h"

#include "flash_if.h"
#include "download.h"

 
typedef void (*pFunction)(void);
/********************************************************************************
* ������ : IAP_LoadApp
* ��  �� : Bootloader��ת��APP
* ˵  �� : none
* ��  �� : none
* ��  �� : none
* ��  �� : Shatang                    ʱ  �� : 2020.06.23
* ��  �� : none                       ʱ  �� : none
********************************************************************************/
void IAP_LoadApp(uint32_t appxaddr)
{
    pFunction JumpApp;
    
	if(((*(__IO uint32_t*)appxaddr) & 0x2FFE0000) == 0x20000000)//���ջ����ַ�Ƿ�Ϸ�.
	{ 
		JumpApp = (pFunction)*(__IO uint32_t*)(appxaddr+4);		//�û��������ڶ�����Ϊ����ʼ��ַ(��λ��ַ)		
		__set_MSP(*(__IO uint32_t*)appxaddr);				    //��ʼ��APP��ջָ��(�û��������ĵ�һ�������ڴ��ջ����ַ)
		JumpApp();								                //��ת��APP.
	}
}



int main(void)
{
    SD_Error le_SD_InitResult = SD_ERROR;
    uint32_t lu32_SD_InitTick = 0;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
    LED_PortInit();
    TIM3_Init();
    FLASH_If_Init();
    
	my_mem_init(SRAMIN);		//��ʼ���ڲ��ڴ��
//	my_mem_init(SRAMCCM);		//��ʼ��CCM�ڴ��

    LED(1);
    
    lu32_SD_InitTick = SysTimeGet();
 	while(le_SD_InitResult)//��ⲻ��SD��
	{
        le_SD_InitResult = SD_Init();
        if(SysTimeGet() - lu32_SD_InitTick > 2000)break;//����2s�����м��
	}
    
    if(SD_OK == le_SD_InitResult)
    {
        exfuns_init();							//Ϊfatfs��ر��������ڴ�
        f_mount(fs[0],"0:",1); 					//����SD��
        
        LED(0);//����ʱ���
        SDCard_Download();//��ȡ�����ļ���д��
    }

    FLASH_If_Finish();
    IAP_Close();
    IAP_LoadApp(APPLICATION_START_ADDRESS); //������ת
    
    while(1){}

    return 0;
}




