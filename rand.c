#include <windows.h>

int x[5];

int BRandom() {
	__int64 sum;
	sum = (__int64)2111111111UL * (__int64)x[3] +
		(__int64)1492 * (__int64)(x[2]) +
		(__int64)1776 * (__int64)(x[1]) +
		(__int64)5115 * (__int64)(x[0]) +
		(__int64)x[4];
	x[3] = x[2];  x[2] = x[1];  x[1] = x[0];
	x[4] = (int)(sum >> 32);                  // Carry
	x[0] = (int)sum;                          // Low 32 bits of sum
	return x[0];
} 


int _rand()
{
	static init=TRUE;
	int min=0;
	int max=0xFFFFFF;
	unsigned int interval;                  // Length of interval
	__int64 longran;                   // Random bits * interval
	unsigned int iran;                      // Longran / 2^32
	
	if(init){
		int i;
		for (i = 0; i < 5; i++) {
			x[i] = rand();
		}
		init=FALSE;
	}
	interval = (unsigned int)(max - min + 1);
	longran  = (__int64)BRandom() * interval;
	iran = (unsigned int)(longran >> 32);
	// Convert back to signed and return result
	return (int)iran + min;
	
}


