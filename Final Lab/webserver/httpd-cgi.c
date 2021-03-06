/**
 * \addtogroup httpd
 * @{
 */

/**
 * \file
 *         Web server script interface
 * \author
 *         Adam Dunkels <adam@sics.se>
 *
 */

/*
 * Copyright (c) 2001-2006, Adam Dunkels.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * This file is part of the uIP TCP/IP stack.
 *
 * $Id: httpd-cgi.c,v 1.2 2006/06/11 21:46:37 adam Exp $
 *
 */

#include "uip.h"
#include "psock.h"
#include "httpd.h"
#include "httpd-cgi.h"
#include "httpd-fs.h"
#include "../tasks.h"

#include <stdio.h>
#include <string.h>

HTTPD_CGI_CALL(rtos, "rtos-stats", rtos_stats );
HTTPD_CGI_CALL(io, "led-io", led_io );
HTTPD_CGI_CALL(header, "generate-header", generate_header);

static const struct httpd_cgi_call *calls[] = { &rtos, &io, &header, NULL };

extern RemoteData remoteData;

/*---------------------------------------------------------------------------*/
static
PT_THREAD(nullfunction(struct httpd_state *s, char *ptr))
{
  PSOCK_BEGIN(&s->sout);
  PSOCK_END(&s->sout);
}
/*---------------------------------------------------------------------------*/
// This function is called by httpd.c and returns the function to run
// Used to start getting our structs passed to generate_rtos_stats
httpd_cgifunction
httpd_cgi(char *name)
{
  const struct httpd_cgi_call **f;

  /* Find the matching name in the table, return the function. */
  for(f = calls; *f != NULL; ++f) {
    if(strncmp((*f)->name, name, strlen((*f)->name)) == 0) {
      return (*f)->function;
    }
  }
  return nullfunction;
}
/*---------------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
static const char closed[] =   /*  "CLOSED",*/
{0x43, 0x4c, 0x4f, 0x53, 0x45, 0x44, 0};
static const char syn_rcvd[] = /*  "SYN-RCVD",*/
{0x53, 0x59, 0x4e, 0x2d, 0x52, 0x43, 0x56,
 0x44,  0};
static const char syn_sent[] = /*  "SYN-SENT",*/
{0x53, 0x59, 0x4e, 0x2d, 0x53, 0x45, 0x4e,
 0x54,  0};
static const char established[] = /*  "ESTABLISHED",*/
{0x45, 0x53, 0x54, 0x41, 0x42, 0x4c, 0x49, 0x53, 0x48,
 0x45, 0x44, 0};
static const char fin_wait_1[] = /*  "FIN-WAIT-1",*/
{0x46, 0x49, 0x4e, 0x2d, 0x57, 0x41, 0x49,
 0x54, 0x2d, 0x31, 0};
static const char fin_wait_2[] = /*  "FIN-WAIT-2",*/
{0x46, 0x49, 0x4e, 0x2d, 0x57, 0x41, 0x49,
 0x54, 0x2d, 0x32, 0};
static const char closing[] = /*  "CLOSING",*/
{0x43, 0x4c, 0x4f, 0x53, 0x49,
 0x4e, 0x47, 0};
static const char time_wait[] = /*  "TIME-WAIT,"*/
{0x54, 0x49, 0x4d, 0x45, 0x2d, 0x57, 0x41,
 0x49, 0x54, 0};
static const char last_ack[] = /*  "LAST-ACK"*/
{0x4c, 0x41, 0x53, 0x54, 0x2d, 0x41, 0x43,
 0x4b, 0};

static const char *states[] = {
  closed,
  syn_rcvd,
  syn_sent,
  established,
  fin_wait_1,
  fin_wait_2,
  closing,
  time_wait,
  last_ack};


/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/

extern void vTaskList( signed char *pcWriteBuffer );
static char cCountBuf[ 128 ];
static char docpatient[ 128 ];
extern DisplayData displayData;   // Temporary maybe?
// ALL OF OUR DATA STRUCTURE STUFF GOES HERE
long lRefreshCount = 0;

extern long numWarnings;
extern Bool bpOutOfRange;
extern Bool tempOutOfRange;
extern Bool pulseOutOfRange;
extern Bool batteryOutOfRange;

char blinkStart[] = "<blink>";
char blinkEnd[] = "</blink>";
char empty[] = " ";

