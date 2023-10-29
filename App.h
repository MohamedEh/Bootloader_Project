#ifndef APP_H
#define APP_H

#define BLD_SEC		1
#define APP_SEC		0
#define GICR	*((volatile uint8*)0x5B)
#define GICR_IVCE	0
#define GICR_IVSEL	1

extern void APP_vMoveIvt(uint8 copy_u8section);

#endif
