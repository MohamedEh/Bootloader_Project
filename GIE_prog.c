#include "STD_TYPES.h"
#include "BIT_MATH.h"

#include "GIE_interface.h"
#include "GIE_reg.h"

void GIE_voidEnable(void){
	__asm volatile ("SEI");		//inline assembly instruction executed in one clock cycle
	//SET_BIT(GIE_u8SREG,GIE_u8SREG_I);		executed in 3 clock cycles
}

void GIE_voidDisable(void){
	__asm volatile ("CLI");		//inline assembly instruction executed in one clock cycle

	//CLR_BIT(GIE_u8SREG,GIE_u8SREG_I); 	executed in 3 clock cycles
}