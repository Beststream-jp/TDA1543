/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include <project.h>

#define TABLE_LENGTH 128
//#define TABLE_LENGTH 2

/* Defines for DMA_1 */
#define DMA_1_BYTES_PER_BURST 1
#define DMA_1_REQUEST_PER_BURST 1
#define DMA_1_SRC_BASE (CYDEV_SRAM_BASE)
#define DMA_1_DST_BASE (CYDEV_PERIPH_BASE)

/* This table stores the 128 points in Flash for smoother sine wave generation */
/*
CYCODE const uint8 sineTable[TABLE_LENGTH] = 
{
	128, 134, 140, 147, 153, 159, 165, 171,
	177, 183, 188, 194, 199, 204, 209, 214,
	218, 223, 227, 231, 234, 238, 241, 244,
	246, 248, 250, 252, 253, 254, 255, 255,
	255, 255, 255, 254, 253, 252, 250, 248,
	246, 244, 241, 238, 234, 231, 227, 223,
	218, 214, 209, 204, 199, 194, 188, 183, 
	177, 171, 165, 159, 153, 147, 140, 134, 
	128, 122, 115, 109, 103,  97,  91,  85,
	 79,  73,  68,  62,  57,  52,  47,  42,
	 37,  33,  29,  25,  22,  18,  15,  12,
	 10,   7,   6,   4,   2,   1,   1,   0,
	  0,   0,   1,   1,   2,   4,   6,   7,
	 10,  12,  15,  18,  22,  25,  29,  33,
	 37,  42,  47,  52,  57,  62,  68,  73,
	 79,  85,  91,  97, 103, 109, 115, 122
};
*/
/*
CYCODE const uint8 sineTable[TABLE_LENGTH] = 
{
    0,	1,	2,	3,	4,	5,	6,	7,
    8,	9,	10,	11,	12,	13,	14,	15,
    16,	17,	18,	19,	20,	21,	22,	23,
    24,	25,	26,	27,	28,	29,	30,	31,
    32,	33,	34,	35,	36,	37,	38,	39,
    40,	41,	42,	43,	44,	45,	46,	47,
    48,	49,	50,	51,	52,	53,	54,	55,
    56,	57,	58,	59,	60,	61,	62,	63,
    64,	65,	66,	67,	68,	69,	70,	71,
    72,	73,	74,	75,	76,	77,	78,	79,
    80,	81,	82,	83,	84,	85,	86,	87,
    88,	89,	90,	91,	92,	93,	94,	95,
    96,	97,	98,	99,	100,	101,	102,	103,
    104,	105,	106,	107,	108,	109,	110,	111,
    112,	113,	114,	115,	116,	117,	118,	119,
    120,	121,	122,	123,	124,	125,	126,	127
};
*/
CYCODE const uint8 sineTable[TABLE_LENGTH] = 
{
    1,	0,	1,	0,	1,	0,	1,	0,
    1,	0,	1,	0,	1,	0,	1,	0,
    1,	0,	1,	0,	1,	0,	1,	0,
    1,	0,	1,	0,	1,	0,	1,	0,
    1,	0,	1,	0,	1,	0,	1,	0,
    1,	0,	1,	0,	1,	0,	1,	0,
    1,	0,	1,	0,	1,	0,	1,	0,
    1,	0,	1,	0,	1,	0,	1,	0,
    1,	0,	1,	0,	1,	0,	1,	0,
    1,	0,	1,	0,	1,	0,	1,	0,
    1,	0,	1,	0,	1,	0,	1,	0,
    1,	0,	1,	0,	1,	0,	1,	0,
    1,	0,	1,	0,	1,	0,	1,	0,
    1,	0,	1,	0,	1,	0,	1,	0,
    1,	0,	1,	0,	1,	0,	1,	0,
    1,	0,	1,	0,	1,	0,	1,	0
};

//CYCODE const uint8 sineTable[TABLE_LENGTH] = { 0x00, 0x00 };

/* Variable declarations for DMA_1 */
/* Move these variable declarations to the top of the function */
uint8 DMA_1_Chan;
uint8 DMA_1_TD[1];

int main()
{
    CyGlobalIntEnable; /* Enable global interrupts. */
    
    I2S_1_Start();

    /* DMA Configuration for DMA_1 */
    DMA_1_Chan = DMA_1_DmaInitialize(DMA_1_BYTES_PER_BURST, DMA_1_REQUEST_PER_BURST, 
        HI16(DMA_1_SRC_BASE), HI16(DMA_1_DST_BASE));
    DMA_1_TD[0] = CyDmaTdAllocate();
    CyDmaTdSetConfiguration(DMA_1_TD[0], TABLE_LENGTH, DMA_1_TD[0], TD_INC_SRC_ADR);
    CyDmaTdSetAddress(DMA_1_TD[0], LO16((uint32)sineTable), LO16((uint32)I2S_1_TX_CH0_F0_PTR));
    CyDmaChSetInitialTd(DMA_1_Chan, DMA_1_TD[0]);
    CyDmaChEnable(DMA_1_Chan, 1);

    while(0u != (I2S_1_ReadTxStatus() & I2S_1_TX_FIFO_0_NOT_FULL))
    {
        /* Wait for TxDMA to fill Tx FIFO */
    }
    CyDelayUs(1);
    
    I2S_1_EnableTx();
    
    for(;;)
    {
        /* Place your application code here. */
    }
}

/* [] END OF FILE */
