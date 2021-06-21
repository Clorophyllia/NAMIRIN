#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <memory.h>
#include <ctype.h>
#include <termios.h>
#include <assert.h>

//#include <cvt_board_commons.h>
//#include <cvt_common_defs.h>
//#include <cvt_V1190.h>
#include <mfe_TDC.h>
//#include "CAEN_PLU.h"
#include "midas.h"
#include "odb_trigger.h"
#include "detector_conf.h"
//#include "PLUscalerLib.h"

#include "mfe.h"
#include "msystem.h"
//#include "strlcpy.h"  // kobradaq PC doesn't include this Lib.
#include "msystem.h"

#include "kobra.h"

#define BL_FORBID 1.e+7
#define PERI_INTV 1000
#define SCAN_INTV 5000
#define NUMB_STAT 10

//INT status;

//static cvt_V1190_data F3_PPAC_type;
//extern uint32_t time_stamp1;
//extern uint32_t time_stamp2;
//UINT32 f3_pp_f_time;
//UINT64 f3_pp_time_tag;
char *vme_base_address = "32100000";
uint32_t vme_base_address1 = 0x3210;

DWORD stick, stok;//, stick2;


// timetag coordinates

int logi = 0;
int ini_def = 0;
int acount = 0;
long double toff,soff;
long double iibuf,jjjbuf;
long double tcount, scount;
long double trate;
int srate;

uint32_t jj = 0;
uint32_t f1ppac_event_tmp0 = 0;
uint32_t f3ppac_event_tmp0 = 0;
uint32_t silicon_ary_event_tmp0 = 0;


uint64_t tag_time2;

uint32_t Counters_tmp=0;
//uint32_t Counters_tmp1=0;
uint32_t Counters_tmp2=0;
//uint32_t Counters_tmp3=0;

float Counters_recount=0;
//float Counters_recount1=0;
float Counters_recount2=0;

//extern uint16_t sil_or_logic;
//extern uint16_t ppa_or_logic;
extern uint32_t OR_COUNT;
uint32_t OR_COUNT_tmp;

float ltmp1, ltmp2, ltmp3, ltmp_test;
//float live_time;
float sintv,  ediff_fp, ediff2_fp; //, sintv2;

uint32_t ddata,dbuf,djj;
char str[256],str1[128];
HNDLE hDb = 1;
HNDLE hDB1 = 2;
HNDLE hKey,hKey1,hKey2;

//extern BEAMLINE bline[];

BEAMLINE_STATS *eq_scal;


INT status = 0;
BEAMLINE *eqr;

int buff_size_intv_f24;
int buff_size_intv_f24_c;

int icc = 0;
uint64_t TT100,GCOUNT,GCOUNT_tmp;
int32_t pcount, pcount64;
uint64_t TT100_tmp=0;
double rat1,rat2,rat3,rat0;
double rat11,rat12,rat13,rat10;

uint32_t beamline_triggered;
uint32_t u_detector_triggered;

extern uint32_t silicon_ary_event;
extern uint32_t f1ppac_event;
extern uint32_t f3ppac_event;
extern float erate, drate, erate_p, drate_p;

uint64_t TimeTag;
uint64_t TimeTag_tmp=0;

extern uint64_t D1COUNT, DCCOUNT;
uint64_t d1count_tmp, dccount_tmp;
float COR_LT; //COR_LT_NO, COR_LT_DE;

uint32_t *fraction_count = (uint32_t *)malloc( (1 + NUMB_STAT*(1 + SCAN_INTV/PERI_INTV)) * sizeof(uint32_t));

FILE *flog = fopen("/home/ldy/online/Run_log","a");




