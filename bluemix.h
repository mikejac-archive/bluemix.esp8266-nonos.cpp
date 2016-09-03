/* 
 * The MIT License (MIT)
 * 
 * ESP8266 Non-OS Firmware
 * Copyright (c) 2015 Michael Jacobsen (github.com/mikejac)
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
 *
 */

#ifndef BLUEMIX_H
#define	BLUEMIX_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <github.com/mikejac/realtimelogic.json.esp8266-nonos.cpp/JVal.h>

#define BMix_DecoderBegin(buf)  BMix_DecoderBeginEx(buf, true)

/******************************************************************************************************************
 * prototypes
 *
 */

/**
 * 
 * @return 
 */
int BMix_Initialize(void);
/**
 * 
 * @param service_id
 * @param feed_id
 * @param value
 * @param message
 * @return 
 */
int BMix_Int_Message(const char* service_id, const char* feed_id, int value, char* message);
/**
 * 
 * @param service_id
 * @param feed_id
 * @param value
 * @param message
 * @return 
 */
int BMix_Long_Message(const char* service_id, const char* feed_id, long value, char* message);
/**
 * 
 * @param service_id
 * @param feed_id
 * @param value
 * @param message
 * @return 
 */
int BMix_Double_Message(const char* service_id, const char* feed_id, double value, char* message);

//int bluemix_t::bmixAddString(const char* name, const char* value)
//int bluemix_t::bmixAddInt(const char* name, int32_t value)
//int bluemix_t::bmixAddLong(const char* name, int64_t value)
//int bluemix_t::bmixAddBool(const char* name, bool value)
//int bluemix_t::bmixAddDouble(const char* name, double value)

/**
 * 
 * @param buf
 * @return 
 */
JVal* BMix_DecoderBeginEx(const char* buf, bool d);
/**
 * 
 * @return 
 */
int BMix_DecoderEnd(void);
/**
 * 
 * @param name
 * @param value
 * @return 
 */
int BMix_GetString(const char* name, const char** value);
/**
 * 
 * @param name
 * @param value
 * @return 
 */
int BMix_GetBool(const char* name, bool* value);
/**
 * 
 * @param name
 * @param value
 * @return 
 */
int BMix_GetInt(const char* name, int* value);
/**
 * 
 * @param name
 * @param value
 * @return 
 */
int BMix_GetU64(const char* name, uint64_t* value);
/**
 * 
 * @param name
 * @param value
 * @return 
 */
int BMix_GetDouble(const char* name, double* value);
/**
 * 
 * @param name
 * @param array
 * @param len
 * @return 
 */
int BMix_GetArrayByte(const char* name, uint8_t* array, int len);

#ifdef	__cplusplus
}
#endif

#endif	/* BLUEMIX */

