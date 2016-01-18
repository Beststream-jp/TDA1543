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
#include "wavetable.h"

#define TABLE_SIZE    2048

volatile int waveIdx;

void writeDataToI2S()
{
  
    while (0u != (I2S_1_ReadTxStatus() & I2S_1_TX_FIFO_0_NOT_FULL)) 
    {
        //I2S_1_WriteByte(*((uint8 *)sineTable + waveIdx), 0);
        I2S_1_WriteByte(0xff, 0);
        I2S_1_WriteByte(0xff, 0);
        waveIdx++;
        if (waveIdx == TABLE_SIZE) {
            waveIdx = 0;
        }
    }
}

CY_ISR(isr_i2s_1_tx_handler)
{
    Pin_Check_Write(1u);
    Pin_Check_Write(0u);
    writeDataToI2S();
}

int main()
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    I2S_1_Start();
    ISR_I2S_1_TX_StartEx(isr_i2s_1_tx_handler);
    
    writeDataToI2S();
    CyDelay(1);
    I2S_1_EnableTx();
    
    for(;;)
    {
        //writeDataToI2S();
    }
}

/* [] END OF FILE */
