#define SIZE_OF_OTV 6
#define FOD             40.257
#define LF3		36.7718
#define LF2		23.0018		// L of F2ppac(tof_i) [m]


INT PID_ONLINE(EVENT_HEADER *pheader, void *pevent);
INT PID_INIT(void);
INT PID_BOR(INT run_number);
INT PID_EOR(INT run_number);


/*typedef struct  _OTM {

	//char def_struct[50];
	double f0_2_f1;
	double f1_2_f2;
	double f2_2_f3;
	double f3_2_f4;
	double f4_2_f5;
	double f0_2_f2;
	double f0_2_f3;
	double f1_2_f3;

} TRANSFER_MATRIX;
*/
/*static const double *tmr[] = {
	1.01, 0.0001, -0.98, -0.0001, 0.0001, 2.05
}*/

