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
 * \file
 *   This file contains the source code for the Sample App.
 */

/*
** Include Files:
*/
#include "{{cookiecutter.__app_filename_slug}}_events.h"
#include "{{cookiecutter.__app_filename_slug}}_version.h"
#include "{{cookiecutter.__app_filename_slug}}.h"
#include "{{cookiecutter.__app_filename_slug}}_table.h"

/* The sample_lib module provides the SAMPLE_LIB_Function() prototype */
#include <string.h>
#include "sample_lib.h"

/*
** global data
*/
{{cookiecutter.__app_slug_uc}}_Data_t {{cookiecutter.__app_slug_uc}}_Data;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * *  * * * * **/
/*                                                                            */
/* Application entry point and main process loop                              */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * *  * * * * **/
void {{cookiecutter.__app_slug_uc}}_Main(void)
{
    int32            status;
    CFE_SB_Buffer_t *SBBufPtr;

    /*
    ** Create the first Performance Log entry
    */
    CFE_ES_PerfLogEntry({{cookiecutter.__app_slug_uc}}_PERF_ID);

    /*
    ** Perform application specific initialization
    ** If the Initialization fails, set the RunStatus to
    ** CFE_ES_RunStatus_APP_ERROR and the App will not enter the RunLoop
    */
    status = {{cookiecutter.__app_slug_uc}}_Init();
    if (status != CFE_SUCCESS)
    {
        {{cookiecutter.__app_slug_uc}}_Data.RunStatus = CFE_ES_RunStatus_APP_ERROR;
    }

    /*
    ** SAMPLE Runloop
    */
    while (CFE_ES_RunLoop(&{{cookiecutter.__app_slug_uc}}_Data.RunStatus) == true)
    {
        /*
        ** Performance Log Exit Stamp
        */
        CFE_ES_PerfLogExit({{cookiecutter.__app_slug_uc}}_PERF_ID);

        /* Pend on receipt of command packet */
        status = CFE_SB_ReceiveBuffer(&SBBufPtr, {{cookiecutter.__app_slug_uc}}_Data.CommandPipe, CFE_SB_PEND_FOREVER);

        /*
        ** Performance Log Entry Stamp
        */
        CFE_ES_PerfLogEntry({{cookiecutter.__app_slug_uc}}_PERF_ID);

        if (status == CFE_SUCCESS)
        {
            {{cookiecutter.__app_slug_uc}}_ProcessCommandPacket(SBBufPtr);
        }
        else
        {
            CFE_EVS_SendEvent({{cookiecutter.__app_slug_uc}}_PIPE_ERR_EID, CFE_EVS_EventType_ERROR,
                              "SAMPLE APP: SB Pipe Read Error, App Will Exit");

            {{cookiecutter.__app_slug_uc}}_Data.RunStatus = CFE_ES_RunStatus_APP_ERROR;
        }
    }

    /*
    ** Performance Log Exit Stamp
    */
    CFE_ES_PerfLogExit({{cookiecutter.__app_slug_uc}}_PERF_ID);

    CFE_ES_ExitApp({{cookiecutter.__app_slug_uc}}_Data.RunStatus);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  */
/*                                                                            */
/* Initialization                                                             */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
int32 {{cookiecutter.__app_slug_uc}}_Init(void)
{
    int32 status;

    {{cookiecutter.__app_slug_uc}}_Data.RunStatus = CFE_ES_RunStatus_APP_RUN;

    /*
    ** Initialize app command execution counters
    */
    {{cookiecutter.__app_slug_uc}}_Data.CmdCounter = 0;
    {{cookiecutter.__app_slug_uc}}_Data.ErrCounter = 0;

    /*
    ** Initialize app configuration data
    */
    {{cookiecutter.__app_slug_uc}}_Data.PipeDepth = {{cookiecutter.__app_slug_uc}}_PIPE_DEPTH;

    strncpy({{cookiecutter.__app_slug_uc}}_Data.PipeName, "{{cookiecutter.__app_slug_uc}}_CMD_PIPE", sizeof({{cookiecutter.__app_slug_uc}}_Data.PipeName));
    {{cookiecutter.__app_slug_uc}}_Data.PipeName[sizeof({{cookiecutter.__app_slug_uc}}_Data.PipeName) - 1] = 0;

    /*
    ** Register the events
    */
    status = CFE_EVS_Register(NULL, 0, CFE_EVS_EventFilter_BINARY);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Sample App: Error Registering Events, RC = 0x%08lX\n", (unsigned long)status);
        return status;
    }

    /*
    ** Initialize housekeeping packet (clear user data area).
    */
    CFE_MSG_Init(CFE_MSG_PTR({{cookiecutter.__app_slug_uc}}_Data.HkTlm.TelemetryHeader), CFE_SB_ValueToMsgId({{cookiecutter.__app_slug_uc}}_HK_TLM_MID),
                 sizeof({{cookiecutter.__app_slug_uc}}_Data.HkTlm));

    /*
    ** Create Software Bus message pipe.
    */
    status = CFE_SB_CreatePipe(&{{cookiecutter.__app_slug_uc}}_Data.CommandPipe, {{cookiecutter.__app_slug_uc}}_Data.PipeDepth, {{cookiecutter.__app_slug_uc}}_Data.PipeName);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Sample App: Error creating pipe, RC = 0x%08lX\n", (unsigned long)status);
        return status;
    }

    /*
    ** Subscribe to Housekeeping request commands
    */
    status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId({{cookiecutter.__app_slug_uc}}_SEND_HK_MID), {{cookiecutter.__app_slug_uc}}_Data.CommandPipe);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Sample App: Error Subscribing to HK request, RC = 0x%08lX\n", (unsigned long)status);
        return status;
    }

    /*
    ** Subscribe to ground command packets
    */
    status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId({{cookiecutter.__app_slug_uc}}_CMD_MID), {{cookiecutter.__app_slug_uc}}_Data.CommandPipe);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Sample App: Error Subscribing to Command, RC = 0x%08lX\n", (unsigned long)status);

        return status;
    }

    /*
    ** Register Table(s)
    */
    status = CFE_TBL_Register(&{{cookiecutter.__app_slug_uc}}_Data.TblHandles[0], "SampleAppTable", sizeof({{cookiecutter.__app_slug_uc}}_Table_t),
                              CFE_TBL_OPT_DEFAULT, {{cookiecutter.__app_slug_uc}}_TblValidationFunc);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Sample App: Error Registering Table, RC = 0x%08lX\n", (unsigned long)status);

        return status;
    }
    else
    {
        status = CFE_TBL_Load({{cookiecutter.__app_slug_uc}}_Data.TblHandles[0], CFE_TBL_SRC_FILE, {{cookiecutter.__app_slug_uc}}_TABLE_FILE);
    }

    CFE_EVS_SendEvent({{cookiecutter.__app_slug_uc}}_STARTUP_INF_EID, CFE_EVS_EventType_INFORMATION, "SAMPLE App Initialized.%s",
                      {{cookiecutter.__app_slug_uc}}_VERSION_STRING);

    return CFE_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/*  Purpose:                                                                  */
