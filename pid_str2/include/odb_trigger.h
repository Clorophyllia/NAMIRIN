#pragma once
#define ODB_TRIGGER_H

#include "detector_conf.h"

typedef struct {
//#ifdef USE_PPAC_F1
	int f1_ppac_window_width;
	int f1_ppac_window_offset;
	int f1_ppac_extra_search_margin;
	int f1_ppac_reject_margin;
//#endif
//#ifdef USE_PPAC_F2
	int f2_ppac_window_width;
	int f2_ppac_window_offset;
	int f2_ppac_extra_search_margin;
	int f2_ppac_reject_margin;
//#endif
//#ifdef USE_PPAC_F3
	int f3_ppac_window_width;
	int f3_ppac_window_offset;
	int f3_ppac_extra_search_margin;
	int f3_ppac_reject_margin;
//#endif
//#ifdef USE_SILICON_F3
	double f3_si_th[32];
//#endif
//#ifdef USE_SILICON_ARYA
	double si_arya_th[32];
//#endif
//#ifdef USE_SILICON_ARYB
	double si_aryb_th[16];
//#endif
} TRIGGER_SETTINGS;

static const char *ts[]={
		"[.]",
#ifdef USE_PPAC_F1
		"F1PPAC Window Width (ns)		 = INT 1000",
		"F1PPAC Window Offset (ns)       = INT -400",
		"F1PPAC Extra Search Margin (ns) = INT 200",
		"F1PPAC Reject Margin (ns)       = INT 100",
#endif
#ifdef USE_PPAC_F2
		"F2PPAC Window Width (ns)		 = INT 1000",
		"F2PPAC Window Offset (ns)       = INT -400",
		"F2PPAC Extra Search Margin (ns) = INT 200",
		"F2PPAC Reject Margin (ns)       = INT 100",
#endif
#ifdef USE_PPAC_F3
		"F3PPAC Window Width (ns)		 = INT 1000",
		"F3PPAC Window Offset (ns)       = INT -400",
		"F3PPAC Extra Search Margin (ns) = INT 200",
		"F3PPAC Reject Margin (ns)       = INT 100",
#endif
#ifdef USE_SILICON_F3
		"F3 Si Detector Threshold (x16) = DOUBLE[32] :",
		"[0]	10",
		"[1]	10",
		"[2]	10",
		"[3]	10",
		"[4]	10",
		"[5]	10",
		"[6]	10",
		"[7]	10",
		"[8]	10",
		"[9]	10",
		"[10]	10",
		"[11]	10",
		"[12]   10",
		"[13]	10",
		"[14]	10",
		"[15]	10",
		"[16]	10",
		"[17]	10",
		"[18]	10",
		"[19]	10",
		"[20]	10",
		"[21]	10",
		"[22]	10",
		"[23]	10",
		"[24]	10",
		"[25]	10",
		"[26]	10",
		"[27]	10",
		"[28]	10",
		"[29]	10",
		"[30]	10",
		"[31]	10",

#endif
#ifdef USE_SILICON_ARYA
		"Si ary a Threshold (x16) = DOUBLE[32] :",
		"[0]	10",
		"[1]	10",
		"[2]	10",
		"[3]	10",
		"[4]	10",
		"[5]	10",
		"[6]	10",
		"[7]	10",
		"[8]	10",
		"[9]	10",
		"[10]	10",
		"[11]	10",
		"[12]	10",
		"[13]	10",
		"[14]	10",
		"[15]	10",
		"[16]	10",
		"[17]	10",
		"[18]	10",
		"[19]	10",
		"[20]	10",
		"[21]	10",
		"[22]	10",
		"[23]	10",
		"[24]	10",
		"[25]	10",
		"[26]	10",
		"[27]	10",
		"[28]	10",
		"[29]	10",
		"[30]	10",
		"[31]	10",
#endif
#ifdef USE_SILICON_ARYB
		"Si ary b Threshold (x16) = DOUBLE[16] :",
		"[0]	10",
		"[1]	10",
		"[2]	10",
		"[3]	10",
		"[4]	10",
		"[5]	10",
		"[6]	10",
		"[7]	10",
		"[8]	10",
		"[9]	10",
		"[10]	10",
		"[11]	10",
		"[12]	10",
		"[13]	10",
		"[14]	10",
		"[15]	10",/*
		"[16]	10",
		"[17]	10",
		"[18]	10",
		"[19]	10",
		"[20]	10",
		"[21]	10",
		"[22]	10",
		"[23]	10",
		"[24]	10",
		"[25]	10",
		"[26]	10",
		"[27]	10",
		"[28]	10",
		"[29]	10",
		"[30]	10",
		"[31]	10",*/
#endif

		"",
		NULL};
