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

/*
** File: coveragetest_{{cookiecutter.__app_filename_slug}}.c
**
** Purpose:
** Coverage Unit Test cases for the SAMPLE Application
**
** Notes:
** This implements various test cases to exercise all code
** paths through all functions defined in the SAMPLE application.
**
** It is primarily focused at providing examples of the various
** stub configurations, hook functions, and wrapper calls that
** are often needed when coercing certain code paths through
** complex functions.
*/

/*
 * Includes
 */

{% if cookiecutter.library_dep == "yes" %}#include "{{cookiecutter.library_h_file}}" /* For {{cookiecutter.library_function}} */{% endif -%}
#include "{{cookiecutter.__app_filename_slug}}_coveragetest_common.h"
#include "ut_{{cookiecutter.__app_filename_slug}}.h"

/*
 * Unit test check event hook information
 */
typedef struct
{
    uint16      ExpectedEvent;
    uint32      MatchCount;
    const char *ExpectedFormat;
} UT_CheckEvent_t;

/*
 * An example hook function to check for a specific event.
 */
static int32 UT_CheckEvent_Hook(void *UserObj, int32 StubRetcode, uint32 CallCount, const UT_StubContext_t *Context,
                                va_list va)
{
    UT_CheckEvent_t *State = UserObj;
    uint16           EventId;
    const char *     Spec;

    /*
     * The CFE_EVS_SendEvent stub passes the EventID as the
     * first context argument.
     */
    if (Context->ArgCount > 0)
    {
        EventId = UT_Hook_GetArgValueByName(Context, "EventID", uint16);
        if (EventId == State->ExpectedEvent)
        {
            if (State->ExpectedFormat != NULL)
            {
                Spec = UT_Hook_GetArgValueByName(Context, "Spec", const char *);
                if (Spec != NULL)
                {
                    /*
                     * Example of how to validate the full argument set.
                     * ------------------------------------------------
                     *
                     * If really desired one can call something like:
                     *
                     * char TestText[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];
                     * vsnprintf(TestText, sizeof(TestText), Spec, va);
                     *
                     * And then compare the output (TestText) to the expected fully-rendered string.
                     *
                     * NOTE: While this can be done, use with discretion - This isn't really
                     * verifying that the FSW code unit generated the correct event text,
                     * rather it is validating what the system snprintf() library function
                     * produces when passed the format string and args.
                     *
                     * This type of check has been demonstrated to make tests very fragile,
                     * because it is influenced by many factors outside the control of the
                     * test case.
                     *
                     * __This derived string is not an actual output of the unit under test__
                     */
                    if (strcmp(Spec, State->ExpectedFormat) == 0)
                    {
                        ++State->MatchCount;
                    }
                }
            }
            else
            {
                ++State->MatchCount;
            }
        }
    }

    return 0;
}

