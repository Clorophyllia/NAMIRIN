//#######################################################################################################################################################################
/************************************************************************************************************************************************************************

 Virtual events generator for the DAQ' online PID SW test and evaluation.

It would create pseudo date set and would replace pdata in the frontend layer.  (e.g. which means it will be interact with ODB in MIDAS DAQ)

Data have been gathered from LISE++ simulation.
Current version is using a Normal distribution approximztion about distributions of ADC and TDC values.

************************************************************************************************************************************************************************/
//#######################################################################################################################################################################


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

#include "midas.h"
#include "odb_trigger.h"
#include "detector_conf.h"
#include "virtual_gaussian.h"
#include "focal_cond.h"

#include "mfe_PID.h"
#include "mfe.h"
#include "msystem.h"
#include "msystem.h"


#define BL_FORBID 1.e+7
#define PERI_INTV 1000
#define SCAN_INTV 5000
#define NUMB_STAT 10
#define Pi		3.141592

#define TDC_VIR_DELAY	100.	//	TDC counting -> measurment	[ns]
#define ADC_VIR_PEDST	0.125	//	Silicon pedestal	[MeV]
#define ADC_CONV_GRAD	41.667
//#define FOD		40.257	// dp/p [%] - x [mm]
#define TOF_X_COEF	-0.01113
#define SYS_BARRIER_L	-181.	//	[mm] Specific shortest TOF boundary condition. Would be cut out forbidden values on the local Cal. function
#define SYS_BARRIER_H	187.5
#define INTENSITY_NORM	7.94692
#define PPAC_ANODE	500
#define PPAC_CATHODE	500
#define PPAC_CONV_GRAD	0.01522
#define PPAC_TDC_SUM	7000

#define brho_ref	1.9709

int seed;
int VEG_TAG;

uint32_t vir_ev_count;
uint32_t vir_tdc_value[13];
uint32_t vir_adc_value[32];
uint32_t vir_tdc_tof[2];
int vir_tdc_ch;

double elem_prop[15] = {0};
GAUSS_ISOTOPE sort_isotopes[15];



//------------------------------	model variables		-----------------------------------------//
//@													@//
/*
extern GAUSS_ISOTOPE S38;
extern GAUSS_ISOTOPE S39;
extern GAUSS_ISOTOPE P35;
extern GAUSS_ISOTOPE P36;
extern GAUSS_ISOTOPE P37;
extern GAUSS_ISOTOPE P38;
extern GAUSS_ISOTOPE SI33;
extern GAUSS_ISOTOPE SI34;
extern GAUSS_ISOTOPE SI35;
*/


//@													@//
//-------------------------------------------------------------------------------------------------------//





//------------------------------	local funtions for mathmatical Cal.	---------------------------------------------//

double gauss_engine(double sigma, double mu){


        double pt, po, P1;

	while(1){	// hard endge cut of a 2.4 sigma window

	        pt = (double)drand48();
	        po = (double)drand48();

	        P1 = (double)sqrt(-2*log(1-pt))*cos(2*Pi*(1-po));
		if(fabs(P1) > /*0.0223945*/0.0022/sigma) break;

	}

        return P1*sigma + mu;

}


/*double ambi_engine(double *f(double), double min, double max){	// General distribution generator; This may have a low precision (relatively), and a low computing efficiency.

	srand48((unsigned int)seed);

	double x, x_uni, x_intv, xi, norm;
	double fmax =  0.;

	x = (double)drand48();
	x_uni = (max - min)*x + min;

	x_intv = (max - min)/20000.;

	for(xi = min; xi <= max; xi += x_intv) if(fmax < f(xi)) fmax = f(xi);
	norm = 1./fmax;

	return x_uni*(1. - norm*f(x_uni));

}*/


double br_tof_cor(double ti, double tf, double intercept, double div){	// tof start; tof end; conversion factor; deviation of tof;
									// set brho value based on the tof correlation
	return gauss_engine(div ,(tf - ti))*TOF_X_COEF + intercept;

}

