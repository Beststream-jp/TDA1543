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

/* Defines for DMA_0 */
#define DMA_0_BYTES_PER_BURST 1
#define DMA_0_REQUEST_PER_BURST 1
#define DMA_0_SRC_BASE (CYDEV_SRAM_BASE)
#define DMA_0_DST_BASE (CYDEV_PERIPH_BASE)
/* Defines for DMA_1 */
#define DMA_1_BYTES_PER_BURST 1
#define DMA_1_REQUEST_PER_BURST 1
#define DMA_1_SRC_BASE (CYDEV_SRAM_BASE)
#define DMA_1_DST_BASE (CYDEV_PERIPH_BASE)

/* Variable declarations for DMA_0 */
/* Move these variable declarations to the top of the function */
uint8 DMA_0_Chan;
uint8 DMA_0_TD[1];
/* Variable declarations for DMA_1 */
/* Move these variable declarations to the top of the function */
uint8 DMA_1_Chan;
uint8 DMA_1_TD[1];

int8 waveTable0[TABLE_LENGTH*2];
int8 waveTable1[TABLE_LENGTH*2];

const uint8 sineTable8[TABLE_LENGTH] = 
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

CY_ISR (dma_0_done_handler)
{
    Pin_Check0_Write(1u);
    Pin_Check0_Write(0u);
}

CY_ISR (dma_1_done_handler)
{
    Pin_Check1_Write(1u);
    Pin_Check1_Write(0u);
}

int main()
{
    int i;

    // 符号付き16bit sineTableの生成
    for (i = 0; i < TABLE_LENGTH; i++) {
        waveTable0[i*2]   = (int)sineTable8[i] - 128;
        waveTable0[i*2+1] = 0;
    }
    for (i = 0; i < TABLE_LENGTH; i++) {
        waveTable1[i*2]   = i;
        waveTable1[i*2+1] = 0;
    }
    
    CyGlobalIntEnable; /* Enable global interrupts. */
    
    I2S_1_Start();
    
    /* DMA Configuration for DMA_0 */
    DMA_0_Chan = DMA_0_DmaInitialize(DMA_0_BYTES_PER_BURST, DMA_0_REQUEST_PER_BURST, 
        HI16(DMA_0_SRC_BASE), HI16(DMA_0_DST_BASE));
    DMA_0_TD[0] = CyDmaTdAllocate();
    CyDmaTdSetConfiguration(DMA_0_TD[0], TABLE_LENGTH*2, DMA_0_TD[0], DMA_0__TD_TERMOUT_EN | TD_INC_SRC_ADR);
    CyDmaTdSetAddress(DMA_0_TD[0], LO16((uint32)waveTable0), LO16((uint32)I2S_1_TX_CH0_F0_PTR));
    CyDmaChSetInitialTd(DMA_0_Chan, DMA_0_TD[0]);
    CyDmaChEnable(DMA_0_Chan, 1);
    
    /* DMA Configuration for DMA_1 */
    DMA_1_Chan = DMA_1_DmaInitialize(DMA_1_BYTES_PER_BURST, DMA_1_REQUEST_PER_BURST, 
        HI16(DMA_1_SRC_BASE), HI16(DMA_1_DST_BASE));
    DMA_1_TD[0] = CyDmaTdAllocate();
    CyDmaTdSetConfiguration(DMA_1_TD[0], TABLE_LENGTH*2, DMA_1_TD[0], DMA_1__TD_TERMOUT_EN | TD_INC_SRC_ADR);
    CyDmaTdSetAddress(DMA_1_TD[0], LO16((uint32)waveTable1), LO16((uint32)I2S_1_TX_CH0_F1_PTR));
    CyDmaChSetInitialTd(DMA_1_Chan, DMA_1_TD[0]);
    CyDmaChEnable(DMA_1_Chan, 1);

    ISR_DMA_0_Done_StartEx(dma_0_done_handler);
    ISR_DMA_1_Done_StartEx(dma_1_done_handler);

    while(0u != (I2S_1_ReadTxStatus() & (I2S_1_TX_FIFO_0_NOT_FULL | I2S_1_TX_FIFO_1_NOT_FULL)))
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
