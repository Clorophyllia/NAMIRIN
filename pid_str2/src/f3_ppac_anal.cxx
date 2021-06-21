/********************************************************************\

  Name:         tdccalib.c
  Created by:   Stefan Ritt

  Contents:     Example analyzer module for TDC calibration. Looks
                for TDC0 bank, subtracts pedestals and applies gain
                calibration. The resulting values are appended to
                the event as an CGTIG bank ("calibrated TDC"). The
                pedestal values and software gains are stored in
                tdccalib_param structure which was defined in the ODB
                and transferred to experim.h.

  $Id:$

\********************************************************************/

/*-- Include files -------------------------------------------------*/

/* standard includes */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>


#include <midas.h>

#include "detector_conf.h"
#include "analyzer.h"
/* midas includes */

#include "f3_ppac_anal.h"

//#include "f1_ppac_anal.h"

#include <TH1D.h>
#include <TH2D.h>
#include <TTree.h>
#include <TDirectory.h>



/* root includes */


/*-- Parameters ----------------------------------------------------*/


/*-- module-local variables ----------------------------------------*/


/*-- init routine --------------------------------------------------*/

static TH1D *hPTDCHists_raw[N_TDC_f3_ppac];
static TH1D *hPTDCHists_raw0[N_TDC_f1_PPAC];
		
static TH1D *hPXDIFFTDCHists_raw;
static TH1D *hPYDIFFTDCHists_raw;
static TH2D *hP2DTDCHists_raw;
static TH1D *hPXDIFFTDCHists_raw2;

static TH1D *hPYDIFFTDCHists_raw2;
static TH1D *hPYDIFFTDCHists_raw3;

static TH1D *hPXDIFFTDCHists_cor;
static TH1D *hPXDIFFTDCHists_cor_ch8;
static TH1D *hPXDIFFTDCHists_ind;
static TH2D *hP2DADCARYABHists_raw;
static TH2D *hP2DADCARYABHists_raw_1_16;
static TH2D *hPXDIFFTDCHists_sum;
static TH2D *hPXDIFFTDCHists_sum_zoom;

static TH1D *harti;

static TH1D *h_tdc_systemdiffer;
static TH1D *h_tdc;
static TH2D *h2d_detof_raw;

FILE *tet = fopen("test_aryanal.dat","w");


