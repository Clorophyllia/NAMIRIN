
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

uint32_t jjs3b = 0;

uint32_t mcst_reg_orderb;
uint32_t user_channel_b;

//static cvt_V792_data SILICON_aryb_type;

uint32_t addr_aryb;
//CVErrorCodes ret_aryb;

uint64_t TimeTag_ary_tmpb;

uint32_t u_detector_event_tmpb;
uint32_t Counters_tmp_udetectorb;
int buff_size_intv_us2;
int buff_size_intv_us2_c;

int iiab ,iibb, iicb;

INT silicon_aryb_init(int32_t lBHandle, TRIGGER_SETTINGS *tsr){

/*
  UINT16 firmware_rev, reg_value, c_reg_value, geo_add, mcst_reg, mcst_reg_order, status_reg, boardversion_reg, Bit_set_1_reg;

	printf("\n========================== Initializing SILICON ARY ADC...\n");
	printf("V785 initialization...");
	if(!cvt_V792_open(&SILICON_aryb_type, SILICON_ARYB_ADDR, lBHandle, CVT_V785_TYPE_N))
	{
		printf("\n Error executing cvt_SILICON_aryb_type_open \n");
		return FE_ERR_HW;
	}
	printf("Done\n");

	//==================================firmware version
	addr_aryb=(SILICON_ARYB_ADDR<<16)|0x1000;
	ret_aryb=(CAENVME_ReadCycle(lBHandle, addr_aryb, &firmware_rev, cvA32_S_DATA, cvD16));

	printf("Firmware Rev. : %x\n", firmware_rev);

	//==================================geo_add initial
	addr_aryb=(SILICON_ARYB_ADDR<<16)|0x1002;
	ret_aryb=(CAENVME_ReadCycle(lBHandle, addr_aryb, &geo_add, cvA32_S_DATA, cvD16));
	printf("geo_add(1002) : %x\n", geo_add);
   	//==================================mcst reg for MCST address setting  
	addr_aryb=(SILICON_ARYB_ADDR<<16)|0x1004;
	mcst_reg = 0xAA;
	ret_aryb=(CAENVME_WriteCycle(lBHandle, addr_aryb, &mcst_reg, cvA32_S_DATA, cvD16));
	printf("mcst_reg(1004) set: %x\n", mcst_reg);

	//==================================mcst reg for MCST board last
        addr_aryb=(SILICON_ARYB_ADDR<<16)|0x101A;
        mcst_reg_order = 0x01; //1: last board
        ret_aryb=(CAENVME_WriteCycle(lBHandle, addr_aryb, &mcst_reg_order, cvA32_S_DATA, cvD16));
	printf("mcst_reg(1004) order: %x\n", mcst_reg_order);
	//==================================status reg
	addr_aryb=(SILICON_ARYB_ADDR<<16)|0x100E;
	ret_aryb=(CAENVME_ReadCycle(lBHandle, addr_aryb, &status_reg, cvA32_S_DATA, cvD16));
	printf("status_reg(100E) : %x\n", status_reg);

	//==================================board version
	addr_aryb=(SILICON_ARYB_ADDR<<16)|0x8032;
	ret_aryb=(CAENVME_ReadCycle(lBHandle, addr_aryb, &boardversion_reg, cvA32_S_DATA, cvD16));
	printf("Board version(8032) : %x\n", boardversion_reg);
	//uint16_t c_reg_value;

	//=======================set Block Transfer Mode====================//--->see V785N manual page 65-66
	addr_aryb=(SILICON_ARYB_ADDR<<16)|0x1010; 
	reg_value=0x0044;
	CAENVME_WriteCycle(lBHandle, addr_aryb, &reg_value, cvA32_S_DATA, cvD16);


	//=======================set operation mode====================//--->see V785N manual page 53-55
	addr_aryb=(SILICON_ARYB_ADDR<<16)|0x1032; //==================bit set 2
	reg_value=0x0880; //default 0x4980 4980 for *2 4880 for *16, sliding scale tech. only up to 3840 is valid, not 4096, 0x880->count only accepted event
	CAENVME_WriteCycle(lBHandle, addr_aryb, &reg_value, cvA32_S_DATA, cvD16);

	addr_aryb=(SILICON_ARYB_ADDR<<16)|0x1034; //==================bit clear 2
	c_reg_value=(~reg_value)&0x7fff;
	CAENVME_WriteCycle(lBHandle, addr_aryb, &c_reg_value, cvA32_S_DATA, cvD16);

	//================================set threshold=================//-->see V785N manual 59-60

	printf("Setting threshold...\n");
	int i=0;
	uint16_t th_val;

	for(i=0; i<32; i++) //if 16 channel ADC(V785N), channel step should be 4 instead of 2
	{
		addr_aryb=(SILICON_ARYB_ADDR<<16)|0x1080|2*i;
		th_val=tsr->si_aryb_th[i];
		reg_value=0x0000|th_val;
	    ret_aryb=CAENVME_WriteCycle(lBHandle, addr_aryb, &reg_value, cvA32_S_DATA, cvD16);
	    if(ret_aryb){printf("Error executing %d_channel threshold setting\n", i);}
	printf("array b th ch: %i, val: %i\n",i,reg_value);
	}
	printf("Done\n");

	//==================================mcst reg bit set1
        addr_aryb=(SILICON_ARYB_ADDR<<16)|0x1006; //bit set 1
        Bit_set_1_reg=0x0080;
        ret_aryb=CAENVME_WriteCycle(lBHandle, addr_aryb, &Bit_set_1_reg, cvA32_S_DATA, cvD16);
        if(ret_aryb){printf("\nError executing buffer writing_1\n");}

	//=================================data clear
	printf("Sending data clear...");
	ret_aryb=static_cast<CVErrorCodes>(silicon_aryb_clear_buffer(lBHandle));
	printf("Done");
*/
	return SUCCESS;
}