INT RUN_logger(int logging_mode){

/* logging mode val description:

0: clearing scaler buffer
1: start a run
2: stop the run
3: stop the sub-run
4: pause

*/
	double tot_daq_trigg, bl_daq_trigg, det_daq_trigg, TOR;
	float ltime_t, ltime_bl, ltime_det, ltime_cor;
	float rate_t, rate_bl, rate_det;

	int sir;
	int sir_tmp = 4;
	int sir_tmp8 = 8;
	int sirl_legnth = 64;
	char sirl[64], sirm[64];
	//std::string strtmp;
	char strtmp[128];

	double scount[8];


	db_find_key(hDb,0, "/Runinfo/Run number", &hKey2);
	db_get_value(hDb, 0, "/Runinfo/Run number", &sir, &sir_tmp, TID_INT32, 0);

	db_find_key(hDb,0, "/Runinfo/Start time", &hKey2);
	db_get_value(hDb, 0, "/Runinfo/Start time", sirl, &sirl_legnth, TID_STRING, 0);

	db_find_key(hDb,0, "/Runinfo/Stop time", &hKey2);
	db_get_value(hDb, 0, "/Runinfo/Stop time", sirm, &sirl_legnth, TID_STRING, 0);

	db_find_key(hDb,0, "/Equipment/Trigger1 (Beamline)/Statistics/Events sent", &hKey2);
	db_get_value(hDb, 0, "/Equipment/Trigger1 (Beamline)/Statistics/Events sent", &tot_daq_trigg, &sir_tmp8, TID_DOUBLE, 0);

	db_find_key(hDb,0, "/Custom/Trigger3 (Beamline only)/Events sent", &hKey2);
	db_get_value(hDb, 0, "/Custom/Trigger3 (Beamline only)/Events sent", &bl_daq_trigg, &sir_tmp8, TID_DOUBLE, 0);

	db_find_key(hDb,0, "/Custom/Trigger2 (Detector)/Events sent", &hKey2);
	db_get_value(hDb, 0, "/Custom/Trigger2 (Detector)/Events sent", &det_daq_trigg, &sir_tmp8, TID_DOUBLE, 0);

	for(int i = 0; i < 8; i++){
		sprintf(strtmp, "/Custom/Trigger1 (Beamline)/CH%d", i+1/*std::__cxx11::to_string(i)*/);
		db_find_key(hDb,0, /*static_cast<const char*>(strtmp)*/strtmp, &hKey2);
		db_get_value(hDb, 0, /*static_cast<const char*>(strtmp)*/strtmp, &scount[i], &sir_tmp8, TID_DOUBLE, 0);
	}

	TOR = scount[0]/1.e+8;

	ltime_t = static_cast<float>(tot_daq_trigg/scount[4]);
	ltime_bl = static_cast<float>(bl_daq_trigg/scount[3]);
	ltime_det = static_cast<float>(det_daq_trigg/scount[2]);

	rate_t = static_cast<float>(tot_daq_trigg/TOR);
	rate_bl = static_cast<float>(bl_daq_trigg/TOR);
	rate_det = static_cast<float>(det_daq_trigg/TOR);

	switch(logging_mode){


		case 1: {

			fprintf(flog,"**------------------\nRUN%d\toperation had started at: %s \nScaler accumulated value:\n", sir, sirl);
			break;
		}

		case 2: {

			//fprintf(flog,"Scaler Channels Count:\n");
			for(int i = 0; i < 8; i++) fprintf(flog,"Ch%d: %lf\n", i+1, scount[i]);

			fprintf(flog,"\nRuntime statics:\n");
			fprintf(flog,"Total Live-time: %f\nBeamline Live-time: %f\nDetector Live-time: %f\n", ltime_t, ltime_bl, ltime_det);
			fprintf(flog,"\nTotal Rate: %f\nBeamline Rate: %f\nDetector Rate: %f\n", rate_t, rate_bl, rate_det);

			fprintf(flog,"\noperation had ended at: %s\nScaler accumulated value:\n",  sirm);
			fprintf(flog,"------------------**\n\n");
			break;
		}
	}

	return 0;


}

/*INT clear_buffer(int32_t BHandle, CAEN_PLU_ERROR_CODE ret){

		// empty the FIFO in the main FPGA
		CAEN_PLU_WriteReg(BHandle, MAIN_SOFT_RESET,0x1);


		unsigned int channelenable = 31;
 	       ret = CAEN_PLU_WriteReg(BHandle, 0x1028, channelenable);

 		unsigned int controlregister = 1985;
		ret = CAEN_PLU_WriteReg(BHandle, 0x1000, controlregister);
        	printf("FW2495SC control resgister at initial %x\n", controlregister);

	return 0;
}*/

