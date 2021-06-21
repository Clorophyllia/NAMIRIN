//###############################################################################################################################
//#																#
//						LISE++ data file header file
//
//		Create backend space for the monitoring V2495 FPGA(Scaler & Timestamp) and User system
//		ver. 10.08.20, Dong-Yeup Lee, PH.D student at the Korea Univ.
//
//
//
//
//#																#
//###############################################################################################################################


#ifndef _PID_SIMPLE_H_
#define _PID_SIMPLE_H_

/* struct g_isotopes{

        double ade_mean;         double ade_sigm;
        double atof_mean;        double atof_sigm;
        double abr_intv;         double abr_intc;
        double ainten;


}; */



typedef struct {

	double de_mean;		double de_sigm;
	double tof_mean;	double tof_sigm;
	double br_intv;		double br_intc;
	double inten;

} GAUSS_ISOTOPE;


//------------------* list of LISEPP (Aprox.) Calulated elements for the  40Ar + 12C -> 38S system setup *-------------------------------//

GAUSS_ISOTOPE S37{

	4.201e+1,	6.520e-1,
	1.8005e+2,	1.314,
	.3,		3.9132,
	3.09

}; //-0.01114 slope

GAUSS_ISOTOPE S38{

	4.231e+1,	8.598e-1,
	1.808e+2,	1.813,
	.29,		3.967,
	1.56

};	//1112

GAUSS_ISOTOPE S39{

	4.319e+1,	1.078,
	1.830e+2,	2.191,
	.37,		4.0105,
	0.0168

};	//1111

GAUSS_ISOTOPE P35{

	3.7014e+1,	9.011e-1,
	1.8017e+2,	2.066,
	.4,		3.929,
	1.58

};	//1112

GAUSS_ISOTOPE P36{

	3.731e+1,	9.435e-1,
	1.8168e+2,	2.2,
	.34,		3.9831,
	0.536

};	//1112

GAUSS_ISOTOPE P37{

	3.837e+1,	1.01,
	1.841e+2,	2.274,
	.27,		4.0377,
	0.104

};	//1112

GAUSS_ISOTOPE P38{

	4.e+1,		1.646,
	1.887e+2,	2.361,
	.31,		4.0916,
	0.00092

};	//	1090

GAUSS_ISOTOPE SI33{

	3.2285e+1,	7.957e-1,
	1.8032e+2,	2.01,
	.36,		3.9425,
	0.82

};	//	1113

GAUSS_ISOTOPE SI34{

	3.288e+1,	8.869e-1,
	1.8217e+2,	2.344,
	.4,		3.9996,
	0.21
	
		//1114

};

GAUSS_ISOTOPE SI35{

	3.424e+1,	9.273e-1,
	1.8449e+2,	2.419,
	.45,		4.0596,
	0.0292

};



//-------------------------------------------------------------------------------------------------------------------------------------------

#endif

