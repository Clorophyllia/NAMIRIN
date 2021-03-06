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
//#include <cvt_V1190.h>
//#include "cvt_V1190.c"
#include <mfe_TDC.h>


#include "midas.h"
#include "odb_trigger.h"
#include "detector_conf.h"
#include "kobra_bline.h"

#define TDC_SUM_BD_UP 3100
#define TDC_SUM_BD_LO 2500

//static cvt_V1190_data F3_PPAC_type;

extern uint32_t time_stamp1;
extern uint32_t time_stamp2;
extern uint64_t GCOUNT;
extern int32_t pcount, pcount64;
extern uint32_t measurement_silicon_tmp;
extern uint32_t channel_silicon_tmp;
extern uint64_t TT100;

extern uint32_t vir_tdc_tof[2];
extern uint32_t vir_ev_count;
extern int VEG_TAG;


uint64_t tag_time_bl;
//uint16_t ppa_or_logic;

float erate_p, drate_p, intv_p, ediff_p;
uint32_t p_event_count_tmp;
uint32_t f3ppac_event;//, f3ppac_event_tmp;

uint32_t  selective_tdc_tmp = 0 ;
uint64_t DGCOUNT;
bool ppac_event_on;

uint32_t uchannel, uchannel_tmp;
UINT32 f3_pp_f_time;
UINT64 f3_pp_time_tag;
UINT64 f3_pp_time_tag_tmp;
UINT32 measurement_f3ppac_tmp;

BEAMLINE_STATS_ONLY *eq_bline;
INT statusp = 0;
BEAMLINEONLY *eqp;

HNDLE hDbp = 1;
HNDLE hKeyp;

//DWORD tick_p, tok_p;
extern float sintv;


char strp[256];

int buff_size_intv_f3p, buff_size_intv_bl, buff_size_intv_bl_c, buff_size_intv_f3p_c;
extern int buff_size_intv_f2p, buff_size_intv_f1p, buff_size_intv_f3s, buff_size_intv_f24;
extern int buff_size_intv_f2p_c, buff_size_intv_f1p_c, buff_size_intv_f3s_c, buff_size_intv_f24_c;


INT f3_ppac_init(int32_t lBHandle){

	//######################	BLINE MONITOR	#########################

        sprintf(strp, "/Custom/%s",beamlineonly[0].name3);

        db_create_key(hDbp, 0, strp, TID_KEY);
        db_find_key(hDbp,0, strp, &hKeyp);


             statusp = db_check_record(hDbp, 0, strp, EQUIPMENT_SCAL_STR3, TRUE);
              if (statusp != DB_SUCCESS) {
                 printf("Cannot create/check statistics record \'%s\', error %d\n", strp, statusp);
                 ss_sleep(500);
              }

              statusp = db_find_key(hDbp, 0, strp, &hKeyp);
              if (statusp != DB_SUCCESS) {
                 printf("Cannot find statistics record \'%s\', error %d\n", strp, statusp);
                 ss_sleep(500);
              }

                statusp = 0;
                eqp = NULL;

                eq_bline = &beamlineonly[0].scal3;
                eq_bline->events = 0;
                eq_bline->events_per_sec = 0;
                eq_bline->data_per_sec = 0;


	//#########################################################################
/*
	f3ppac_event = 0;

	int32_t addr;
	//Make sure V1270N opened OK!
	printf("\n===================== Initializing F3 PPAC TDC...");

	memset( &F3_PPAC_type, 0, sizeof(F3_PPAC_type));

	if(!cvt_V1190_open(&F3_PPAC_type, F3_PPAC_ADDR, lBHandle, CVT_V1290_TYPE_N))
	{
		printf("\n Error executing cvt_F3_PPAC_type_open \n");
		return FE_ERR_HW;
	}
	printf("Done\n");

	UINT16 firmware_rev;

	//get serial number of F1_PPAC_type
	printf("\nGetting system information...");

	//read firmware revision


	addr=(F3_PPAC_ADDR<<16)|CVT_V1190_FW_REV_ADD;
	CAENVME_ReadCycle(lBHandle, addr, &firmware_rev, CVT_V1190_FW_REV_AM, CVT_V1190_FW_REV_DATA_SIZE);
	printf("Firmware Rev. : %d.%d\n", firmware_rev&0xffff0000, firmware_rev&0xffff);

	//clear data
	printf("\nSending data clear...");
		if(!cvt_V1190_data_clear(&F3_PPAC_type)){printf("\nError executing software clean\n");}
	printf("Done\n");


	INT16 default_val=0x0239; //see V1290 manual page 82
	INT16 data;

	//control_register setting
	addr=(F3_PPAC_ADDR<<16)|CVT_V1190_CONTROL_ADD; //CVT_V1190_CONTROL_ADD = 0x1000

	printf("\nControl register setting...");
	CAENVME_WriteCycle(lBHandle, addr, &default_val, CVT_V1190_CONTROL_AM, CVT_V1190_CONTROL_DATA_SIZE);
	CAENVME_ReadCycle(lBHandle, addr, &data, CVT_V1190_CONTROL_AM, CVT_V1190_CONTROL_DATA_SIZE);
	printf("0X%04x\n\n", data);



	sleep(1);

*/
//########################      Passive record of new odb tree  ######################################

    statusp = db_open_record(hDbp, hKeyp, eq_bline, sizeof(BEAMLINE_STATS_ONLY), MODE_WRITE, NULL, NULL);
      if (statusp == DB_NO_ACCESS) {

        statusp = db_set_mode(hDbp, hKeyp, MODE_READ | MODE_WRITE | MODE_DELETE, TRUE);
         if (statusp != DB_SUCCESS)
            cm_msg(MERROR, "register_equipment", "Cannot change access mode for record \'%s\', error %d", strp, statusp);
         else
            cm_msg(MINFO, "register_equipment", "Recovered access mode for record \'%s\'", strp);
         statusp = db_open_record(hDbp, hKeyp, eq_bline, sizeof(BEAMLINE_STATS_ONLY), MODE_WRITE, NULL, NULL);
      }
      if (statusp != DB_SUCCESS) {
         cm_msg(MERROR, "register_equipment", "Cannot open statistics record, error %d. Probably other FE is using it", statusp);
         ss_sleep(3000);
      }

//#######################################################################################################



	return SUCCESS;
}

