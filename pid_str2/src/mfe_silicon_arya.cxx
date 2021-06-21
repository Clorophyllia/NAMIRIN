
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <memory.h>
#include <ctype.h>


#include "midas.h"
#include "mfe_ADC.h"
#include "mfe_TDC.h"
#include "odb_trigger.h"
#include "detector_conf.h"
#include "kobra_user.h"
#include "msystem.h"

#define ADC_VAL_BD_UP 1100
#define ADC_VAL_BD_LO 400

USER_STATS *eq_user; //BEAMLINE_STATS *eq_user;
INT statusa = 0;
USER_DETECTOR *eqa; //BEAMLINE *eqa;

HNDLE hDba = 1;
HNDLE hKeya;


char stra[256];

extern uint32_t user_channel_b;
uint32_t user_channel_a;
uint32_t mcst_reg_order;
uint32_t silicon_ary_event;

//static cvt_V792_data SILICON_arya_type;

uint32_t addr_ary;

extern uint32_t u_detector_triggered;
extern uint32_t time_stamp1;
extern uint32_t time_stamp2;
extern uint32_t measurement_f3ppac_tmp;
UINT64 silicon_pp_time_tag;
UINT32 measurement_silicon_tmp;
UINT32 channel_silicon_tmp;
extern uint64_t TT100;
uint64_t tag_time_dt;


extern uint64_t TimeTag;
uint64_t TimeTag_ary_tmp;
extern uint64_t GCOUNT;
extern int32_t pcount, pcount64;

uint32_t u_detector_event_tmp;
uint32_t Counters_tmp_udetector;

int iia ,iib, iic;
uint32_t jjs3 = 0;

float erate,drate,intv, ediff;
uint32_t event_count_tmp;

int multi = 0;
uint32_t selective_adc_tmp=0;
uint32_t selective_adc_tmp2=0;

int buff_size_intv, buff_size_intv_us1;
int buff_size_intv_c, buff_size_intv_us1_c;
extern int buff_size_intv_us2, buff_size_intv_us2_c;

extern bool ppac_event_on;
uint64_t D1COUNT, DCCOUNT;

