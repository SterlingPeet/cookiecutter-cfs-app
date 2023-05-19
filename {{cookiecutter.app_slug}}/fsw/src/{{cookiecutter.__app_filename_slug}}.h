/************************************************************************
 * NASA Docket No. GSC-18,719-1, and identified as “core Flight System: Bootes”
 *
 * Copyright (c) 2020 United States Government as represented by the
 * Administrator of the National Aeronautics and Space Administration.
 * All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ************************************************************************/

/**
 * @file
 *
 * Main header file for the SAMPLE application
 */

#ifndef {{cookiecutter.__app_slug_uc}}_H
#define {{cookiecutter.__app_slug_uc}}_H

/*
** Required header files.
*/
#include "cfe.h"
#include "cfe_error.h"
#include "cfe_evs.h"
#include "cfe_sb.h"
#include "cfe_es.h"

#include "sample_app_perfids.h"
#include "sample_app_msgids.h"
#include "sample_app_msg.h"

/***********************************************************************/
#define {{cookiecutter.__app_slug_uc}}_PIPE_DEPTH 32 /* Depth of the Command Pipe for Application */

#define {{cookiecutter.__app_slug_uc}}_NUMBER_OF_TABLES 1 /* Number of Table(s) */

/* Define filenames of default data images for tables */
#define SAMPLE_APP_TABLE_FILE "/cf/sample_app_tbl.tbl"

#define {{cookiecutter.__app_slug_uc}}_TABLE_OUT_OF_RANGE_ERR_CODE -1

#define {{cookiecutter.__app_slug_uc}}_TBL_ELEMENT_1_MAX 10
/************************************************************************
** Type Definitions
*************************************************************************/

/*
** Global Data
*/
typedef struct
{
    /*
    ** Command interface counters...
    */
    uint8 CmdCounter;
    uint8 ErrCounter;

    /*
    ** Housekeeping telemetry packet...
    */
    {{cookiecutter.__app_slug_uc}}_HkTlm_t HkTlm;

    /*
    ** Run Status variable used in the main processing loop
    */
    uint32 RunStatus;

    /*
    ** Operational data (not reported in housekeeping)...
    */
    CFE_SB_PipeId_t CommandPipe;

    /*
    ** Initialization data (not reported in housekeeping)...
    */
    char   PipeName[CFE_MISSION_MAX_API_LEN];
    uint16 PipeDepth;

    CFE_TBL_Handle_t TblHandles[{{cookiecutter.__app_slug_uc}}_NUMBER_OF_TABLES];
} {{cookiecutter.__app_slug_uc}}_Data_t;

/****************************************************************************/
/*
** Local function prototypes.
**
** Note: Except for the entry point ({{cookiecutter.__app_slug_uc}}_Main), these
**       functions are not called from any other source module.
*/
void  {{cookiecutter.__app_slug_uc}}_Main(void);
int32 {{cookiecutter.__app_slug_uc}}_Init(void);
void  {{cookiecutter.__app_slug_uc}}_ProcessCommandPacket(CFE_SB_Buffer_t *SBBufPtr);
void  {{cookiecutter.__app_slug_uc}}_ProcessGroundCommand(CFE_SB_Buffer_t *SBBufPtr);
int32 {{cookiecutter.__app_slug_uc}}_ReportHousekeeping(const CFE_MSG_CommandHeader_t *Msg);
int32 {{cookiecutter.__app_slug_uc}}_ResetCounters(const {{cookiecutter.__app_slug_uc}}_ResetCountersCmd_t *Msg);
int32 {{cookiecutter.__app_slug_uc}}_Process(const {{cookiecutter.__app_slug_uc}}_ProcessCmd_t *Msg);
int32 {{cookiecutter.__app_slug_uc}}_Noop(const {{cookiecutter.__app_slug_uc}}_NoopCmd_t *Msg);
void  {{cookiecutter.__app_slug_uc}}_GetCrc(const char *TableName);

int32 {{cookiecutter.__app_slug_uc}}_TblValidationFunc(void *TblData);

bool {{cookiecutter.__app_slug_uc}}_VerifyCmdLength(CFE_MSG_Message_t *MsgPtr, size_t ExpectedLength);

#endif /* {{cookiecutter.__app_slug_uc}}_H */