double br_to_tdc(double ti, double tf, double intercept, double div){		// doesn't give a damn about a y-axis :P
				// ignore calibration & segment edge correction on this test code

	int i;

	double brho_c = br_tof_cor(ti, tf, intercept,div);		// [T * m]
	double podp = (brho_c - brho_ref)/brho_ref*100.;		// [%]

	double xpos = podp*FOD;		// [mm]


	for(i = 0; i < 12; i++) vir_tdc_value[i] = 0;

	vir_tdc_value[12] = PPAC_ANODE;

	if(xpos < -100){

		vir_tdc_value[0] = (xpos + 150.)/PPAC_CONV_GRAD*0.5 + PPAC_TDC_SUM*0.5;
		vir_tdc_value[1] = PPAC_TDC_SUM - vir_tdc_value[0];

	}

	if(xpos >= -100 && xpos < 0 ){

                vir_tdc_value[2] = (xpos + 50.)/PPAC_CONV_GRAD*0.5 + PPAC_TDC_SUM*0.5;
                vir_tdc_value[3] = PPAC_TDC_SUM - vir_tdc_value[2];

		//printf("F1_CH3 Value: %u  F1_CH4 Value: %u \n",vir_tdc_value[2],vir_tdc_value[3]);
        }

        if(xpos >= 0 && xpos < 100 ){

                vir_tdc_value[4] = (xpos - 50.)/PPAC_CONV_GRAD*0.5 + PPAC_TDC_SUM*0.5;
                vir_tdc_value[5] = PPAC_TDC_SUM - vir_tdc_value[4];

                //printf("F1_CH5 Value: %u  F1_CH6 Value: %u \n",vir_tdc_value[4],vir_tdc_value[5]);

        }

        if(xpos > 100){

                vir_tdc_value[6] = (xpos - 150.)/PPAC_CONV_GRAD*0.5 + PPAC_TDC_SUM*0.5;
                vir_tdc_value[7] = PPAC_TDC_SUM - vir_tdc_value[6];

        }

	//if(fabs(xpos) > 200) return 1;					// reject the event?
	//else return 0;
		return xpos;

}


double de_to_adc(double de){		//	energy loss in MeV

	int i;
	double ch_sel;

	ch_sel = 16.*drand48();
	vir_tdc_ch = (int)ch_sel;
	if(vir_tdc_ch == 16) ch_sel  = 16.*drand48();
	vir_tdc_ch = (int)ch_sel;


	for(i = 0 ; i < 32; i++) vir_adc_value[i] =  (uint32_t)(ADC_VIR_PEDST*ADC_CONV_GRAD);

	vir_adc_value[vir_tdc_ch] = (uint32_t)((ADC_VIR_PEDST + de)*ADC_CONV_GRAD);

	printf("generated adc: %u,  channel: %d \n",vir_adc_value[vir_tdc_ch], vir_tdc_ch);

	return vir_adc_value[vir_tdc_ch];
}



bool accept_cut(double pos){

	if( pos < SYS_BARRIER_L || pos > SYS_BARRIER_H) return 1;	//	if a generated event has a forbiden value, reject the event.
	else return 0;

}

//----------------------------------------------------------------------------------------------------------------------------//


INT veg_init(){

        srand48((unsigned int)seed);
        int i;

	vir_ev_count = 0;

	sort_isotopes[0] = S37;
	sort_isotopes[1] = S38;
	sort_isotopes[2] = S39;
	sort_isotopes[3] = P35;
	sort_isotopes[4] = P36;
	sort_isotopes[5] = P37;
	sort_isotopes[6] = P38;
	sort_isotopes[7] = SI33;
	sort_isotopes[8] = SI34;
	sort_isotopes[9] = SI35;

	for(i = 0; i <= 9; i++) elem_prop[i] += S37.inten;
	for(i = 1; i <= 9; i++) elem_prop[i] += S38.inten;
	for(i = 2; i <= 9; i++) elem_prop[i] += S39.inten;
	for(i = 3; i <= 9; i++) elem_prop[i] += P35.inten;
	for(i = 4; i <= 9; i++) elem_prop[i] += P36.inten;
	for(i = 5; i <= 9; i++) elem_prop[i] += P37.inten;
	for(i = 6; i <= 9; i++) elem_prop[i] += P38.inten;
	for(i = 7; i <= 9; i++) elem_prop[i] += SI33.inten;
	for(i = 8; i <= 9; i++) elem_prop[i] += SI34.inten;
	elem_prop[9] += SI35.inten;

	VEG_TAG = 0;
	//for(i = 0; i < 10; i++) printf("Elements of an elem array: %lf \n", elem_prop[i]);

	return SUCCESS;
}