INT v2495_init(int32_t lBHandle){

  beamline_triggered = 0;
  u_detector_triggered = 0;

//  CAEN_PLU_ERROR_CODE ret_2495;

  uint32_t   val;
  char       sn[100];
  char *vme_base_address = "32100000";



     /*---- Create just the key , leave it empty ---------------------------------*/
//      sprintf(str, "/Custom/%s/Variables", beamline[0].name);
//	sprintf(str, "/Custom/Scaler");
  //    db_create_key(hDb, 0, str, TID_KEY);
    //  db_find_key(hDb, 0, str, &hKey);
     // beamline[0].hkey_variables = hKey;


	INT idx;

//#############		create new odb tree 2 save bline statistics	################

	int ta1 = 0;
	int ta2 = 4;

	     sprintf(str, "/Custom/%s", beamline[0].name);

		db_create_key(hDb, 0, str, TID_KEY);
		db_find_key(hDb,0, str, &hKey);

		db_create_key(hDb, 0, "/Custom/Scaler", TID_KEY);
		db_find_key(hDb,0, "/Custom/Scaler", &hKey1);
		db_set_value(hDb, 0, "/Custom/Scaler/logi1", &ta1, ta2, 1, TID_INT);


	     status = db_check_record(hDb, 0, str, EQUIPMENT_SCAL_STR, TRUE);
	      if (status != DB_SUCCESS) {
	         printf("Cannot create/check statistics record \'%s\', error %d\n", str, status);
	         ss_sleep(500);
	      }

	      status = db_find_key(hDb, 0, str, &hKey);
	      if (status != DB_SUCCESS) {
	         printf("Cannot find statistics record \'%s\', error %d\n", str, status);
	         ss_sleep(500);
	      }

		status = 0;
		eqr = NULL;

			eq_scal = &beamline[0].scal;

		      eq_scal->total_events_sent = 0;
		      eq_scal->total_events_per_sec = 0;
		      eq_scal->livetime = 0;
		      eq_scal->total_events_sent2 = 0;
		      eq_scal->total_events_per_sec2 = 0;
		      eq_scal->livetime2 = 0;
		      eq_scal->livetime3 = 0;
		      eq_scal->livetimec = 0;
		      eq_scal->chann1 = 0;
		      eq_scal->chann2 = 0;
		      eq_scal->chann3 = 0;
		      eq_scal->chann4 = 0;
		      eq_scal->chann5 = 0;
		      eq_scal->chann6 = 0;
		      eq_scal->chann7 = 0;
		      eq_scal->chann8 = 0;

//#####################################################################################


//	db_find_key(hDb, 0, str, &hKey);
//	equipment[index].hkey_variables = hKey;

/*-------------

                ret_2495 = CAEN_PLU_OpenDevice(CAEN_PLU_CONNECT_VME_V2718, vme_base_address, 0, 0, &lBHandle);
                if (ret_2495 != CAEN_PLU_OK) {
                        printf("Error %d\n", ret_2495);
                        exit(0);
                }

        printf("Device connected\n");

        ret_2495 = CAEN_PLU_GetSerialNumber(lBHandle, sn, 100);
        if (ret_2495 != CAEN_PLU_OK) {
                printf("Error %d\n", ret_2495);
                exit(0);
        }
        printf("Device Serial Number: %s\n", sn);
        //! [Registers]

 unsigned int firmware;
        //CAENVME_ReadRegister(lBHandle, static_cast<CVRegisters>(0x32101038), &firmware);
        ret_2495 = CAEN_PLU_ReadReg(lBHandle, 0x100C, &firmware);
        printf("FW2495SC firmware revision %x\n", firmware);

 unsigned int userfirmware;
        //CAENVME_ReadRegister(lBHandle, static_cast<CVRegisters>(0x32101038), &firmware);
        ret_2495 = CAEN_PLU_ReadReg(lBHandle, 0x8200, &userfirmware);
        printf("Scaler firmware revision: %d.%d\n\n", (userfirmware >> 8) & 0xFF, userfirmware & 0xFF);

                //db_check(&des);
		// empty the FIFO in the main FPGA
		CAEN_PLU_WriteReg(lBHandle, MAIN_SOFT_RESET,0x1);
		// Reset the board
		CAEN_PLU_WriteReg(lBHandle, PLUSCALER_SHOT, PLUSCALER_SHOT_RESET);
		// program the scaler
		//PLUProgram(&des);
	unsigned int channelenable;

        channelenable = 31;
        ret_2495 = CAEN_PLU_WriteReg(lBHandle, 0x1028, channelenable);
        printf("FW2495SC ch of D slot ch 1 enable %u\n", channelenable);

							------------------*/
        /*     channelenable = 0;
	ret_2495 = CAEN_PLU_WriteReg(lBHandle, 0x102c, channelenable);
        printf("FW2495SC ch of E slot enable %u\n", channelenable);

             channelenable = 3;
	ret_2495 = CAEN_PLU_WriteReg(lBHandle, 0x1034, channelenable);
        printf("FW2495SC ch of F slot enable %u\n", channelenable);
		CAEN_PLU_WriteReg(lBHandle, PLUSCALER_SHOT, PLUSCALER_SHOT_CLEARDATA);*/
		//PLUWriteRegister(lBHandle, PLUSCALER_CTRL_BITSET, des.TriggerMode & 0x3);    //Enable Triggers

/*
 unsigned int controlregister = 1985;//1217;//1857;//[1: external trigger, 64: time tag, 128:enable channal mask, 256: counter upper 32 bit, 512: time upper bit, 1024: v2495 data format];
	ret_2495 = CAEN_PLU_WriteReg(lBHandle, 0x1000, controlregister);
        printf("FW2495SC control resgister at initial %x\n", controlregister);

	ret_2495 = CAEN_PLU_ReadReg(lBHandle, 0x1000, &controlregister);
        printf("FW2495SC control resgister at initial %x\n", controlregister);

unsigned int controlregisterbitset;
	ret_2495 = CAEN_PLU_ReadReg(lBHandle, 0x1004, &controlregisterbitset);
        printf("FW2495SC control resgisterbitset at initial %x\n", controlregisterbitset);

unsigned int controlregisterbitclear;
	ret_2495 = CAEN_PLU_ReadReg(lBHandle, 0x1008, &controlregisterbitclear);
        printf("FW2495SC control resgisterbitclear at initial %x\n", controlregisterbitclear);

        //ret_2495 = CAEN_PLU_SetGateAndDelayGenerator(lBHandle,10,1,1,1,1);
unsigned int GDGcontrolregister = 2;
	ret_2495 = CAEN_PLU_WriteReg(lBHandle, 0x1800, GDGcontrolregister);

	sleep(1);
*/
//########################	Passive record of new odb tree	######################################

	      /* open hot link to statistics tree */
    status = db_open_record(hDb, hKey, eq_scal, sizeof(BEAMLINE_STATS), MODE_WRITE, NULL, NULL);
      if (status == DB_NO_ACCESS) {
         /* record is probably still in exclusive access by dead FE, so reset it */
        status = db_set_mode(hDb, hKey, MODE_READ | MODE_WRITE | MODE_DELETE, TRUE);
         if (status != DB_SUCCESS)
            cm_msg(MERROR, "register_equipment", "Cannot change access mode for record \'%s\', error %d", str, status);
         else
            cm_msg(MINFO, "register_equipment", "Recovered access mode for record \'%s\'", str);
         status = db_open_record(hDb, hKey, eq_scal, sizeof(BEAMLINE_STATS), MODE_WRITE, NULL, NULL);
      }
      if (status != DB_SUCCESS) {
         cm_msg(MERROR, "register_equipment", "Cannot open statistics record, error %d. Probably other FE is using it", status);
         ss_sleep(3000);
      }


//########################################################################################################


	//stick = ss_millitime();

	return SUCCESS;
}

