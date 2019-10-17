/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */
 
/* linux/drivers/hwmon/lis33de.c
 *
 * (C) Copyright 2008
 * MediaTek <www.mediatek.com>
 *
 * AFA830 driver for MT6516
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#ifndef AFA830_H
#define AFA830_H

#include <linux/ioctl.h>

extern struct acc_hw* AFA830_get_cust_acc_hw(void);


#if 0 // Johnson_Qian 2012_09_29
#define AFA830_I2C_SLAVE_ADDR   (0x3C<<1) //0x78 //0x3D<->SA0=1 ;0x3C<->SA0=0  0x01111000
#else
#define AFA830_I2C_SLAVE_ADDR   (0x3D<<1)//0x78 //0x3D<->SA0=1 ;0x3C<->SA0=0  0x01111000
#endif
/*----------------------------------------------------------------------------*/
#define I2C_DEVICEID_AFA830     0x3C

/* AFA830 Register Map */
#define THRESH_TAP    0x21    /* R/W Tap threshold */
#define TAP_MIN       0x24    /* R/W Tap min duration */
#define TAP_MAX       0x25    /* R/W Tap max duration */
#define LATENT        0x22    /* R/W Tap latency */
#define DLATENTCY     0x26    /* R/W DTap latency */
#define THRESH_ACT    0x1D    /* R/W Activity threshold */

#define AFA830_REG_THRESH_FF    0x1A    /* R/W Free-fall threshold */
#define AFA830_REG_FF_LATENCY   0x1B    /* R/W Free-fall time */
#define AFA830_REG_ACT_TAP_STATUS1  0x0D    /* R   Source of tap/double tap */
#define AFA830_REG_ACT_TAP_STATUS2  0x0E
#define AFA830_REG_DATA_RATE        0x05    /* R/W Data rate */
#define AFA830_REG_POWER_CTL    0x03    /* R/W Power saving features control :Frances*/
#define AFA830_REG_INT_ENABLE   0x09    /* R/W Interrupt enable control */
#define AFA830_REG_INT_MAP      0x0B    /* R/W Interrupt mapping control */
#define AFA830_REG_DATAX0       0x10    /* R   X-Axis Data 0 */
#define AFA830_REG_DATAX1       0x11    /* R   X-Axis Data 1 */
#define AFA830_REG_DATAY0       0x12    /* R   Y-Axis Data 0 */
#define AFA830_REG_DATAY1       0x13    /* R   Y-Axis Data 1 */
#define AFA830_REG_DATAZ0       0x14    /* R   Z-Axis Data 0 */
#define AFA830_REG_DATAZ1       0x15    /* R   Z-Axis Data 1 */
#define AFA830_REG_FIFO_CTL     0x04    /* R/W FIFO control */
#define AFA830_REG_FIFO_STATUS  0x0F    /* R   FIFO status */

#define AFA830_REG_WHO_AM_I    0X37    /* R   Device ID (0x3D) */

#define WMA    0x07     //Weighted moving average
#define NODR    0x05     //output data rate


/* INT_ENABLE/INT_MAP/INT_SOURCE Bits */
#define CNT_DATA_RDY (1 << 0)
#define FIFO_EMPTY   (1 << 1)
#define FIFO_OVER    (1 << 2)
#define FIFO_FULL    (1 << 3)
#define FF_EN        (1 << 4)
#define MOTION_EN    (1 << 5)
#define TAP_EN       (1 << 6)
#define ORN_EN       (1 << 7)

/* ACT_TAP_STATUS1 Bits */
#define FREE_FALL   (1 << 0)
#define MOTION      (1 << 1)
#define SINGLE_TAP  (1 << 2)
#define DOUBLE_TAP  (1 << 3)
#define ORIENTATION (1 << 4)

/* ACT_TAP_STATUS2 Bits */
#define CNT_RDY     (1 << 0)
#define FIFO_EMPTY  (1 << 1)
#define FIFO_OVER   (1 << 2)
#define FIFO_FULL   (1 << 3)


/* DATA_RATE Bits */
#define ODR_400     0x0
#define ODR_200     0x01
#define ODR_100     0x02
#define ODR_50      0x03
#define ODR_25      0x04
#define ODR_12p5    0x05
#define ODR_6p256   0x06
#define ODR_3p128   0x07
#define ODR_1p564   0x08
#define ODR_0p782   0x09
#define ODR_0p391   0x0A

#define RATE(x)        ((x) & 0xF)

/* POWER_CTL Bits */
#define NORMAL 0
#define LOW_PWR    1
#define PWR_DOWN (1 << 1)
#define Wakeup (1 << 2)


/*
 * Maximum value our axis may get in full res mode for the input device
 * (signed 16 bits)
 */
#define AFA_FULLRES_MAX_VAL 32767
#define AFA_FULLRES_MIN_VAL 32768


/* FIFO_CTL Bits */
#define FIFO_EN         1
#define FIFO_CLEAN      (1 << 1)
#define FIFO_BYPASS     (1 << 2)
#define FIFO_STREAM     (1 << 3)
#define FIFO_TRIGGER    AFA_FIFO_BYPASS | AFA_FIFO_STREAM
#define FIFO_INT1       (0 << 4) //INT1
#define FIFO_INT2       (1 << 4) //INT2

/* WMA value */
#define WMA_CTL_0    0
#define WMA_CTL_1    1
#define WMA_CTL_2    2
#define WMA_CTL_3    3
#define WMA_CTL_4    4
#define WMA_CTL_5    5
#define WMA_CTL_6    6
#define WMA_CTL_7    7
#define WMA_CTL_8    8
#define WMA_CTL_9    9
#define WMA_CTL_10    10
#define WMA_CTL_11    11
#define WMA_CTL_12    12
#define WMA_CTL_13    13
#define WMA_CTL_14    14
#define WMA_CTL_15    15


#define AFA830_SUCCESS                  0
#define AFA830_ERR_I2C                  -1
#define AFA830_ERR_STATUS               -3
#define AFA830_ERR_SETUP_FAILURE        -4
#define AFA830_ERR_GETGSENSORDATA       -5
#define AFA830_ERR_IDENTIFICATION       -6

#define AFA830_BUFSIZE                  256


#endif

