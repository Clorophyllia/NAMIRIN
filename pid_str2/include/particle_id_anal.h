#pragma once

#define PID_ANAL_HD

#include "midas.h"
#include "rmidas.h"
#include <stdio.h>

#include "detector_conf.h"



/*-- Module declaration --------------------------------------------*/
INT PID_ONLINE(EVENT_HEADER *, void *);
INT PID_INIT(void);
INT PID_BOR(INT run_number);
INT PID_EOR(INT run_number);

typedef struct {
   double gain;
} PID_CAL;


#define PID_ONLINE_STR(_name) const char *_name[] = {\
"[.]",\
"GAIN          = DOUBLE : 0.025",\
"",\
NULL };


extern RUNINFO runinfo;

PID_CAL pid_online;
PID_ONLINE_STR(pid_online_str);


ANA_MODULE PID = {
	PID_ANAL_HD,           /* module name           */
   "Chlorophylia",               /* author                */
   PID_ONLINE,                   /* event routine         */
   PID_BOR,               /* BOR routine           */
   PID_EOR,               /* EOR routine           */
   PID_INIT,              /* init routine          */
   NULL,                        /* exit routine          */
   &pid_online,             /* parameter structure   */
   sizeof(pid_online),      /* structure size        */
   pid_online_str,   /* initial parameters    */
};


#endif