/* Macro to get expected event name */
#define UT_CHECKEVENT_SETUP(Evt, ExpectedEvent, ExpectedFormat) \
    UT_CheckEvent_Setup_Impl(Evt, ExpectedEvent, #ExpectedEvent, ExpectedFormat)

/*
 * Helper function to set up for event checking
 * This attaches the hook function to CFE_EVS_SendEvent
 */
static void UT_CheckEvent_Setup_Impl(UT_CheckEvent_t *Evt, uint16 ExpectedEvent, const char *EventName,
                                     const char *ExpectedFormat)
{
    if (ExpectedFormat == NULL)
    {
        UtPrintf("CheckEvent will match: %s(%u)", EventName, ExpectedEvent);
    }
    else
    {
        UtPrintf("CheckEvent will match: %s(%u), \"%s\"", EventName, ExpectedEvent, ExpectedFormat);
    }
    memset(Evt, 0, sizeof(*Evt));
    Evt->ExpectedEvent  = ExpectedEvent;
    Evt->ExpectedFormat = ExpectedFormat;
    UT_SetVaHookFunction(UT_KEY(CFE_EVS_SendEvent), UT_CheckEvent_Hook, Evt);
}

/*
**********************************************************************************
**          TEST CASE FUNCTIONS
**********************************************************************************
*/

void Test_{{cookiecutter.__app_slug_uc}}_Main(void)
{
    CFE_SB_MsgId_t MsgId = CFE_SB_INVALID_MSG_ID;

    /*
     * Test Case For:
     * void {{cookiecutter.__app_slug_uc}}_Main( void )
     */

    UT_CheckEvent_t EventTest;

    /*
     * {{cookiecutter.__app_slug_uc}}_Main does not return a value,
     * but it has several internal decision points
     * that need to be exercised here.
     *
     * First call it in "nominal" mode where all
     * dependent calls should be successful by default.
     */
    {{cookiecutter.__app_slug_uc}}_Main();

    /*
     * Confirm that CFE_ES_ExitApp() was called at the end of execution
     */
    UtAssert_STUB_COUNT(CFE_ES_ExitApp, 1);

    /*
     * Now set up individual cases for each of the error paths.
     * The first is for {{cookiecutter.__app_slug_uc}}_Init().  As this is in the same
     * code unit, it is not a stub where the return code can be
     * easily set.  In order to get this to fail, an underlying
     * call needs to fail, and the error gets propagated through.
     * The call to CFE_EVS_Register is the first opportunity.
     * Any identifiable (non-success) return code should work.
     */
    UT_SetDeferredRetcode(UT_KEY(CFE_EVS_Register), 1, CFE_EVS_INVALID_PARAMETER);

    /*
     * Just call the function again.  It does not return
     * the value, so there is nothing to test for here directly.
     * However, it should show up in the coverage report that
     * the {{cookiecutter.__app_slug_uc}}_Init() failure path was taken.
     */
    {{cookiecutter.__app_slug_uc}}_Main();

    /*
     * This can validate that the internal "RunStatus" was
     * set to CFE_ES_RunStatus_APP_ERROR, by querying the struct directly.
     */
    UtAssert_UINT32_EQ({{cookiecutter.__app_slug_uc}}_Data.RunStatus, CFE_ES_RunStatus_APP_ERROR);

    /*
     * Note that CFE_ES_RunLoop returns a boolean value,
     * so in order to exercise the internal "while" loop,
     * it needs to return TRUE.  But this also needs to return
     * FALSE in order to get out of the loop, otherwise
     * it will stay there infinitely.
     *
     * The deferred retcode will accomplish this.
     */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RunLoop), 1, true);

    /* Set up buffer for command processing */
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);

    /*
     * Invoke again
     */
    {{cookiecutter.__app_slug_uc}}_Main();

    /*
     * Confirm that CFE_SB_ReceiveBuffer() (inside the loop) was called
     */
    UtAssert_STUB_COUNT(CFE_SB_ReceiveBuffer, 1);

    /*
     * Now also make the CFE_SB_ReceiveBuffer call fail,
     * to exercise that error path.  This sends an
     * event which can be checked with a hook function.
     */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RunLoop), 1, true);
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_ReceiveBuffer), 1, CFE_SB_PIPE_RD_ERR);
    UT_CHECKEVENT_SETUP(&EventTest, {{cookiecutter.__app_slug_uc}}_PIPE_ERR_EID, "SAMPLE APP: SB Pipe Read Error, App Will Exit");

    /*
     * Invoke again
     */
    {{cookiecutter.__app_slug_uc}}_Main();

    /*
     * Confirm that the event was generated
     */
    UtAssert_UINT32_EQ(EventTest.MatchCount, 1);
}

