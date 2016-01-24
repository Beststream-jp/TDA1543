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

#define TABLE_LENGTH    512

volatile uint8 waveTable_0[TABLE_LENGTH];

void genSineTable(volatile uint8 *table, int length)
{
    int i;
    int16 v;
    int8* p8;
    
    for (i = 0; i < length / 2; i++) {
        v = (int16)(sin(2.0f * M_PI * i * 2 / length) * 32767);
        p8 = (int8 *)&v;
        table[i*2]   = *(p8+1);
        table[i*2+1] = *p8;
    }
}

int main()
{
    CyGlobalIntEnable; /* Enable global interrupts. */
    
    for(;;)
    {
        Pin_Check_0_Write(1u);
        genSineTable(waveTable_0, TABLE_LENGTH);
        Pin_Check_0_Write(0u);
        CyDelay(1);
    }
}
/* [] END OF FILE */
