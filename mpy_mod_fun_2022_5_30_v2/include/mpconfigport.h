/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Damien P. George
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include <stdint.h>
#include "polygon_sys_cfg.h" 
// options to control how MicroPython is built

// You can disable the built-in MicroPython compiler by setting the following
// config option to 0.  If you do this then you won't get a REPL prompt, but you
// will still be able to execute pre-compiled scripts, compiled with mpy-cross.

#define DASHIXIONG_BOARD            (HW_TYPE_DASHIXIONG)
#define TQ_BOARD                    (HW_TYPE_TIANQI)
#define BUILD_BOARD                 (HARDWARE_TYPE)
#define MICROPY_ENABLE_COMPILER     (1)
#define MICROPY_ENABLE_FINALISER    (1)



#define MICROPY_QSTR_BYTES_IN_HASH  (1)
#define MICROPY_QSTR_EXTRA_POOL     mp_qstr_frozen_const_pool
#define MICROPY_ALLOC_PATH_MAX      (256)
#define MICROPY_ALLOC_PARSE_CHUNK_INIT (16)
#define MICROPY_COMP_CONST          (0)
#define MICROPY_COMP_DOUBLE_TUPLE_ASSIGN (0)
#define MICROPY_ENABLE_GC           (1)
#define MICROPY_GC_ALLOC_THRESHOLD  (1)
#define MICROPY_HELPER_REPL         (1)
#define MICROPY_ERROR_REPORTING     (MICROPY_ERROR_REPORTING_TERSE)
#define MICROPY_BUILTIN_METHOD_CHECK_SELF_ARG (0)
#define MICROPY_PY_ASYNC_AWAIT      (0)
#define MICROPY_PY_ASSIGN_EXPR      (0)

#define MICROPY_PY_URANDOM                  (1)
#define MICROPY_PY_URANDOM_EXTRA_FUNCS      (1)
#define MICROPY_PY_URANDOM_SEED_INIT_FUNC   (pz_random())

#define MICROPY_PY_BUILTINS_BYTEARRAY (1)
#define MICROPY_PY_BUILTINS_DICT_FROMKEYS (0)
#define MICROPY_PY_BUILTINS_ENUMERATE (0)
#define MICROPY_PY_BUILTINS_FILTER  (0)
#define MICROPY_PY_BUILTINS_REVERSED (0)
#define MICROPY_PY_BUILTINS_SET     (0)
#define MICROPY_PY_BUILTINS_SLICE   (0)
#define MICROPY_PY_BUILTINS_PROPERTY (0)
#define MICROPY_PY_BUILTINS_MIN_MAX (0)
#define MICROPY_PY_BUILTINS_STR_COUNT (0)
#define MICROPY_PY_BUILTINS_STR_OP_MODULO (0)
#define MICROPY_PY___FILE__         (0)
#define MICROPY_PY_GC               (1)
#define MICROPY_PY_ARRAY            (1)
#define MICROPY_PY_ATTRTUPLE        (1)
#define MICROPY_PY_COLLECTIONS      (0)
#define MICROPY_PY_IO               (1)
#define MICROPY_PY_STRUCT           (0)
#define MICROPY_PY_SYS              (1)
#define MICROPY_MODULE_FROZEN_MPY   (1)
#define MICROPY_CPYTHON_COMPAT      (1)
#define MICROPY_MODULE_GETATTR      (0)

// type definitions for the specific machine
typedef intptr_t mp_int_t; // must be pointer size
typedef uintptr_t mp_uint_t; // must be pointer size
typedef long mp_off_t;

// extra built in names to add to the global namespace
#define MICROPY_PORT_BUILTINS \
    { MP_ROM_QSTR(MP_QSTR_open), MP_ROM_PTR(&mp_builtin_open_obj) }, 
    
    //{ MP_ROM_QSTR(MP_QSTR_upload), MP_ROM_PTR(&mp_builtin_upload_obj) }, 

// We need to provide a declaration/definition of alloca()
#include <alloca.h>

#define MICROPY_HW_BOARD_NAME "minimal"
#define MICROPY_HW_MCU_NAME "Hi3861"

#ifdef __linux__
#define MICROPY_MIN_USE_STDOUT (1)
#endif

#ifdef __thumb__
#define MICROPY_MIN_USE_CORTEX_CPU (1)
#define MICROPY_MIN_USE_STM32_MCU (1)
#endif

#define MP_STATE_PORT MP_STATE_VM

#define MICROPY_PORT_ROOT_POINTERS \
    const char *readline_hist[8];
extern const struct _mp_obj_module_t mp_module_os;  // 注意_mp_obj_module_t是带下划线前缀的
extern const struct _mp_obj_module_t mp_module_led;  // 注意_mp_obj_module_t是带下划线前缀的
extern const struct _mp_obj_module_t mp_module_time;  // 注意_mp_obj_module_t是带下划线前缀的
extern const struct _mp_obj_module_t mp_module_network;  // 注意_mp_obj_module_t是带下划线前缀的
extern const struct _mp_obj_type_t   mp_type_socket;  // 注意_mp_obj_module_t是带下划线前缀的
extern const struct _mp_obj_module_t mp_module_machine;  // 注意_mp_obj_module_t是带下划线前缀的
extern const struct _mp_obj_module_t mp_module_device;  // 注意_mp_obj_module_t是带下划线前缀的
extern const struct _mp_obj_module_t mp_module_ext_device;
extern const struct _mp_obj_module_t mp_module_Sentry;




#if (BUILD_BOARD == DASHIXIONG_BOARD)

#define MICROPY_PORT_BUILTIN_MODULES \
    {MP_ROM_QSTR(MP_QSTR_os), MP_ROM_PTR(&mp_module_os)}, \
    {MP_ROM_QSTR(MP_QSTR_machine), MP_ROM_PTR(&mp_module_machine)}, \
    {MP_ROM_QSTR(MP_QSTR_device), MP_ROM_PTR(&mp_module_device)}, \
    {MP_ROM_QSTR(MP_QSTR_ext_device), MP_ROM_PTR(&mp_module_ext_device)}, \
    {MP_ROM_QSTR(MP_QSTR_network), MP_ROM_PTR(&mp_module_network)}, \
    {MP_ROM_QSTR(MP_QSTR_socket), MP_ROM_PTR(&mp_type_socket)}, \
    {MP_ROM_QSTR(MP_QSTR_time), MP_ROM_PTR(&mp_module_time)}, \
    {MP_ROM_QSTR(MP_QSTR_Sentry),    MP_ROM_PTR(&mp_module_Sentry)},\	



#elif (BUILD_BOARD == TQ_BOARD)

#define MICROPY_PORT_BUILTIN_MODULES \
	{MP_ROM_QSTR(MP_QSTR_os), MP_ROM_PTR(&mp_module_os)}, \
	{MP_ROM_QSTR(MP_QSTR_machine), MP_ROM_PTR(&mp_module_machine)}, \
	{MP_ROM_QSTR(MP_QSTR_device), MP_ROM_PTR(&mp_module_device)}, \
	{MP_ROM_QSTR(MP_QSTR_ext_device), MP_ROM_PTR(&mp_module_ext_device)}, \
	{MP_ROM_QSTR(MP_QSTR_network), MP_ROM_PTR(&mp_module_network)}, \
	{MP_ROM_QSTR(MP_QSTR_socket), MP_ROM_PTR(&mp_type_socket)}, \
	{MP_ROM_QSTR(MP_QSTR_time), MP_ROM_PTR(&mp_module_time)}, \

#endif






