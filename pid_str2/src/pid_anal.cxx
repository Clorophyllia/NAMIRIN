//#######################################################################################################################################################################
/************************************************************************************************************************************************************************

 Virtual events generator for the DAQ' online PID SW test and evaluation.

It would create pseudo date set and would replace pdata in the frontend layer.  (e.g. which means it will be interact with ODB in MIDAS DAQ)

Data have been gathered from LISE++ simulation.
Current version is using a Normal distribution approximztion about distributions of ADC and TDC values.

************************************************************************************************************************************************************************/
//#######################################################################################################################################################################



/*-- Include files -------------------------------------------------*/

/* standard includes */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

/* midas includes */

#include <midas.h>

#include "detector_conf.h"
#include "analyzer.h"



//#include "particle_id_anal.h"
#include "pid_anal.h"

/* root includes */

#include <TStyle.h>
//#include <TCanvas.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TTree.h>
#include <TDirectory.h>


/*#######  root tvariables  #######*/


static TH2D *hDEVTOF;
static TH2D *hBRVTOF;
static TH2D *hAOQVZ;



/*-- Parameters ----------------------------------------------------*/

#define PI	3.141592
#define c	299792458
#define m_e	5.10998e+5  // in eV
#define m_u	9.3891868e+8
#define qe	1.602176e-19
#define eps0	8.8541878128e-12

#define PPAC_CONV_GRAD	0.01522
#define brho_ref        1.9709
#define TDC_TO_NS	0.025
#define ADC_VIR_PEDST   0.125
#define ADC_CONV_GRAD   41.667
#define DEPTH_OF_DSSD	50.e-6
/*-- module-local variables ----------------------------------------*/

double brho;
double velocity, TOF, beta;
double dE, ZZ, AOQ;
int u;



/*-------------	ana_module I/O		-----------------------*/
//extern int32_t tdc_value_ou[N_TDC_f3_ppac];


/*-- init routine --------------------------------------------------*/

INT PID_INIT(void){

 //  char name[256];
//   char title[256];
/*   for (int i=0; i<N_TDC_f3_ppac; i++)
   {
       //       printf("%0x\n",id);
       sprintf(name,  "F3PA-%03x", i);
       sprintf(title, "F3PA-%03x", i);
       }*/

	hDEVTOF = h2_book<TH2D>("dE VS TOF", "#Delta E VS TOF", 200, 25, 55, 200, 80, 400);
	hBRVTOF = h2_book<TH2D>("Br VS TOF", "B#rho VS TOF", 200, 1.5, 2.5, 200, 80, 400);
	//hAOQVZ = h2_book<TH2D>("AoQ VS Z", "AoQ VS Z", 200, 2.26, 2.58, 200, 13.5, 16.5);
	hAOQVZ = h2_book<TH2D>("AoQ VS Z", "A/Q VS Z", 200, 2.37, 2.64, 200, 13.3, 16.6);


		u = 1;

   return SUCCESS;

}

//###### initial



//######  1st order brho reconstruction  #############################

void br_recon_first(double POS_X_PPAC){

//	TRANSFER_MATRIX *odata;

	int i, j;

//	double POS_X_PPAC = TDC_DIFF*PPAC_CONV_GRAD;

//	odata = (TRANSFER_MATRIX *)malloc(sizeof(TRANSFER_MATRIX)* SIZE_OF_OTV);
//	for(i = 0; i < SIZE_OF_OTV; i++) &odata[i]->f0_2_f1 = &tmr[i];

	brho = brho_ref*(1. + POS_X_PPAC/FOD/100.);		// [T * m]

	return;

}

//######  high-order term brho perturbation  #############################
void br_recon_HO(){

        return;
}

//######  time of flight calculation  ####################################
void tof_if(double t_start, double t_end){

	double t_start_trans_delay, t_end_trans_delay, t_start_proc_delay, t_end_proc_delay;
	double beam_path_length;

	beam_path_length = LF3 - LF2;

	TOF = (t_end*TDC_TO_NS - t_end_trans_delay - t_end_proc_delay) - (t_start*TDC_TO_NS - t_start_trans_delay - t_start_proc_delay);		// [ns]
	velocity = beam_path_length/TOF;		// [m/ns]
	beta = velocity/c*1.e+9;

	return;

}

