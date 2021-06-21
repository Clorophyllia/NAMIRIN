#include <iostream>
#include <cstring>
#include <cstdio>
#include <cstdlib>

//#include <stdio.h>
//#include <stdlib.h>
//#include <math.h>
#include <assert.h>

#include <inttypes.h>
#include "midas.h"

//#include "experim.h"
#include "msystem.h"
#include "mfe.h"

#include <time.h>
#include <math.h>
//#include <string.h>
#include <unistd.h>

#include <vector>
#include <memory.h>
#include <ctype.h>

//#include "CAENVMElib.h"
//#include "CAENVMEtypes.h"

#include "odb_trigger.h"
#include "detector_conf.h"
#include "mfe_TDC.h"
#include "mfe_ADC.h"
#include "mfe_PID.h"
#include "mfe_common.h"
//#include "CAEN_PLU.h"
//#include "PLUscalerLib.h"


static HNDLE hDB_obd;
static TRIGGER_SETTINGS tr_set;

uint32_t time_stamp1, time_stamp2;

unsigned int scaler_high;
unsigned int scaler_low;
unsigned int BOP;
bool BOF = 0;	// Simulaeous module activation 

uint32_t s_evt;

/*-- Globals -------------------------------------------------------*/

/* The frontend name (client name) as seen by other MIDAS clients   */
const char *frontend_name = "frontend";

/* The frontend file name, don't change it */
const char *frontend_file_name = __FILE__;

/* frontend_loop is called periodically if this variable is TRUE    */
BOOL frontend_call_loop = FALSE;

/* a frontend status page is displayed with this frequency in ms */
INT display_period = 0;

/* maximum event size produced by this frontend */
INT max_event_size = 4*1024*1024;

/* maximum event size for fragmented events (EQ_FRAGMENTED) */
INT max_event_size_frag = 5 * 1024 * 1024;

/* buffer size to hold events */
INT event_buffer_size = 32 * 1024*1024;

/*-- Function declarations mfe_tigress.o -----------------------------------------*/

INT frontend_init();
INT frontend_exit();
INT begin_of_run(INT run_number, char *error);
INT end_of_run(INT run_number, char *error);
INT pause_run(INT run_number, char *error);
INT resume_run(INT run_number, char *error);
INT frontend_loop();

INT read_trigger_event(char *pevent, INT off);

INT poll_event(INT source, INT count, BOOL test);
INT interrupt_configure(INT cmd, INT source, POINTER_T adr);

/*-- Equipment list ------------------------------------------------*/

BOOL equipment_common_overwrite = TRUE;

EQUIPMENT equipment[] = {

  {"Trigger1 (Beamline)",            /* equipment name  : DAQ Trigger */
   {1, 0,                   /* event ID, trigger mask */
    "SYSTEM",               /* event buffer */
    EQ_POLLED,
    0,                       /* event source crate 0, all stations */
    "MIDAS",                /* format */
    TRUE,                   /* enabled */
    RO_RUNNING,             /* read only when running */
    100,
    0,                      /* stop run after this event limit */
    0,                      /* number of sub events */
    0,                      /* don't log history */
    "", "", "",},
	read_trigger_event,      /* readout routine */
  },

   {""}
};


/*-- Frontend Init -------------------------------------------------*/

static int32_t BHandle;
static int32_t BHandle2;


const unsigned int DATA_BUFF_SIZE=4*1024*1024;

uint32_t *f1_ppac_buff=NULL;
uint32_t *f1_plastic_buff=NULL;

uint32_t *f2_ppac_buff=NULL;

uint32_t *f3_silicon_buff=NULL;
uint32_t *f3_plastic_buff=NULL;
uint32_t *f3_ppac_buff=NULL;
uint32_t *silicon_arya_buff=NULL;
uint32_t *silicon_aryb_buff=NULL;
uint32_t *v2495_buff=NULL;

uint32_t OR_COUNT = 0; 


