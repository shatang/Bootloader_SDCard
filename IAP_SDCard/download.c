#include "download.h"
#include "ff.h"
#include "exfuns.h"
#include "flash_if.h"


/********************************************************************************
* 函数名 : IAP_Close
* 功  能 : IAP功能关闭
* 说  明 : 负责关闭一些中断、时钟；防止跳转至App时死机
* 入  参 : none
* 返  回 : none
* 设  计 : Shatang                    时  间 : 2020.09.17
* 修  改 : none                       时  间 : none
********************************************************************************/
void IAP_Close(void)
{
    TIM_Cmd(TIM3, DISABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,  DISABLE);
}


char *m_file = "0:Shatang.BIN"; //APP 文件名称 默认起始为 "2:"后跟文件名，文件名不能含中文

#define READ_COUNT 512
uint8_t   ReadAppBuffer[READ_COUNT];

/********************************************************************************
* 函数名 : SDCard_Download
* 功  能 : SD卡文件烧录进Flash
* 说  明 : none
* 入  参 : none
* 返  回 : none
* 设  计 : Shatang                    时  间 : 2020.09.17
* 修  改 : none                       时  间 : none
********************************************************************************/
void SDCard_Download(void)
{ 
	FIL    FP_Struct;			               //用于打开文件
    uint8_t gCheckFat=0;

    gCheckFat = f_open(&FP_Struct,m_file,FA_READ);
    if(gCheckFat == 0)
    {
        uint32_t i = 0;
        uint16_t ReadNum = 0;
        
        uint32_t APP_Sector = 0;
        uint16_t APP_Byte = 0;
        uint32_t FlashDestination = APPLICATION_START_ADDRESS;//初始化写入Flash地址变量
        
        APP_Sector = FP_Struct.fsize / READ_COUNT;
        APP_Byte = FP_Struct.fsize % READ_COUNT;

        
        if(APP_Sector+APP_Byte > APPLICATION_SIZE - 1)return;//超出芯片可存储大小

        FLASH_If_Erase(APPLICATION_START_ADDRESS);//计算需要擦除Flash的页
        
        for(i = 0;i < APP_Sector;i++)
        {
            f_read (&FP_Struct, ReadAppBuffer, READ_COUNT, (UINT *)&ReadNum);
            FLASH_If_Write(&FlashDestination, (u32*)ReadAppBuffer, READ_COUNT/4);
        }
        if(APP_Byte != 0)
        {
            f_read (&FP_Struct,ReadAppBuffer,APP_Byte,(UINT *)&ReadNum);
            FLASH_If_Write(&FlashDestination, (u32*)ReadAppBuffer, APP_Byte/4);
        }
        f_close (&FP_Struct);	
//        printf("系统更新成功！\r\n");
    }
}