INT silicon_arya_init(int32_t lBHandle, TRIGGER_SETTINGS *tsr){

//#####
	sprintf(stra, "/Custom/%s",detector[0].name2);

	db_create_key(hDba, 0, stra, TID_KEY);
	db_find_key(hDba,0, stra, &hKeya);


	     statusa = db_check_record(hDba, 0, stra, EQUIPMENT_SCAL_STR2, TRUE);
	      if (statusa != DB_SUCCESS) {
	         printf("Cannot create/check statistics record \'%s\', error %d\n", stra, statusa);
	         ss_sleep(500);
	      }

	      statusa = db_find_key(hDba, 0, stra, &hKeya);
	      if (statusa != DB_SUCCESS) {
	         printf("Cannot find statistics record \'%s\', error %d\n", stra, statusa);
	         ss_sleep(500);
	      }

		statusa = 0;
		eqa = NULL;

		eq_user = &detector[0].dstat;
		eq_user->events = 0;
		eq_user->events_per_sec = 0;
		eq_user->data_per_sec = 0;
// ###### custome space def
  u_detector_event_tmp = 0;
  Counters_tmp_udetector = 0;
  TimeTag_ary_tmp = 0;

/*

  UINT16 firmware_rev, reg_value, c_reg_value, geo_add, mcst_reg, msct_reg_order, status_reg, boardversion_reg, Bit_set_1_reg; 
  UINT16 interrupt_level,interrupt_vector,evt_tri, evt_tri_write, interrupt_level_write;

	printf("\n========================== Initializing SILICON ARYA ADC...\n");
	printf("V785N initialization...");
	if(!cvt_V792_open(&SILICON_arya_type, SILICON_ARYA_ADDR, lBHandle, CVT_V785_TYPE_N))
	{
		printf("\n Error executing cvt_SILICON_arya_type_open \n");
		return FE_ERR_HW;
	}
	printf("Done\n");

	//==================================firmware version
	addr_ary=(SILICON_ARYA_ADDR<<16)|0x1000;
	ret_ary=(CAENVME_ReadCycle(lBHandle, addr_ary, &firmware_rev, cvA32_S_DATA, cvD16));

	printf("Firmware Rev. : %x\n", firmware_rev);

	//==================================geo_add initial
	addr_ary=(SILICON_ARYA_ADDR<<16)|0x1002;
	ret_ary=(CAENVME_ReadCycle(lBHandle, addr_ary, &geo_add, cvA32_S_DATA, cvD16));
	printf("geo_add(1002) : %x\n", geo_add);
   	//==================================mcst reg for MCST address setting  
	addr_ary=(SILICON_ARYA_ADDR<<16)|0x1004;
	mcst_reg = 0xAB;
	ret_ary=(CAENVME_WriteCycle(lBHandle, addr_ary, &mcst_reg, cvA32_S_DATA, cvD16));
	printf("mcst_reg(1004) set: %x\n", mcst_reg);

	//==================================mcst reg for MCST board first
        addr_ary=(SILICON_ARYA_ADDR<<16)|0x101A;
        mcst_reg_order = 0x00; //0: inactive
        ret_ary=(CAENVME_WriteCycle(lBHandle, addr_ary, &mcst_reg_order, cvA32_S_DATA, cvD16));
	printf("mcst_reg(1004) order: %x\n", mcst_reg_order);
	//==================================interrupt level
	addr_ary=(SILICON_ARYA_ADDR<<16)|0x100A;
	ret_ary=(CAENVME_ReadCycle(lBHandle, addr_ary, &interrupt_level, cvA32_S_DATA, cvD16));
	printf("interrupt level(100A) : %x\n", interrupt_level);

	//==================================interrupt vector
	addr_ary=(SILICON_ARYA_ADDR<<16)|0x100C;
	ret_ary=(CAENVME_ReadCycle(lBHandle, addr_ary, &interrupt_vector, cvA32_S_DATA, cvD16));
	printf("interrupt vector(100C) : %x\n", interrupt_vector);

	//==================================status reg
	addr_ary=(SILICON_ARYA_ADDR<<16)|0x100E;
	ret_ary=(CAENVME_ReadCycle(lBHandle, addr_ary, &status_reg, cvA32_S_DATA, cvD16));
	printf("status_reg(100E) : %x\n", status_reg);
	//==================================event trigger register
	addr_ary=(SILICON_ARYA_ADDR<<16)|0x1020;
	ret_ary=(CAENVME_ReadCycle(lBHandle, addr_ary, &evt_tri, cvA32_S_DATA, cvD16));
	printf("event_trigger(1020) : %x\n", evt_tri);

	//==================================board version
	addr_ary=(SILICON_ARYA_ADDR<<16)|0x8032;
	ret_ary=(CAENVME_ReadCycle(lBHandle, addr_ary, &boardversion_reg, cvA32_S_DATA, cvD16));
	printf("Board version(8032) : %x\n", boardversion_reg);
	//uint16_t c_reg_value;

	//=======================set Block Transfer Mode====================//--->see V785N manual page 65-66
	addr_ary=(SILICON_ARYA_ADDR<<16)|0x1010;
	reg_value=0x0044;
	CAENVME_WriteCycle(lBHandle, addr_ary, &reg_value, cvA32_S_DATA, cvD16);


	//=======================set operation mode====================//--->see V785N manual page 53-55
	addr_ary=(SILICON_ARYA_ADDR<<16)|0x1032; //==================bit set 2
	reg_value=0x0880; //default 0x4980 4980 for *2 4880 for *16, sliding scale tech. only up to 3840 is valid, not 4096, 0x880->count only accepted event
	CAENVME_WriteCycle(lBHandle, addr_ary, &reg_value, cvA32_S_DATA, cvD16);

	addr_ary=(SILICON_ARYA_ADDR<<16)|0x1034; //==================bit clear 2
	c_reg_value=(~reg_value)&0x7fff;
	CAENVME_WriteCycle(lBHandle, addr_ary, &c_reg_value, cvA32_S_DATA, cvD16);

	//================================set threshold=================//-->see V785N manual 59-60

	printf("Setting threshold...\n");
	int i=0;
	uint16_t th_val;

	for(i=0; i<32; i++) //if 16 channel ADC(V785N), channel step should be 4 instead of 2
	{
		addr_ary=(SILICON_ARYA_ADDR<<16)|0x1080|2*i;
		th_val=tsr->si_arya_th[i];
		reg_value=0x0000|th_val;
	    ret_ary=CAENVME_WriteCycle(lBHandle, addr_ary, &reg_value, cvA32_S_DATA, cvD16);
	    if(ret_ary){printf("Error executing %d_channel threshold setting\n", i);}
	printf("array a th ch: %i, val: %i\n",i,reg_value);
	}
	printf("Done\n");

	//==================================mcst reg bit set1
        addr_ary=(SILICON_ARYA_ADDR<<16)|0x1006; //bit set 1
        Bit_set_1_reg=0x0080;
        ret_ary=CAENVME_WriteCycle(lBHandle, addr_ary, &Bit_set_1_reg, cvA32_S_DATA, cvD16);
        if(ret_ary){printf("\nError executing buffer writing_1\n");}

	//=================================data clear
	printf("Sending data clear...");
	ret_ary=static_cast<CVErrorCodes>(silicon_arya_clear_buffer(lBHandle));
	printf("Done");
*/

//########################	Passive record of new odb tree	######################################


    statusa = db_open_record(hDba, hKeya, eq_user, sizeof(USER_STATS), MODE_WRITE, NULL, NULL);
      if (statusa == DB_NO_ACCESS) {

        statusa = db_set_mode(hDba, hKeya, MODE_READ | MODE_WRITE | MODE_DELETE, TRUE);
         if (statusa != DB_SUCCESS)
            cm_msg(MERROR, "register_equipment", "Cannot change access mode for record \'%s\', error %d", stra, statusa);
         else
            cm_msg(MINFO, "register_equipment", "Recovered access mode for record \'%s\'", stra);
         statusa = db_open_record(hDba, hKeya, eq_user, sizeof(USER_STATS), MODE_WRITE, NULL, NULL);
      }
      if (statusa != DB_SUCCESS) {
         cm_msg(MERROR, "register_equipment", "Cannot open statistics record, error %d. Probably other FE is using it", statusa);
         ss_sleep(3000);
      }

//#######################################################################################################





	return SUCCESS;
}