int first_Call_chks_arya;
int32_t testtest;
INT F3_TDC_INIT(void)
{

fprintf(tet,"chann  ADC Val  Gcount  Ecount  Pcount  Pcount64\n");

   char name[256];
   char title[256];

   /* book GTIG histos */

   for (int i=0; i<N_TDC_f3_ppac; i++)
   {
       //       printf("%0x\n",id);
       sprintf(name,  "F3TDC-%03d", i);
       sprintf(title, "F3TDC-%03d", i);
       hPTDCHists_raw[i] = h1_book<TH1D>(name, title, PTAC_N_BINS, PTAC_X_LOW, PTAC_X_HIGH);
    }
	   hPXDIFFTDCHists_raw = h1_book<TH1D>("F3 X DIFF", "F3 X DIFF", 1000, -4000, 4000);
	   hPYDIFFTDCHists_raw = h1_book<TH1D>("F3 Y DIFF", "F3 Y DIFF", 1000, -4000, 4000);
	   hP2DTDCHists_raw = h2_book<TH2D>("F3 2D", "F3 2D plot", 1000, -4000, 4000, 1000, -4000, 4000);

	hPXDIFFTDCHists_cor = h1_book<TH1D>("F3 X COR", "F3 X COR", 250, -50, 50);
        //hPXDIFFTDCHists_cor_ch8 = h1_book<TH1D>("F3 X COR", "F3 X COR", 250, -50, 50);
	hPXDIFFTDCHists_ind = h1_book<TH1D>("F3 X IND", "F3 X IND", 250, -50, 50);
	hPXDIFFTDCHists_sum =  h2_book<TH2D>("F3 2D sum", "F3 2D sum", 300, -16000, 8000, 300, -16000, 8000);
	hPXDIFFTDCHists_sum_zoom =  h2_book<TH2D>("F3 2D sum_zoom", "F3 2D sum_zoom", 300, 1500, 3500, 300, 1500, 3500);
	hPXDIFFTDCHists_raw2 = h1_book<TH1D>("F3 X RAW", "F3 X RAW", 250, -50, 50);

	hPYDIFFTDCHists_raw2 = h1_book<TH1D>("F3 Y RAW", "F3 Y RAW", 250, -50, 50);
	hPYDIFFTDCHists_raw3 = h1_book<TH1D>("F3 Y RAW (X&&Y)", "F3 Y RAW (X&&Y)", 250, -50, 50);

	hP2DADCARYABHists_raw = h2_book<TH2D>("F3 sili", "F3 sili", 32, 0, 32, 500, 0,5000);
 	hP2DADCARYABHists_raw_1_16 = h2_book<TH2D>("F3 sili_1_16", "F3 sili_1_16", 16, 0, 16, 500, 0,5000);
	first_Call_chks_arya = 1;


	harti = h1_book<TH1D>("Art silicon COR", "Art silicon COR",  250, -1000, 1000);
	
	   char name0[256];
   char title0[256];

   /* book GTIG histos */

/*
   for (int i=0; i<N_TDC_f1_PPAC; i++)
   {
       //       printf("%0x\n",id);
       sprintf(name0,  "F1PA-%03d", i);
       sprintf(title0, "F1PA-%03d", i);
       hPTDCHists_raw0[i] = h1_book<TH1D>(name0, title0, PTAC_N_BINS, PTAC_X_LOW, PTAC_X_HIGH);
    }
    */
    h_tdc_systemdiffer = h1_book<TH1D>("F3 - F1", "F3 - F1", 200, -40, 40);
    h_tdc = h1_book<TH1D>("TOF(F3 and F2)", "TOF(F3-F2)", 200, -40, 40);//F1 TDC module ch14 - ch15
    h2d_detof_raw = h2_book<TH2D>("DE vs TOF(F3 and F2)", "DE vs TOF(F3 and F2)", 200, -40, 40, 50, 0,50);
	
	
   return SUCCESS;
}

/*-- BOR routine ---------------------------------------------------*/

INT F3_TDC_BOR(INT run_number)
{
   return SUCCESS;
}

/*-- eor routine ---------------------------------------------------*/

INT F3_TDC_EOR(INT run_number)
{
   fclose(tet);
   return SUCCESS;
}

/*-- event routine -------------------------------------------------*/

