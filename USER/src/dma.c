#include "dma.h"

/*************************
函数名称：ADC1_DMA2_Stream0_CH0_Init
函数功能：配置 DMA2_Stream0 通道0，用于 ADC1 数据搬运
返回值：无
形参：
        num  ：DMA 传输的数据个数
        data ：存放 ADC 转换结果的数组地址
作者：me
版本：1.0
函数说明：
        ADC1 -> DMA2_Stream0 -> Memory
        DMA负责把ADC转换结果自动搬运到数组中
*************************/
void ADC1_DMA2_Stream0_CH0_Init(u16 num,u16 *data)
{
	// 使能 ADC1 的 DMA 传输（关键！）
  ADC_DMACmd(ADC1, ENABLE);
	
	// 允许ADC在最后一次传输后继续产生DMA请求 (对应ADC_CR2的DDS位)
	ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);
	
	//打开时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);
	
 /* 3. DMA关闭后配置 */
	DMA_Cmd(DMA2_Stream0, DISABLE);
	while(DMA_GetCmdStatus(DMA2_Stream0) != DISABLE);
	
	//配置DMA2
	DMA_InitTypeDef DMA_InitStruct = {0};
	
	DMA_InitStruct.DMA_BufferSize = num;//4. 设置DMA传输数据的次数，每次传输一个半字
	DMA_InitStruct.DMA_Channel = DMA_Channel_0;//5. 选择DMA通道（ADC1对应DMA2_Stream0_Channel0）
	DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralToMemory;//6. 设置数据传输方向：外设 -> 内存
	DMA_InitStruct.DMA_Memory0BaseAddr = (u32)data;//7. 设置内存地址（ADC数据存放的数组）
	DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;//8. 设置内存数据宽度（ADC是12位，用16位存储）
	DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;//9. 内存地址递增（每次存到数组下一个位置）
	DMA_InitStruct.DMA_Mode = DMA_Mode_Circular;//10. DMA工作模式：循环模式（采集完成后自动重新开始）
	
	DMA_InitStruct.DMA_PeripheralBaseAddr = (u32)&ADC1->DR;//11. 外设地址（ADC数据寄存器）
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//12. 外设数据宽度（ADC数据寄存器16位）
	DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//13. 外设地址不递增（ADC数据寄存器固定）
	DMA_InitStruct.DMA_Priority = DMA_Priority_Medium;//14. DMA优先级设置
	
	DMA_Init(DMA2_Stream0,&DMA_InitStruct);
	
	//使能DMA2
	DMA_Cmd(DMA2_Stream0, ENABLE);
}

