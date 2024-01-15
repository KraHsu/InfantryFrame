/**
 * @file Infantry/algorithm
 * @author CharlesHsu
 * @date 11/12/2023
 */

#include "infantry_math.h"
#include "plugins/logger/Inc/infantry_log.h"


float Math_RadToAngle(float rad) {
    return (rad * 180.0f / PI);
}


float Math_Fal(float e, float alpha, float zeta) {
    int16_t s = 0;
    float fal_output = 0;
    s = (Math_Sign(e + zeta) - Math_Sign(e - zeta)) / 2;
    fal_output = e * s / (powf(zeta, 1 - alpha)) + powf(fabs(e), alpha) * Math_Sign(e) * (1 - s);
    return fal_output;
}


int16_t Math_Fsg(float x, float d) {
    int16_t output = 0;
    output = (Math_Sign(x + d) - Math_Sign(x - d)) / 2;
    return output;
}

int16_t Math_Sign(float x) {
    int16_t output = 0;
    if (x > 0) {
        output = 1;
    } else if (x < 0) {
        output = -1;
    } else {
        output = 0;
    }
    return output;
}


float Math_InvSqrt(float x) {
    float halfx = 0.5f * x;
    float y = x;
    long i = *(long *) &y;
    i = 0x5f3759df - (i >> 1);
    y = *(float *) &i;
    y = y * (1.5f - (halfx * y * y));
    return y;
}


float MathDifferential(float arr[], uint8_t order) {
    float value;
    switch (order) {
        case 1:
            value = arr[0] - arr[1];
            break;
        case 2:
            value = arr[2] - 2 * arr[1] + arr[0];
            break;
        default:
            value = arr[0];
            break;
    }
    return value;
}


void Math_InitSlopeParam(Math_SlopeParamTypeDef *pparam, float acc, float dec) {
    pparam->acc = acc;
    pparam->dec = dec;
}

float Math_CalcSlopeRef(float rawref, float targetref, Math_SlopeParamTypeDef *pparam) {
    float newref;
    if (pparam->acc == 0 | pparam->dec == 0) {
        return targetref;
    }
    if (rawref < targetref - pparam->acc) {
        newref = rawref + pparam->acc;
    } else if (rawref > targetref + pparam->dec) {
        newref = rawref - pparam->dec;
    } else {
        newref = targetref;
    }
    return newref;
}

float Math_CalcAbsSlopeRef(float rawref, float targetref, Math_SlopeParamTypeDef *pparam) {
    float newref;
    if (pparam->acc == 0 | pparam->dec == 0) {
        return targetref;
    }
    if (rawref > 0) {
        if (rawref < targetref - pparam->acc) {
            newref = rawref + pparam->acc;
        } else if (rawref > targetref + pparam->dec) {
            newref = rawref - pparam->dec;
        } else {
            newref = targetref;
        }
    } else {
        if (rawref > targetref + pparam->acc) {
            newref = rawref - pparam->acc;
        } else if (rawref < targetref - pparam->dec) {
            newref = rawref + pparam->dec;
        } else {
            newref = targetref;
        }
    }
    return newref;
}

int16_t buff2i16(uint8_t *buff) {
    uint8_t tmpbuff[2];
    memcpy(tmpbuff, buff, 2);
    return *((int16_t *) tmpbuff);
}

float SaturateWithTanhK(float input, float k) {
    return std::tanh(input) * k;
}
