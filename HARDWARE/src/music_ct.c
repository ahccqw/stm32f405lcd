#include "music_ct.h"

u8 music_start            = 0;
u8 u3_music_processed     = 0;
u8 u2_music_page4_handled = 0;
/*************************
函数名称：void music_ct(void)
函数功能：音乐播放控制
返回值：无 
形参：无
作者：me
版本：1.0 
*************************/
void music_ct(void)
{

    
    if(cst.current_page == 4 && music_start == 1)
    {

        Audio_MusicPlay();

    }
}







