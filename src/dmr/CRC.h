#include "../main.h"


#ifndef _CRC_H_
#define	_CRC_H_



bool CRC_checkFiveBit(bool* in, unsigned int tcrc);
void CRC_encodeFiveBit(const bool* in, unsigned int *tcrc);

void CRC_addCCITT161(unsigned char* in, unsigned int length);
void CRC_addCCITT162(unsigned char* in, unsigned int length);

bool CRC_checkCCITT161(const unsigned char* in, unsigned int length);
bool CRC_checkCCITT162(const unsigned char* in, unsigned int length);

unsigned char CRC_crc8(const unsigned char* in, unsigned int length);


#endif
