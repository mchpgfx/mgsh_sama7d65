#include "gfx/legato/core/legato_real_i16.h"

leReal_i16 __AA_SampleWeights[5] =
{
    LE_REAL_I16_FROM_FLOAT(1.0f),
    LE_REAL_I16_FROM_FLOAT(1.0f / 2.0f),
    LE_REAL_I16_FROM_FLOAT(1.0f / 4.0f),
    LE_REAL_I16_FROM_FLOAT(1.0f / 8.0f),
    LE_REAL_I16_FROM_FLOAT(1.0f / 16.0f),
};

static leReal_i16 _AAN_Samples[2] =
{
     LE_REAL_I16_FROM_FLOAT(0), LE_REAL_I16_FROM_FLOAT(0),
};

static leReal_i16 _AA0_Samples[4] =
{
    LE_REAL_I16_FROM_FLOAT(0.25), LE_REAL_I16_FROM_FLOAT(0.25),
    LE_REAL_I16_FROM_FLOAT(-0.25), LE_REAL_I16_FROM_FLOAT(-0.25),
};

static leReal_i16 _AA1_Samples[8] =
{
    LE_REAL_I16_FROM_FLOAT(-0.125), LE_REAL_I16_FROM_FLOAT(-0.375),
    LE_REAL_I16_FROM_FLOAT(0.375), LE_REAL_I16_FROM_FLOAT(-0.125),
    LE_REAL_I16_FROM_FLOAT(-0.375), LE_REAL_I16_FROM_FLOAT(0.125),
    LE_REAL_I16_FROM_FLOAT(0.125), LE_REAL_I16_FROM_FLOAT(0.375),
};

static leReal_i16 _AA2_Samples[16] =
{
    LE_REAL_I16_FROM_FLOAT(0.0625), LE_REAL_I16_FROM_FLOAT(-0.1875),
    LE_REAL_I16_FROM_FLOAT(-0.0625), LE_REAL_I16_FROM_FLOAT(0.1875),
    LE_REAL_I16_FROM_FLOAT(0.3125), LE_REAL_I16_FROM_FLOAT(0.0625),
    LE_REAL_I16_FROM_FLOAT(-0.1875), LE_REAL_I16_FROM_FLOAT(-0.3125),
    LE_REAL_I16_FROM_FLOAT(-0.3125), LE_REAL_I16_FROM_FLOAT(0.3125),
    LE_REAL_I16_FROM_FLOAT(-0.4375), LE_REAL_I16_FROM_FLOAT(-0.0625),
    LE_REAL_I16_FROM_FLOAT(0.1875), LE_REAL_I16_FROM_FLOAT(0.4375),
    LE_REAL_I16_FROM_FLOAT(0.4375), LE_REAL_I16_FROM_FLOAT(-0.4375),
};

static leReal_i16 _AA3_Samples[32] =
{
    LE_REAL_I16_FROM_FLOAT(0.0625), LE_REAL_I16_FROM_FLOAT(0.0625),
    LE_REAL_I16_FROM_FLOAT(-0.0625), LE_REAL_I16_FROM_FLOAT(-0.1875),
    LE_REAL_I16_FROM_FLOAT(-0.1875), LE_REAL_I16_FROM_FLOAT(0.125),
    LE_REAL_I16_FROM_FLOAT(0.25), LE_REAL_I16_FROM_FLOAT(-0.0625),
    LE_REAL_I16_FROM_FLOAT(-0.3125), LE_REAL_I16_FROM_FLOAT(-0.125),
    LE_REAL_I16_FROM_FLOAT(0.125), LE_REAL_I16_FROM_FLOAT(0.3125),
    LE_REAL_I16_FROM_FLOAT(0.3125), LE_REAL_I16_FROM_FLOAT(0.1875),
    LE_REAL_I16_FROM_FLOAT(0.1875), LE_REAL_I16_FROM_FLOAT(-0.3125),
    LE_REAL_I16_FROM_FLOAT(-0.125), LE_REAL_I16_FROM_FLOAT(0.375),
    LE_REAL_I16_FROM_FLOAT(0), LE_REAL_I16_FROM_FLOAT(-0.4375),
    LE_REAL_I16_FROM_FLOAT(-0.25), LE_REAL_I16_FROM_FLOAT(-0.375),
    LE_REAL_I16_FROM_FLOAT(-0.375), LE_REAL_I16_FROM_FLOAT(0.25),
    LE_REAL_I16_FROM_FLOAT(-0.5), LE_REAL_I16_FROM_FLOAT(0),
    LE_REAL_I16_FROM_FLOAT(0.4375), LE_REAL_I16_FROM_FLOAT(-0.25),
    LE_REAL_I16_FROM_FLOAT(0.375), LE_REAL_I16_FROM_FLOAT(0.4375),
    LE_REAL_I16_FROM_FLOAT(-0.4375), LE_REAL_I16_FROM_FLOAT(-0.5),
};

leReal_i16* _AA_SampleData[5] =
{
    (leReal_i16*)_AAN_Samples,
    (leReal_i16*)_AA0_Samples,
    (leReal_i16*)_AA1_Samples,
    (leReal_i16*)_AA2_Samples,
    (leReal_i16*)_AA3_Samples
};

uint32_t _AA_SampleCount[5] =
{
    1,
    2,
    4,
    8,
    16
};