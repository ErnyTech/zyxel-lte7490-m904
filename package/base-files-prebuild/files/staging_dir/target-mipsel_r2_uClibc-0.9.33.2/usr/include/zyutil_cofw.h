#ifndef _ZYUTIL_COFW_H
#define _ZYUTIL_COFW_H

typedef enum
{
    MODEL_UNDEFINED = 0,
    MODEL_VMG3312_T20A,
    MODEL_VMG1312_T20A,
    MODEL_VMG8623_T50A,
    MODEL_VMG3625_T50A,
    MODEL_VMG8623_T50B,
    MODEL_EMG5523_T50A,
    //VMG8825-T50K series
    MODEL_VMG8825_T50,
    MODEL_VMG3927_T50K,
    MODEL_EMG5723_T50K,
    MODEL_VMG3927_B50B,
    MODEL_EMG8726_B10A,
    MODEL_VMG4927_B50A,
    MODEL_VMG9827_B50A,
    MODEL_EMG6726_B10A,
    MODEL_PX7501_B0,
    MODEL_AX7501_B0,
    MODEL_PMG5817_T20A,
    MODEL_PMG5917_T20A,
    MODEL_VMG8825_B50B,
    MODEL_AM3100_B0,
    MODEL_DX5301_B0,
    MODEL_EX5301_B0,
    MODEL_DX5301_B1,
    MODEL_EX5301_B1,
    MODEL_DX5301_B2,
    /*MODEL_EX5301_B2,*/
    MODEL_EX5300_B3,
    MODEL_DX5301_B3,
    MODEL_EX5301_B3,
    MODEL_DX3301_T0,
    MODEL_EX3301_T0,
    MODEL_DX3300_T0,
    MODEL_EX3300_T0,
    MODEL_DX5401_B0,
    MODEL_EX5401_B0
} model_name_t;

/*!
 *  Get Model Name
 *
 *  @return     model_name_t -  model name enum value
 *              ex. MODEL_VMG1312_T20A
 *
 */
model_name_t zyutil_cofw_modelNameGet(void);

#endif