//######  energy loss && Z calculation  ####################################
void RECON_Z(double adc_val, double vel){

	double I,N,zz;		// calibration factors
	double Z;

	double A,B; 		// fit coef

	B = 0.9;
	A = 7.45e-8;

	dE = (double)(adc_val/ADC_CONV_GRAD) - ADC_VIR_PEDST;	// [MeV]

//	DSSD parameters		//
	zz = 14;
	N = 2.33e-6;
	I = 173.*qe;


	//vel: [m/s]
//	ZZ = dE*(m_e/c/c*qe)*vel*vel/(4.*PI*pow(qe,4)*N*zz)/(log(2*(m_e/c/c*qe*vel*vel/I) - log(1.-pow(vel/c,2)) - pow(vel/c,2));
	//ZZ = dE/DEPTH_OF_DSSD*(m_e*beta*beta/4./PI/pow(qe,3))*N*zz/(log(2*m_e*qe/I) + log(beta*beta/(1.-beta*beta)) - beta*beta);
	//ZZ = sqrt((dE*1.e+6)/DEPTH_OF_DSSD*(m_e*beta*beta/(4.*PI*qe*qe))*N*zz*(log(2.*m_e*beta*beta*qe/I)-log(1.-beta*beta)-beta*beta));
	ZZ = vel*A*sqrt(dE/(log(2.*m_e*beta*beta*qe/I)-log(1.-beta*beta)-beta*beta))+B ;
	return;

}


//#####	A/q reconstruction ###################################################
void RECON_AOQ(double br, double vel){

	AOQ = br/vel/(m_u/c/c); // mass -> Atomic mass number

        return;

}


/*-- BOR routine ---------------------------------------------------*/

INT PID_BOR(INT run_number)
{
   return SUCCESS;
}

/*-- eor routine ---------------------------------------------------*/

INT PID_EOR(INT run_number)
{
   return SUCCESS;
}

/*-- event routine -------------------------------------------------*/

