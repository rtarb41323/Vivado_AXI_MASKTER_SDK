#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "xsetmem_rmt.h"
#include <stdint.h>
#include "xil_cache.h"


//#define MEM_BASE_ADDR   (0x800000000)
#define MEM_BASE_ADDR1   (0x0000)
#define PL_TO_DDR_BUFFER_BASE  (MEM_BASE_ADDR1 + 0x00100000)
#define DDR_TO_PL_BUFFER_BASE  (MEM_BASE_ADDR1 + 0x00300000)


//"8" seems to be MAX value when using 32 bit values
#define SIZE_ARRAY  	4

XSetmem_rmt			doSetMem;
XSetmem_rmt_Config *doSetMem_cfg;

float *someMemPLtoDDR = (float *)PL_TO_DDR_BUFFER_BASE;
float *someMemDDRtoPL = (float *)DDR_TO_PL_BUFFER_BASE;


/////////////////////////////////
//Function Prototypes:
void initSetMem(void);
float u32_to_float(unsigned int val);
unsigned int float_to_u32(float val);
/////////////////////////////////

volatile uint32_t loop_counter_int = 0;
volatile float loop_counter_float = 0.0f;
int main()
{
	int error = 0;
    init_platform();

    xil_printf("Hello World\n\r");
    xil_printf("Initializing SetMem...\n\r");
    initSetMem();

    //Init some test array to a known "zero" value:
    for (int idx = 0; idx < SIZE_ARRAY; idx++)
	{
    	someMemDDRtoPL[idx] = 55.0f;
    	someMemPLtoDDR[idx] = 1.0f;
	}


///////////////////////////////////////////////////////////////
//PRINT INPUTS FOR DEBUGGING:
    for (int idx = 0; idx < SIZE_ARRAY; idx++)
    {
    	for (volatile long long counter = 0; counter < 10000000; counter++)  	{}
    	printf("Value of input[%d]=%f\n",idx,someMemDDRtoPL[idx]);
    }
///////////////////////////////////////////////////////////////


    for (volatile long long counter = 0; counter < 100000000; counter++)  	{}

    while(1)
    {
    	xil_printf("\nLoop Counter int: %d\n", loop_counter_int);
    	printf("Loop Counter float: %f\n", loop_counter_float);

        //Simple delay//
        for (volatile long long counter = 0; counter < 100000000; counter++)  	{}

        ///////////////////////////////////////////////////////////////////////////
        //FLUSH the source buffer before transfer:
        ///////////////////////////////////////////////////////////////////////////
        Xil_DCacheFlushRange((UINTPTR)someMemPLtoDDR, SIZE_ARRAY);
        Xil_DCacheFlushRange((UINTPTR)someMemDDRtoPL, SIZE_ARRAY);
        //Configure the AXI MASTER to address "TX_BUFFER_BASE" with a value:
        XSetmem_rmt_Set_addrMasterReadFrom(&doSetMem, DDR_TO_PL_BUFFER_BASE);
        XSetmem_rmt_Set_addrMasterWriteTo(&doSetMem, PL_TO_DDR_BUFFER_BASE);
        //Configure the variable to add:
        XSetmem_rmt_Set_size(&doSetMem, SIZE_ARRAY);
        XSetmem_rmt_Set_value_to_add(&doSetMem, float_to_u32(  (float)loop_counter_float  ));
        //GO:
        print("Setting memory....\n\r");
        XSetmem_rmt_Start(&doSetMem);
        while (!XSetmem_rmt_IsDone(&doSetMem)); //Wait until it's done
        error = XSetmem_rmt_Get_return(&doSetMem); //Get the error (if any)
        xil_printf("Test result error: %d\n", error);
        //Now, print results:
        for (int idx = 0; idx < SIZE_ARRAY; idx++)
        {
        	printf("Value of someMem[%d]=%f\n",idx,someMemPLtoDDR[idx]);
        }

        loop_counter_int++;
        loop_counter_float++;
    }

    cleanup_platform();
    return 0;
}



///////////////////////////////////////////////////////////

void initSetMem(void)
{
	int status = 0;
	doSetMem_cfg = XSetmem_rmt_LookupConfig(XPAR_SETMEM_RMT_0_DEVICE_ID);

	if (doSetMem_cfg)
	{
		status = XSetmem_rmt_CfgInitialize(&doSetMem, doSetMem_cfg);
		if(status != XST_SUCCESS)
		{
			xil_printf("\nFailed to initialize SetMem\n");
		}
	}

}

float u32_to_float(unsigned int val)
{
	union {
		float val_float;
		unsigned char bytes[4];
	} data;
	data.bytes[3] = (val >> 24) & 0xff;
	data.bytes[2] = (val >> 16) & 0xff;
	data.bytes[1] = (val >> 8)  & 0xff;
	data.bytes[0] = (val)       & 0xff;
	return data.val_float;
}

unsigned int float_to_u32(float val)
{
	unsigned int result;
	union float_bytes{
	  float v;
	  unsigned char bytes[4];
	} data;

	data.v = val;
	result = (data.bytes[3] << 24) |
			 (data.bytes[2] << 16) |
			 (data.bytes[1] << 8)  |
			 (data.bytes[0]);
	return result;
}
