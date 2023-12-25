
#ifndef __TIM_PWM_H_
#define __TIM_PWM_H_
#ifdef __cplusplus
extern "C"{
#endif
#include "main.h"
typedef struct
{
    TIM_TypeDef* value;
    uint32_t (*tim_GetPCLK1Freq)(void);
} tim_GetFreq_u;

typedef struct
{
    TIM_HandleTypeDef *htim;
    uint32_t channel;
    void *userdata;
    uint32_t (*tim_GetPCLK1Freq)(void); // 用于自动更新时钟频率的函数
    uint32_t clock_freq;                // 可以由列表获取，也可由用户输入
} tim_pwm_t;
#define CLAMP(x, low, high) (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))

#ifdef STM32H7

// 定时器列表第一项的时钟获取函数
#define TIM_CLOCK_TAB {TIM12, HAL_RCC_GetPCLK1Freq}, \
                      {TIM12, HAL_RCC_GetPCLK1Freq}, \
                      {TIM12, HAL_RCC_GetPCLK1Freq}, \
                      {TIM12, HAL_RCC_GetPCLK1Freq},
#endif
#ifdef  STM32F4
// 定时器列表第一项的时钟获取函数
#define TIM_CLOCK_TAB {TIM12, HAL_RCC_GetPCLK1Freq}, 
#endif
#ifdef STM32F1  

#endif

extern tim_pwm_t pwm0;

void tim_pwm_init(tim_pwm_t *tim_pwm, TIM_HandleTypeDef *htim, uint32_t channel, uint32_t clock_freq);

void tim_pwm_set_duty(tim_pwm_t *tim_pwm, float duty);
void tim_pwm_set_freq(tim_pwm_t *tim_pwm, float freq);

float tim_pwm_get_duty(tim_pwm_t *tim_pwm);
float tim_pwm_get_freq(tim_pwm_t *tim_pwm);

uint32_t tim_pwm_auto_updata_clock(tim_pwm_t *tim_pwm);
void tim_pwm_set_clock_freq(tim_pwm_t *tim_pwm, uint32_t clock_freq);
#ifdef __cplusplus
}
#endif   
#endif
