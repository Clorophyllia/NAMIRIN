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

struct g_isotopes{

        double ade_mean;         double ade_sigm;
        double atof_mean;        double atof_sigm;
        double abr_intv;         double abr_intc;
        double ainten;


};



typedef struct {

	double de_mean;		double de_sigm;
	double tof_mean;	double tof_sigm;
	double br_intv;		double br_intc;
	double inten;

} GAUSS_ISOTOPE;


//------------------* list of LISEPP (Aprox.) Calulated elements for the  40Ar + 12C -> 38S system setup *-------------------------------//

GAUSS_ISOTOPE *S37{

	4.776e+1,	6.520e-1,
	1.777e+2,	8.637e-1,
	.3,		174.62,
	3.09

};

GAUSS_ISOTOPE *S38{

	4.231e+1,	8.598e-1,
	1.808e+2,	1.813e+2,
	.029,		179.08,
	1.56

};

GAUSS_ISOTOPE *S39{

	4.319e+1,	1.078,
	1.830e+2,	2.191,
	.37,		183.58,
	0.0168

};

GAUSS_ISOTOPE *P35{

	3.638e+1,	7.342e-1,
	1.783e+2,	1.382,
	0.4,		176.,
	1.58

};

GAUSS_ISOTOPE *P36{

	3.731e+1,	9.435e-1,
	1.8168e+2,	2.2,
	.34,		180.65,
	0.536

};

GAUSS_ISOTOPE *P37{

	3.837e+1,	1.01,
	1.841e+2,	2.274,
	.27,		185.67,
	0.104

};

GAUSS_ISOTOPE *P38{

	4.e+1,		1.646,
	1.887e+2,	2.361,
	.31,		190.42,
	0.000092

};

GAUSS_ISOTOPE *SI33{

	3.186e+1,	7.957e-1,
	1.788e+2,	2.01,
	.36,		177.71,
	0.82

};

GAUSS_ISOTOPE *SI34{

	3.288e+1,	8.869e-1,
	1.822e+2,	2.344,
	.4,		182.82,
	0.21

};

GAUSS_ISOTOPE *SI35{

	3.424e+1,	9.273e-1,
	1.8662e+2,	2.419,
	.45,		187.94,
	0.0292

};



//-------------------------------------------------------------------------------------------------------------------------------------------

#endif

