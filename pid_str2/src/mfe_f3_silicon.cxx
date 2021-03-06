
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <memory.h>
#include <ctype.h>

//#include <cvt_board_commons.h>
//#include <cvt_common_defs.h>


//extern "C"{
//#include <cvt_V792.h>
//}

#include "midas.h"
#include "mfe_ADC.h"
#include "odb_trigger.h"
#include "detector_conf.h"

//static cvt_V792_data F3_SILICON_type;

uint32_t addr;
//CVErrorCodes ret;

int buff_size_intv_f3s;
int buff_size_intv_f3s_c;

uint32_t addr_aryc;
extern uint32_t vir_adc_value[32];
extern int vir_tdc_ch;
extern uint32_t vir_ev_count;
extern int VEG_TAG;
uint32_t vir_ev_count_tmp_f3a = -1;

//extern "C" {
INT f3_silicon_init(int32_t BHandle, TRIGGER_SETTINGS *tsr){

/*	UINT16 firmware_rev, reg_value, c_reg_value;

	printf("\n========================== Initializing F3 SILICON ADC...\n");
	printf("V785 initialization...");
	if(!cvt_V792_open(&F3_SILICON_type, F3_SILICON_ADDR, BHandle, CVT_V785_TYPE_N))
	{
		printf("\n Error executing cvt_F3_SILICON_type_open \n");
		return FE_ERR_HW;
	}
	printf("Done\n");

	//==================================firmware version
	addr=(F3_SILICON_ADDR<<16)|0x1000;
	ret=(CAENVME_ReadCycle(BHandle, addr, &firmware_rev, cvA32_S_DATA, cvD16));

	printf("Firmware Rev. : %x\n", firmware_rev);

        //=======================set operation mode====================//--->see V785N manual page 53-55
	addr=(F3_SILICON_ADDR<<16)|0x1032; //==================bit set 2
	reg_value=0x0880; //default 0x4980 4980 for *2 4880 for *16
	CAENVME_WriteCycle(BHandle, addr, &reg_value, cvA32_S_DATA, cvD16);

	addr=(F3_SILICON_ADDR<<16)|0x1034; //==================bit clear 2
	c_reg_value=(~reg_value)&0x7fff;
	CAENVME_WriteCycle(BHandle, addr, &c_reg_value, cvA32_S_DATA, cvD16);

	//=======================set Block Transfer Mode====================//--->see V785N manual page 65-66

	addr=(F3_SILICON_ADDR<<16)|0x1010; 
	reg_value=0x0044;
	CAENVME_WriteCycle(BHandle, addr, &c_reg_value, cvA32_S_DATA, cvD16);

	//================================set threshold=================//-->see V785N manual 59-60

	printf("Setting threshold...\n");
	int i=0;
	uint16_t th_val;

	for(i=0; i<16; i++)
	{
		addr=(F3_SILICON_ADDR<<16)|0x1080|4*i;
		th_val=tsr->f3_si_th[i];
		reg_value=0x0000|th_val;
	    ret=CAENVME_WriteCycle(BHandle, addr, &reg_value, cvA32_S_DATA, cvD16);
	    if(ret){printf("Error executing %d_channel threshold setting\n", i);}
	    printf("beamline silicon ch: %i, val: %i\n",i,reg_value);
	}
	printf("Done\n");

	UINT16 mcst_reg, mcst_reg_order;
//==================================mcst reg for MCST address setting  
	addr_aryc=(F3_SILICON_ADDR<<16)|0x1004;
	mcst_reg = 0xAA;
	ret=(CAENVME_WriteCycle(BHandle, addr_aryc, &mcst_reg, cvA32_S_DATA, cvD16));
	printf("mcst_reg(1004) set: %x\n", mcst_reg);

	//==================================mcst reg for MCST board first
        addr_aryc=(SILICON_ARYA_ADDR<<16)|0x101A;
        mcst_reg_order = 0x02; //2: first
        ret=(CAENVME_WriteCycle(BHandle, addr_aryc, &mcst_reg_order, cvA32_S_DATA, cvD16));

#if 0
	for(i=0; i<32; i++)
	{
		addr=(F3_SILICON_ADDR<<16)|0x1080|2*i;
		th_val=tsr->f3_si_th[i]*4096/4000;
		reg_value=0x0000|th_val;
	    ret=CAENVME_WriteCycle(BHandle, addr, &reg_value, cvA32_S_DATA, cvD16);
	    if(ret){printf("Error executing %d_channel threshold setting\n", i);}
	}
	printf("Done\n");
#endif
	//=================================data clear
	printf("Sending data clear...");
	ret=static_cast<CVErrorCodes>(f3_silicon_clear_buffer(BHandle));
	printf("Done");
*/
	return SUCCESS;
}


INT f3_silicon_exit(int32_t BHandle){\

//	f3_silicon_clear_buffer(BHandle);
//	cvt_V792_close(&F3_SILICON_type);
	return SUCCESS;
}