INT frontend_init(){


  INT ret;

  // Set ODB
//  ret = set_odb(&hDB_obd, equipment[0].name, ts, "kobra_detectors", &tr_set, sizeof(TRIGGER_SETTINGS));
//  if ( ret != SUCCESS ) return ret;


  //VME initialization
 // v2495_init(BHandle); // v2495 intialization initialize v2718 together

	//CAENVME_ReadRegister(BHandle, static_cast<CVRegisters>(0x36), &Rotaryswitch);
	//printf("1st Controller rotary switch:%i\n", Rotaryswitch);


        short Link=0;
  	short Device=1;
   //     CVBoardTypes VMEBoard = cvV2718;

     //   printf("\n1st V2718 initialization...");


  	//Make sure V2718 opened OK!

  /*	if(CAENVME_Init(VMEBoard, Link, Device, &BHandle2)!=cvSuccess){
  		cm_msg(MERROR, "kobra_detector", "Failed to Open CAEN V2718_board1");
  		return FE_ERR_HW;
  	}
  	printf("Done\n");

	unsigned int Rotaryswitch;

	CAENVME_ReadRegister(BHandle, static_cast<CVRegisters>(0x36), &Rotaryswitch);
	printf("1st Controller rotary switch:%i\n", Rotaryswitch);
*/
	char str0[256];
	char str00[128];


#ifdef USE_PPAC_F1
  	f1_ppac_init(BHandle);
#endif

#ifdef USE_PPAC_F2
  	f2_ppac_init(BHandle);
#endif

#ifdef USE_PPAC_F3
  	f3_ppac_init(BHandle);
#endif

#ifdef USE_SILICON_F3
  	f3_silicon_init(BHandle, &tr_set);
#endif
  	//2nd controller initialization
  /*	printf("\n2nd V2718 initialization...");

  	printf("Done\n");

	CAENVME_ReadRegister(BHandle2, static_cast<CVRegisters>(0x36), &Rotaryswitch);
	printf("2nd Controller rotary switch:%i\n", Rotaryswitch);
*/
#ifdef USE_SILICON_ARYA
  	silicon_arya_init(BHandle2, &tr_set);
#endif

#ifdef USE_SILICON_ARYB
  	silicon_aryb_init(BHandle2, &tr_set);
#endif

#ifdef USE_PID
	veg_init();
#endif

//#######
//	Creat Link for the KOBRA DAQ STATUS PAGE

 	sprintf(str00,"/home/ldy/%s","online/pid_str2/custom/custom.html");
	sprintf(str0, "/Custom/%s", "KoBRA Beamline Status&");

	db_set_value(1, 0, str0, str00, 128, 1, TID_STRING);

//###### FINAL LINK


	printf("\n==========  Enjoy!\n");

  return SUCCESS;
}






/*-- Frontend Exit -------------------------------------------------*/

INT frontend_exit() {

#ifdef USE_V2495
	v2495_exit(BHandle);
	free(v2495_buff);
#endif

#ifdef USE_PPAC_F1
	f1_ppac_exit();
	free(f1_ppac_buff);
#endif

#ifdef USE_PLASTIC_F1
	f1_plastic_exit();
	free(f1_plastic_buff);
#endif

#ifdef USE_PPAC_F2
	f2_ppac_exit();
	free(f2_ppac_buff);
#endif

#ifdef USE_SILICON_F3
	f3_silicon_exit(BHandle);
	free(f3_silicon_buff);
#endif

#ifdef USE_PLASTIC_F3
	f3_plastic_exit(BHandle);
	free(f3_plastic_buff);
#endif

#ifdef USE_PPAC_F3
	f3_ppac_exit();
	free(f3_ppac_buff);
#endif

#ifdef USE_SILICON_ARYA
	silicon_arya_exit(BHandle2);
	free(silicon_arya_buff);
#endif

#ifdef USE_SILICON_ARYB
	silicon_aryb_exit(BHandle2);
	free(silicon_aryb_buff);
#endif

	return SUCCESS;
}

/*-- Begin of Run --------------------------------------------------*/

INT begin_of_run(INT run_number, char *error){


#ifdef USE_V2495
	v2495_buff = static_cast<uint32_t*>( malloc(DATA_BUFF_SIZE));
#endif

#ifdef USE_PPAC_F1
	f1_ppac_buff= static_cast<uint32_t*>( malloc(DATA_BUFF_SIZE));
	f1_ppac_begin(run_number, error, &tr_set);
#endif

#ifdef USE_PPAC_F2
	f2_ppac_buff= static_cast<uint32_t*>( malloc(DATA_BUFF_SIZE));
	f2_ppac_begin(run_number, error, &tr_set);
#endif

#ifdef USE_SILICON_F3
	f3_silicon_buff= static_cast<uint32_t*>(malloc(DATA_BUFF_SIZE));
	f3_silicon_begin(BHandle, run_number, error, &tr_set);
#endif

#ifdef USE_PPAC_F3
	f3_ppac_buff= static_cast<uint32_t*>( malloc(DATA_BUFF_SIZE));
	f3_ppac_begin(run_number, error, &tr_set);
#endif

#ifdef USE_SILICON_ARYA
	silicon_arya_buff= static_cast<uint32_t*>(malloc(DATA_BUFF_SIZE));
	silicon_arya_begin(BHandle2, run_number, error, &tr_set);
#endif

#ifdef USE_SILICON_ARYB
	silicon_aryb_buff= static_cast<uint32_t*>(malloc(DATA_BUFF_SIZE));
	silicon_aryb_begin(BHandle2, run_number, error, &tr_set);
#endif

#ifdef USE_V2495
	v2495_begin();
#endif



  return SUCCESS;
}

/*-- End of Run ----------------------------------------------------*/