INT v2495_exit(int32_t lBHandle){

	free(fraction_count);
//	CAEN_PLU_CloseDevice(lBHandle);

		      eq_scal->total_events_sent = 0;
		      eq_scal->total_events_per_sec = 0;
		      eq_scal->livetime = 0;
		      eq_scal->total_events_sent2 = 0;
		      eq_scal->total_events_per_sec2 = 0;
		      eq_scal->livetime2 = 0;
		      eq_scal->livetime3 = 0;
		      eq_scal->livetimec = 0;
		      eq_scal->chann1 = 0;
		      eq_scal->chann2 = 0;
		      eq_scal->chann3 = 0;
		      eq_scal->chann4 = 0;
		      eq_scal->chann5 = 0;
		      eq_scal->chann6 = 0;
		      eq_scal->chann7 = 0;
		      eq_scal->chann8 = 0;		      

	//cvt_V1190_module_reset(&F3_PPAC_type);

	return SUCCESS;

}

INT v2495_begin(/*INT run_number, char *error, TRIGGER_SETTINGS *ts */){

	RUN_logger(1);


/*
        CAEN_PLU_ReadReg(llBHandle, 0x1200, &scalenumber); //D connector 0x1200-, ch:0 (A connector 0x1100-, )
        printf("FW2495SC scalenumber at trigger event %u\n", scalenumber);// /100000000

        //uint32_t scalenumber2;

        if (scalenumber<scalenumbertmp)scalenumber2++;
        scalenumbertmp = scalenumber;

        CAEN_PLU_ReadReg(llBHandle, 0x1204, &scalenumber3); //D connector 0x1200-, ch:1 (A connector 0x1100-, )
        printf("FW2495SC scalenumber3 at trigger event %u\n", scalenumber3);// /10000000

        CAEN_PLU_ReadReg(llBHandle, 0x1208, &scalenumber4); //D connector 0x1200-, ch:2 (A connector 0x1100-, )
        printf("FW2495SC scalenumber4 at trigger event %u\n", scalenumber4);// /100000

        printf("FW2495SC uppper digit at trigger event %u\n", scalenumber2);






	/*INT16 window_width=ts->f3_ppac_window_width/25;

	INT16 window_offset=0;
	if (ts->f3_ppac_window_offset>0 || ts->f3_ppac_window_offset==0){window_offset=ts->f3_ppac_window_offset/25;}
	if (ts->f3_ppac_window_offset<0){window_offset=4096-(ts->f3_ppac_window_offset/25)*-1;}

	INT16 extra_search_margin=ts->f3_ppac_extra_search_margin/25;
	INT16 reject_margin=ts->f3_ppac_reject_margin/25;
	INT blt_event_num=1;
	const UINT16 *p_enable_msk=NULL;*/

	//trigger matching mode operation

//	printf("Setting_trigger_acquisition mode ...");


	/*if(! cvt_V1190_set_trigger_matching_acquisition_mode(&F3_PPAC_type, window_width, window_offset,
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

	f3_pp_f_time=0;
	f3_pp_time_tag=0;
*/
	return SUCCESS;

}


