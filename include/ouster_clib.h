/**
 * @file ouster_clib.h
 * @brief Ouster public API.
 *
 * This file contains the public API for ouster_clib.
 */

#ifndef OUSTER_CLIB_H
#define OUSTER_CLIB_H

/**
 * @defgroup c C API
 *
 * @{
 * @}
 */

/**
 * @defgroup core Core
 * @brief Core Ouster API functionality
 *
 * \ingroup c
 * @{
 */

/**
 * @defgroup options API defines
 * @brief Defines for customizing compile time features.
 * @{
 */

/** \def OUSTER_DEBUG
 * Used for input parameter checking and cheap sanity checks. There are lots of
 * asserts in every part of the code, so this will slow down applications.
 */
#define OUSTER_DEBUG

/** \def OUSTER_USE_DUMP
 * Include dump or print struct functionality
 */
#define OUSTER_USE_DUMP

/** \def OUSTER_USE_UDPCAP
 * Include UDP capture and replay functionality
 */
#define OUSTER_USE_UDPCAP

/** \def OUSTER_ENABLE_LOG
 * Enable logging
 */
#define OUSTER_ENABLE_LOG

/** @} */ // end of options

/** @} */ // end of core

#include "ouster_clib/ouster_types.h"
#include "ouster_clib/ouster_os_api.h"
#include "ouster_clib/ouster_assert.h"
#include "ouster_clib/ouster_field.h"
#include "ouster_clib/ouster_fs.h"
#include "ouster_clib/ouster_lidar.h"
#include "ouster_clib/ouster_log.h"
#include "ouster_clib/ouster_lut.h"
#include "ouster_clib/ouster_meta.h"
#include "ouster_clib/ouster_net.h"
#include "ouster_clib/ouster_sock.h"
#include "ouster_clib/ouster_vec.h"
#include "ouster_clib/ouster_http.h"

#ifdef OUSTER_NO_UDPCAP
#undef OUSTER_USE_UDPCAP
#endif

#ifdef OUSTER_NO_DUMP
#undef OUSTER_USE_DUMP
#endif


#ifdef OUSTER_USE_UDPCAP
#include "ouster_clib/ouster_udpcap.h"
#endif

#ifdef OUSTER_USE_DUMP
#include "ouster_clib/ouster_dump.h"
#endif

#endif // OUSTER_CLIB_H