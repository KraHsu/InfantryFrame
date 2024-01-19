/*
 *                        ::
 *                       :;J7, :,                        ::;7:
 *                       ,ivYi, ,                       ;LLLFS:
 *                       :iv7Yi                       :7ri;j5PL
 *                      ,:ivYLvr                    ,ivrrirrY2X,
 *                      :;r@Wwz.7r:                :ivu@kexianli.
 *                     :iL7::,:::iiirii:ii;::::,,irvF7rvvLujL7ur
 *                    ri::,:,::i:iiiiiii:i:irrv177JX7rYXqZEkvv17
 *                 ;i:, , ::::iirrririi:i:::iiir2XXvii;L8OGJr71i
 *               :,, ,,:   ,::ir@mingyi.irii:i:::j1jri7ZBOS7ivv,
 *                  ,::,    ::rv77iiiriii:iii:i::,rvLq@huhao.Li
 *              ,,      ,, ,:ir7ir::,:::i;ir:::i:i::rSGGYri712:
 *            :::  ,v7r:: ::rrv77:, ,, ,:i7rrii:::::, ir7ri7Lri
 *           ,     2OBBOi,iiir;r::        ,irriiii::,, ,iv7Luur:
 *         ,,     i78MBBi,:,:::,:,  :7FSL: ,iriii:::i::,,:rLqXv::
 *         :      iuMMP: :,:::,:ii;2GY7OBB0viiii:i:iii:i:::iJqL;::
 *        ,     ::::i   ,,,,, ::LuBBu BBBBBErii:i:i:i:i:i:i:r77ii
 *       ,       :       , ,,:::rruBZ1MBBqi, :,,,:::,::::::iiriri:
 *      ,               ,,,,::::i:  @arqiao.       ,:,, ,:::ii;i7:
 *     :,       rjujLYLi   ,,:::::,:::::::::,,   ,:i,:,,,,,::i:iii
 *     ::      BBBBBBBBB0,    ,,::: , ,:::::: ,      ,,,, ,,:::::::
 *     i,  ,  ,8BMMBBBBBBi     ,,:,,     ,,, , ,   , , , :,::ii::i::
 *     :      iZMOMOMBBM2::::::::::,,,,     ,,,,,,:,,,::::i:irr:i:::,
 *     i   ,,:;u0MBMOG1L:::i::::::  ,,,::,   ,,, ::::::i:i:iirii:i:i:
 *     :    ,iuUuuXUkFu7i:iii:i:::, :,:,: ::::::::i:i:::::iirr7iiri::
 *     :     :rk@Yizero.i:::::, ,:ii:::::::i:::::i::,::::iirrriiiri::,
 *      :      5BMBBBBBBSr:,::rv2kuii:::iii::,:i:,, , ,,:,:i@petermu.,
 *           , :r50EZ8MBBBBGOBBBZP7::::i::,:::::,: :,:,::i;rrririiii::
 *               :jujYY7LS0ujJL7r::,::i::,::::::::::::::iirirrrrrrr:ii:
 *            ,:  :@kevensun.:,:,,,::::i:i:::::,,::::::iir;ii;7v77;ii;i,
 *            ,,,     ,,:,::::::i:iiiii:i::::,, ::::iiiir@xingjief.r;7:i,
 *         , , ,,,:,,::::::::iiiiiiiiii:,:,:::::::::iiir;ri7vL77rrirri::
 *          :,, , ::::::::i:::i:::i:i::,,,,,:,::i:i:::iir;@Secbone.ii:::
 * 
 * @Date: 2024-01-16 20:40:50
 * @LastEditors: KraHsu && 1191393280@qq.com
 * @LastEditTime: 2024-01-17 01:56:13
 * Copyright (c) 2024 by KraHsu, All Rights Reserved. 
 */

#include "infantry_dwt_lib.h"

DWT_DataTypeDef DWTData;

void DWT_Init(uint32_t CPU_Freq_MHz) {
    DWT_DataTypeDef *dwt = DWT_GetDWTDataPtr();

    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = (uint32_t) 0u;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

    dwt->CPU_FREQ_Hz = CPU_Freq_MHz * 1000000;
    dwt->CPU_FREQ_Hz_ms = dwt->CPU_FREQ_Hz / 1000;
    dwt->CPU_FREQ_Hz_us = dwt->CPU_FREQ_Hz / 1000000;
    dwt->CYCCNT_RountCount = 0;
    dwt->CYCCNT_LAST = 0;
}

DWT_DataTypeDef *DWT_GetDWTDataPtr() {
    return &DWTData;
}