static unsigned short
generate_rtos_stats(void *arg)
{
        DisplayData* displayDataPtr = &displayData;
        char* temperature = displayDataPtr->tempCorrectedBuf->headPtr;
        char* pulse = displayDataPtr->prCorrectedBuf->headPtr;
        char* battery = displayDataPtr->battCorrected->headPtr;
        char* ekg = displayDataPtr->ekgCorrectedBuf->headPtr;
        char* systolic = displayDataPtr->systolicPressCorrectedBuf->headPtr;
        char* diastolic = displayDataPtr->diastolicPressCorrectedBuf->headPtr;
        char* bpBlink;
        char* bpBlinkE;
        char* tBlink;
        char* tBlinkE;
        char* pBlink;
        char* pBlinkE;
        char* bBlink;
        char* bBlinkE;
        if (bpOutOfRange) {
          bpBlink = blinkStart;
          bpBlinkE = blinkEnd;
        } else {
          bpBlink = empty;
          bpBlinkE = empty;
        }
        if (tempOutOfRange) {
          tBlink = blinkStart;
          tBlinkE = blinkEnd;
        } else {
          tBlink = empty;
          tBlinkE = empty;
        }
        if (pulseOutOfRange) {
          pBlink = blinkStart;
          pBlinkE = blinkEnd;
        } else {
          pBlink = empty;
          pBlinkE = empty;
        }
        if (batteryOutOfRange) {
          bBlink = blinkStart;
          bBlinkE = blinkEnd;
        } else {
          bBlink = empty;
          bBlinkE = empty;
        }
        
        unsigned short* myScroll = displayDataPtr->scroll;
	lRefreshCount++;
        
	sprintf( cCountBuf, "<br />Refresh count = %d<br />Warnings in the last 8 hours: %d", lRefreshCount, numWarnings );
        sprintf( uip_appdata, "<ul><li>%sTemperature: %s C%s</li><li>%sSystolicPressure: %s mmHg%s</li><li>%sDiastolicPressure: %s mmHg%s</li><li>%sPulse Rate: %s BPM%s</li><li>EKG: %s Hz</li><li>%sBattery %s %%%s</li></ul>",
                  tBlink, temperature, tBlinkE, bpBlink, systolic, bpBlinkE, bpBlink, diastolic, bpBlinkE, pBlink, pulse, pBlinkE, ekg, bBlink, battery, bBlinkE);
	strcat( uip_appdata, cCountBuf );
        strcat( uip_appdata, docpatient);

	return strlen( uip_appdata );
}
/*---------------------------------------------------------------------------*/


static
PT_THREAD(rtos_stats(struct httpd_state *s, char *ptr))
{
  PSOCK_BEGIN(&s->sout);
  PSOCK_GENERATOR_SEND(&s->sout, generate_rtos_stats, NULL);
  PSOCK_END(&s->sout);
}


/*---------------------------------------------------------------------------*/

// COMMAND INPUT GOES HERE
extern CommandData commandData;
static unsigned short generate_io_state( void *arg )
{
  
  xSemaphoreTake(*(remoteData.commandData->commandSemaphorePtr),500); // wait for command task to finish
  sprintf( uip_appdata, "<p>%s</p>",remoteData.commandData->transmit);
  xSemaphoreGive(*(remoteData.commandData->commandSemaphorePtr));
  return strlen( uip_appdata );
}
/*---------------------------------------------------------------------------*/

static unsigned short generate_header_output( void *arg )
{
  
  xSemaphoreTake(*(remoteData.commandData->commandSemaphorePtr),500); // wait for command task to finish
  sprintf( uip_appdata, "<p>Doctor: %s &nbsp;&nbsp;&nbsp;Patient: %s</p><hr>",remoteData.docName, remoteData.patName);
  xSemaphoreGive(*(remoteData.commandData->commandSemaphorePtr));
  return strlen( uip_appdata );
}
/*---------------------------------------------------------------------------*/


static PT_THREAD(led_io(struct httpd_state *s, char *ptr))
{
  PSOCK_BEGIN(&s->sout);
  PSOCK_GENERATOR_SEND(&s->sout, generate_io_state, NULL);
  PSOCK_END(&s->sout);
}
static
PT_THREAD(generate_header(struct httpd_state *s, char *ptr))
{
  PSOCK_BEGIN(&s->sout);
  PSOCK_GENERATOR_SEND(&s->sout, generate_header_output, NULL);
  PSOCK_END(&s->sout);
}
/** @} */






