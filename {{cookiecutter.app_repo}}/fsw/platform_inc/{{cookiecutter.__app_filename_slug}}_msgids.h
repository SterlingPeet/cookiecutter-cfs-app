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
 * Define {{cookiecutter.app_display_name}} App Message IDs
 *
 * \note The {{cookiecutter.app_display_name}} App assumes default configuration which uses V1 of message id implementation
 */

#ifndef {{cookiecutter.__app_slug_uc}}_MSGIDS_H
#define {{cookiecutter.__app_slug_uc}}_MSGIDS_H

/* V1 Command Message IDs must be 0x18xx */
#define {{cookiecutter.__app_slug_uc}}_CMD_MID     {{cookiecutter.app_cmd_mid}}
#define {{cookiecutter.__app_slug_uc}}_SEND_HK_MID {{cookiecutter.app_send_hk_mid}}
/* V1 Telemetry Message IDs must be 0x08xx */
#define {{cookiecutter.__app_slug_uc}}_HK_TLM_MID {{cookiecutter.app_hk_tlm_mid}}

#endif /* {{cookiecutter.__app_slug_uc}}_MSGIDS_H */
