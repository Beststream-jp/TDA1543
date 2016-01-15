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

#define TABLE_LENGTH 256

/* Defines for DMA_1 */
#define DMA_1_BYTES_PER_BURST 2
#define DMA_1_REQUEST_PER_BURST 1
#define DMA_1_SRC_BASE (CYDEV_SRAM_BASE)
#define DMA_1_DST_BASE (CYDEV_PERIPH_BASE)

/* Variable declarations for DMA_1 */
/* Move these variable declarations to the top of the function */
uint8 DMA_1_Chan;
uint8 DMA_1_TD[1];

uint16 sawTable[TABLE_LENGTH];

int main()
{
    int i, v;
    
    // sawTableの生成
    for (i = 0; i < TABLE_LENGTH; i += 2) {
        v = i * (0x10000 / TABLE_LENGTH);
        sawTable[i]   = v;
        sawTable[i+1] = v;
    }
    
    CyGlobalIntEnable; /* Enable global interrupts. */

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    
    I2S_1_Start();

    /* DMA Configuration for DMA_1 */
    DMA_1_Chan = DMA_1_DmaInitialize(DMA_1_BYTES_PER_BURST, DMA_1_REQUEST_PER_BURST, 
        HI16(DMA_1_SRC_BASE), HI16(DMA_1_DST_BASE));
    DMA_1_TD[0] = CyDmaTdAllocate();
    CyDmaTdSetConfiguration(DMA_1_TD[0], TABLE_LENGTH, DMA_1_TD[0], TD_INC_SRC_ADR);
    CyDmaTdSetAddress(DMA_1_TD[0], LO16((uint32)sawTable), LO16((uint32)I2S_1_TX_CH0_F0_PTR));
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
