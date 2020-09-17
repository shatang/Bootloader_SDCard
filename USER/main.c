
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
* 函数名 : IAP_LoadApp
* 功  能 : Bootloader跳转至APP
* 说  明 : none
* 入  参 : none
* 返  回 : none
* 设  计 : Shatang                    时  间 : 2020.06.23
* 修  改 : none                       时  间 : none
********************************************************************************/
void IAP_LoadApp(uint32_t appxaddr)
{
    pFunction JumpApp;
    
	if(((*(__IO uint32_t*)appxaddr) & 0x2FFE0000) == 0x20000000)//检查栈顶地址是否合法.
	{ 
		JumpApp = (pFunction)*(__IO uint32_t*)(appxaddr+4);		//用户代码区第二个字为程序开始地址(复位地址)		
		__set_MSP(*(__IO uint32_t*)appxaddr);				    //初始化APP堆栈指针(用户代码区的第一个字用于存放栈顶地址)
		JumpApp();								                //跳转到APP.
	}
}



int main(void)
{
    SD_Error le_SD_InitResult = SD_ERROR;
    uint32_t lu32_SD_InitTick = 0;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
    LED_PortInit();
    TIM3_Init();
    FLASH_If_Init();
    
	my_mem_init(SRAMIN);		//初始化内部内存池
//	my_mem_init(SRAMCCM);		//初始化CCM内存池

    LED(1);
    
    lu32_SD_InitTick = SysTimeGet();
 	while(le_SD_InitResult)//检测不到SD卡
	{
        le_SD_InitResult = SD_Init();
        if(SysTimeGet() - lu32_SD_InitTick > 2000)break;//超过2s不进行检测
	}
    
    if(SD_OK == le_SD_InitResult)
    {
        exfuns_init();							//为fatfs相关变量申请内存
        f_mount(fs[0],"0:",1); 					//挂载SD卡
        
        LED(0);//升级时灭灯
        SDCard_Download();//读取升级文件并写入
    }

    FLASH_If_Finish();
    IAP_Close();
    IAP_LoadApp(APPLICATION_START_ADDRESS); //程序跳转
    
    while(1){}

    return 0;
}




