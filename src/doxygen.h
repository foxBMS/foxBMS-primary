/**
 *
 * @copyright &copy; 2010 - 2016, Fraunhofer-Gesellschaft zur Foerderung der angewandten Forschung e.V. All rights reserved.
 *
 * BSD 3-Clause License
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 * 1.  Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * We kindly request you to use one or more of the following phrases to refer to foxBMS in your hardware, software, documentation or advertising materials:
 *
 * &Prime;This product uses parts of foxBMS&reg;&Prime;
 *
 * &Prime;This product includes parts of foxBMS&reg;&Prime;
 *
 * &Prime;This product is derived from foxBMS&reg;&Prime;
 *
 */

/**
 * @file    doxygen.h
 * @author  foxBMS Team
 * @date    11.11.2015 (date of creation)
 * @ingroup none
 * @prefix  none
 *
 * @brief   Mainpage and Module-list for generating of documents with doxygen
 *
 */

#ifndef DOXYGEN_H_
#define DOXYGEN_H_

/**
 * @mainpage Source Code Documentation
 *
 *
 * This manual describes the functions and variables used in the foxBMS software.
 * Four sections can be found:
 *
 *  - Common Interfaces
 *  - Modules
 *  - Data structures
 *  - Files
 *
 *  They can be browsed with the navigation bar on the left.
 *
 * Direct access to the API:
 * - @ref API_PAGE
 *
 * Direct access to the source modules:
 * - @ref APPLICATION
 * - @ref APPLICATION_CONF
 * - @ref ENGINE
 * - @ref ENGINE_CONF
 * - @ref GENERAL
 * - @ref GENERAL_CONF
 * - @ref DRIVERS
 * - @ref DRIVERS_CONF
 * - @ref UTIL
 * - @ref UTIL_CONF
 * - @ref OS
 *
 *
 * @defgroup APPLICATION   Applications
 * Contains low priority custom and user tasks
 *
 * @defgroup APPLICATION_CONF   Application Configuration
 * Contains configuration for low priority custom and user tasks
 *
 * @defgroup ENGINE   Engine
 * Contains high priority kernel and database handler
 *
 * @defgroup ENGINE_CONF   Engine Configuration
 * Contains configuration for high priority kernel and database handler
 *
 * @defgroup GENERAL   General
 * Contains the "main" function and interrupt declaration
 *
 * @defgroup GENERAL_CONF   General Configuration
 * System related configuration
 *
 * @defgroup DRIVERS   Drivers
 * Contains the drivers used by foxBMS
 *
 * @defgroup DRIVERS_CONF   Driver Configuration
 * Contains the configuration for the drivers used by foxBMS
 *
 * @defgroup UTIL   Utilities
 * Contains utilities used by foxBMS
 *
 * @defgroup UTIL_CONF   Utility Configuration
 * Contains the configuration for the utilities used by foxBMS
 *
 * @defgroup OS   Operating System
 * Contains functions related to the operating system
 *
 *
 */

/**
 * @page API_PAGE User Interfaces (API)
 *
 * foxBMS software provides the following user interfaces:
 *
 *  Reference:
 * - @ref API_OS
 * - @ref API_DIAG
 *
 * @defgroup API_OS     Operating System Interfaces
 * Contains the provided interfaces to FreeRTOS in the foxBMS software
 *
 * @defgroup API_DIAG   Diagnosis Interfaces
 * Contains the provided interfaces to diagnostis module
 *
 * @defgroup API        All Interfaces
 * Contains a list of all interfaces in the foxBMS software
 *
 */
#endif /* DOXYGEN_H_ */