float DWT_GetDeltaT(uint32_t *cnt_last) {
    DWT_DataTypeDef *dwt = DWT_GetDWTDataPtr();

    volatile uint32_t cnt_now = DWT->CYCCNT;
    float dt = ((uint32_t) (cnt_now - *cnt_last)) / ((float) (dwt->CPU_FREQ_Hz));
    *cnt_last = cnt_now;

    return dt;
}

float DWT_GetDeltaTWithoutUpdate(uint32_t *cnt_last) {
    DWT_DataTypeDef *dwt = DWT_GetDWTDataPtr();

    float dt = ((uint32_t) (uint32_t) (DWT->CYCCNT - *cnt_last)) / ((float) (dwt->CPU_FREQ_Hz));

    return dt;
}

void DWT_SysTimeUpdate() {
    DWT_DataTypeDef *dwt = DWT_GetDWTDataPtr();

    volatile uint32_t cnt_now = DWT->CYCCNT;
    uint64_t CNT_TEMP1, CNT_TEMP2, CNT_TEMP3;

    DWT_CNT_Update();

    dwt->CYCCNT64 = (uint64_t) dwt->CYCCNT_RountCount * (uint64_t) UINT32_MAX + (uint64_t) cnt_now;
    CNT_TEMP1 = dwt->CYCCNT64 / dwt->CPU_FREQ_Hz;
    CNT_TEMP2 = dwt->CYCCNT64 - CNT_TEMP1 * dwt->CPU_FREQ_Hz;
    dwt->SysTime.s = CNT_TEMP1;
    dwt->SysTime.ms = CNT_TEMP2 / dwt->CPU_FREQ_Hz_ms;
    CNT_TEMP3 = CNT_TEMP2 - dwt->SysTime.ms * dwt->CPU_FREQ_Hz_ms;
    dwt->SysTime.us = CNT_TEMP3 / dwt->CPU_FREQ_Hz_us;
    dwt->SysTime.ms_tick = dwt->SysTime.s * 1000 + dwt->SysTime.ms;
}

float DWT_GetTimeline_s() {
    DWT_DataTypeDef *dwt = DWT_GetDWTDataPtr();

    DWT_SysTimeUpdate();

    float DWT_Timelinef32 = dwt->SysTime.s + dwt->SysTime.ms * 0.001f + dwt->SysTime.us * 0.000001f;

    return DWT_Timelinef32;
}

float DWT_GetTimeline_ms() {

    DWT_DataTypeDef *dwt = DWT_GetDWTDataPtr();

    DWT_SysTimeUpdate();

    float DWT_Timelinef32 = dwt->SysTime.s * 1000 + dwt->SysTime.ms + dwt->SysTime.us * 0.001f;

    return DWT_Timelinef32;
}

uint64_t DWT_GetTimeline_us() {
    DWT_DataTypeDef *dwt = DWT_GetDWTDataPtr();

    DWT_SysTimeUpdate();

    uint64_t DWT_Timelinef32 = dwt->SysTime.s * 1000000 + dwt->SysTime.ms * 1000 + dwt->SysTime.us;

    return DWT_Timelinef32;
}

void DWT_CNT_Update() {
    DWT_DataTypeDef *dwt = DWT_GetDWTDataPtr();
    static volatile uint8_t pending = 0;
    if (pending == 0) {
        pending = 1;
        volatile uint32_t cnt_now = DWT->CYCCNT;

        if (cnt_now < dwt->CYCCNT_LAST) {
            dwt->CYCCNT_RountCount++;
        }

        dwt->CYCCNT_LAST = cnt_now;
        pending = 0;
    }
}

void DWT_Delay(float Delay) {
    DWT_DataTypeDef *dwt = DWT_GetDWTDataPtr();
    uint32_t tickstart = DWT->CYCCNT;
    float wait = Delay;

    while ((DWT->CYCCNT - tickstart) < wait * (float) dwt->CPU_FREQ_Hz) {
    }
}

void DWT_Delayms(float Delay) {
    DWT_DataTypeDef *dwt = DWT_GetDWTDataPtr();
    uint32_t tickstart = DWT->CYCCNT;
    float wait = Delay;

    while ((DWT->CYCCNT - tickstart) < wait * (float) dwt->CPU_FREQ_Hz_ms) {
    }
}

void DWT_Delayus(float Delay) {
    DWT_DataTypeDef *dwt = DWT_GetDWTDataPtr();
    uint32_t tickstart = DWT->CYCCNT;
    float wait = Delay;

    while ((DWT->CYCCNT - tickstart) < wait * (float) dwt->CPU_FREQ_Hz_us) {
    }
}
