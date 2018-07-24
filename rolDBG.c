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
#include "c965Lib.h"
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

extern UINT32 c965IntLevel;
extern UINT32 c965IntVec;

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

  rocDownload();

  return OK;
}

int
rolDBGPrestart()
{
  if(vmeIntDisconnect(c965IntLevel) != OK)
    {
      printf("ERROR disconnecting Interrupt\n");
    }
  if(vmeIntConnect(c965IntVec,c965IntLevel,rolDBGTrigger,1) != OK)
    {
      printf("ERROR in intConnect()\n");
    }

  rocPrestart();

  return OK;
}

int
rolDBGGo()
{
  rocGo();

  return OK;
}

int
rolDBGEnd()
{

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
