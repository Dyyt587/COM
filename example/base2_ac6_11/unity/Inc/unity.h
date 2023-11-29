#ifndef __UNITY_H
#define __UNITY_H
#ifdef __cplusplus
extern "C"
{
#endif
//限制最大值和最小值的宏
#define _limit(x,min,max) ((x)<(min)?(min):((x)>(max)?(max):(x)))
//限制最小值的宏
#define _limit_min(x,min) ((x)<(min)?(min):(x))
//限制最大值的宏
#define _limit_max(x,max) ((x)>(max)?(max):(x))
//绝对值
#define _abs(x) ((x)>0?(x):-(x))

#ifdef __cplusplus
}
#endif
#endif //__COMMON_H