void Test_{{cookiecutter.__app_slug_uc}}_Init(void)
{
    /*
     * Test Case For:
     * int32 {{cookiecutter.__app_slug_uc}}_Init( void )
     */

    /* nominal case should return CFE_SUCCESS */
    UtAssert_INT32_EQ({{cookiecutter.__app_slug_uc}}_Init(), CFE_SUCCESS);

    /* trigger a failure for each of the sub-calls,
     * and confirm a write to syslog for each.
     * Note that this count accumulates, because the status
     * is _not_ reset between these test cases. */
    UT_SetDeferredRetcode(UT_KEY(CFE_EVS_Register), 1, CFE_EVS_INVALID_PARAMETER);
    UtAssert_INT32_EQ({{cookiecutter.__app_slug_uc}}_Init(), CFE_EVS_INVALID_PARAMETER);
    UtAssert_STUB_COUNT(CFE_ES_WriteToSysLog, 1);

    UT_SetDeferredRetcode(UT_KEY(CFE_SB_CreatePipe), 1, CFE_SB_BAD_ARGUMENT);
    UtAssert_INT32_EQ({{cookiecutter.__app_slug_uc}}_Init(), CFE_SB_BAD_ARGUMENT);
    UtAssert_STUB_COUNT(CFE_ES_WriteToSysLog, 2);

    UT_SetDeferredRetcode(UT_KEY(CFE_SB_Subscribe), 1, CFE_SB_BAD_ARGUMENT);
    UtAssert_INT32_EQ({{cookiecutter.__app_slug_uc}}_Init(), CFE_SB_BAD_ARGUMENT);
    UtAssert_STUB_COUNT(CFE_ES_WriteToSysLog, 3);

    UT_SetDeferredRetcode(UT_KEY(CFE_SB_Subscribe), 2, CFE_SB_BAD_ARGUMENT);
    UtAssert_INT32_EQ({{cookiecutter.__app_slug_uc}}_Init(), CFE_SB_BAD_ARGUMENT);
    UtAssert_STUB_COUNT(CFE_ES_WriteToSysLog, 4);

    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_Register), 1, CFE_TBL_ERR_INVALID_OPTIONS);
    UtAssert_INT32_EQ({{cookiecutter.__app_slug_uc}}_Init(), CFE_TBL_ERR_INVALID_OPTIONS);
    UtAssert_STUB_COUNT(CFE_ES_WriteToSysLog, 5);
}

void Test_{{cookiecutter.__app_slug_uc}}_ProcessCommandPacket(void)
{
    /*
     * Test Case For:
     * void {{cookiecutter.__app_slug_uc}}_ProcessCommandPacket
     */
    /* a buffer large enough for any command message */
    union
    {
        CFE_SB_Buffer_t      SBBuf;
        {{cookiecutter.__app_slug_uc}}_NoopCmd_t Noop;
    } TestMsg;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;
    size_t            MsgSize;
    UT_CheckEvent_t   EventTest;

    memset(&TestMsg, 0, sizeof(TestMsg));
    UT_CHECKEVENT_SETUP(&EventTest, {{cookiecutter.__app_slug_uc}}_INVALID_MSGID_ERR_EID, "SAMPLE: invalid command packet,MID = 0x%x");

    /*
     * The CFE_MSG_GetMsgId() stub uses a data buffer to hold the
     * message ID values to return.
     */
    TestMsgId = CFE_SB_ValueToMsgId({{cookiecutter.__app_slug_uc}}_CMD_MID);
    FcnCode   = {{cookiecutter.__app_slug_uc}}_NOOP_CC;
    MsgSize   = sizeof(TestMsg.Noop);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &MsgSize, sizeof(MsgSize), false);
    {{cookiecutter.__app_slug_uc}}_ProcessCommandPacket(&TestMsg.SBBuf);

    TestMsgId = CFE_SB_ValueToMsgId({{cookiecutter.__app_slug_uc}}_SEND_HK_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    {{cookiecutter.__app_slug_uc}}_ProcessCommandPacket(&TestMsg.SBBuf);

    /* invalid message id */
    TestMsgId = CFE_SB_INVALID_MSG_ID;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    {{cookiecutter.__app_slug_uc}}_ProcessCommandPacket(&TestMsg.SBBuf);

    /*
     * Confirm that the event was generated only _once_
     */
    UtAssert_UINT32_EQ(EventTest.MatchCount, 1);
}