INT f3_ppac_exit(){
/*
	cvt_V1190_module_reset(&F3_PPAC_type);
	cvt_V1190_close(&F3_PPAC_type);
*/
        eq_bline->events_per_sec = 0;
        eq_bline->data_per_sec = 0;
        eq_bline->events = 0;

	return SUCCESS;

}

INT f3_ppac_begin(INT run_number, char *error, TRIGGER_SETTINGS *tsr){

	//tick_p = ss_millitime();
/*
	INT16 window_width=tsr->f3_ppac_window_width/25;

	INT16 window_offset=0;
	if (tsr->f3_ppac_window_offset>0 || tsr->f3_ppac_window_offset==0){window_offset=tsr->f3_ppac_window_offset/25;}
	if (tsr->f3_ppac_window_offset<0){window_offset=4096-(tsr->f3_ppac_window_offset/25)*-1;}

	INT16 extra_search_margin=tsr->f3_ppac_extra_search_margin/25;
	INT16 reject_margin=tsr->f3_ppac_reject_margin/25;
	INT blt_event_num=1;
	const UINT16 *p_enable_msk=NULL;

	//trigger matching mode operation

//	printf("Setting_trigger_acquisition mode ...");


	if(! cvt_V1190_set_trigger_matching_acquisition_mode(&F3_PPAC_type, window_width, window_offset,
			 extra_search_margin, reject_margin, CVT_V1190_ED_LEADING_ONLY,
			 CVT_V1190_PRW_100NS, p_enable_msk, 1, 1, 1))
	{
		printf("Error executing cvt_F3_PPAC_type_set_trigger_matching_acqusition_mode\n");
		return FE_ERR_HW;
	}
//	printf("Done\n");


	//set enable subtraction of trigger time
//	printf("Setting_enable_subtraction_of_trigger_time...");
	if(!cvt_V1190_write_2_micro(&F3_PPAC_type, CVT_V1190_EN_SUB_TRG_OPCODE, NULL, 0))
	{
		printf("Error setting enable subtraction of trigger time\n");
	}
//	printf("Done\n");



	//readout mode
//	printf("Setting readout mode...");
	if(!cvt_V1190_set_readout_mode(&F3_PPAC_type, TRUE, TRUE, blt_event_num))
	{
		printf("Error executing cvt_F3_PPAC_type_set_readout_mode\n");
		return FE_ERR_HW;
	}
//	printf("Done\n");
*/
	f3_pp_f_time=0;
	f3_pp_time_tag=0;
	DGCOUNT = 0;

	return SUCCESS;

}


INT f3_ppac_end(INT run_number, char *error){
//	printf("\nSending data clear...");
//	if(!cvt_V1190_data_clear(&F3_PPAC_type)){printf("\nError executing software clean\n");}

//	printf("Done\n");

	return SUCCESS;
}