INT f3_silicon_begin(int32_t BHandle, INT run_number, char *error, TRIGGER_SETTINGS *tsr){

	return SUCCESS;
}



INT f3_silicon_end(int32_t BHandle, INT run_number, char *error){

//	cvt_V792_data_clear(&F3_SILICON_type);
	return SUCCESS;
}


INT f3_silicon_check_fifo(int32_t BHandle){

	//===============================status register====================================//-->see V785N manual page 47
	uint16_t status_reg1;

//	addr=(F3_SILICON_ADDR<<16)|0x100E;
//	ret=CAENVME_ReadCycle(BHandle, addr, &status_reg1, cvA32_S_DATA, cvD16);

	return status_reg1&0x001;
}

INT f3_silicon_read_fifo(int32_t BHandle, void *buff_tmp, int size){

	int count;

//	addr=(F3_SILICON_ADDR<<16)|0x0000;
//	ret=CAENVME_BLTReadCycle(BHandle, addr, (char*)buff_tmp, size, cvA32_S_BLT, cvD64, &count);
	return count;
}


INT f3_silicon_read_event(int32_t BHandle, const char *bank_name, char *pevent, INT off, uint32_t *buff, int buff_size, uint32_t *pdata, int bkm){

	int i;
	int count=f3_silicon_read_fifo(BHandle, buff, buff_size);


//	printf("==============================%d\n", count);
	bk_create(pevent, bank_name, TID_DWORD, (void**)&pdata);
	bkm = 1;


	if(vir_ev_count_tmp_f3a != vir_ev_count ){

				vir_ev_count_tmp_f3a = vir_ev_count;

		for(i = 0; i < 32; i++){

			UINT32 II = i;
			UINT32 VADC = vir_adc_value[vir_tdc_ch];
			*pdata++=II;
			*pdata++=VADC;
			buff_size_intv_f3s += 2;

		}


		*pdata++=(UINT32)vir_ev_count;
		buff_size_intv_f3s_c += 7;
		buff_size_intv_f3s += 3;

		bkm = 0;


		//printf("F3 Vir ADC measurement;  channel:%d, measurement:%d, \n", vir_tdc_ch, vir_adc_value[vir_tdc_ch]);
		VEG_TAG++;


	}
	/*else for(i=0; i<count/4; i++)
	{
		uint32_t data=buff[i];




		switch(data&CVT_QTP_DATA_TYPE_MSK)
		{
			case CVT_QTP_DATUM:
					{
						UINT32 channel= CVT_V785N_GET_DATUM_CH(data);//CVT_V785A_GET_DATUM_CH(data);
						UINT32 measure= CVT_V785_GET_DATUM_ADC(data);
						UINT32 UV=CVT_V785_GET_DATUM_UN(data);
						UINT32 OV=CVT_V785_GET_DATUM_OV(data);
						*/
						 //*pdata++=(uint32_t)vir_tdc_ch;//channel; 	/* ??? */
						//*pdata++= channel;
						// *pdata++=vir_adc_value[channel];	/* ??? */
						//*pdata++= measure;


/*
						//buff_size_intv_f3s_c += 2;
						buff_size_intv_f3s += 2;
						//tmp_channel = channel;
						//printf("PPAC ADC measurement; uv:%d, ov:%d, channel:%d, measurement:%d, \n", UV, OV, channel, measure\);
					} break;


			case CVT_QTP_EOB:
					{
						UINT32 event_count= CVT_QTP_GET_EOB_EVENT_COUNT(data);
						*pdata++=event_count;

						buff_size_intv_f3s_c += 7;
						buff_size_intv_f3s += 3;

						printf("PPAC ADC measurement;  channel:%d, measurement:%d,  Vcount: %u \n",  vir_tdc_ch,vir_adc_value[vir_tdc_ch],vir_ev_count);
						//printf("f3_silicon_EOB; event_count:%d\n", event_count);
					} break;

			default:
				{bkm = 0;} break;
		}
	}*/

	bk_close(pevent, pdata);

	return bk_size(pevent);

}


BOOL f3_silicon_clear_buffer(int32_t BHandle){

	UINT16 Bit_set_1_reg;
/*
	addr=(F3_SILICON_ADDR<<16)|0x1006; //bit set 1
	Bit_set_1_reg=0x0080;
	ret=CAENVME_WriteCycle(BHandle, addr, &Bit_set_1_reg, cvA32_S_DATA, cvD16);
	if(ret){printf("\nError executing buffer clean_1\n");}

	addr=(F3_SILICON_ADDR<<16)|0x1008; //bit clear 1
	Bit_set_1_reg=0x0080;
	ret=CAENVME_WriteCycle(BHandle, addr, &Bit_set_1_reg, cvA32_S_DATA, cvD16);
	if(ret){printf("\nError executing buffer clean_2\n");}

	return ret;
*/
	return 0;

}