INT v2495_end(){

	RUN_logger(2);

	fclose(flog);

	return SUCCESS;

}

INT v2495_check_fifo(int32_t lBHandle){

	uint32_t addr;
	int16_t output_buffer_event;

//	addr = (SCALER_ADDR<<16)|CVT_V1190_EVENT_STORED_ADD;
//	CAENVME_ReadCycle(lBHandle, addr, &output_buffer_event, cvA32_U_BLT,cvD32);//static_cast<CVAddressModifier>(0x2f), static_cast<CVDataWidth>(0x20));

	return output_buffer_event;

}



INT v2495_read_fifo(int32_t lBHandle, void *buff_tmp, int size){

	int count;
	uint32_t addr;

//	addr = (SCALER_ADDR<<16)|CVT_V1190_OUT_BUFFER_ADD;

//	CAENVME_BLTReadCycle(lBHandle, addr, (char*)buff_tmp, size, cvA32_U_BLT, cvD32, &count);


//      cvA32_S_BLT     = 0x0F,         /* A32 supervisory block transfer               */
//      cvA32_S_PGM     = 0x0E,         /* A32 supervisory program access               */
//      cvA32_S_DATA    = 0x0D,         /* A32 supervisory data access                  */
//	cvA32_U_BLT     = 0x0B,         /* A32 non-privileged block transfer            */
//      cvA32_U_PGM     = 0x0A,         /* A32 non-privileged program access            */
//	cvA32_U_DATA    = 0x09,         /* A32 non-privileged data access               */


	return count;
}

/* ---------------------------------------------------------------------------
   Read one block of data
   ------------------------------------------------------------------------- */

INT ReadBlock(int32_t lBHandle, uint32_t *buffer, int MaxSize, int *nw){

/*
	int ret = 0, pnt = 0, bltnw;
	uint32_t  nw_read;

	*nw = 0;

	// make a loop of MBLT cycles until the board asserts the Bus Error to signal the end of event(s)
	// (or until the buffer size has been exceeded)
	while (pnt < MaxSize) {
		bltnw = (MAX_BLT_SIZE / 4);
		if ((pnt + bltnw) > MaxSize)  // buffer size exceeded
			bltnw = MaxSize - pnt;
		if (bltnw & 1) // if the number of words is odd, read one more to be aligned to 64 bit
			bltnw++;
		//ret = CAENComm_BLTRead(des.handle, 0, , bltnw, &nb); // TODO rimuovere
		ret = CAEN_PLU_ReadFIFO32(lBHandle, 0, bltnw, (uint32_t *)(buffer + pnt), &nw_read);

		pnt += nw_read;
		if (ret == CAEN_PLU_TERMINATED)
			break;
		else if (ret != CAEN_PLU_OK) {
			return -1;
		}
	}

	*nw = pnt;

*/
	return 0;
}

/* ---------------------------------------------------------------------------
   Read one or more events from the PLUscaler
   ------------------------------------------------------------------------- */