INT f3_ppac_check_fifo(int32_t lBHandle){

	uint32_t addr;
	int16_t output_buffer_event;
//	addr = (F3_PPAC_ADDR<<16)|CVT_V1190_EVENT_STORED_ADD;
//	CAENVME_ReadCycle(lBHandle, addr, &output_buffer_event, CVT_V1190_EVENT_STORED_AM, CVT_V1190_EVENT_STORED_DATA_SIZE);
	return output_buffer_event;
}

INT f3_ppac_read_fifo(int32_t lBHandle, void *buff_tmp, int size){

	int count;
	uint32_t addr;

//	addr = (F3_PPAC_ADDR<<16)|CVT_V1190_OUT_BUFFER_ADD;

//	CAENVME_BLTReadCycle(lBHandle, addr, (char*)buff_tmp, size, CVT_V1190_OUT_BUFFER_AM, CVT_V1190_OUT_BUFFER_DATA_SIZE, &count);

	return count;
}


INT f3_ppac_read_event(int32_t lBHandle, const char *bank_name, char *pevent, INT off, uint32_t *buff, int buff_size, uint32_t *pdata, int bkm){

	int i=0;
	int count=f3_ppac_read_fifo(lBHandle, buff, buff_size);

	float pdsize = 4.e-6;
	float cdsize = 1.e-6;
	uint32_t TDC_tmp[5] = {0,0,0,0,0};

	ppac_event_on = 0;


//printf("--------------- %d %d\n", nb*4, count);

	bk_create(pevent, bank_name, TID_DWORD, (void**)&pdata);
	bkm = 1;

/*	for(i=0; i<count/4; i++){

		uint32_t data=buff[i];
		switch(data&CVT_V1190_DATA_TYPE_MSK)
		{
			case CVT_V1190_GLOBAL_HEADER:
				{
					UINT32 event_count= CVT_V1190_GET_GLB_HDR_EVENT_COUNT(data);
					f3ppac_event = event_count;
					*pdata++=event_count; selective_tdc_tmp ++;
					//printf("F3PPAC event_count:%i \n",event_count);

					buff_size_intv_f3p += 3;
					buff_size_intv_f3p_c += 5;

                                                        // ################
                                        eq_bline = &beamlineonly[0].scal3;
                                        eq_bline->events = event_count;


					 eq_bline->events_per_sec = erate_p;



                                        if((TT100 - tag_time_bl)/1.e+8 > 1.5){

						buff_size_intv_bl = buff_size_intv_f3p + buff_size_intv_f2p + buff_size_intv_f1p + buff_size_intv_f3s + buff_size_intv_f24 + 3;
						buff_size_intv_bl_c = buff_size_intv_f3p_c + buff_size_intv_f2p_c + buff_size_intv_f1p_c + buff_size_intv_f3s_c + buff_size_intv_f24_c + 2;

                                                drate_p = (pdsize*(float)buff_size_intv_bl + cdsize*(float)buff_size_intv_bl_c)/(float)(TT100 - tag_time_bl)*1.e+8;

                                               eq_bline->data_per_sec = drate_p;


						buff_size_intv_f3p = buff_size_intv_f2p = buff_size_intv_f1p = buff_size_intv_f3s = buff_size_intv_f24 = 0;
						buff_size_intv_f3p_c = buff_size_intv_f2p_c = buff_size_intv_f1p_c = buff_size_intv_f3s_c = buff_size_intv_f24_c = 0;
						tag_time_bl = TT100;

                                        }       // ############## custom space def



				} break;

			case CVT_V1190_TDC_MEASURE:
				{
					UINT32 channel= CVT_V1290_GET_TDC_MSR_CHANNEL(data);
					UINT32 measure= CVT_V1290_GET_TDC_HDR_MEASURE(data);
					*pdata++=channel;
					if(channel == 15)*pdata++= vir_tdc_tof[1];
					else *pdata++=measure;
					if(channel ==0 && measure > 0) measurement_f3ppac_tmp=measure;

					buff_size_intv_f3p += 2;

					if(channel < uchannel_tmp) uchannel = uchannel_tmp;
					uchannel_tmp = channel;
					//if(measure > 0) printf("PPAC TDC channel:%d, measurement:%05f\n",channel, measure*0.025);

					if(channel == 0 && measure > 0){

						//printf("F3PPAC_TDC event count:%u  GCOUNT:%u pcount:%u, channel:%d, ppac_measurement:%05f, silicon_measurement:%d, silicon_channel:%d",selective_tdc_tmp,GCOUNT, pcount, channel, measure*0.025, measurement_silicon_tmp, channel_silicon_tmp);

					}



					UINT32 global_time_tmp=time_stamp1;
					f3_pp_time_tag_tmp=time_stamp2;

					if(channel < 5 && measure > 0)TDC_tmp[channel] = measure;

					//if(channel == 0 && measure > 0) printf(" time_tag:%d, u_time:%11f\n", global_time_tmp, (f3_pp_time_tag_tmp|global_time_tmp)*800e-9);

				} break;

			case CVT_V1190_GLOBAL_TRIGGER_TIME:
				{
				#ifndef USE_PPAC_F2
					UINT32 global_time=CVT_V1190_GET_GLB_TRG_TIME_TAG(data);
					if(f3_pp_f_time>global_time){f3_pp_time_tag+=0x8000000;}
					f3_pp_f_time=global_time;
				#else
					UINT32 global_time=time_stamp1;
					f3_pp_time_tag=time_stamp2;
				#endif
					*pdata++=GCOUNT;
					*pdata++=f3_pp_time_tag;
					*pdata++=global_time;
					*pdata++=pcount;
					*pdata++=pcount64;
					//printf("F3PPAC_Global_time_tag:%d, u_time:%11f\n", global_time, (f3_pp_time_tag|global_time)*800e-9);
					//printf("GCOUNT:%u, pcount:%u, pcount64:%u, buffsize:%d \n", GCOUNT, pcount, pcount64,buff_size);


					buff_size_intv_f3p += 5;
					buff_size_intv_f3p_c += 9;

				} break;

			default:
				{
					bkm = 0;
					//printf("unknown data packets\n");
				} break;

		}
	}
*/

					*pdata++=vir_ev_count; selective_tdc_tmp ++;

                                        buff_size_intv_f3p += 3;
                                        buff_size_intv_f3p_c += 5;

                                                        // ################
                                        eq_bline = &beamlineonly[0].scal3;
                                        eq_bline->events = vir_ev_count;


                                         eq_bline->events_per_sec = erate_p;



                                        if((TT100 - tag_time_bl)/1.e+8 > 1.5){

                                                buff_size_intv_bl = buff_size_intv_f3p + buff_size_intv_f2p + buff_size_intv_f1p + buff_size_intv_f3s + buff_size_intv_f24 + 3;
                                                buff_size_intv_bl_c = buff_size_intv_f3p_c + buff_size_intv_f2p_c + buff_size_intv_f1p_c + buff_size_intv_f3s_c + buff_size_intv_f24_c + 2;

                                                drate_p = (pdsize*(float)buff_size_intv_bl + cdsize*(float)buff_size_intv_bl_c)/(float)(TT100 - tag_time_bl)*1.e+8;

                                               eq_bline->data_per_sec = drate_p;


                                                buff_size_intv_f3p = buff_size_intv_f2p = buff_size_intv_f1p = buff_size_intv_f3s = buff_size_intv_f24 = 0;
                                                buff_size_intv_f3p_c = buff_size_intv_f2p_c = buff_size_intv_f1p_c = buff_size_intv_f3s_c = buff_size_intv_f24_c = 0;
                                                tag_time_bl = TT100;

                                        }

        for(i = 0; i < 16; i++){

                *pdata++= i;
                if(i == 15) *pdata++= vir_tdc_tof[1];
                else *pdata++= vir_tdc_tof[1] + (int)(500 + 1000*drand48());
                buff_size_intv_f3p += 2;

        }


					*pdata++=GCOUNT;
                                        *pdata++=0;
                                        *pdata++=0;
                                        *pdata++=pcount;
                                        *pdata++=pcount64;

                                        buff_size_intv_f3p += 5;
                                        buff_size_intv_f3p_c += 9;



	bk_close(pevent, pdata);


	if(TDC_tmp[1] + TDC_tmp[2] - 2*TDC_tmp[0] > TDC_SUM_BD_LO && TDC_tmp[1] + TDC_tmp[2] - 2*TDC_tmp[0] < TDC_SUM_BD_UP && TDC_tmp[0] > 0 )
	if(TDC_tmp[3] + TDC_tmp[4] - 2*TDC_tmp[0] > TDC_SUM_BD_LO && TDC_tmp[3] + TDC_tmp[4] - 2*TDC_tmp[0] < TDC_SUM_BD_UP){ ppac_event_on = 1; DGCOUNT = GCOUNT;}

//free(Fbuff);

	return bk_size(pevent);

}

