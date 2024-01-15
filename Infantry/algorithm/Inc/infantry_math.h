/**
 * @file Infantry/algorithm
 * @author CharlesHsu
 * @date 11/12/2023
 */

#pragma once

#include "stm32g4xx_hal.h"
#include "arm_math.h"
#include "infantry_log.h"

#define mat                 arm_matrix_instance_f32
#define Matrix_Init         arm_mat_init_f32
#define Matrix_Add          arm_mat_add_f32
#define Matrix_Subtract     arm_mat_sub_f32
#define Matrix_Multiply     arm_mat_mult_f32
#define Matrix_Transpose    arm_mat_trans_f32
#define Matrix_Inverse      arm_mat_inverse_f32

/**
 * @brief      Limit function
 * @param      input :Limited value
 * @param      max :Max limite value
 * @retval     NULL
 */
#define LimitMax(input, max)   \
    {                          \
        if (input > max)       \
        {                      \
            input = max;       \
        }                      \
        else if (input < -max) \
        {                      \
            input = -max;      \
        }                      \
    }

/**
 * @brief      Maximum and minimum limiting
 * @param      input :Limited value
 * @param      max :Max limite value
 * @param      min :Min limite value
 * @retval     NULL
 */
#define LimitMaxMin(input, max, min) \
    {                                \
        if (input > max)             \
        {                            \
            input = max;             \
        }                            \
        else if (input < min)        \
        {                            \
            input = min;             \
        }                            \
    }

#define GetMaxandMinRange(input, max, min) {    \
    if (input >= max) {                         \
        max = input;                            \
    }                                           \
    else if (input <= min) {                    \
        min = input;                            \
    }                                           \
}

typedef struct {
    float acc;
    float dec;
} Math_SlopeParamTypeDef;

/**
 * @brief      Radian to angle
 * @param      Converted radian
 * @retval     result
 */
float Math_RadToAngle(float rad);

/**
 * @brief      Continuous power function with linear segment near the origin
 * @param      NULL
 * @retval     result
 */
float Math_Fal(float e, float alpha, float zeta);

/**
 * @brief      Calculate fsg
 * @param      x :Number to be calc
 * @retval     result
 */
int16_t Math_Fsg(float x, float d);

/**
 * @brief      Positive and negative judgment function
 * @param      x :Number to be judged
 * @retval     Positive output 1, negative output - 1, otherwise output 0
 */
int16_t Math_Sign(float Input);

/**
 * @brief      This shit is used to calculate the quick square root
 * @param      x :Number of square root
 * @retval     One third of the open results
 */
float Math_InvSqrt(float x);

/**
 * @brief      Initialize ramp function control parameters
 * @param      pparam: Pointer to ramp function control parameter
 * @param      kp: P factor
 * @param      ki: I factor
 * @param      kd: D factor
 * @param      sum_max: Integral limiting
 * @param      output_max: Output limiting
 * @retval     NULL
 */
void Math_InitSlopeParam(Math_SlopeParamTypeDef *pparam, float acc, float dec);

/**
 * @brief      Calculate slope function setting
 * @param      rawref: Current setting value
 * @param      targetref: Target set point
 * @param      pparam: Pointer to ramp function control parameter
 * @retval     Slope function setting value. If slope function is not enabled (parameter is 0), the target setting value is returned
 */
float Math_CalcSlopeRef(float rawref, float targetref, Math_SlopeParamTypeDef *pparam);

/**
 * @brief      Calculate the absolute slope function setting value
 * @param      rawref: Current setting value
 * @param      targetref: Target set point
 * @param      pparam: Pointer to ramp function control parameter
 * @retval     Absolute value ramp function setting value. If ramp function is not enabled, the target setting value is returned
 */
float Math_CalcAbsSlopeRef(float rawref, float targetref, Math_SlopeParamTypeDef *pparam);

/**
 * @brief      Calculation differential (only two order)(To be improved)
 * @param      arr :point to be differential value
 * @param      order :The differential order
 * @retval     NULL
 */
float MathDifferential(float arr[], uint8_t order);

/**
 * @brief      Extract int16 from byte buffer
 * @param      buff: Pointer to byte buffer
 * @retval     The extracted int16
 */
int16_t buff2i16(uint8_t *buff);

/**
 * @brief Saturates the input value using a hyperbolic tangent function.
 * @param input The input value to be saturated.
 * @param k The scaling factor that defines the saturation limit.
 * @return float The saturated output, which is the result of applying the hyperbolic tangent function to the input and then scaling it by k.
 */
float SaturateWithTanhK(float input, float k);
