/*
 * File:
 *    rolDBG.c
 *
 * Description:
 *    program to debug CODA readout lists.
 *
 *
 */


#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "jvme.h"
#include "tiLib.h"
/* #include "remexLib.h" */

void daLogMsg(){};
int bigendian_out;
void partCreate(){};
void end(){};

extern void rocDownload();
extern void rocPrestart();
extern void rocGo();
extern void rocEnd();
extern void rocTrigger(int arg);
extern void rocCleanup();

void rolDBGTrigger(int arg);

extern int tiA32Base;

DMA_MEM_ID vmeIN,vmeOUT;
extern DMANODE *the_event;
extern unsigned int *dma_dabufp;

/* volatile struct TI_A24RegStruct  *TIp=NULL; */
int tsLiveCalc = 0;
void tsLiveFunc() {}

int
rolDBGDownload()
{
  int stat;

  /* remexSetCmsgServer("dafarm28"); */
  /* remexInit(NULL,1); */

  vmeOpenDefaultWindows();

  /* INIT dmaPList */

  dmaPFreeAll();
  vmeIN  = dmaPCreate("vmeIN",1024,4,0);
  vmeOUT = dmaPCreate("vmeOUT",0,0,0);

  dmaPStatsAll();

  dmaPReInitAll();

  tiA32Base=0x08000000;
  tiSetFiberLatencyOffset_preInit(0x20);

  tiInit(0,TI_READOUT_EXT_POLL,TI_INIT_SKIP_FIRMWARE_CHECK);

  tiCheckAddresses();

  rocDownload();

  tiDisableVXSSignals();

  tiClockReset();
  taskDelay(2);
  tiTrigLinkReset();

  return OK;
}

int
rolDBGPrestart()
{
  tiIntConnect(TI_INT_VEC, rolDBGTrigger,0);

  rocPrestart();

  printf("%s: Sending sync as TI master\n",__FUNCTION__);
  sleep(1);
  tiSyncReset(1);
  taskDelay(2);

  return OK;
}

int
rolDBGGo()
{
  rocGo();

  tiSetTriggerSource(TI_TRIGGER_PULSER);

  tiIntEnable(1);
  tiStatus(1);

  tiSetRandomTrigger(1,0x7);

  return OK;
}

int
rolDBGEnd()
{
  tiDisableTriggerSource(1);
  tiDisableRandomTrigger();

  tiIntDisable();
  tiIntDisconnect();


  rocEnd();

  return OK;
}

void
rolDBGTrigger(int arg)
{
  int dCnt, len=0,idata;
  DMANODE *outEvent;
  int tibready=0, timeout=0;
  int printout = 1000;
  int dataCheck=0;

  unsigned int intCount = 0;

  GETEVENT(vmeIN, intCount);

  tibready = tiBReady();
  if(tibready==ERROR)
    {
      printf("%s: ERROR: tiIntPoll returned ERROR.\n",__FUNCTION__);
      return;
    }
  if(tibready==0 && timeout<100)
    {
      printf("NOT READY!\n");
      tibready=tiBReady();
      timeout++;
    }

  if(timeout>=100)
    {
      printf("TIMEOUT!\n");
      return;
    }

  rocTrigger(intCount);

  PUTEVENT(vmeOUT);

  outEvent = dmaPGetItem(vmeOUT);
  dmaPFreeItem(outEvent);

}


int
rolDBGCleanup()
{
  rocCleanup();
  return OK;
}

int
main(int argc, char *argv[])
{
  printf("<Enter> to Download\n");
  getchar();
  rolDBGDownload();

  printf("<Enter> to Prestart\n");
  getchar();
  rolDBGPrestart();

  printf("<Enter> to Go\n");
  getchar();
  rolDBGGo();

  printf("<Enter> to End\n");
  getchar();
  rolDBGEnd();


  exit(OK);
}

/*
  Local Variables:
  compile-command: "make -k -B rolDBG"
  End:
 */
