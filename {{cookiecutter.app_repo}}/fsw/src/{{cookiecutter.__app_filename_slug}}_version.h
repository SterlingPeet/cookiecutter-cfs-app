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
 *  The Sample App header file containing version information
 */

#ifndef {{cookiecutter.__app_slug_uc}}_VERSION_H
#define {{cookiecutter.__app_slug_uc}}_VERSION_H

/* Development Build Macro Definitions */

#define {{cookiecutter.__app_slug_uc}}_BUILD_NUMBER {{cookiecutter.__app_build_number}} /*!< Development Build: Number of commits since baseline */
#define {{cookiecutter.__app_slug_uc}}_BUILD_BASELINE \
    "{{cookiecutter.__app_baseline_version}}" /*!< Development Build: git tag that is the base for the current development */

/*
 * Version Macros, see \ref cfsversions for definitions.
 */
#define {{cookiecutter.__app_slug_uc}}_MAJOR_VERSION {{cookiecutter.app_major_version}}  /*!< @brief Major version number. */
#define {{cookiecutter.__app_slug_uc}}_MINOR_VERSION {{cookiecutter.app_minor_version}}  /*!< @brief Minor version number. */
#define {{cookiecutter.__app_slug_uc}}_REVISION      {{cookiecutter.app_revision}} /*!< @brief Revision version number. Value of 99 indicates a development version.*/

/*!
 * @brief Mission revision.
 *
 * Reserved for mission use to denote patches/customizations as needed.
 * Values 1-254 are reserved for mission use to denote patches/customizations as needed. NOTE: Reserving 0 and 0xFF for
 * cFS open-source development use (pending resolution of nasa/cFS#440)
 */
#define {{cookiecutter.__app_slug_uc}}_MISSION_REV 0xFF

#define {{cookiecutter.__app_slug_uc}}_STR_HELPER(x) #x /*!< @brief Helper function to concatenate strings from integer macros */
#define {{cookiecutter.__app_slug_uc}}_STR(x) \
    {{cookiecutter.__app_slug_uc}}_STR_HELPER(x) /*!< @brief Helper function to concatenate strings from integer macros */

/*! @brief Development Build Version Number.
 * @details Baseline git tag + Number of commits since baseline. @n
 * See @ref cfsversions for format differences between development and release versions.
 */
#define {{cookiecutter.__app_slug_uc}}_VERSION {{cookiecutter.__app_slug_uc}}_BUILD_BASELINE "+dev" {{cookiecutter.__app_slug_uc}}_STR({{cookiecutter.__app_slug_uc}}_BUILD_NUMBER)

/*! @brief Development Build Version String.
 * @details Reports the current development build's baseline, number, and name. Also includes a note about the latest
 * official version. @n See @ref cfsversions for format differences between development and release versions.
 */
#define {{cookiecutter.__app_slug_uc}}_VERSION_STRING                       \
    " {{cookiecutter.app_display_name}} DEVELOPMENT BUILD " {{cookiecutter.__app_slug_uc}}_VERSION \
    ", Last Official Release: {{cookiecutter.__app_baseline_version}}" /* For full support please use this version */

#endif /* {{cookiecutter.__app_slug_uc}}_VERSION_H */