INT ReadEvents(int32_t lBHandle, uint32_t *buffer, int *nw){

    int i, ret = 0;
	uint32_t StatusId,StatusReg, header;
	uint32_t  EventSize;
	int lVMEhandle;
/*
    *nw = 0;



	// Wait until there are data available in the queue in case of socket connection (ethernet)
	// PLUReadRegister(des, 0, &header) will otherwise return wrong data if no events have arrived yet.
	// In case of other links,  PLUReadRegister(des, 0, &header) itself will return error in case of no data available.


	// read the event header from output FIFO using a single read cycle
	ret = CAEN_PLU_ReadReg(lBHandle, 0, &header);
 	//printf("header : %u\n", header);

	if (ret != 0) {
		*nw = 0;
		return 0;
	}
	if (((header >> 30) & 0x01)==0) {
		//printf("V1495 data format : %08X\n", header);
		//printf("Event count %d\n", header>>8 & 0xFFFF);
		EventSize = header & 0xFF;
	}	else {
		//printf("PLU data format: %08X\n", header);
		//printf("Event count %d\n", header>>12 & 0x3FFFF);
		EventSize = header & 0xFFF;
	}
	buffer[0] = header;

	// read the rest of the event with an exact size Block Transfer
	ret = ReadBlock(lBHandle, buffer + 1, EventSize - 1, nw);


	(*nw)++; // the function must return the event words, but the header is not counted yet
*/

    return ret;
}

bool chan_enabled(int i, int chgroup){

/*	if (i > 160) {
		printf("index exceeded the available channels\n");
		return false;
	}
	else {
		return (chgroup >> (i % 32) & 0x1) ? true : false;
	}*/
}