INT silicon_arya_exit(int32_t lBHandle){

//	silicon_arya_clear_buffer(lBHandle);
//	cvt_V792_close(&SILICON_arya_type);
//####
	eq_user->events_per_sec = 0;
	eq_user->data_per_sec = 0;
	eq_user->events = 0;
					// #### custom space def
	return SUCCESS;
}



INT silicon_arya_begin(int32_t lBHandle, INT run_number, char *error, TRIGGER_SETTINGS *tsr){

	//tick = ss_millitime(); 	//custom space def

	silicon_pp_time_tag = 0;

//	silicon_arya_clear_buffer(lBHandle);

	D1COUNT = DCCOUNT = 0;

	return SUCCESS;
}


//extern "C"{

INT silicon_arya_end(int32_t lBHandle, INT run_number, char *error){


//	cvt_V792_data_clear(&SILICON_arya_type);


	return SUCCESS;
}

//}//extern

INT silicon_arya_check_fifo(int32_t lBHandle){

	//===============================status register====================================//-->see V785N manual page 47
	uint16_t status_reg1;
//	addr_ary=(SILICON_ARYA_ADDR<<16)|0x100E;
//	ret_ary=CAENVME_ReadCycle(lBHandle, addr_ary, &status_reg1, cvA32_S_DATA, cvD16);
	return status_reg1&0x001;
}

INT silicon_arya_read_fifo(int32_t lBHandle, void *buff_tmp, int size){

	int count;

//	addr_ary=(SILICON_ARYA_ADDR<<16)|0x0000;
//	ret_ary=CAENVME_BLTReadCycle(lBHandle, addr_ary, (char*)buff_tmp, size, cvA32_S_BLT, cvD64, &count);

	return count;
}


