/*
 ******************************************************************************
 * @file    Appli/Core/Inc/time_monitor.h
 * @author  MCD Application Team
 * @brief   Time monitor public header
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 STMicroelectronics. All rights reserved.
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may obtain a copy of the License in the LICENSE file
 * in the root directory of this software component.
 ******************************************************************************
 */

#ifndef __TIME_MONITOR_H__
#define __TIME_MONITOR_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void timeMonitor(void);
void timeMonitorStart(void);
void timeMonitorStop(void);
void testTimeMonitor(void);

#ifdef __cplusplus
}
#endif

#endif /* __TIME_MONITOR_H__ */