INT PID_ONLINE(EVENT_HEADER *pheader, void *pevent)
{


	DWORD *pdata_t1, *pdata_t2, *pdata_t3, *pdata_a3;
	int bk_size_t1, bk_size_t2, bk_size_t3, bk_size_a3;
	int32_t dssd_adc = 0;
	uint32_t  dump;
        int32_t event_count_t1, event_count_t2, event_count_t3, event_count_a3;
        int32_t channel_t1, channel_t2, channel_t3, channel_a3;
        int32_t tdc_value1[N_TDC_f1_PPAC], tdc_value2[N_TDC_f2_PPAC], tdc_value3[N_TDC_f3_ppac];
        int32_t  ADC_value[32];
        uint64_t global_time_stamp;
	int32_t gcount, pcount, pcount64;
	int tdcducheck = 0;
	int tdc1st[32] = {0};
	int tdc2nd[32] = {0};
	int ch_du[32] = {0};
	uint32_t data1[64]={0};
	int tmp_ch;

/*----------------	Brho measurment		---------------------*/

   bk_size_t1 = bk_locate(pevent, BANK_NAME_F1PPAC, &pdata_t1);
   if (!bk_size_t1){

      return 1;
   }

   memset(tdc_value1, 0, sizeof(int32_t)*N_TDC_f1_PPAC);

   for(int i=0; i<1; i++){event_count_t1 = *pdata_t1++;}

   for (int i=0; i<bk_size_t1-3; ){

           channel_t1 = *pdata_t1++; i++;
           tdc_value1[channel_t1] = *pdata_t1++; i++;

   }

   for(int i=0; i<2; ){

           uint32_t time2 = *pdata_t1++; i++;
           uint32_t time1 = *pdata_t1++; i++;
           global_time_stamp = time2|time1;

   }


/*----------------      dE measurment         ---------------------*/

   bk_size_a3 = bk_locate(pevent, BANK_NAME_F3SILICON, &pdata_a3);
   if (!bk_size_a3){

      return 1;
   }

	memset(ADC_value, 0, 32);

           //for(int i=0; i<bk_size_a3; i++) dump=*pdata_a3++;

           for(int i=0; i<bk_size_a3-1; ){

                   channel_a3=*pdata_a3++; i++;

                if (channel_a3>N_ADC_f3_silicon){

			channel_a3=*pdata_a3++; i++;

		}

                if (channel_a3<N_ADC_f3_silicon){

                	ADC_value[channel_a3]=*pdata_a3++; i++;

                }

	}
           event_count_a3 = *pdata_a3++;

	//for(int i = 0 ; i < 16; i++) printf("ADC Ch. %d Val. = %u \n",i,ADC_value[i]);
           //bk_close(pevent, pdata_cal);


/*----------------      TOF measurment         ---------------------*/

	bk_size_t2 = bk_locate(pevent, BANK_NAME_F2PPAC, &pdata_t2);
	if (!bk_size_t2){

		return 1;
	}

	memset(tdc_value2, 0, sizeof(int32_t)*N_TDC_f2_PPAC);

	for(int i=0; i<1; i++) event_count_t2 = *pdata_t2++;

	for(int i=0; i<bk_size_t2-3;){

		channel_t2=*pdata_t2++; i++;
		tdc_value2[channel_t2]=*pdata_t2++; i++;

	}

	for(int i=0; i<2;){

		uint32_t time2_t2 = *pdata_t2++; i++;
		uint32_t time1_t2 = *pdata_t2++; i++;
		//global_time_stamp=time2|time1;

	}


        bk_size_t3 = bk_locate(pevent, BANK_NAME_F3PPAC, &pdata_t3);
        if (!bk_size_t3){

                return 1;
        }


        memset(tdc_value3, 0, sizeof(int32_t)*N_TDC_f3_ppac);

        //-----------------------------


        	for (int i=0; i<32; i++){
		ch_du[i] = 0;
	}

	for(int i=0; i<bk_size_t3; i++){
	
					data1[i]=((uint32_t*)pdata_t3)[i];
					if (i==0) event_count_t3 = data1[0];
					if (i>1&&i%2==0&&i<bk_size_t3-5){//find channel and tdc value

  						if(data1[i-1]<32) {
  							tmp_ch = data1[i-1]; ch_du[tmp_ch]++; tdc_value3[tmp_ch] = data1[i];
  							if (ch_du[tmp_ch]==1) tdc1st[tmp_ch] = tdc_value3[tmp_ch];
					      	        if (ch_du[tmp_ch]==2) tdc2nd[tmp_ch] = tdc_value3[tmp_ch];
  						}
						if(ch_du[tmp_ch]>1) {
							//printf("PPAC banklength = %d, 1st tdc_value = %d, 2nd tdc_value = %d, multi hit channel === %d, ch duplication = %d \n", bk_size_t3, tdc1st[tmp_ch], tdc2nd[tmp_ch], tmp_ch, ch_du[tmp_ch]);
							tdcducheck = 1;
							}
						if(ch_du[tmp_ch]==2)
					 	 {
                                          	tdc_value3[tmp_ch] = tdc1st[tmp_ch];
                                          	}
					}
				        if (i== bk_size_t3-1)//find channel and tdc value
					{
					  gcount = data1[i-4];
					  uint32_t time2 = data1[i-3];
					  uint32_t time1 = data1[i-2];
					  global_time_stamp=time2|time1;
					  pcount = data1[i-1];
					  pcount64 = data1[i];
					  if(tdcducheck==1) {
					  //printf("PPAC eventid = %d, GCOUNT = %d, pcount = %d, pcount64 = %d\n", event_count, gcount, pcount, pcount64); 
					  tdcducheck = 0;
 
					  }
					}
				}


        //--------------------------------
/*
        for(int i=0; i<1; i++) event_count_t3 = *pdata_t3++;

        for(int i=0; i<bk_size_t3-3;){

                channel_t3=*pdata_t3++; i++;
                tdc_value3[channel_t3]=*pdata_t3++; i++;

        }

	for(int i=0; i<5; ){

		gcount = *pdata_t3++; i++;
                uint32_t time2_t3 = *pdata_t3++; i++;
                uint32_t time1_t3 = *pdata_t3++; i++;
                //global_time_stamp_t3 = time2_t3|time1_t3;
        	pcount = *pdata_t3++; i++;
        	pcount64 = *pdata_t3++; i++;

         }*/




//#				EO Bank I/o				#//
/*-----------------------------------------------------------------------*/


	double pos_x;

	if(tdc_value1[0] > 0 &&  tdc_value1[1] > 0 &&  tdc_value1[12] > 0 && tdc_value1[0] + tdc_value1[1] > 6500 && tdc_value1[0] + tdc_value1[1] < 7500) pos_x = (tdc_value1[0] - tdc_value1[1])*PPAC_CONV_GRAD - 150.;
	if(tdc_value1[2] > 0 &&  tdc_value1[3] > 0 &&  tdc_value1[12] > 0 && tdc_value1[2] + tdc_value1[3] > 6500 && tdc_value1[2] + tdc_value1[3] < 7500) pos_x = (tdc_value1[2] - tdc_value1[3])*PPAC_CONV_GRAD - 50.;
	if(tdc_value1[4] > 0 &&  tdc_value1[5] > 0 &&  tdc_value1[12] > 0 && tdc_value1[4] + tdc_value1[5] > 6500 && tdc_value1[4] + tdc_value1[5] < 7500) pos_x = (tdc_value1[4] - tdc_value1[5])*PPAC_CONV_GRAD + 50.;
	if(tdc_value1[6] > 0 &&  tdc_value1[7] > 0 &&  tdc_value1[12] > 0 && tdc_value1[6] + tdc_value1[7] > 6500 && tdc_value1[6] + tdc_value1[7] < 7500) pos_x = (tdc_value1[6] - tdc_value1[7])*PPAC_CONV_GRAD + 150.;
	//else printf("no event from the F1 PPAC %u  %u\n",tdc_value1[0], tdc_value1[12]);

	br_recon_first(pos_x);
	tof_if(tdc_value2[0], tdc_value3[15]);

	//for(int i; i < N_ADC_f3_silicon; i++) if(ADC_value[i] > 10 && ADC_value[i] < 4096 )  dssd_adc = ADC_value[i];
	dssd_adc = ADC_value[channel_a3];

	RECON_Z(dssd_adc, velocity*1.e+9);
	RECON_AOQ(brho, velocity*1.e+9);

	if(AOQ > 0 && ZZ > 0){hAOQVZ->Fill(AOQ, ZZ, 1);    /*gStyle->SetPalette(1);*/  }

	if(brho > brho_ref*0.92 && brho < brho_ref*1.08 && TOF > (LF3 - LF2)/c*1.e+9 && TOF < (LF3 - LF2)/c*1.e+11){ hBRVTOF->Fill(brho, TOF, 1);	// (+TOF) spectrometer acceptance cut


	//if(dE < 1600.  && dE > 0 && TOF > (LF3 - LF2)/c*1.e+9 && TOF < (LF3 - LF2)/c*1.e+11){	// primary energy & minimum energy & speed of light & undetectable velocity cut

		hDEVTOF->Fill(dE, TOF, 1);

		//if(brho > brho_ref*0.92 && brho < brho_ref*1.08) hAOQVZ->Fill(AOQ, ZZ, 1);
}


	//}

	printf("Rigidity: %lf [T m],  TOF: %lf [ns],  beta: %lf,  ELoss: %lf\n",brho, TOF, beta, dE); // Major parameters monitoring...
	printf("A/q: %lf ,  Z: %lf \n", AOQ, ZZ);

		gStyle->SetPalette(kBird);
		hAOQVZ->Draw("CONT4Z");

   return SUCCESS;
}