INT v2495_read_event(int32_t lBHandle, const char *bank_name, char *pevent, INT off, uint32_t *buff, int buff_size, uint32_t *pdata, int bkm){


	INT si0 =4;
	INT si1 = 0;
	int kx;
//	CAEN_PLU_ERROR_CODE ret_2495;



	//#########		v2495 buffer initialization when target ODB value = 1 (triggered by a button in custompage)	###############

	db_find_key(hDb,0, "/Custom/Scaler/logi1", &hKey1);
	kx = db_get_value(hDb, 0, "/Custom/Scaler/logi1", &si1, &si0, TID_INT, 0);

	if(si1 == 1){//printf( "logi value = %d\n",kx);


 	 //	char       sn1[100];
 	 //	char *vme_base_address2 = "32100000";

		// empty the FIFO in the main FPGA
		//CAEN_PLU_WriteReg(lBHandle, MAIN_SOFT_RESET,0x1);
		/* Reset the board */
	//	CAEN_PLU_WriteReg(lBHandle, PLUSCALER_SHOT, PLUSCALER_SHOT_RESET);
		/* program the scaler */

		si1 = 0;
		kx = db_set_value(hDb, 0, "/Custom/Scaler/logi1", &si1, sizeof(si1), 1, TID_INT);

	//	clear_buffer(lBHandle, ret_2495);

		/*kx = db_set_value(hDb, 0, "/Custom/Scaler/logi1", &si1, sizeof(si1), 1, TID_INT);

		unsigned int channelenable;
		        channelenable = 31;
 	       ret_2495 = CAEN_PLU_WriteReg(lBHandle, 0x1028, channelenable);

 		unsigned int controlregister = 1985;
		ret_2495 = CAEN_PLU_WriteReg(lBHandle, 0x1000, controlregister);
        	printf("FW2495SC control resgister at initial %x\n", controlregister);

		TT100_tmp = 0;
		Counters_recount = 0;*/

	}

	//##########################################################################################################################################

	//int wcount;
	//CAENVME_BLTReadWait(lBHandle, &wcount);
//	if(OR_COUNT >= 1 && ini_def == 0 ){ clear_buffer(lBHandle, ret_2495); ini_def = 1;}

	int i=0;
	int ret;
	uint32_t EventSize;

	int pnt, nw = 0;
	int idx;

	ReadEvents(lBHandle, buff, &nw);
	bk_create(pevent, bank_name, TID_DWORD, (void**)&pdata);
	bkm = 0;

	int Time_Tag, mask_en, cnt64_en, time64_en, ports;

	uint32_t Counters[160], Counters64[160];
	int ChEnable[5];


	//float ltmp1_u, ltmp1_d;



	for (i = 0; i < 160; i++) Counters[i] = Counters64[i] = 0.;

/* analyze and save event data */
	pnt = 0;

	while (pnt < nw) {

		if (((buff[pnt] >> 30) & 0x01)==0) {

			EventSize = buff[pnt++] & 0xFF;

		}	else {

			EventSize = buff[pnt++] & 0xFFF;

				Time_Tag = (buff[pnt] & 0x1);
				mask_en = ((buff[pnt] >> 1) & 0x1);
				cnt64_en = ((buff[pnt] >> 2) & 0x1);
			        time64_en = ((buff[pnt] >> 3) & 0x1);
				ports = ((buff[pnt++] >> 4) & 0x1F);
				for (i = 0; i < 5; i++) {
				}
		if (Time_Tag) {
				TimeTag = buff[pnt++];
				if (time64_en) {
					//printf("Time tag (32 MSBs): %lu\n", buff[pnt]);
					TimeTag = TimeTag + ((uint64_t)buff[pnt++] << 32);
				}
			}
		}
		if (!mask_en) {
				return -1;
			}
			// fill counters
			for (i = 0; i < 160; i++) {
				if ((ports >> (i / 32)) & 0x1) {
					if (i % 32 == 0) ChEnable[i / 32] = buff[pnt++]; //read from packet only in PLU mode
					if (chan_enabled(i,ChEnable[i / 32])) {
						Counters[i] = buff[pnt++];
						if (cnt64_en) Counters64[i] = buff[pnt++];
					} else {
					}
				}
			}

	jj++;
	TT100 = Counters[64] + Counters64[64]*pow(2,32);
	GCOUNT = Counters[68] + Counters64[68]*pow(2,32);

	beamline_triggered = Counters[65] + Counters64[65]*pow(2,32);
        u_detector_triggered = Counters[66] + Counters64[66]*pow(2,32);

	pcount = Counters[64];
	pcount64 = Counters64[64];


		// ################# Record Scaler values on  custom ODB ####################

				eq_scal=&beamline[0].scal;
				eq_scal->total_events_sent = Counters[65];
				eq_scal->total_events_per_sec = Counters_recount;	// event rate for the Beamline trigger
				eq_scal->livetime = ltmp1;
				eq_scal->total_events_sent2 = TT100;
				eq_scal->total_events_per_sec2 = Counters_recount2; // event rate for the Down Scaled,  Beamline trigger
				eq_scal->livetime2 = ltmp2;
				eq_scal->livetime3 = ltmp3;
				eq_scal->livetimec = COR_LT*100.;//(double)(COR_LT_NO/COR_LT_DE*100.);

			      eq_scal->chann1 = TT100;
			      eq_scal->chann2 = beamline_triggered;
			      eq_scal->chann3 = u_detector_triggered;
			      eq_scal->chann4 = Counters[67] + Counters64[67]*pow(2,32);
			      eq_scal->chann5 = GCOUNT;
			      eq_scal->chann6 = Counters[69] + Counters64[69]*pow(2,32);
			      eq_scal->chann7 = Counters[70] + Counters64[70]*pow(2,32);
			      eq_scal->chann8 = Counters[71] + Counters64[71]*pow(2,32);


		*pdata++=pcount;			// 100 MHZ pulse source timetag
		*pdata++=pcount64;



		//##############################################################################


//###################		testing new satus monitor algorithm #####################################################################
//#																	#

	if((TT100 - tag_time2)/1.e+8 > 1.){

		fraction_count[acount] = OR_COUNT; acount++;
		fraction_count[acount] = Counters[68]; acount++;
		fraction_count[acount] = silicon_ary_event; acount++;
		fraction_count[acount] = Counters[66]; acount++;
		fraction_count[acount] = f3ppac_event; acount++;
		fraction_count[acount] = Counters[67]; acount++;
		fraction_count[acount] = DCCOUNT; acount++;
		fraction_count[acount] = D1COUNT; acount++;
		fraction_count[acount] = Counters[65]; acount++;

		fraction_count[acount] = Counters[64]; acount++;

		if (acount == NUMB_STAT*(SCAN_INTV/PERI_INTV)) for(int k = 0 ; k < NUMB_STAT; k++ ) fraction_count[acount + k] = fraction_count[k];

		ltmp1 = /*ltmp1_u/ltmp1_d;*/ /*static_cast<float>*/ ((float)(fraction_count[acount-NUMB_STAT]-fraction_count[acount])/(float)(fraction_count[acount-NUMB_STAT + 1]-fraction_count[acount + 1]));
		if(ltmp1 < 0 ) ltmp1 = -1;
		if(fraction_count[acount-NUMB_STAT + 1]-fraction_count[acount + 1] == 0 ) ltmp1 = -2;

		ltmp2 =((float)(fraction_count[acount-NUMB_STAT + 2]-fraction_count[acount + 2])/(float)(fraction_count[acount-NUMB_STAT + 3]-fraction_count[acount + 3]));
		if(ltmp2 < 0 ) ltmp2 = -2;
		if(fraction_count[acount-NUMB_STAT + 3]-fraction_count[acount + 3] == 0 ) ltmp2 = -2;

		ltmp3 =((float)(fraction_count[acount-NUMB_STAT + 4]-fraction_count[acount + 4])/(float)(fraction_count[acount-NUMB_STAT + 5]-fraction_count[acount + 5]));
		if(ltmp3 < 0 ) ltmp3 = -2;
		if(fraction_count[acount-NUMB_STAT + 5]-fraction_count[acount + 5] == 0 ) ltmp3 = -2;

		COR_LT =((float)(fraction_count[acount-NUMB_STAT + 6]-fraction_count[acount + 6])/(float)(fraction_count[acount-NUMB_STAT + 7]-fraction_count[acount + 7]));

		Counters_recount = ((float)(fraction_count[acount-NUMB_STAT + 8]-fraction_count[acount + 8])/(float)(fraction_count[acount - NUMB_STAT + 9]-fraction_count[acount + 9])*1.e+8);
		Counters_recount2 = ((float)(fraction_count[acount-NUMB_STAT + 5]-fraction_count[acount + 5])/(float)(fraction_count[acount - 1]-fraction_count[acount + NUMB_STAT - 1])*1.e+8);

		erate = ((float)(fraction_count[acount-NUMB_STAT + 2]-fraction_count[acount + 2])/(float)(fraction_count[acount - 1]-fraction_count[acount + NUMB_STAT - 1])*1.e+8);
		erate_p = ((float)(fraction_count[acount-NUMB_STAT + 4]-fraction_count[acount + 4])/(float)(fraction_count[acount - 1]-fraction_count[acount + NUMB_STAT - 1])*1.e+8);

		//printf("buf1 = %d buf2 = %d each2:%f d time: %f \n",buff_size_intv,buff_size_intv_c,drate,sintv);
		if(acount == NUMB_STAT*(SCAN_INTV/PERI_INTV)) acount = 0;

	//printf("\n!!!@#$%@#$%@#$%@ fraction count test val1: %u val2: %u val3: %u val4: %u @#$%@#$%@!!!\n\n",fraction_count[0],fraction_count[1],fraction_count[2],fraction_count[3]);

			if(OR_COUNT > 8*pow(2,31)) OR_COUNT = 1;
			if(silicon_ary_event > 8*pow(2,31)) silicon_ary_event = 1;
			if(f1ppac_event > 8*pow(2,31)) f1ppac_event = 1;

		tag_time2 = TT100;

	}

//#																	#
//#######################################################################################################################################

		buff_size_intv_f24_c += 8;
		buff_size_intv_f24 += 4;


	}



	bk_close(pevent, pdata);


	return bk_size(pevent);

}

