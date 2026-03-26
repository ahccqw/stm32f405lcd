#include "systick.h"

u8 g_touch_event = 0; 
static u32 g_tick_ms = 0;
/*************************
函数名称：SysTick_Intrerput(u16 nms)
函数功能：SysTick_Intrerput(u16 nms)初始化
返回值：无
形参：u16 nms
作者：me
版本：1.0
函数说明：参考时钟源，21mhz
*************************/
void SysTick_Intrerput_Init(u16 nms)
{
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
	
	SysTick->LOAD = nms * 21000 -1;;
	
	// 清除当前计数值
	SysTick->VAL = 0;
	
	// 使能SysTick中断，使能计数器
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;
	
	NVIC_SetPriority(TIM7_IRQn,NVIC_EncodePriority (0,8,0));//设置优先级
	
	// 注意：不需要调用 NVIC_EnableIRQ(SysTick_IRQn)，因为 SysTick 的中断使能
  // 是由 SysTick->CTRL 的 TICKINT 位控制的，而不是 NVIC 的 ISER 寄存器。
//	NVIC_InitTypeDef NVIC_InitStruct;
//	NVIC_InitStruct.NVIC_IRQChannel = SysTick_IRQn;
//	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 8;
//	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
//	NVIC_Init(&NVIC_InitStruct);

}

/*************************
函数名称：SysTick_Handler(void)
函数功能：定时中断处理函数
返回值：无
形参：无
作者：me
版本：1.0
函数说明：参考时钟源，21mhz
*************************/
// 全局标志，只在中断里写，主循环里读
  // 有新的触摸事件待处理

void SysTick_Handler(void)
{
    g_tick_ms++;
 
    /* 非音乐页面不做触摸处理，节省中断时间 */
    if(cst.current_page != 4) return;
 
    static u16 touch_tick      = 0;
    static u8  touch_processed = 0;
    static u8  swipe_processed = 0;
 
    touch_tick++;
    if(touch_tick < 10) return;   /* 每 10ms 采样一次 */
    touch_tick = 0;
 
    /* -------- 触摸采样（直接读寄存器，不调封装函数）-------- */
    if(TP_INT == 0)
    {
        u8  flag    = 0xFF;
        u8  temp[4] = {0};
        u16 tx, ty;
				
				//手势判断，其他手势则 出去
        Touch_IIc_Re_Ct_Byte(0x01, &flag, 1);
        if(flag == 0x00 || flag > 0x05) goto release_check;
				
				//判断获取x，y
        Touch_IIc_Re_Ct_Byte(0x03, temp, 4);
        tx = (temp[0] & 0x0f) << 8 | temp[1];
        ty = (temp[2] & 0x0f) << 8 | temp[3];
 
        /* --- 单击 --- */
        if(flag == 0x05 && touch_processed == 0)
        {
            touch_processed = 1;
 
            if     (tx > 34  && tx < 77  && ty > 143 && ty < 188) cst.rang_flag = 5;
            else if(tx > 99  && tx < 143 && ty > 143 && ty < 188) cst.rang_flag = 6;
            else if(tx > 162 && tx < 208 && ty > 143 && ty < 188) cst.rang_flag = 7;
        }
        /* --- 滑动 --- */
        else if((flag == 0x01 || flag == 0x02 ||
                 flag == 0x03 || flag == 0x04) && swipe_processed == 0)
        {
            swipe_processed = 1;
            switch(flag)
            {
                case 0x01: cst.rang_flag = 5; break;   /* 下滑：上一首 */
                case 0x02: cst.rang_flag = 7; break;   /* 上滑：下一首 */
                case 0x03:                              /* 左滑：退出音乐页 */
                case 0x04:                              /* 右滑：退出音乐页 */
                    cst.current_page     = 0;
                    music_start          = 0;
                    status_dev.PlayState = PLAY_STOP;
                    break;
                default: break;
            }
        }
    }
 
release_check:
    /* 手指抬起，复位去抖标志 */
    if(TP_INT != 0)
    {
        touch_processed = 0;
        swipe_processed = 0;
    }
 
    /* -------- rang_flag → PlayState -------- */
    if(cst.rang_flag == 5)
    {
        status_dev.PlayState = PLAY_PREVIOUS;
        music_start          = 1;
        cst.rang_flag        = 0;
    }
    else if(cst.rang_flag == 6)
    {
        music_start = !music_start;
        if(music_start == 0) status_dev.PlayState = PLAY_STOP;
        cst.rang_flag = 0;
    }
    else if(cst.rang_flag == 7)
    {
        status_dev.PlayState = PLAY_NEXT;
        music_start          = 1;
        cst.rang_flag        = 0;
    }
 
    /* -------- WiFi 音乐指令（阻塞期间由中断接管）-------- */
    if(u2.rec_flag == 1 && u2_music_page4_handled == 0)
    {
        char *b = (char *)u2.buff;
        if(strstr(b, "\"music\":\"1\"") != NULL)
        {
            status_dev.PlayState    = PLAY_PREVIOUS;
            music_start             = 1;
						u2.rec_flag = 0;
        }
        else if(strstr(b, "\"music\":\"2\"") != NULL)
        {
            music_start             = 0;
            status_dev.PlayState    = PLAY_STOP;
						u2.rec_flag = 0;
        }
        else if(strstr(b, "\"music\":\"3\"") != NULL)
        {
            music_start             = 1;
						u2.rec_flag = 0;
        }
        else if(strstr(b, "\"music\":\"4\"") != NULL)
        {
            status_dev.PlayState    = PLAY_NEXT;
            music_start             = 1;
						u2.rec_flag = 0;
						
        }

        /*
         * 注意：此处故意不清 u2.rec_flag。
         * 若帧中同时含有风扇/LED等非音乐指令，
         * 主循环退出音乐页面后的下一次 Wifi_Control 会处理它们。
         * 若只含音乐指令，主循环在 cst.current_page==4 时会跳过，
         * u2.rec_flag 将在下一帧覆盖前被新数据清除。
         * 如果希望更干净，可在此处清 u2.rec_flag，
         * 但需确保非音乐指令不会丢失。
         */
    }
 
    /* -------- 语音音乐指令（不清 u3.flag，主循环同步执行完整逻辑）-------- */
    if(u3.flag == 1 && u3_music_processed == 0)
    {
        u3_music_processed = 1;
        switch(u3.buff[1])
        {
            case 0x0c: status_dev.PlayState = PLAY_PREVIOUS; music_start = 1; break;
            case 0x07: music_start = 1;                                        break;
            case 0x08: music_start = 0; status_dev.PlayState = PLAY_STOP;     break;
            case 0x0D: status_dev.PlayState = PLAY_NEXT; music_start = 1;     break;
            default:   break;
        }
        /*
         * u3.flag 保持为 1，让主循环的 Voice_Control 在
         * Audio_MusicPlay 结束后（非阻塞间隙）完成页面切换等逻辑。
         */
    }
}

// 新增：获取毫秒数
u32 Get_Ms(void)
{
  return g_tick_ms;
}