INT F3_TDC(EVENT_HEADER *pheader, void *pevent)
{
   DWORD *pdata1;
   DWORD *pdata3;
   DWORD *pdata0;
   DWORD *pdata4;

   /* look for bank, return if not present */
   
   int bk_sizef1tdc;
//   bk_sizef1tdc = bk_locate(pevent, BANK_NAME_F1PPAC, &pdata0);
   int32_t event_count0;
   int32_t channel0;
   int32_t tdc_value0[N_TDC_f1_PPAC];
   uint64_t global_time_stamp0;
 //  memset(tdc_value0, 0, sizeof(int32_t)*32);
   
   
   int bk_size;
 //  bk_size = bk_locate(pevent, BANK_NAME_F3PPAC, &pdata1);
   int32_t event_count, event_count2;
   int32_t channel;
   int32_t tdc_value[N_TDC_f3_ppac];
   int32_t tdc_value_ou[N_TDC_f3_ppac];
   uint64_t global_time_stamp;
   uint32_t gcount1,gcount2,gcountbs;//,adc_head_tagger;
   uint32_t pcount1, pcount641, pcountbs;
   uint32_t pcount2, pcount642, pcountbs64;
   
  // memset(tdc_value, 0, sizeof(int32_t)*N_TDC_f3_ppac);
   
//============================== beamline silicon =========================//   
   int bk_size_bs;	 
  // bk_size_bs = bk_locate(pevent, BANK_NAME_F3SILICON, &pdata4);
   int32_t dumps_arybs;
   int32_t event_count_bs;
   uint32_t channels_arybs;
   uint64_t ev_tagbs1, ev_tagbs2;
   int64_t ADC_bs_value[N_ADC_f3_silicon];
   int adcducheck_bs = 0; 
  // memset(ADC_bs_value, 0, sizeof(int32_t)*N_ADC_f3_silicon);
   
//============================== silicon array a =========================//    
   int bk_size_a;	 
 //  bk_size_a = bk_locate(pevent, BANK_NAME_U1SILLICON, &pdata3);
   int32_t dumps_arya;
   int32_t event_counts_arya;
   uint32_t channels_arya;
   uint64_t ev_tag1, ev_tag2;
   int64_t ADC_arya_value[N_ADC_arya_silicon];
 //  memset(ADC_arya_value, 0, sizeof(int32_t)*N_ADC_arya_silicon);
   
   uint32_t data1[64]={0};
   uint32_t data3[1000]={0};
   uint32_t data4[1000]={0};
   int ch_du[32] = {0};
   int ch_du_adc[32] = {0};
   int adc1st[32] = {0};
   int adc2nd[32] = {0};
   int ch_du_adcbs[32] = {0};
   int adc1stbs[32] = {0};
   int adc2ndbs[32] = {0};
   int tdc1st[32] = {0};
   int tdc2nd[32] = {0};
   int adcducheck = 0; 
   int tdcducheck = 0;
   int tmp_ch;
   int meck = 0;
   
//	uint32_t *adc_tmp = (int *)malloc( (181 * sizeof(int));		// multi-hit handlling buffer
//	int *adc_tmp_val = (int *)malloc( (81 * sizeof(int));		// multi-hit handlling buffer - > ADC alt input
//printf("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n")

//##############################	gathering f1 ppac data for the tdc.val compairson	################################
//#																#
/*
   for(int i=0; i<1; i++){event_count0 = *pdata0++;}

   for (int i=0; i<bk_sizef1tdc-3; )
   {
	   channel0=*pdata0++; i++;
	   tdc_value0[channel0]=*pdata0++; i++;
   }

   for(int i=0; i<2; )
   {
	   uint32_t time02=*pdata0++; i++;
	   uint32_t time01=*pdata0++; i++;
	   global_time_stamp0=time02|time01;

   }

*/
     char name0[256];
/*
   for(int i=0; i<N_TDC_f1_PPAC; i++)
    {
    	 sprintf(name0,  "F1PPAC-%03x", i);
    	 hPTDCHists_raw0[i]->Fill(tdc_value0[i]*f1_tdc_cali_param.gain, 1);
    }
*/






//=============================== f3 ppac ===========================//
/*	for (int i=0; i<32; i++){
		ch_du[i] = 0;
	}
	
	for(int i=0; i<bk_size; i++)
				{
					data1[i]=((uint32_t*)pdata1)[i];	
					if (i==0) event_count = data1[0];
					if (i>1&&i%2==0&&i<bk_size-5)//find channel and tdc value
					{
  						if(data1[i-1]<32) {
  							tmp_ch = data1[i-1]; ch_du[tmp_ch]++; tdc_value[tmp_ch] = data1[i];
  							if (ch_du[tmp_ch]==1) tdc1st[tmp_ch] = tdc_value[tmp_ch];
					      	        if (ch_du[tmp_ch]==2) tdc2nd[tmp_ch] = tdc_value[tmp_ch];
  							}
						if(ch_du[tmp_ch]>1) {
							printf("PPAC banklength = %d, 1st tdc_value = %d, 2nd tdc_value = %d, multi hit channel === %d, ch duplication = %d \n", bk_size, tdc1st[tmp_ch], tdc2nd[tmp_ch], tmp_ch, ch_du[tmp_ch]);
							tdcducheck = 1;
							}
						if(ch_du[tmp_ch]==2)
					 	 {
                                          	tdc_value[tmp_ch] = tdc1st[tmp_ch];
                                          	}
					}
				        if (i== bk_size-1)//find channel and tdc value
					{
					  gcount1 = data1[i-4];
					  uint32_t time2 = data1[i-3];
					  uint32_t time1 = data1[i-2];
					  global_time_stamp=time2|time1;
					  pcount1 = data1[i-1];
					  pcount641 = data1[i];
					  if(tdcducheck==1) {
					  printf("PPAC eventid = %d, GCOUNT1 = %d, pcount1 = %d, pcount641 = %d\n", event_count, gcount1, pcount1, pcount641); 
					  tdcducheck = 0;
					  }
					}
				}

     char name[256];

	for(int j; j < N_TDC_f3_ppac ; j++) tdc_value_ou[j] = tdc_value[j];

 if (tdc_value[0]>0 && tdc_value[1]>0 && tdc_value[2]>0) hPXDIFFTDCHists_raw2->Fill(tdc_value[1]*0.0153-tdc_value[2]*0.0153,1);


 hPXDIFFTDCHists_sum->Fill((tdc_value[1]+tdc_value[2]-2.*tdc_value[0]), (tdc_value[3]+tdc_value[4]-2.*tdc_value[0]), 1);

 if (tdc_value[0]>0 && tdc_value[3]>0 && tdc_value[4]>0) hPYDIFFTDCHists_raw2->Fill(tdc_value[3]*0.0153-tdc_value[4]*0.0153,1); //Y RAW

 if (tdc_value[0]>0 && tdc_value[3]>0 && tdc_value[4]>0 && tdc_value[1]>0 && tdc_value[2]>0) hPYDIFFTDCHists_raw3->Fill(tdc_value[3]*0.0153-tdc_value[4]*0.0153,1); //Y RAW for X&&Y
 if((tdc_value[1]+tdc_value[2]-2.*tdc_value[0])>1500&&(tdc_value[1]+tdc_value[2]-2.*tdc_value[0])<3500&&(tdc_value[3]+tdc_value[4]-2.*tdc_value[0])>1500&&(tdc_value[3]+tdc_value[4]-2.*tdc_value[0])<3500)
 hPXDIFFTDCHists_sum_zoom->Fill((tdc_value[1]+tdc_value[2]-2.*tdc_value[0]), (tdc_value[3]+tdc_value[4]-2.*tdc_value[0]), 1);



   for(int i=0; i<N_TDC_f3_ppac; i++)
    {
    	 sprintf(name,  "F3TDC-%03x", i);
    	 hPTDCHists_raw[i]->Fill(tdc_value[i]*f3_ppac_tdc.gain, 1);
    }
    if (tdc_value[1]+tdc_value[2]-2.*tdc_value[0]>1500 && tdc_value[3]+tdc_value[4]-2.*tdc_value[0] > 1500 && tdc_value[1]+tdc_value[2]-2.*tdc_value[0] < 3500 && tdc_value[3]+tdc_value[4]-2.*tdc_value[0] < 3500) hPXDIFFTDCHists_raw->Fill(tdc_value[1]-tdc_value[2],1);
    if (tdc_value[2]+tdc_value[3]>1000) hPYDIFFTDCHists_raw->Fill(tdc_value[2]-tdc_value[3],1);
    if (tdc_value[0]+tdc_value[1]>1000 && tdc_value[2]+tdc_value[3]>1000)hP2DTDCHists_raw->Fill(tdc_value[0]-tdc_value[1], tdc_value[2]-tdc_value[3], 1);

*/

//============================== beamline silicon =========================//
/*		for (int i=0; i<32; i++){
			ch_du_adcbs[i] = 0;
			adc1stbs[i] = 0;
			adc2ndbs[i] = 0;
		}
		
		
		for(int i=0; i<bk_size_bs; i++){
	
			data4[i]=((uint32_t*)pdata4)[i];
			//printf("ary bank entry: %d\n",data4[i]);
			//adc_head_tagger = data4[i];	
			if (i<bk_size_bs-4)//find channel and adc value
			{ 
				if((i+1)%2==0){
				      	if(data4[i-1] < 32) {
				      	        //printf("tmp_ch = %d, measure = %d\n",data4[i-1],data4[i]);
					  	tmp_ch = data4[i-1]; ADC_bs_value[tmp_ch] = data4[i]; ch_du_adcbs[tmp_ch]++;
					  	//printf("ADC_bs_value[%d] : %d\n",tmp_ch, ADC_bs_value[tmp_ch]);
				      		if (ch_du_adcbs[tmp_ch]==1) adc1stbs[tmp_ch] = ADC_bs_value[tmp_ch];
				      		if (ch_du_adcbs[tmp_ch]==2) adc2ndbs[tmp_ch] = ADC_bs_value[tmp_ch];
				      		if (ch_du_adcbs[tmp_ch]==2 && adc1stbs[tmp_ch] > 0 && adc1stbs[tmp_ch] < 5100 && tmp_ch < 32) {
				      		
				      			ADC_bs_value[tmp_ch]=adc1stbs[tmp_ch];
				      			//printf("silicon ch: %d duplication... 1st adc value = %d, 2nd adc value = %d \n",tmp_ch, adc1stbs[tmp_ch], adc2ndbs[tmp_ch]);
				      			
				      		}
						
	 				}
				}
			}
			
			if (i == bk_size_bs-1){
				gcountbs = data4[i-3];
				event_count_bs = data4[i-2];
				pcountbs = data4[i-1];
				pcountbs64 = data4[i];
				if (adcducheck_bs==1) {
				//printf("Beamline silicon event id === %d, GCOUNT = %d, pcount = %d, pcount64=%d\n\n", event_count,gcount,pcount,pcount64); 
				adcducheck_bs=0;
				}
						    
			}
			
		}	
		


*/


//=============================== silicon array ===========================//
//	printf("bank size: %d\n",bk_size_a);
	
	//if(bk_size_a == 36){	### 03226
/*		for (int i=0; i<32; i++){
			ch_du_adc[i] = 0;
			adc1st[i] = 0;
			adc2nd[i] = 0;
		}
		
		for(int i=0; i<bk_size_a; i++){
			data3[i]=((uint32_t*)pdata3)[i];
			//printf("ary bank entry: %d\n",data3[i]);
			//adc_head_tagger = data3[i];	
			if (i<bk_size_a-4)//find channel and adc value
			{ 
				if((i+1)%2==0){
				      	if(data3[i-1] < 32) {
					  	tmp_ch = data3[i-1]; ADC_arya_value[tmp_ch] = data3[i]; ch_du_adc[tmp_ch]++;
				      		if (ch_du_adc[tmp_ch]==1) adc1st[tmp_ch] = ADC_arya_value[tmp_ch];
				      		if (ch_du_adc[tmp_ch]==2) adc2nd[tmp_ch] = ADC_arya_value[tmp_ch];
				      		if (ch_du_adc[tmp_ch]==2 && adc1st[tmp_ch] > 400 && adc1st[tmp_ch] < 1100 && tmp_ch < 16) {
				      		
				      			ADC_arya_value[tmp_ch]=adc1st[tmp_ch];
				      			printf("silicon ch: %d duplication... 1st adc value = %d, 2nd adc value = %d \n",tmp_ch, adc1st[tmp_ch], adc2nd[tmp_ch]);
				      			
				      		}
						
	 				}
				}
			}
			if (i == bk_size_a-1){
				gcount2 = data3[i-3];
				event_count2 = data3[i-2];
				pcount2 = data3[i-1];
				pcount642 = data3[i];
				if (adcducheck==1) {
				printf("Silicon event id === %d, GCOUNT2 = %d, pcount2 = %d, pcount642=%d\n\n", event_count2,gcount2,pcount2,pcount642); 
				adcducheck=0;
				}
						    
			}
			
		}*/
	//}###3226
	/* ###3226
	else if(bk_size_a >= 74 && bk_size_a < 181){
	
		for (int i=0; i<32; i++){
			ch_du_adc[i] = 0;
			adc1st[i] = 0;
			adc2nd[i] = 0;
		}
		
		for(int i = 0; i < 36; i++){
			data3[i]=((uint32_t*)pdata3)[i];
			//printf("ary bank entry: %d\n",data3[i]);	
			if (i<bk_size_a-4)//find channel and adc value
			{ 
				if((i+1)%2==0){
				      	if(data3[i-1]<32) {
					  	tmp_ch = data3[i-1]; ADC_arya_value[tmp_ch] = data3[i]; ch_du_adc[tmp_ch]++;
				      		if (ch_du_adc[tmp_ch]==1) adc1st[tmp_ch] = ADC_arya_value[tmp_ch];
				      		if (ch_du_adc[tmp_ch]==2) adc2nd[tmp_ch] = ADC_arya_value[tmp_ch];
				      		if (ch_du_adc[tmp_ch]==2 && adc1st[tmp_ch]>400 && adc1st[tmp_ch]<1100 && tmp_ch<16) {
				      			ADC_arya_value[tmp_ch]=adc1st[tmp_ch];
				      			printf("silicon ch: %d duplication... 1st adc value = %d, 2nd adc value = %d \n",tmp_ch, adc1st[tmp_ch], adc2nd[tmp_ch]);
				      			}
						
	 				}
				}
			}
			if (i == bk_size_a-1){
				gcount2 = data3[i-3];
				event_count2 = data3[i-2];
				pcount2 = data3[i-1];
				pcount642 = data3[i];
				if (adcducheck==1) {
				printf("Silicon event id === %d, GCOUNT2 = %d, pcount2 = %d, pcount642=%d\n\n", event_count2,gcount2,pcount2,pcount642); 
				adcducheck=0;
				}
						    
			}
			
		}
		
		for(int i = 36; i < bk_size_a; i++){
		
			adc_tmp[i] =((uint32_t*)pdata3)[i];
			printf("ary bank entry: %u\n",adc_tmp[i]);	
			
			if (bk_size_a % 36 < 32)if((i+1)%2==0){
				      	if(adc_tmp[i-1]<32) {
					  	tmp_ch = adc_tmp[i-1]; adc_tmp_val[tmp_ch] = adc_tmp[i]; 

				      		//printf("silicon ch: %d duplication... 1st adc value = %d, 2nd adc value = %d \n",tmp_ch, adc1st[tmp_ch], adc2nd[tmp_ch]);		
				    
	 				}
				
			}
			else{
				gcount2 =adc_tmp[i-3];
				event_count2 = adc_tmp[i-2];
				pcount2 = adc_tmp[i-1];
				pcount642 = dc_tmp[i];
				
				if (adcducheck==1) {
					printf("Silicon event id === %d, GCOUNT2 = %d, pcount2 = %d, pcount642=%d\n\n", event_count2,gcount2,pcount2,pcount642); 
					adcducheck=0;
				}
						    
			}
			
		}		
	
	}
	meck = bk_size_a & 36;
	### 3226*/
			 /*channels_arya=*pdata3++; 
		 printf("ary bank entry: %d\n",data3[i]);
		 //printf("ary bank entry: %d\n",channels_arya);	
		 if(channels_arya<N_ADC_arya_silicon){ 
			ADC_arya_value[channels_arya]=*pdata3++; i++;
			if(channels_arya<16&&ADC_arya_value[channels_arya]>400&&ADC_arya_value[channels_arya]<1100){
			printf("ary a channel:%d, \t ADC value:%d \n", channels_arya, ADC_arya_value[channels_arya]);
			hP2DADCARYABHists_raw_1_16->Fill(channels_arya,ADC_arya_value[channels_arya], 1);
			//testtest++;
			}
		 else {
		 }
		}*/	

      /*    
       for(int i=0; i<4; ){   
	   	gcount2=*pdata3++; i++;
          	event_count2=*pdata3++; i++;	
	   	pcount2=*pdata3++; i++;
           	pcount642=*pdata3++; i++;	
       }*/
       //printf("gcount2:%d, \t event_count2:%d\n", gcount2, event_count2);

//printf("testtest = %d\n", testtest);
for(int i=0; i < 16; i++) if(ADC_arya_value[i] > 400 && ADC_arya_value[i] < 1100 ) { 
	hP2DADCARYABHists_raw_1_16->Fill(i,ADC_arya_value[i], 1);
	fprintf(tet,"%u \t %u \t %u \t %u \t %u \t %u\n",i,ADC_arya_value[i],gcount2, event_count2,pcount2, pcount642);
}
for(int i=0; i < 32; i++) if(ADC_arya_value[i] > 20 && ADC_arya_value[i] < 4000) hP2DADCARYABHists_raw->Fill(i,ADC_arya_value[i], 1);

bool logic = 0;
bool logic2 = 0;

for(int i=0; i < 16; i++) if(ADC_arya_value[i] > 200 && ADC_arya_value[i] < 2000) logic = 1;
for(int i=16; i < 32; i++) if(ADC_arya_value[i] > 200 && ADC_arya_value[i] < 2000) logic2 = 1;
 //if (tdc_value[1]+tdc_value[2]-2.*tdc_value[0]>2800 && tdc_value[3]+tdc_value[4]-2.*tdc_value[0] > 2800 && tdc_value[1]+tdc_value[2]-2.*tdc_value[0] < 3200 && tdc_value[3]+tdc_value[4]-2.*tdc_value[0] < 3200 && logic) hPXDIFFTDCHists_cor->Fill(tdc_value[1]*0.0153-tdc_value[2]*0.0153,1);



if (tdc_value[1]+tdc_value[2]-2.*tdc_value[0]>1500 && tdc_value[3]+tdc_value[4]-2.*tdc_value[0] > 1500 && tdc_value[1]+tdc_value[2]-2.*tdc_value[0] < 3500 && tdc_value[3]+tdc_value[4]-2.*tdc_value[0] < 3500 && logic == 1)hPXDIFFTDCHists_cor->Fill(tdc_value[1]*0.0153-tdc_value[2]*0.0153,1);


if ( logic2 == 1)harti->Fill(tdc_value[9]*f3_ppac_tdc.gain,1);

 h_tdc_systemdiffer->Fill(tdc_value[15]*f3_ppac_tdc.gain - tdc_value0[15]*f3_ppac_tdc.gain,1);
  h_tdc->Fill(tdc_value0[14]*f3_ppac_tdc.gain - tdc_value0[15]*f3_ppac_tdc.gain,1); //F1 TDC module ch14 - ch15
for(int i=0; i < 32; i++){ 
if(ADC_bs_value[i]>0)
h2d_detof_raw->Fill(tdc_value[15]*f3_ppac_tdc.gain - tdc_value0[15]*f3_ppac_tdc.gain, ADC_bs_value[i], 1);
}
if (tdc_value[1]+tdc_value[2]-2.*tdc_value[0]>2800 && tdc_value[3]+tdc_value[4]-2.*tdc_value[0] > 2800 && tdc_value[1]+tdc_value[2]-2.*tdc_value[0] < 3200 && tdc_value[3]+tdc_value[4]-2.*tdc_value[0] < 3200 && logic == 0) hPXDIFFTDCHists_ind->Fill(tdc_value[1]*0.0153-tdc_value[2]*0.0153,1);


//	free(adc_tmp);

   return SUCCESS;
}