INT local_timestamp(int32_t lBHandle, uint32_t *buff, uint64_t lgcount, uint64_t lpcount, uint64_t lpcount64, int thandle){


	int i=0;
	uint32_t EventSize;
	uint32_t retv;

	int pnt, nw = 0;
	int idx;

	ReadEvents(lBHandle, buff, &nw);

	int Time_Tag, mask_en, cnt64_en, time64_en, ports;

	uint32_t Counters[160], Counters64[160];
	int ChEnable[5];


		for (i = 0; i < 160; i++) Counters[i] = Counters64[i] = 0.;

	/// analyze and save event data
		pnt = 0;

		while (pnt < nw) {

			if (((buff[pnt] >> 30) & 0x01)==0) {

				EventSize = buff[pnt++] & 0xFF;

			}	else {

				EventSize = buff[pnt++] & 0xFFF;

					Time_Tag = (buff[pnt] & 0x1);
					mask_en = ((buff[pnt] >> 1) & 0x1);
					cnt64_en = ((buff[pnt] >> 2) & 0x1);
					time64_en = ((buff[pnt] >> 3) & 0x1);
					ports = ((buff[pnt++] >> 4) & 0x1F);

			if (Time_Tag) {
					TimeTag = buff[pnt++];
					if (time64_en) {

						TimeTag = TimeTag + ((uint64_t)buff[pnt++] << 32);
					}
				}
			}
			if (!mask_en) {
					return -1;
				}
				// fill counters
				for (i = 0; i < 160; i++) {
					if ((ports >> (i / 32)) & 0x1) {
						if (i % 32 == 0) ChEnable[i / 32] = buff[pnt++]; //read from packet only in PLU mode
						if (chan_enabled(i,ChEnable[i / 32])) {
							Counters[i] = buff[pnt++];
							if (cnt64_en) Counters64[i] = buff[pnt++];
						} else {
						}
					}
				}
			}

		if(thandle == 1) return Counters[68] + Counters64[68]*pow(2,32);
		else return Counters[64];


}