INT silicon_arya_read_async(int32_t lBHandle, void *buff_tmp, int size){

	int count;

//	addr_ary=(SILICON_ARYA_ADDR<<16)|0x0000;
//	count = CAENVME_BLTReadAsync(lBHandle, addr_ary, (char*)buff_tmp, size, cvA32_S_MBLT, cvD64);

	return count;
}



uint32_t u_detector_triggered_tmp;
uint32_t ch_check;

INT silicon_arya_read_event(int32_t lBHandle, int32_t lBHandle2, const char *bank_name, char *pevent, INT off, uint32_t *buff, int buff_size, uint32_t *pdata, int bkm){

	int i;
	uint32_t event_count;

	float pdsize = 4.e-6;		// size of data buffer per event channel
	float cdsize = 1.e-6;

	int thandle;
	bool logic = 0;


	uint32_t ADC_tmp[16] = {0};

	int count=silicon_arya_read_fifo(lBHandle, buff, buff_size);
	ch_check = 0;
//	printf("==============================%d\n", count);

	bk_create(pevent, bank_name, TID_DWORD, (void**)&pdata);
	bkm = 1;

	//printf("Arr approaching count loop########\n");
	iia = iib = iic = 0;


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
						if(channel == 7) {ch_check++;}
						//if(ch_check > 1) {printf("ADC over counting,, ch_check = %d\n",ch_check); break;}
						*pdata++=channel;
						*pdata++=measure;
						measurement_silicon_tmp=measure;
						channel_silicon_tmp=channel;
						//tmp_channel = channel;


						//if (multi > 1 && measure > 400 && measure < 700)printf("Channel measurement ERROR!!; channel:%d, measurement:%d, multi stack:%d, \n", channel, measure,multi);

						//if(channel<16) printf("(S3 ARY A ADC)event:%u GCOUNT:%u, pcount:%u, uv:%d, ov:%d, ppac_measure:%05f, silicon measurement:%d, silicon channel:%d ",selective_adc_tmp,GCOUNT, pcount, UV, OV, measurement_f3ppac_tmp*0.025, measure, channel);
						UINT32 global_time=time_stamp1;
						silicon_pp_time_tag=time_stamp2;
						//if(channel<16) printf("time_tag:%d, u_time:%11f\n", global_time, (silicon_pp_time_tag|global_time)*800e-9);
						//if(channel==16) printf("From gate generator (S3 ARY A ADC)event:%u GCOUNT:%u, pcount:%u, uv:%d, ov:%d, ppac_measure:%05f, silicon measurement:%d, silicon channel:%d ",selective_adc_tmp,GCOUNT, pcount, UV, OV, measurement_f3ppac_tmp*0.025, measure, channel);
						//if(channel==16) printf("time_tag:%d, u_time:%11f\n", global_time, (silicon_pp_time_tag|global_time)*800e-9);
						//printf("external trigger from scaler(ary a u_detector); %u\n", u_detector_triggered);
						//printf("external clock_number_lower from scaler(ary a u_detector); %u\n", clock_number_lower);

						buff_size_intv_us1 += 2;
						//buff_size_intv_us1_c += 2;
						if(channel<16) selective_adc_tmp++;

						if( channel<16 && measure > 0 && measure < 4096) ADC_tmp[channel] = measure;

						iia++;

					} break;


			case CVT_QTP_EOB:
					{
						event_count= CVT_QTP_GET_EOB_EVENT_COUNT(data);
						user_channel_a = CVT_QTP_GET_HDR_CH_COUNT(data);
						*pdata++=GCOUNT;
						*pdata++=event_count;
						*pdata++=pcount;
					        *pdata++=pcount64;

						buff_size_intv_us1 += 6;
						buff_size_intv_us1_c += 7;

							// ################
						eq_user = &detector[0].dstat;
						eq_user->events = event_count;


						silicon_ary_event = event_count;
					//	intv = (float)(tok - tick)/1000.;
						multi = 0;

						eq_user->events_per_sec = erate;

						if((TT100 - tag_time_dt)/1.e+8 > 1.5){

							buff_size_intv = buff_size_intv_us1 + buff_size_intv_us2 + 3;
							buff_size_intv_c = buff_size_intv_us1_c + buff_size_intv_us2_c + 2;

							//ediff = event_count - event_count_tmp;
							//erate = (float)ediff/(float)intv;
							drate = (pdsize*(float)buff_size_intv + cdsize*(float)buff_size_intv_c)/(float)(TT100 - tag_time_dt)*1.e+8;

							eq_user->data_per_sec = drate;

							//event_count_tmp = event_count;
							buff_size_intv_us1 = buff_size_intv_us2 = 0;
							buff_size_intv_us1_c = buff_size_intv_us2_c = 0;

							tag_time_dt = TT100;
						}


						//printf("AADC1; event_count; GCOUNT; clock count:%u, %lu \n", event_count, GCOUNT);
						//printf("external trigger from scaler(ary a u_detector); %u\n", u_detector_triggered);
						//printf("external clock_number_lower from scaler(ary a u_detector); %u\n", clock_number_lower);
						//printf("external clock_number_ from scaler(ary a u_detector); %u\n", clock_number_upper);
						iib++;

					} break;

			default:
				{iic++; bkm = 0;} break;
		}
	}*/
        jjs3++;
	//printf("Processeed event reading(S3 ADC): count:%d\n", jjs3);//count
	//printf("Processeed event reading(S3 ADC): count:%d\n", jjs3);//count

	//float livelive; 			// #### custom space def
	//livelive = (float)(event_count-u_detector_event_tmp)/(float)(u_detector_triggered-Counters_tmp_udetector);	// #### custom space def

