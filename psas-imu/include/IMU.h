/*
 * L3G4200D.h
 *
 *  Created on: Nov 19, 2011
 *      Author: theo
 */

#ifndef IMU_H_
#define IMU_H_

void IMU_isr() __attribute__ ((interrupt("IRQ")));

#define VIC_GPIO_BIT        17
#define ENABLE_GPIO_INT     (VICIntEnable |= (1<<VIC_GPIO_BIT))
#define DISABLE_GPIO_INT    (VICIntEnClr   = (1<<VIC_GPIO_BIT))
#define RAISE_GPIO_INT      (VICSoftInt   |= (1<<VIC_GPIO_BIT))
#define CLR_SW_GPIO_INT     (VICSoftIntClr = (1<<VIC_GPIO_BIT))

#define VIC_EINT1_BIT        15
#define ENABLE_EINT1_INT     (VICIntEnable |= (1<<VIC_EINT1_BIT))
#define DISABLE_EINT1_INT    (VICIntEnClr   = (1<<VIC_EINT1_BIT))
#define RAISE_EINT1_INT      (VICSoftInt   |= (1<<VIC_EINT1_BIT))
#define CLR_SW_EINIT1_INT    (VICSoftIntClr = (1<<VIC_EINT1_BIT))

#define VIC_SWINT_BIT        1
#define ENABLE_SWINT_INT     (VICIntEnable |= (1<<VIC_SWINT_BIT))
#define RAISE_SWINT_INT		 (VICSoftInt   |= (1<<VIC_SWINT_BIT))
#define CLR_SW_SWINT_INT     (VICSoftIntClr = (1<<VIC_SWINT_BIT))

typedef struct axis_data{
	int x;
	int y;
	int z;
	int modified;
} axis_data;
/*GPIO 0 wiring*/
//P0.00 CAN
//P0.01 CAN
//P0.02 UART
//P0.03 UART
//P0.04 CAN
//P0.05 ----
//P0.06 ---- = ACCEL SA0
//P0.07 ---- = ACCEL INT2
//P0.08 ---- = ACCEL INT1
//P0.09 ---- = ACCEL CS
//P0.10 SDA2 = MAG SDA
//P0.11 SCL2 = MAG SCL
//-----
//P0.15 ---- = MAG INT1
//P0.16 ----
//P0.17 ----
//P0.18 ---- = MAG INT2
//P0.19 SDA1 = ACCEL SDA
//P0.20 SCL1 = ACCEL SCL
//P0.21 ---- = MAG SA
//P0.22 ---- = MAG DRDY
//P0.23 ---- = GYRO INT1
//P0.24 ---- = GYRO INT2
//P0.25 ---- = GYRO CS
//P0.26 ---- = GYRO SA0
//P0.27 SDA0 = GYRO SDA
//P0.28 SCL0 = GYRO SCL
//P0.29 USB
//P0.30 USB

#define ACCEL_SA0	(1<<6)
#define ACCEL_INT2	(1<<7)
#define ACCEL_INT1	(1<<8)
#define ACCEL_CS	(1<<9)
#define MAG_INT1	(1<<15)
#define MAG_INT2	(1<<18)
#define MAG_SA		(1<<21)
#define MAG_DRDY	(1<<22)
#define GYRO_INT1	(1<<23)
#define GYRO_INT2	(1<<24)
#define GYRO_CS		(1<<25)
#define	GYRO_SA0	(1<<26)

#endif /* IMU_H_ */
