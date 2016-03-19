#ifndef _CRC_MOD_RTU_16_H_
#define _CRC_MOD_RTU_16_H_

#include "stdint.h"

#ifdef __cplusplus
extern "C"
{
#endif

	typedef enum
	{
		CRC_Ok = 0,
		CRC_Err
	}CRC_CheckType;

	uint16_t CRC_ModRtuCrcCalc(uint8_t* buf, int16_t len);
	CRC_CheckType CRC_ModRtuCrcCheck(uint16_t crc, uint8_t* buf, int16_t len);

#ifdef __cplusplus
}
#endif
#endif