//	printf("Live time(beamline): %u / %u\n", f1ppac_event-f1ppac_event_tmp,beamline_triggered-Counters_tmp_ppac);
//	if( (u_detector_triggered-Counters_tmp_udetector) !=0 && u_detector_triggered>u_detector_triggered_tmp)printf("Live time(u_detector): %u \ %u \n",event_count-u_detector_event_tmp,  u_detector_triggered-Counters_tmp_udetector);

	//if( (u_detector_triggered-Counters_tmp_udetector) !=0 && u_detector_triggered>u_detector_triggered_tmp)printf("Live time(u_detector): %f\n",livelive);
	//	#######
	if(TimeTag > TimeTag_ary_tmp+1000000){//reset per 1 sec.

		u_detector_event_tmp = event_count;
		TimeTag_ary_tmp = TimeTag;

	}
			// ##### custom space def
	//u_detector_triggered_tmp = u_detector_triggered;
	bk_close(pevent, pdata);

	for(i=0; i < 16; i++) if(ADC_tmp[i] > ADC_VAL_BD_LO && ADC_tmp[i] < ADC_VAL_BD_UP) {logic = 1; i = 16;}

	if(logic == 1){
		D1COUNT ++;
		if(ppac_event_on == 1)	DCCOUNT ++;
	}


	return bk_size(pevent);

}


BOOL silicon_arya_clear_buffer(int32_t lBHandle){

/*	UINT16 Bit_set_1_reg;

	addr_ary=(SILICON_ARYA_ADDR<<16)|0x1006; //bit set 1
	Bit_set_1_reg=0x0080;
	ret_ary=CAENVME_WriteCycle(lBHandle, addr_ary, &Bit_set_1_reg, cvA32_S_DATA, cvD16);
	if(ret_ary){printf("\nError executing buffer clean_1\n");}

	addr_ary=(SILICON_ARYA_ADDR<<16)|0x1008; //bit clear 1
	Bit_set_1_reg=0x0080;
	ret_ary=CAENVME_WriteCycle(lBHandle, addr_ary, &Bit_set_1_reg, cvA32_S_DATA, cvD16);
	if(ret_ary){printf("\nError executing buffer clean_2\n");}
*/
//	return ret_ary;
	return 0;

}

