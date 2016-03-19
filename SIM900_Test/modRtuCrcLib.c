#include "modRtuCrcLib.h"



 CRC_CheckType CRC_ModRtuCrcCheck(uint16_t crc, uint8_t* buf, int16_t len)
{
	if (crc == CRC_ModRtuCrcCalc(buf, len))
	{
		return CRC_Ok;
	}
	return CRC_Err;
}

 uint16_t CRC_ModRtuCrcCalc(uint8_t* buf, int16_t len)
{
	uint16_t crc = 0xFFFF;

	for (int pos = 0; pos < len; pos++) {
		crc ^= (uint16_t)buf[pos];          // XOR byte into least sig. byte of crc

		for (int i = 8; i != 0; i--) {    // Loop over each bit
			if ((crc & 0x0001) != 0) {      // If the LSB is set
				crc >>= 1;                    // Shift right and XOR 0xA001
				crc ^= 0xA001;
			}
			else                            // Else LSB is not set
				crc >>= 1;                    // Just shift right
		}
	}
	// Note, this number has low and high bytes swapped, so use it accordingly (or swap bytes)
	return crc;
}