void Test_{{cookiecutter.__app_slug_uc}}_ProcessGroundCommand(void)
{
    /*
     * Test Case For:
     * void {{cookiecutter.__app_slug_uc}}_ProcessGroundCommand
     */
    CFE_MSG_FcnCode_t FcnCode;
    size_t            Size;

    /* a buffer large enough for any command message */
    union
    {
        CFE_SB_Buffer_t               SBBuf;
        {{cookiecutter.__app_slug_uc}}_NoopCmd_t          Noop;
        {{cookiecutter.__app_slug_uc}}_ResetCountersCmd_t Reset;
        {{cookiecutter.__app_slug_uc}}_ProcessCmd_t       Process;
    } TestMsg;
    UT_CheckEvent_t EventTest;

    memset(&TestMsg, 0, sizeof(TestMsg));

    /*
     * call with each of the supported command codes
     * The CFE_MSG_GetFcnCode stub allows the code to be
     * set to whatever is needed.  There is no return
     * value here and the actual implementation of these
     * commands have separate test cases, so this just
     * needs to exercise the "switch" statement.
     */

    /* test dispatch of NOOP */
    FcnCode = {{cookiecutter.__app_slug_uc}}_NOOP_CC;
    Size    = sizeof(TestMsg.Noop);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_CHECKEVENT_SETUP(&EventTest, {{cookiecutter.__app_slug_uc}}_COMMANDNOP_INF_EID, NULL);

    {{cookiecutter.__app_slug_uc}}_ProcessGroundCommand(&TestMsg.SBBuf);

    UtAssert_UINT32_EQ(EventTest.MatchCount, 1);

    /* test dispatch of RESET */
    FcnCode = {{cookiecutter.__app_slug_uc}}_RESET_COUNTERS_CC;
    Size    = sizeof(TestMsg.Reset);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_CHECKEVENT_SETUP(&EventTest, {{cookiecutter.__app_slug_uc}}_COMMANDRST_INF_EID, NULL);

    {{cookiecutter.__app_slug_uc}}_ProcessGroundCommand(&TestMsg.SBBuf);

    UtAssert_UINT32_EQ(EventTest.MatchCount, 1);

    /* test dispatch of PROCESS */
    /* note this will end up calling {{cookiecutter.__app_slug_uc}}_Process(), and as such it needs to
     * avoid dereferencing a table which does not exist. */
    FcnCode = {{cookiecutter.__app_slug_uc}}_PROCESS_CC;
    Size    = sizeof(TestMsg.Process);
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetAddress), CFE_TBL_ERR_UNREGISTERED);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);

    {{cookiecutter.__app_slug_uc}}_ProcessGroundCommand(&TestMsg.SBBuf);

    /* test an invalid CC */
    FcnCode = 1000;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    UT_CHECKEVENT_SETUP(&EventTest, {{cookiecutter.__app_slug_uc}}_COMMAND_ERR_EID, "Invalid ground command code: CC = %d");
    {{cookiecutter.__app_slug_uc}}_ProcessGroundCommand(&TestMsg.SBBuf);

    /*
     * Confirm that the event was generated only _once_
     */
    UtAssert_UINT32_EQ(EventTest.MatchCount, 1);
}

void Test_{{cookiecutter.__app_slug_uc}}_ReportHousekeeping(void)
{
    /*
     * Test Case For:
     * void {{cookiecutter.__app_slug_uc}}_ReportHousekeeping( const CFE_SB_CmdHdr_t *Msg )
     */
    CFE_MSG_Message_t *MsgSend;
    CFE_MSG_Message_t *MsgTimestamp;
    CFE_SB_MsgId_t     MsgId = CFE_SB_ValueToMsgId({{cookiecutter.__app_slug_uc}}_SEND_HK_MID);

    /* Set message id to return so {{cookiecutter.__app_slug_uc}}_Housekeeping will be called */
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);

    /* Set up to capture send message address */
    UT_SetDataBuffer(UT_KEY(CFE_SB_TransmitMsg), &MsgSend, sizeof(MsgSend), false);

    /* Set up to capture timestamp message address */
    UT_SetDataBuffer(UT_KEY(CFE_SB_TimeStampMsg), &MsgTimestamp, sizeof(MsgTimestamp), false);

    /* Call unit under test, NULL pointer confirms command access is through APIs */
    {{cookiecutter.__app_slug_uc}}_ProcessCommandPacket((CFE_SB_Buffer_t *)NULL);

    /* Confirm message sent*/
    UtAssert_STUB_COUNT(CFE_SB_TransmitMsg, 1);
    UtAssert_ADDRESS_EQ(MsgSend, &{{cookiecutter.__app_slug_uc}}_Data.HkTlm);

    /* Confirm timestamp msg address */
    UtAssert_STUB_COUNT(CFE_SB_TimeStampMsg, 1);
    UtAssert_ADDRESS_EQ(MsgTimestamp, &{{cookiecutter.__app_slug_uc}}_Data.HkTlm);

    /*
     * Confirm that the CFE_TBL_Manage() call was done
     */
    UtAssert_STUB_COUNT(CFE_TBL_Manage, 1);
}

