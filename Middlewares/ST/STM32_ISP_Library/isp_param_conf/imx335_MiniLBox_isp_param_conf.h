/**
 ******************************************************************************
 * @file    isp_param_conf.h
 * @author  AIS Application Team
 * @brief   Header file for IQT parameters of the ISP middleware.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under SLA0044 terms that can be found here:
 * https://www.st.com/resource/en/license_agreement/SLA0044.txt
 *
 * THIS FILE WAS GENERATED BY THE IQ TUNING TOOL ON 2024-03-28 11:07:57
 *
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ISP_PARAM_CONF__H
#define __ISP_PARAM_CONF__H

/* DCMIPP ISP configuration for IMX335 sensor */
static const ISP_IQParamTypeDef ISP_IQParamCacheInit_IMX335 = {
    .sensorGainStatic = {
        .gain = 0,
    },
    .sensorExposureStatic = {
        .exposure = 0,
    },
    .AECAlgo = {
        .enable = 1,
        .exposureCompensation = 0,
    },
    .statRemoval = {
        .enable = 0,
        .nbHeadLines = 0,
        .nbValidLines = 0,
    },
    .badPixelStatic = {
        .enable = 0,
        .strength = 0,
    },
    .badPixelAlgo = {
        .enable = 0,
        .threshold = 0,
    },
    .blackLevelStatic = {
        .enable = 1,
        .BLCR = 12,
        .BLCG = 12,
        .BLCB = 12,
    },
    .demosaicing = {
        .enable = 1,
        .type = 0,
        .peak = 2,
        .lineV = 4,
        .lineH = 4,
        .edge = 6,
    },
    .ispGainStatic = {
        .enable = 0,
        .ispGainR = 0,
        .ispGainG = 0,
        .ispGainB = 0,
    },
    .colorConvStatic = {
        .enable = 0,
        .coeff = { { 0, 0, 0, }, { 0, 0, 0, }, { 0, 0, 0, }, }
    },
    .AWBAlgo = {
        .enable = 1,
        .id = { "MiniLBox A", "MiniLBox TL84", "MiniLBox D65", "Free slot", "Free slot", },
        .referenceColorTemp = { 2665, 3750, 6140, 0, 0, },
        .ispGainR = { 126000000, 157000000, 210000000, 0, 0, },
        .ispGainG = { 100000000, 100000000, 100000000, 0, 0, },
        .ispGainB = { 279000000, 199000000, 155000000, 0, 0, },
        .coeff = {
            { { 277500000, -75420000, -46310000, }, { -91640000, 272360000, -29450000, }, { -1060000, -126200000, 334070000, }, },
            { { 178510000, -54460000, -19030000, }, { -46390000, 162010000, -21660000, }, { 1520000, -56399999, 168130000, }, },
            { { 176680000, -60550000, -15590000, }, { -33130000, 138950000, -21970000, }, { -1080000, -40140000, 135020000, }, },
            { { 0, 0, 0, }, { 0, 0, 0, }, { 0, 0, 0, }, },
            { { 0, 0, 0, }, { 0, 0, 0, }, { 0, 0, 0, }, },
        },
    },
    .contrast = {
        .enable = 0,
        .coeff.LUM_0 = 0,
        .coeff.LUM_32 = 0,
        .coeff.LUM_64 = 0,
        .coeff.LUM_96 = 0,
        .coeff.LUM_128 = 0,
        .coeff.LUM_160 = 0,
        .coeff.LUM_192 = 0,
        .coeff.LUM_224 = 0,
        .coeff.LUM_256 = 0,
    },
    .statAreaStatic = {
        .X0 = 648,
        .Y0 = 486,
        .XSize = 1296,
        .YSize = 972,
    },
    .gamma = {
        .enablePipe1 = 1,
        .enablePipe2 = 1,
    },
};

static const ISP_IQParamTypeDef* ISP_IQParamCacheInit[] = {
    &ISP_IQParamCacheInit_IMX335
};

#endif /* __ISP_PARAM_CONF__H */