/*     This routine will process any packet that is received on the SAMPLE    */
/*     command pipe.                                                          */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
void {{cookiecutter.__app_slug_uc}}_ProcessCommandPacket(CFE_SB_Buffer_t *SBBufPtr)
{
    CFE_SB_MsgId_t MsgId = CFE_SB_INVALID_MSG_ID;

    CFE_MSG_GetMsgId(&SBBufPtr->Msg, &MsgId);

    switch (CFE_SB_MsgIdToValue(MsgId))
    {
        case {{cookiecutter.__app_slug_uc}}_CMD_MID:
            {{cookiecutter.__app_slug_uc}}_ProcessGroundCommand(SBBufPtr);
            break;

        case {{cookiecutter.__app_slug_uc}}_SEND_HK_MID:
            {{cookiecutter.__app_slug_uc}}_ReportHousekeeping((CFE_MSG_CommandHeader_t *)SBBufPtr);
            break;

        default:
            CFE_EVS_SendEvent({{cookiecutter.__app_slug_uc}}_INVALID_MSGID_ERR_EID, CFE_EVS_EventType_ERROR,
                              "SAMPLE: invalid command packet,MID = 0x%x", (unsigned int)CFE_SB_MsgIdToValue(MsgId));
            break;
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* SAMPLE ground commands                                                     */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
void {{cookiecutter.__app_slug_uc}}_ProcessGroundCommand(CFE_SB_Buffer_t *SBBufPtr)
{
    CFE_MSG_FcnCode_t CommandCode = 0;

    CFE_MSG_GetFcnCode(&SBBufPtr->Msg, &CommandCode);

    /*
    ** Process "known" SAMPLE app ground commands
    */
    switch (CommandCode)
    {
        case {{cookiecutter.__app_slug_uc}}_NOOP_CC:
            if ({{cookiecutter.__app_slug_uc}}_VerifyCmdLength(&SBBufPtr->Msg, sizeof({{cookiecutter.__app_slug_uc}}_NoopCmd_t)))
            {
                {{cookiecutter.__app_slug_uc}}_Noop(({{cookiecutter.__app_slug_uc}}_NoopCmd_t *)SBBufPtr);
            }

            break;

        case {{cookiecutter.__app_slug_uc}}_RESET_COUNTERS_CC:
            if ({{cookiecutter.__app_slug_uc}}_VerifyCmdLength(&SBBufPtr->Msg, sizeof({{cookiecutter.__app_slug_uc}}_ResetCountersCmd_t)))
            {
                {{cookiecutter.__app_slug_uc}}_ResetCounters(({{cookiecutter.__app_slug_uc}}_ResetCountersCmd_t *)SBBufPtr);
            }

            break;

        case {{cookiecutter.__app_slug_uc}}_PROCESS_CC:
            if ({{cookiecutter.__app_slug_uc}}_VerifyCmdLength(&SBBufPtr->Msg, sizeof({{cookiecutter.__app_slug_uc}}_ProcessCmd_t)))
            {
                {{cookiecutter.__app_slug_uc}}_Process(({{cookiecutter.__app_slug_uc}}_ProcessCmd_t *)SBBufPtr);
            }

            break;

        /* default case already found during FC vs length test */
        default:
            CFE_EVS_SendEvent({{cookiecutter.__app_slug_uc}}_COMMAND_ERR_EID, CFE_EVS_EventType_ERROR,
                              "Invalid ground command code: CC = %d", CommandCode);
            break;
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/*  Purpose:                                                                  */
/*         This function is triggered in response to a task telemetry request */
/*         from the housekeeping task. This function will gather the Apps     */
/*         telemetry, packetize it and send it to the housekeeping task via   */
/*         the software bus                                                   */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
int32 {{cookiecutter.__app_slug_uc}}_ReportHousekeeping(const CFE_MSG_CommandHeader_t *Msg)
{
    int i;

    /*
    ** Get command execution counters...
    */
    {{cookiecutter.__app_slug_uc}}_Data.HkTlm.Payload.CommandErrorCounter = {{cookiecutter.__app_slug_uc}}_Data.ErrCounter;
    {{cookiecutter.__app_slug_uc}}_Data.HkTlm.Payload.CommandCounter      = {{cookiecutter.__app_slug_uc}}_Data.CmdCounter;

    /*
    ** Send housekeeping telemetry packet...
    */
    CFE_SB_TimeStampMsg(CFE_MSG_PTR({{cookiecutter.__app_slug_uc}}_Data.HkTlm.TelemetryHeader));
    CFE_SB_TransmitMsg(CFE_MSG_PTR({{cookiecutter.__app_slug_uc}}_Data.HkTlm.TelemetryHeader), true);

    /*
    ** Manage any pending table loads, validations, etc.
    */
    for (i = 0; i < {{cookiecutter.__app_slug_uc}}_NUMBER_OF_TABLES; i++)
    {
        CFE_TBL_Manage({{cookiecutter.__app_slug_uc}}_Data.TblHandles[i]);
    }

    return CFE_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* SAMPLE NOOP commands                                                       */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
int32 {{cookiecutter.__app_slug_uc}}_Noop(const {{cookiecutter.__app_slug_uc}}_NoopCmd_t *Msg)
{
    {{cookiecutter.__app_slug_uc}}_Data.CmdCounter++;

    CFE_EVS_SendEvent({{cookiecutter.__app_slug_uc}}_COMMANDNOP_INF_EID, CFE_EVS_EventType_INFORMATION, "SAMPLE: NOOP command %s",
                      {{cookiecutter.__app_slug_uc}}_VERSION);

    return CFE_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/*  Purpose:                                                                  */
/*         This function resets all the global counter variables that are     */
/*         part of the task telemetry.                                        */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
int32 {{cookiecutter.__app_slug_uc}}_ResetCounters(const {{cookiecutter.__app_slug_uc}}_ResetCountersCmd_t *Msg)
{
    {{cookiecutter.__app_slug_uc}}_Data.CmdCounter = 0;
    {{cookiecutter.__app_slug_uc}}_Data.ErrCounter = 0;

    CFE_EVS_SendEvent({{cookiecutter.__app_slug_uc}}_COMMANDRST_INF_EID, CFE_EVS_EventType_INFORMATION, "SAMPLE: RESET command");

    return CFE_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/*  Purpose:                                                                  */
/*         This function Process Ground Station Command                       */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
int32 {{cookiecutter.__app_slug_uc}}_Process(const {{cookiecutter.__app_slug_uc}}_ProcessCmd_t *Msg)
{
    int32               status;
    {{cookiecutter.__app_slug_uc}}_Table_t *TblPtr;
    const char *        TableName = "{{cookiecutter.__app_slug_uc}}.SampleAppTable";

    /* Sample Use of Table */

    status = CFE_TBL_GetAddress((void *)&TblPtr, {{cookiecutter.__app_slug_uc}}_Data.TblHandles[0]);

    if (status < CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Sample App: Fail to get table address: 0x%08lx", (unsigned long)status);
        return status;
    }

    CFE_ES_WriteToSysLog("Sample App: Table Value 1: %d  Value 2: %d", TblPtr->Int1, TblPtr->Int2);

    {{cookiecutter.__app_slug_uc}}_GetCrc(TableName);

    status = CFE_TBL_ReleaseAddress({{cookiecutter.__app_slug_uc}}_Data.TblHandles[0]);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Sample App: Fail to release table address: 0x%08lx", (unsigned long)status);
        return status;
    }

    /* Invoke a function provided by SAMPLE_APP_LIB */
    SAMPLE_LIB_Function();

    return CFE_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* Verify command packet length                                               */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
bool {{cookiecutter.__app_slug_uc}}_VerifyCmdLength(CFE_MSG_Message_t *MsgPtr, size_t ExpectedLength)
{
    bool              result       = true;
    size_t            ActualLength = 0;
    CFE_SB_MsgId_t    MsgId        = CFE_SB_INVALID_MSG_ID;
    CFE_MSG_FcnCode_t FcnCode      = 0;

    CFE_MSG_GetSize(MsgPtr, &ActualLength);

    /*
    ** Verify the command packet length.
    */
    if (ExpectedLength != ActualLength)
    {
        CFE_MSG_GetMsgId(MsgPtr, &MsgId);
        CFE_MSG_GetFcnCode(MsgPtr, &FcnCode);

        CFE_EVS_SendEvent({{cookiecutter.__app_slug_uc}}_LEN_ERR_EID, CFE_EVS_EventType_ERROR,
                          "Invalid Msg length: ID = 0x%X,  CC = %u, Len = %u, Expected = %u",
                          (unsigned int)CFE_SB_MsgIdToValue(MsgId), (unsigned int)FcnCode, (unsigned int)ActualLength,
                          (unsigned int)ExpectedLength);

        result = false;

        {{cookiecutter.__app_slug_uc}}_Data.ErrCounter++;
    }

    return result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Verify contents of First Table buffer contents                  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 {{cookiecutter.__app_slug_uc}}_TblValidationFunc(void *TblData)
{
    int32               ReturnCode = CFE_SUCCESS;
    {{cookiecutter.__app_slug_uc}}_Table_t *TblDataPtr = ({{cookiecutter.__app_slug_uc}}_Table_t *)TblData;

    /*
    ** Sample Table Validation
    */
    if (TblDataPtr->Int1 > {{cookiecutter.__app_slug_uc}}_TBL_ELEMENT_1_MAX)
    {
        /* First element is out of range, return an appropriate error code */
        ReturnCode = {{cookiecutter.__app_slug_uc}}_TABLE_OUT_OF_RANGE_ERR_CODE;
    }

    return ReturnCode;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Output CRC                                                      */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void {{cookiecutter.__app_slug_uc}}_GetCrc(const char *TableName)
{
    int32          status;
    uint32         Crc;
    CFE_TBL_Info_t TblInfoPtr;

    status = CFE_TBL_GetInfo(&TblInfoPtr, TableName);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Sample App: Error Getting Table Info");
    }
    else
    {
        Crc = TblInfoPtr.Crc;
        CFE_ES_WriteToSysLog("Sample App: CRC: 0x%08lX\n\n", (unsigned long)Crc);
    }
}
