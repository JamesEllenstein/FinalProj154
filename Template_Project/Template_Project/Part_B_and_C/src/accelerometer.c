#include "SPI.h"
#include "SysTimer.h"
#include "accelerometer.h"

void accWrite(uint8_t addr, uint8_t val){
	// DONE access SPI_Transfer_Data
	uint16_t bitstring = 0;
	bitstring |= (addr << 8) | val;	
	SPI_Transfer_Data(bitstring);
}
int16_t twosCompToDec(uint16_t two_compliment_val)
{
    // [0x0000; 0x7FFF] corresponds to [0; 32,767]
    // [0x8000; 0xFFFF] corresponds to [-32,768; -1]
    // int16_t has the range [-32,768; 32,767]

    uint16_t sign_mask = 0x8000;

    // if positive
    if ( (two_compliment_val & sign_mask) == 0 ) {
        return two_compliment_val;
    //  if negative
    } else {
        // invert all bits, add one, and make negative
        return -(~two_compliment_val + 1);
    }
}
uint8_t accRead(uint8_t addr){
	// access SPI_Transfer_Data
	uint16_t bitstring = 0;
	bitstring |= (addr << 8) | (1UL << 15);	
	return SPI_Transfer_Data(bitstring);
}

void initAcc(void){
	// set full range mode
	accWrite(0x31,((1UL << 3) | 3UL));
	// enable measurement
	accWrite(0x2d, 1UL << 3);
}

void readValues(double* x, double* y, double* z){
	// TODO
	uint16_t xval = 0;
	uint16_t yval = 0;
	uint16_t zval = 0;
	xval |= accRead(0x32);
	xval |= accRead(0x33) << 8 ;
	yval |= accRead(0x34);
	yval |= accRead(0x35) << 8 ;
	zval |= accRead(0x36);
	zval |= accRead(0x37) << 8 ;
	// find scaler from data sheet
	*x = twosCompToDec(xval)/31.2;
	*y = twosCompToDec(yval)/31.2;
	*z = twosCompToDec(zval)/31.2;
	// read values into x,y,z using accRead
}
