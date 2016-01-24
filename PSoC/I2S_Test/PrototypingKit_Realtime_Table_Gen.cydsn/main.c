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
#include <math.h>

#define TABLE_LENGTH    8192
#define BUFFER_LENGTH   32

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


volatile uint8 waveTable_0[TABLE_LENGTH*2];
volatile uint8 waveTable_1[TABLE_LENGTH*2];

volatile uint8 waveBuffer_0[BUFFER_LENGTH];
volatile uint8 waveBuffer_1[BUFFER_LENGTH];

/*
void genSineTable()
{
    int i;
    int16 v_i16;
    int8* p_i8;
    
    for (i = 0; i < TABLE_LENGTH; i++) {
        v_i16 = (int16)(sin(2.0f * M_PI * i / TABLE_LENGTH) * INT16_MAX);
        p_i8  = (int8 *)&v_i16;
        sineTable8[i*2]   = *p_i8;
        sineTable8[i*2+1] = *(p_i8 + 1);
    }
}
*/

void genSawTable_0()
{
    int i;
    int16 v;
    uint8 *p8;
    
    for (i = 0; i < BUFFER_LENGTH; i++) {
        v = (int32)i * UINT16_MAX / BUFFER_LENGTH;
        p8 = (uint8 *)&v; 
        waveBuffer_0[i*2]   = *(p8 + 1);
        waveBuffer_0[i*2+1] = *p8;
    }
}

void genSawTable_1()
{
    int i;
    int16 v;
    uint8 *p8;
    
    for (i = 0; i < BUFFER_LENGTH; i++) {
        v = (int32)i * UINT16_MAX / BUFFER_LENGTH;
        p8 = (uint8 *)&v; 
        waveBuffer_1[i*2]   = *(p8 + 1);
        waveBuffer_1[i*2+1] = *p8;
    }
}


CY_ISR (dma_0_done_handler)
{
    Pin_Check_0_Write(1u);
    //genSawTable_0();
    Pin_Check_0_Write(0u);
}

CY_ISR (dma_1_done_handler)
{
    Pin_Check_1_Write(1u);
    //genSawTable_1();
    Pin_Check_1_Write(0u);
}

CY_ISR (i2s_1_tx_handler)
{
    if (I2S_1_ReadTxStatus() & I2S_1_TX_FIFO_UNDERFLOW) {
        // Underflow Alert
    }
}

int main()
{
    genSawTable_0();
    genSawTable_1();
    
    CyGlobalIntEnable; /* Enable global interrupts. */
    
    I2S_1_Start();
    
    /* DMA Configuration for DMA_0 */
    DMA_0_Chan = DMA_0_DmaInitialize(DMA_0_BYTES_PER_BURST, DMA_0_REQUEST_PER_BURST, 
        HI16(DMA_0_SRC_BASE), HI16(DMA_0_DST_BASE));
    DMA_0_TD[0] = CyDmaTdAllocate();
    CyDmaTdSetConfiguration(DMA_0_TD[0], BUFFER_LENGTH, DMA_0_TD[0], DMA_0__TD_TERMOUT_EN | TD_INC_SRC_ADR);
    CyDmaTdSetAddress(DMA_0_TD[0], LO16((uint32)waveBuffer_0), LO16((uint32)I2S_1_TX_CH0_F0_PTR));
    CyDmaChSetInitialTd(DMA_0_Chan, DMA_0_TD[0]);
    CyDmaChEnable(DMA_0_Chan, 1);
    
    /* DMA Configuration for DMA_1 */
    DMA_1_Chan = DMA_1_DmaInitialize(DMA_1_BYTES_PER_BURST, DMA_1_REQUEST_PER_BURST, 
        HI16(DMA_1_SRC_BASE), HI16(DMA_1_DST_BASE));
    DMA_1_TD[0] = CyDmaTdAllocate();
    CyDmaTdSetConfiguration(DMA_1_TD[0], BUFFER_LENGTH, DMA_1_TD[0], DMA_1__TD_TERMOUT_EN | TD_INC_SRC_ADR);
    CyDmaTdSetAddress(DMA_1_TD[0], LO16((uint32)waveBuffer_1), LO16((uint32)I2S_1_TX_CH0_F1_PTR));
    CyDmaChSetInitialTd(DMA_1_Chan, DMA_1_TD[0]);
    CyDmaChEnable(DMA_1_Chan, 1);
    
    ISR_DMA_0_Done_StartEx(dma_0_done_handler);
    ISR_DMA_1_Done_StartEx(dma_1_done_handler);
    ISR_I2S_1_TX_StartEx(i2s_1_tx_handler);
        
    while(0u != (I2S_1_ReadTxStatus() & (I2S_1_TX_FIFO_0_NOT_FULL | I2S_1_TX_FIFO_1_NOT_FULL)))
    {
        /* Wait for TxDMA to fill Tx FIFO */
    }
    CyDelay(1);

    I2S_1_EnableTx();

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */

    for(;;)
    {
        /* Place your application code here. */
    }
}

/* [] END OF FILE */