void Test_{{cookiecutter.__app_slug_uc}}_NoopCmd(void)
{
    /*
     * Test Case For:
     * void {{cookiecutter.__app_slug_uc}}_NoopCmd( const {{cookiecutter.__app_slug_uc}}_Noop_t *Msg )
     */
    {{cookiecutter.__app_slug_uc}}_NoopCmd_t TestMsg;
    UT_CheckEvent_t      EventTest;

    memset(&TestMsg, 0, sizeof(TestMsg));

    /* test dispatch of NOOP */
    UT_CHECKEVENT_SETUP(&EventTest, {{cookiecutter.__app_slug_uc}}_COMMANDNOP_INF_EID, NULL);

    UtAssert_INT32_EQ({{cookiecutter.__app_slug_uc}}_Noop(&TestMsg), CFE_SUCCESS);

    /*
     * Confirm that the event was generated
     */
    UtAssert_UINT32_EQ(EventTest.MatchCount, 1);
}

void Test_{{cookiecutter.__app_slug_uc}}_ResetCounters(void)
{
    /*
     * Test Case For:
     * void {{cookiecutter.__app_slug_uc}}_ResetCounters( const {{cookiecutter.__app_slug_uc}}_ResetCounters_t *Msg )
     */
    {{cookiecutter.__app_slug_uc}}_ResetCountersCmd_t TestMsg;
    UT_CheckEvent_t               EventTest;

    memset(&TestMsg, 0, sizeof(TestMsg));

    UT_CHECKEVENT_SETUP(&EventTest, {{cookiecutter.__app_slug_uc}}_COMMANDRST_INF_EID, "SAMPLE: RESET command");

    UtAssert_INT32_EQ({{cookiecutter.__app_slug_uc}}_ResetCounters(&TestMsg), CFE_SUCCESS);

    /*
     * Confirm that the event was generated
     */
    UtAssert_UINT32_EQ(EventTest.MatchCount, 1);
}

void Test_{{cookiecutter.__app_slug_uc}}_ProcessCC(void)
{
    /*
     * Test Case For:
     * void  {{cookiecutter.__app_slug_uc}}_ProcessCC( const {{cookiecutter.__app_slug_uc}}_Process_t *Msg )
     */
    {{cookiecutter.__app_slug_uc}}_ProcessCmd_t TestMsg;
    {{cookiecutter.__app_slug_uc}}_Table_t      TestTblData;
    void *                  TblPtr = &TestTblData;

    memset(&TestTblData, 0, sizeof(TestTblData));
    memset(&TestMsg, 0, sizeof(TestMsg));

    /* Provide some table data for the {{cookiecutter.__app_slug_uc}}_Process() function to use */
    TestTblData.Int1 = 40;
    TestTblData.Int2 = 50;
    UT_SetDataBuffer(UT_KEY(CFE_TBL_GetAddress), &TblPtr, sizeof(TblPtr), false);
    UtAssert_INT32_EQ({{cookiecutter.__app_slug_uc}}_Process(&TestMsg), CFE_SUCCESS);

    /*
     * Confirm that the CFE_TBL_GetAddress() call was done
     */
    UtAssert_STUB_COUNT(CFE_TBL_GetAddress, 1);

{% if cookiecutter.library_dep == "yes" %}    /*
     * Confirm that the {{cookiecutter.library_function}}() call was done
     * NOTE: This stub is provided by the {{cookiecutter.library_name}} library
     */
    UtAssert_STUB_COUNT({{cookiecutter.library_function}}, 1);{% endif %}

    /*
     * Configure the CFE_TBL_GetAddress function to return an error
     * Exercise the error return path
     */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetAddress), CFE_TBL_ERR_UNREGISTERED);
    UtAssert_INT32_EQ({{cookiecutter.__app_slug_uc}}_Process(&TestMsg), CFE_TBL_ERR_UNREGISTERED);
}

