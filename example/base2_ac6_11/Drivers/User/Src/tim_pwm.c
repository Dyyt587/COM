#include "tim.h"
#include "tim_pwm.h"
#include "stdint.h"
#include "stdlib.h"
#include "elog.h"

tim_pwm_t pwm0;


tim_GetFreq_u tim_tab[] = {
    TIM_CLOCK_TAB{0, 0}};
// 内部使用
void tim_pwm_set_reload(tim_pwm_t *tim_pwm, uint32_t reload);
void tim_pwm_set_prescaler(tim_pwm_t *tim_pwm, uint32_t prescaler);
uint32_t tim_pwm_get_cmp(tim_pwm_t *tim_pwm);
uint32_t tim_pwm_get_reload(tim_pwm_t *tim_pwm);
uint32_t tim_pwm_get_prescaler(tim_pwm_t *tim_pwm);
uint32_t tim_pwm_get_clock_freq(tim_pwm_t *tim_pwm);
uint32_t tim_pwm_get_psc(tim_pwm_t *tim_pwm);
// 通过匹配定时器地址以获取定时器时钟
uint32_t tim_pwm_auto_updata_clock(tim_pwm_t *tim_pwm)
{
    int i = 0;
    while (((tim_pwm->htim->Instance)) != tim_tab[i].value)
    {
        i++;
        if (((tim_tab[i].value) == (TIM_TypeDef *)0) && (tim_tab[i].tim_GetPCLK1Freq == 0))
            return 0xfffffff;
    }
    tim_pwm->tim_GetPCLK1Freq = tim_tab[i].tim_GetPCLK1Freq;
		tim_pwm->clock_freq = tim_pwm->tim_GetPCLK1Freq();
    return tim_pwm->clock_freq;
}
uint32_t tim_auto_updata_clock(tim_pwm_t *tim_pwm)
{
    //tim_pwm->clock_freq = tim_pwm->tim_GetPCLK1Freq();
    return tim_pwm->clock_freq;
}
// 定时器初始化
void tim_pwm_init(tim_pwm_t *tim_pwm, TIM_HandleTypeDef *htim, uint32_t channel, uint32_t clock_freq)
{
     tim_pwm->htim = htim;
    tim_pwm->channel = channel;
		SCB_CleanInvalidateDCache_by_Addr((uint32_t*)tim_pwm,sizeof(tim_pwm_t));
    tim_pwm->clock_freq = clock_freq;
}

// 设置占空比
void tim_pwm_set_duty(tim_pwm_t *tim_pwm, float duty)
{
    // 根据频率计算占空比对应的比较值
    duty = CLAMP(duty, 0, 1);
    __HAL_TIM_SET_COMPARE(tim_pwm->htim, tim_pwm->channel, (tim_pwm_get_reload(tim_pwm) * duty));
}
// 获取占空比
float tim_pwm_get_duty(tim_pwm_t *tim_pwm)
{
			SCB_CleanInvalidateDCache_by_Addr((uint32_t*)tim_pwm,sizeof(tim_pwm_t));

    return (float)tim_pwm_get_cmp(tim_pwm) / tim_pwm_get_reload(tim_pwm);
}

// 内部使用
void calculate_timer_values(uint32_t clock_freq, float timer_freq, uint16_t *psc, uint16_t *arr)
{
//    uint16_t _arr = 0;
////    _psc = 0;
//    _arr = *arr / timer_freq - 1;
     float max_freq = clock_freq / (9 + 1);                       // 最大pwm频率
     float min_freq = (float)clock_freq / (4295098368); // 最小pwm频率
     float mid_freq = clock_freq / (65536 );                  // 最小pwm频率

    if (timer_freq >= max_freq)
    {
        *psc = 0;
        *arr = 9;
        return;
    }
    if (timer_freq <= mid_freq)
    {
			if(timer_freq <= min_freq){
			 *arr = 0xFFFF;
        *psc = 0xFFFF;
				return;
			}
         *arr = 0xFFFF;
        *psc = clock_freq/(65536*timer_freq) -1;
        return;
    }


    uint32_t low = 0, high = 0xFFFF;
    while (low <= high) {
        uint32_t mid = (low + high) / 2;
        uint32_t freq = clock_freq / ((mid + 1)*(0xFFFF+1));    
        if (freq > timer_freq) {
            low = mid + 1;
        } else if (freq < timer_freq) {
            high = mid - 1;
        } else {
            *psc = mid;
            *arr = 0xFFFF;
            return;
        }
        if(high-low<=1){
            *psc = low; 
            *arr = clock_freq / (timer_freq * (*psc + 1)) - 1;
            return;
        }
    }
    *psc = high;
    *arr = clock_freq / (timer_freq * (high + 1)) - 1;
}
// 获取比较值
uint32_t tim_pwm_get_cmp(tim_pwm_t *tim_pwm)
{
    return __HAL_TIM_GET_COMPARE(tim_pwm->htim, tim_pwm->channel);
}

// 获取分频
uint32_t tim_pwm_get_psc(tim_pwm_t *tim_pwm)
{

    return tim_pwm->htim->Instance->PSC;
    //__HAL_TIM_GET_PRESCALER(tim_pwm->htim);
}
// 设置频率
void tim_pwm_set_freq(tim_pwm_t *tim_pwm, float freq)
{

    uint16_t psc,arr;
	//记录比较值和重装载值
	uint16_t cmp = tim_pwm_get_cmp(tim_pwm);
	uint16_t reload = tim_pwm_get_reload(tim_pwm);
    calculate_timer_values(tim_pwm->clock_freq, freq, &psc, &arr);

    __HAL_TIM_SET_PRESCALER(tim_pwm->htim, psc);
    __HAL_TIM_SET_AUTORELOAD(tim_pwm->htim, arr);
    tim_pwm_set_duty(tim_pwm,(float)cmp/(float)reload);
		log_d("psc=%d",psc);
		log_d("arr=%d",arr);
		log_v("(arr+1)*(psc+1)=%d",(arr+1)*(psc+1));
}
// 获取频率
float tim_pwm_get_freq(tim_pwm_t *tim_pwm)
{
			//SCB_CleanInvalidateDCache_by_Addr((uint32_t*)tim_pwm,sizeof(tim_pwm_t));

	float tmp= (((long long)(tim_pwm->htim->Instance->PSC)+1) * ((long long)(__HAL_TIM_GET_AUTORELOAD(tim_pwm->htim))+1));
	uint32_t clock = tim_pwm->clock_freq;
		float data = (float)clock / tmp;

    return data;
//	return clock;
}

// 获取自动重装值
uint32_t tim_pwm_get_reload(tim_pwm_t *tim_pwm)
{
    return __HAL_TIM_GET_AUTORELOAD(tim_pwm->htim);
}
// 启动定时器
void tim_pwm_start(tim_pwm_t *tim_pwm)
{
    HAL_TIM_PWM_Start(tim_pwm->htim, tim_pwm->channel);
}
// 停止定时器
void tim_pwm_stop(tim_pwm_t *tim_pwm)
{
    HAL_TIM_PWM_Stop(tim_pwm->htim, tim_pwm->channel);
}

void tim_updata_clock(tim_pwm_t *tim_pwm, uint32_t clock)
{
    //tim_pwm->clock_freq = clock;
}
// 获取时钟频率
uint32_t tim_pwm_get_clock(tim_pwm_t *tim_pwm)
{
    return tim_pwm->clock_freq;
}