INT veg_exit(){



	return SUCCESS;

}

INT veg_begin(){


	return SUCCESS;

}


INT veg_end(){


	return SUCCESS;

}

INT veg_check_fifo(int32_t lBHandle){

	uint32_t addr;
	int16_t output_buffer_event;

	return output_buffer_event;

}



INT veg_read_fifo(int32_t lBHandle, void *buff_tmp, int size){

	int count;
	uint32_t addr;


	return count;
}

/* ---------------------------------------------------------------------------
   Read one block of data
   ------------------------------------------------------------------------- */

INT veg_ReadBlock(int32_t lBHandle, uint32_t *buffer, int MaxSize, int *nw){

	return 0;
}

/* ---------------------------------------------------------------------------
   Read one or more events from the PLUscaler
   ------------------------------------------------------------------------- */
INT veg_ReadEvents(int32_t lBHandle, uint32_t *buffer, int *nw){

    return 0;
}


INT veg_create_event(){


	int i;
	int selem = -1;
        double elem_select;
	double t_start, t_end, e_loss;
	double s_brho,adc_tmp;

	//##########		Virtual isotope selection	#################//

	if(VEG_TAG == 0 || VEG_TAG > 4){

		elem_select = INTENSITY_NORM*drand48();

			 if(elem_select < elem_prop[0]) selem = 0;
			 for(i = 1; i < 9; i++) if(elem_select <= elem_prop[i] && elem_select > elem_prop[i-1]) selem = i;
			 if(selem == -1) selem = 9;

		GAUSS_ISOTOPE *SELI = &sort_isotopes[selem];
		printf("ISOTOPES SELECTION : #%d,  RV: %lf \n",selem,elem_select);

		//###########           Virtual Timing generation       #################//

		 t_start = gauss_engine(SELI->tof_sigm/sqrt(2), TDC_VIR_DELAY);
		 t_end = gauss_engine(SELI->tof_sigm/sqrt(2), TDC_VIR_DELAY + SELI->tof_mean);



		//###########           Virtual ADC generation          #################//

		 e_loss = gauss_engine(SELI->de_sigm, SELI->de_mean);	// any problem on offset..?



		s_brho = br_tof_cor(t_start, t_end, SELI->br_intc, SELI->br_intv);
		double xp = br_to_tdc(t_start, t_end, SELI->br_intc, SELI->br_intv);			//	create corresponding F1_PPAC data set

		adc_tmp = de_to_adc(e_loss);								//	create corresponding F3_Silicon data set

		vir_tdc_tof[0] = (int32_t)(t_start/0.025);	vir_tdc_tof[1] = (int32_t)(t_end/0.025);				// 	create corresponding F2_PPAC and F3_Plastic data set
		//(Probably;) Need to add gain Ch. specification function on the conversion fth

		printf("t0: %lf,  t1: %lf,  E: %lf,  ADC: %lf,  brho: %lf,  pos: %lf\n", t_start, t_end, e_loss, adc_tmp, s_brho, xp);
		//for(i = 0; i < 8 ; i++) if(vir_tdc_value[i] != 0 ) printf("F1_CH: %d, Value: %lf  ",i,vir_tdc_value[i]);
		printf("\n");

		vir_ev_count ++;

		VEG_TAG = 1;
	}
//-------------------------------------------------------------------------------------------------------------------------//

	return 0;// bk_size(pevent);

}