void Test_{{cookiecutter.__app_slug_uc}}_VerifyCmdLength(void)
{
    /*
     * Test Case For:
     * bool {{cookiecutter.__app_slug_uc}}_VerifyCmdLength
     */
    UT_CheckEvent_t   EventTest;
    size_t            size    = 1;
    CFE_MSG_FcnCode_t fcncode = 2;
    CFE_SB_MsgId_t    msgid   = CFE_SB_ValueToMsgId(3);

    /*
     * test a match case
     */
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &size, sizeof(size), false);
    UT_CHECKEVENT_SETUP(&EventTest, {{cookiecutter.__app_slug_uc}}_LEN_ERR_EID,
                        "Invalid Msg length: ID = 0x%X,  CC = %u, Len = %u, Expected = %u");

    {{cookiecutter.__app_slug_uc}}_VerifyCmdLength(NULL, size);

    /*
     * Confirm that the event was NOT generated
     */
    UtAssert_UINT32_EQ(EventTest.MatchCount, 0);

    /*
     * test a mismatch case
     */
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &size, sizeof(size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &msgid, sizeof(msgid), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &fcncode, sizeof(fcncode), false);
    {{cookiecutter.__app_slug_uc}}_VerifyCmdLength(NULL, size + 1);

    /*
     * Confirm that the event WAS generated
     */
    UtAssert_UINT32_EQ(EventTest.MatchCount, 1);
}

void Test_{{cookiecutter.__app_slug_uc}}_TblValidationFunc(void)
{
    /*
     * Test Case For:
     * int32 {{cookiecutter.__app_slug_uc}}_TblValidationFunc( void *TblData )
     */
    {{cookiecutter.__app_slug_uc}}_Table_t TestTblData;

    memset(&TestTblData, 0, sizeof(TestTblData));

    /* nominal case (0) should succeed */
    UtAssert_INT32_EQ({{cookiecutter.__app_slug_uc}}_TblValidationFunc(&TestTblData), CFE_SUCCESS);

    /* error case should return {{cookiecutter.__app_slug_uc}}_TABLE_OUT_OF_RANGE_ERR_CODE */
    TestTblData.Int1 = 1 + {{cookiecutter.__app_slug_uc}}_TBL_ELEMENT_1_MAX;
    UtAssert_INT32_EQ({{cookiecutter.__app_slug_uc}}_TblValidationFunc(&TestTblData), {{cookiecutter.__app_slug_uc}}_TABLE_OUT_OF_RANGE_ERR_CODE);
}

void Test_{{cookiecutter.__app_slug_uc}}_GetCrc(void)
{
    /*
     * Test Case For:
     * void {{cookiecutter.__app_slug_uc}}_GetCrc( const char *TableName )
     */

    /*
     * The only branch point here is CFE_TBL_GetInfo()
     *
     * Either way this function just does a write to syslog,
     * and it is the same in both cases, just with
     * a different message.  This could actually verify
     * the message using a hook function, if desired.
     */

    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetInfo), CFE_TBL_ERR_INVALID_NAME);
    {{cookiecutter.__app_slug_uc}}_GetCrc("UT");
    UtAssert_STUB_COUNT(CFE_ES_WriteToSysLog, 1);

    UT_ClearDefaultReturnValue(UT_KEY(CFE_TBL_GetInfo));
    {{cookiecutter.__app_slug_uc}}_GetCrc("UT");
    UtAssert_STUB_COUNT(CFE_ES_WriteToSysLog, 2);
}

/*
 * Setup function prior to every test
 */
void Sample_UT_Setup(void)
{
    UT_ResetState(0);
}

/*
 * Teardown function after every test
 */
void Sample_UT_TearDown(void) {}

/*
 * Register the test cases to execute with the unit test tool
 */
void UtTest_Setup(void)
{
    ADD_TEST({{cookiecutter.__app_slug_uc}}_Main);
    ADD_TEST({{cookiecutter.__app_slug_uc}}_Init);
    ADD_TEST({{cookiecutter.__app_slug_uc}}_ProcessCommandPacket);
    ADD_TEST({{cookiecutter.__app_slug_uc}}_ProcessGroundCommand);
    ADD_TEST({{cookiecutter.__app_slug_uc}}_ReportHousekeeping);
    ADD_TEST({{cookiecutter.__app_slug_uc}}_NoopCmd);
    ADD_TEST({{cookiecutter.__app_slug_uc}}_ResetCounters);
    ADD_TEST({{cookiecutter.__app_slug_uc}}_ProcessCC);
    ADD_TEST({{cookiecutter.__app_slug_uc}}_VerifyCmdLength);
    ADD_TEST({{cookiecutter.__app_slug_uc}}_TblValidationFunc);
    ADD_TEST({{cookiecutter.__app_slug_uc}}_GetCrc);
}