INT silicon_aryb_exit(int32_t lBHandle){

//	silicon_aryb_clear_buffer(lBHandle);
//	cvt_V792_close(&SILICON_aryb_type);
	return SUCCESS;
}



INT silicon_aryb_begin(int32_t lBHandle, INT run_number, char *error, TRIGGER_SETTINGS *tsr){

//	silicon_aryb_clear_buffer(lBHandle);

	return SUCCESS;
}


//extern "C"{

INT silicon_aryb_end(int32_t lBHandle, INT run_number, char *error){

//	cvt_V792_data_clear(&SILICON_aryb_type);
	return SUCCESS;
}


INT silicon_aryb_check_fifo(int32_t lBHandle){

	//===============================status register====================================//-->see V785N manual page 47
	uint16_t status_reg1;

//	addr_aryb=(SILICON_ARYB_ADDR<<16)|0x100E;
//	ret_aryb=CAENVME_ReadCycle(lBHandle, addr_aryb, &status_reg1, cvA32_S_DATA, cvD16);

	return status_reg1&0x001;
}

INT silicon_aryb_read_fifo(int32_t lBHandle, void *buff_tmp, int size){

	int count;

//	addr_aryb=(SILICON_ARYB_ADDR<<16)|0x0000;
//	ret_aryb=CAENVME_BLTReadCycle(lBHandle, addr_aryb, (char*)buff_tmp, size, cvA32_S_BLT, cvD64, &count);

	return count;
}


uint32_t ch_check_b;

INT silicon_aryb_read_event(int32_t lBHandle, int32_t lBHandle2, const char *bank_name, char *pevent1, INT off, uint32_t *buff, int buff_size, uint32_t *pdata, int bkm){



	int i;
	int count=silicon_aryb_read_fifo(lBHandle, buff, buff_size);


	ch_check_b = 0;
//	printf("==============================%d\n", count);
	uint32_t event_countb;
	bk_create(pevent1, bank_name, TID_DWORD, (void**)&pdata);
	bkm = 1;

	//printf("Arr approaching count loop########\n");
	iiab = iibb = iicb = 0;

/*
	for(i=0; i<count/4; i++)
	{
		uint32_t data=buff[i];

		switch(data&CVT_QTP_DATA_TYPE_MSK)
		{
			case CVT_QTP_DATUM:
					{
						UINT32 channel= CVT_V785A_GET_DATUM_CH(data);
						UINT32 measure= CVT_V785_GET_DATUM_ADC(data);
						UINT32 UV=CVT_V785_GET_DATUM_UN(data);
						UINT32 OV=CVT_V785_GET_DATUM_OV(data);
						if(channel == 1) {ch_check_b++;}
//						if(ch_check_b > 1) {printf("ADC over counting,, ch_check_b = %d\n",ch_check_b); break;}
						*pdata++=channel;
						*pdata++=measure;
						//tmp_channel = channel;
						//printf("(ARY B ADC) measurement; uv:%d, ov:%d, channel:%d, measurement:%d, \n", UV, OV, channel, measure);
						//printf("external trigger from scaler(ary b u_detector); %u\n", u_detector_triggered);
						//printf("external clock_number_lower from scaler(ary b u_detector); %u\n", clock_number_lower);
						buff_size_intv_us2 += 2;

						iiab++;

					} break;


			case CVT_QTP_EOB:
					{

						user_channel_b = CVT_QTP_GET_HDR_CH_COUNT(data);
						event_countb= CVT_QTP_GET_EOB_EVENT_COUNT(data);
						*pdata++=event_countb;
					//	printf("AADC2; event_count; GCOUNT; clock count:%u, %lu, %lu \n", event_countb, GCOUNT, TT100);
						//printf("Processeed event reading(SB3 ADC): EOB event_count:%d\n", event_countb);
						//printf("external trigger from scaler(ary b u_detector); %u\n", u_detector_triggered);
						//printf("external clock_number_lower from scaler(ary b u_detector); %u\n", clock_number_lower);
						//printf("external clock_number_upper from scaler(ary b u_detector); %u\n", clock_number_upper);

						buff_size_intv_us2 += 3;
						buff_size_intv_us2_c += 7;

						iibb++;

					} break;

			default:
				{iicb++; bkm = 0;} break;
		}
	}*/
        jjs3b++;
	//printf("Processeed event reading(S3 ADC): count:%d\n", jjs3);//count
	//printf("Processeed event reading(S3 ADC): count:%d\n", jjs3);//count

	bk_close(pevent1, pdata);

return bk_size(pevent1);
}


BOOL silicon_aryb_clear_buffer(int32_t lBHandle)
{

/*	UINT16 Bit_set_1_reg;

	addr_aryb=(SILICON_ARYB_ADDR<<16)|0x1006; //bit set 1
	Bit_set_1_reg=0x0080;
	ret_aryb=CAENVME_WriteCycle(lBHandle, addr_aryb, &Bit_set_1_reg, cvA32_S_DATA, cvD16);
	if(ret_aryb){printf("\nError executing buffer clean_1\n");}

	addr_aryb=(SILICON_ARYB_ADDR<<16)|0x1008; //bit clear 1
	Bit_set_1_reg=0x0080;
	ret_aryb=CAENVME_WriteCycle(lBHandle, addr_aryb, &Bit_set_1_reg, cvA32_S_DATA, cvD16);
	if(ret_aryb){printf("\nError executing buffer clean_2\n");}

	return ret_aryb; */
	return 0;
}








//#ifdef __cplusplus
//}
//#endif


