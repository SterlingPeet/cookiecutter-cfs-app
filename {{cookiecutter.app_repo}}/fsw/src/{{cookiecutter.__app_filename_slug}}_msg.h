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
 * Define {{cookiecutter.__app_slug_uc}} App  Messages and info
 */

#ifndef {{cookiecutter.__app_slug_uc}}_MSG_H
#define {{cookiecutter.__app_slug_uc}}_MSG_H

/*
** {{cookiecutter.__app_slug_uc}} App command codes
*/
#define {{cookiecutter.__app_slug_uc}}_NOOP_CC           0
#define {{cookiecutter.__app_slug_uc}}_RESET_COUNTERS_CC 1
#define {{cookiecutter.__app_slug_uc}}_PROCESS_CC        2

/*************************************************************************/

/*
** Type definition (generic "no arguments" command)
*/
typedef struct
{
    CFE_MSG_CommandHeader_t CmdHeader; /**< \brief Command header */
} {{cookiecutter.__app_slug_uc}}_NoArgsCmd_t;

/*
** The following commands all share the "NoArgs" format
**
** They are each given their own type name matching the command name, which
** allows them to change independently in the future without changing the prototype
** of the handler function
*/
typedef {{cookiecutter.__app_slug_uc}}_NoArgsCmd_t {{cookiecutter.__app_slug_uc}}_NoopCmd_t;
typedef {{cookiecutter.__app_slug_uc}}_NoArgsCmd_t {{cookiecutter.__app_slug_uc}}_ResetCountersCmd_t;
typedef {{cookiecutter.__app_slug_uc}}_NoArgsCmd_t {{cookiecutter.__app_slug_uc}}_ProcessCmd_t;

/*************************************************************************/
/*
** Type definition ({{cookiecutter.app_display_name}} App housekeeping)
*/

typedef struct
{
    uint8 CommandErrorCounter;
    uint8 CommandCounter;
    uint8 spare[2];
} {{cookiecutter.__app_slug_uc}}_HkTlm_Payload_t;

typedef struct
{
    CFE_MSG_TelemetryHeader_t  TelemetryHeader; /**< \brief Telemetry header */
    {{cookiecutter.__app_slug_uc}}_HkTlm_Payload_t Payload;         /**< \brief Telemetry payload */
} {{cookiecutter.__app_slug_uc}}_HkTlm_t;

#endif /* {{cookiecutter.__app_slug_uc}}_MSG_H */
