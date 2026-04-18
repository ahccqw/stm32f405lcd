#include "tim7_7ms.h"

/*
函数名称：TIM7_7ms_Config
函数功能：初始化 TIM7 定时器 (用于 Modbus 帧超时检测)
函数参数：
    psc   -- 预分频系数 (例如 84，对应 1MHz 计数频率)
    reload-- 重装载值 (例如 7000，对应 7ms 超时)
		此中断用来判断数据是否接收完成，虽然帧间隔是 3.5ms 但是为了防止安全
函数返回值：无
*/
void TIM7_7ms_Config(void)
{
    NVIC_InitTypeDef NVIC_InitStruct;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
    
    // 1. 使能 TIM7 时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE); 

    // 2. 配置 TIM7 时基单元
    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;     // 时钟分频因子
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up; // 向上计数模式
    TIM_TimeBaseInitStruct.TIM_Period = 7000 - 1;              // 自动重装载值
    TIM_TimeBaseInitStruct.TIM_Prescaler = 84 - 1;              // 预分频值
    TIM_TimeBaseInit(TIM7, &TIM_TimeBaseInitStruct); 

    // 3. 清除中断标志位并开启中断
    TIM_ClearITPendingBit(TIM7, TIM_IT_Update); // 清除更新中断请求位
    TIM_ITConfig(TIM7, TIM_IT_Update, ENABLE);  // 开启更新中断
    
    // 4. 配置 NVIC 中断优先级
    NVIC_InitStruct.NVIC_IRQChannel = TIM7_IRQn;        // TIM7 中断通道
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;        // 使能中断通道
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;     // 次级优先级
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2; // 抢占优先级
    NVIC_Init(&NVIC_InitStruct);                        // 初始化 NVIC

    // 5. 初始状态先关闭定时器，等待串口接收到数据后再开启
    TIM_Cmd(TIM7, DISABLE); 
}

/*
函数名称：TIM7_IRQHandler
函数功能：TIM7 中断服务函数
说明：当定时器溢出（即超过设定时间未收到新数据）时触发
*/
void TIM7_IRQHandler(void)
{
    // 1. 判断是否为更新中断
    if(TIM_GetITStatus(TIM7, TIM_IT_Update) != RESET)
    {
        // 2. 清除中断标志位 (建议先清除，防止嵌套)
        TIM_ClearITPendingBit(TIM7, TIM_IT_Update);
        
        // 3. 关闭定时器
        TIM_Cmd(TIM7, DISABLE); 

        // 4. 【核心修正】加上大括号和模式判断
        if (current_mode == MODE_MASTER)
        {
            // 主机模式：只有主机计数大于1（说明收到了有效数据），才置位主机标志
            if(modbus_master.rxcount > 1)
            {
                modbus_master.rxover = 1;
            }
        }
        else
        {
            printf("tim7 timeout, rxcount=%d\r\n", modbus_slve.rxcount);
            // 从机模式：只有从机计数大于1，才置位从机标志
            if(modbus_slve.rxcount > 1)
            {
                modbus_slve.rxover = 1;
            }
        }
    }
}