INT end_of_run(INT run_number, char *error) {

#ifdef USE_PPAC_F1
	f1_ppac_end(run_number, error);
	free(f1_ppac_buff);
#endif


#ifdef USE_PPAC_F2
	f2_ppac_end(run_number, error);
	free(f2_ppac_buff);
#endif

#ifdef USE_SILICON_F3
	f3_silicon_end(BHandle, run_number, error);
	free(f3_silicon_buff);
#endif


#ifdef USE_PPAC_F3
	f3_ppac_end(run_number, error);
	free(f3_ppac_buff);
#endif

#ifdef USE_SILICON_ARYA
	silicon_arya_end(BHandle2, run_number, error);
	free(silicon_arya_buff);
#endif

#ifdef USE_SILICON_ARYB
	silicon_aryb_end(BHandle2, run_number, error);
	free(silicon_aryb_buff);
#endif
/*
  CAEN_PLU_ERROR_CODE ret_2495;

                ret_2495 = CAEN_PLU_CloseDevice(BHandle);
                if (ret_2495 != CAEN_PLU_OK) {
                        printf("close Error %d\n", ret_2495);
                        exit(0);
                }

*/
	db_close_all_records ();

#ifdef USE_V2495
	v2495_end();
#endif

  printf("PLU stopped OK\n");
  free(v2495_buff);




  return SUCCESS;
}

/*-- Pause Run -----------------------------------------------------*/

INT pause_run(INT run_number, char *error) {

 // tigress_pause(run_number,error);

  return SUCCESS;
}

/*-- Resume Run ----------------------------------------------------*/

INT resume_run(INT run_number, char *error) {

 // tigress_resume(run_number,error);

  return SUCCESS;
}

/*-- Frontend Loop -------------------------------------------------*/

INT frontend_loop() {
  return SUCCESS;
}

/*-- Trigger event routines ----------------------------------------*/
//unsigned short tmp_Data = 0;
INT poll_event(INT source, INT count, BOOL test) {

	uint lam=0;


// 	int f3_ppac_fifo_check = f3_ppac_check_fifo(BHandle);
 //	int silicon_arya_fifo_check = silicon_arya_check_fifo(BHandle2);
//	lam = (f3_ppac_fifo_check || silicon_arya_fifo_check);
	lam = 1;

	if (lam>0) {/*OR_COUNT++;*/ return lam;}


	return 0;
}

/*-- Interrupt configuration ---------------------------------------*/

INT interrupt_configure(INT cmd, INT source, POINTER_T adr) {

  switch (cmd) {
  case CMD_INTERRUPT_ENABLE:
    break;
  case CMD_INTERRUPT_DISABLE:
    break;
  case CMD_INTERRUPT_ATTACH:
    break;
  case CMD_INTERRUPT_DETACH:
    break;
  }

  return SUCCESS;
}

/*-- Event readout -------------------------------------------------*/

INT read_trigger_event(char *pevent, INT off) {

	int bank_moni = 1;
	uint64_t OR_COUNT_tmp0;


	bk_init(pevent);

#ifdef USE_PID
	veg_create_event();
#endif

#ifdef USE_V2495
	uint32_t *v2495_data;
	v2495_read_event(BHandle, BANK_NAME_SCALER, pevent, off, v2495_buff, DATA_BUFF_SIZE, v2495_data, bank_moni);
#endif

#ifdef USE_PPAC_F1
	uint32_t *f1ppac_data;
	f1_ppac_read_event(BHandle, BANK_NAME_F1PPAC, pevent, off, f1_ppac_buff, DATA_BUFF_SIZE, f1ppac_data, bank_moni);
#endif

#ifdef USE_PPAC_F2
	uint32_t *f2ppac_data;
	f2_ppac_read_event(BHandle, BANK_NAME_F2PPAC, pevent, off, f2_ppac_buff, DATA_BUFF_SIZE, f2ppac_data, bank_moni);
#endif

#ifdef USE_SILICON_F3
	uint32_t *f3silicon_data;
	f3_silicon_read_event(BHandle, BANK_NAME_F3SILICON, pevent, off, f3_silicon_buff, DATA_BUFF_SIZE, f3silicon_data, bank_moni);	
#endif

#ifdef USE_PPAC_F3
	uint32_t *f3ppac_data;
	f3_ppac_read_event(BHandle, BANK_NAME_F3PPAC, pevent, off, f3_ppac_buff, DATA_BUFF_SIZE, f3ppac_data, bank_moni);
#endif

#ifdef USE_SILICON_ARYA
	uint32_t *siliconarya_data;
	silicon_arya_read_event(BHandle2, BHandle, BANK_NAME_U1SILLICON, pevent, off, silicon_arya_buff, DATA_BUFF_SIZE, siliconarya_data, bank_moni);
#endif

#ifdef USE_SILICON_ARYB
	uint32_t *siliconaryb_data;
	silicon_aryb_read_event(BHandle2, BHandle, BANK_NAME_U2SILLICON, pevent, off, silicon_aryb_buff, DATA_BUFF_SIZE, siliconaryb_data, bank_moni);
#endif

	if(bank_moni != 0) OR_COUNT++;

	ss_sleep(1);


	return bk_size(pevent);
}

