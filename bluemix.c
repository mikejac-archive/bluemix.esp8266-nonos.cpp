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

#include "bluemix.h"
#include <github.com/mikejac/date_time.esp8266-nonos.cpp/system_time.h>
#include <github.com/mikejac/misc.esp8266-nonos.cpp/espmissingincludes.h>
#include <github.com/mikejac/realtimelogic.json.esp8266-nonos.cpp/AllocatorIntf.h>
#include <github.com/mikejac/realtimelogic.json.esp8266-nonos.cpp/BufPrint.h>
#include "osapi.h"
#include "mem.h"

#define DTXT(...)   os_printf(__VA_ARGS__)

/******************************************************************************************************************
 * 
 *
 */
static AllocatorIntf* alloc;

static char memberName[10];

static JParser parser;
static JErr err;

static JParserValFact val_fact;        // JSON parser value factory. Used by the parser
static JVal* m_Jv_d;


static JVal* bmixFind(const char* name);

/******************************************************************************************************************
 * public functions
 *
 */

/**
 * 
 * @return 
 */
int ICACHE_FLASH_ATTR BMix_Initialize(void)
{
    alloc = AllocatorIntf_getDefault();
    JParserValFact_constructor(&val_fact, alloc, alloc);
    
    JErr_constructor(&err);
    
    return 0;
}
/**
 * 
 * @param service_id
 * @param feed_id
 * @param value
 * @param message
 * @return 
 */
int ICACHE_FLASH_ATTR BMix_Int_Message(const char* service_id, const char* feed_id, int value, char* message)
{
    basprintf(message, "{"                            
                            "\"d\":{"
                                "\"_type\":\"%s\","
                                "\"feed_id\":\"%s\","
                                "\"value\":%d,"
                                "\"clock\":%lu"
                            "}"
                        "}", 
                        service_id,
                        feed_id,
                        value,
                        (uint32_t) esp_time(0));
    
    return 0;
}
/**
 * 
 * @param service_id
 * @param feed_id
 * @param value
 * @param message
 * @return 
 */
int ICACHE_FLASH_ATTR BMix_Long_Message(const char* service_id, const char* feed_id, long value, char* message)
{
    basprintf(message, "{"                            
                            "\"d\":{"
                                "\"_type\":\"%s\","
                                "\"feed_id\":\"%s\","
                                "\"value\":%ld,"
                                "\"clock\":%lu"
                            "}"
                        "}", 
                        service_id,
                        feed_id,
                        value,
                        (uint32_t) esp_time(0));
    
    return 0;
}
/**
 * 
 * @param service_id
 * @param feed_id
 * @param value
 * @param message
 * @return 
 */
int ICACHE_FLASH_ATTR BMix_Double_Message(const char* service_id, const char* feed_id, double value, char* message)
{
    basprintf(message, "{"                            
                            "\"d\":{"
                                "\"_type\":\"%s\","
                                "\"feed_id\":\"%s\","
                                "\"value\":%.4f,"
                                "\"clock\":%lu"
                            "}"
                        "}", 
                        service_id,
                        feed_id,
                        value,
                        (uint32_t) esp_time(0));
    
    return 0;
}
/**
 * 
 * @param buf
 * @return 
 */
JVal* ICACHE_FLASH_ATTR BMix_DecoderBeginEx(const char* buf, bool d)
{
    JParser_constructor(&parser, (JParserIntf*)&val_fact, memberName, sizeof(memberName), alloc, 0);

    // delete the JSON value tree, thus making the JParserValFact ready for the next object in the stream
    JParserValFact_termFirstVal(&val_fact);
    
    int status = JParser_parse(&parser, (const uint8_t*)buf, os_strlen(buf));
    
    if(status < 0) {
        DTXT("BMix_DecoderBegin(): JSON parse error\n");
        return NULL;
    }
    else if(status > 0) {                                                       // a complete JSON object
        //DTXT("BMix_DecoderBegin(): JSON parse ok\n");
        
        JVal* jv_root = JParserValFact_getFirstVal(&val_fact);                  // root tree node
        
        /******************************************************************************************************************
         * validate root object
         *
         */    
        if(JVal_getName(jv_root) != NULL) {
            DTXT("BMix_DecoderBegin(): root != 0; root = %s\n", JVal_getName(jv_root));
            return NULL;
        }
        else if(JVal_getType(jv_root) != JVType_Object) {
            DTXT("BMix_DecoderBegin(): jv_root->getType() != JVType_Object\n");
            return NULL;
        }
        
        /******************************************************************************************************************
         * validate 'd' object
         *
         */    
        if(d) {
            // get the json child object
            JVal* jv_d = JVal_getJ(jv_root, 0);

            const char* d = JVal_getName(jv_d);

            if(d == 0 || strcmp("d", d) != 0) {
                DTXT("BMix_DecoderBegin(): d == 0 || strcmp(\"d\", d) != 0\n");
                return NULL;
            }
            else if(JVal_getType(jv_d) != JVType_Object) {
                DTXT("BMix_DecoderBegin(): jv_d->getType() != JVType_Object\n");
                return NULL;
            }

            JErr_reset(&err);

            // get the 'd' child object
            m_Jv_d = JVal_getJ(jv_d, 0);
        } else {
            JErr_reset(&err);

            m_Jv_d = JVal_getJ(jv_root, 0);
        }
        
        //DTXT("BMix_DecoderBegin(): top object ok\n");
    }
    else {
        DTXT("BMix_DecoderBegin(): incomplete JSON object\n");
        return NULL;
    }
    
    return m_Jv_d;
}
/**
 * 
 * @return 
 */
int ICACHE_FLASH_ATTR BMix_DecoderEnd(void)
{
    JParser_destructor(&parser);
    
    return 0;
}
/**
 * 
 * @param name
 * @param value
 * @return 
 */
int ICACHE_FLASH_ATTR BMix_GetString(const char* name, const char** value)
{
    JVal* jv_dd = bmixFind(name);
    
    if(jv_dd != 0 && JVal_getType(jv_dd) == JVType_String) {
        *value = JVal_getString(jv_dd, &err);
        
        // check for errors
        if(JErr_isError(&err)) {        
            DTXT("BMix_GetString(): JSON decoder error; %s\n", JErr_getErrS(&err));
            return -2;
        }
        else {
            return 0;
        }
    }
    else {
        // not found
        return -1;
    }
}
/**
 * 
 * @param name
 * @param value
 * @return 
 */
int ICACHE_FLASH_ATTR BMix_GetBool(const char* name, bool* value)
{
    JVal* jv_dd = bmixFind(name);
    
    if(jv_dd != 0 && JVal_getType(jv_dd) == JVType_Boolean) {
        *value = JVal_getBoolean(jv_dd, &err);
        
        // check for errors
        if(JErr_isError(&err)) {        
            DTXT("BMix_GetBool(): JSON decoder error; %s\n", JErr_getErrS(&err));
            return -2;
        }
        else {
            return 0;
        }
    }
    else {
        // not found
        return -1;
    }
}
/**
 * 
 * @param name
 * @param value
 * @return 
 */
int ICACHE_FLASH_ATTR BMix_GetInt(const char* name, int* value)
{
    JVal* jv_dd = bmixFind(name);
    
    if(jv_dd != 0 && JVal_getType(jv_dd) == JVType_Int) {
        *value = JVal_getInt(jv_dd, &err);
        
        // check for errors
        if(JErr_isError(&err)) {        
            DTXT("BMix_GetInt(): JSON decoder error; %s\n", JErr_getErrS(&err));
            return -2;
        }
        else {
            return 0;
        }
    }
    else {
        // not found
        return -1;
    }
}
/**
 * 
 * @param name
 * @param value
 * @return 
 */
int ICACHE_FLASH_ATTR BMix_GetU64(const char* name, uint64_t* value)
{
    JVal* jv_dd = bmixFind(name);
    
    if(jv_dd != 0 && (JVal_getType(jv_dd) == JVType_Int || JVal_getType(jv_dd) == JVType_Long)) {
        *value = JVal_getLong(jv_dd, &err);
        
        // check for errors
        if(JErr_isError(&err)) {        
            DTXT("BMix_GetU64(): JSON decoder error; %s\n", JErr_getErrS(&err));
            return -2;
        }
        else {
            return 0;
        }
    }
    else {
        // not found
        return -1;
    }
}
/**
 * 
 * @param name
 * @param value
 * @return 
 */
int ICACHE_FLASH_ATTR BMix_GetDouble(const char* name, double* value)
{
    JVal* jv_dd = bmixFind(name);
    
    if(jv_dd != 0 && (JVal_getType(jv_dd) == JVType_Double || JVal_getType(jv_dd) == JVType_Int || JVal_getType(jv_dd) == JVType_Long)) {
        *value = JVal_getDouble(jv_dd, &err);
        
        // check for errors
        if(JErr_isError(&err)) {        
            DTXT("BMix_GetDouble(): JSON decoder error; %s\n", JErr_getErrS(&err));
            return -2;
        }
        else {
            return 0;
        }
    }
    else {
        // not found
        return -1;
    }
}
/**
 * 
 * @param name
 * @param array
 * @param len
 * @return 
 */
int ICACHE_FLASH_ATTR BMix_GetArrayByte(const char* name, uint8_t* array, int len)
{
    JVal* jv_dd = bmixFind(name);
    
    if(jv_dd != 0 && JVal_getType(jv_dd) == JVType_Array) {
        jv_dd = JVal_getArray(jv_dd, &err);

        // check for errors
        if(JErr_isError(&err)) {        
            DTXT("BMix_GetArrayByte(1): JSON decoder error; %s\n", JErr_getErrS(&err));
            return -2;
        }
        else {
            int i;
            
            for(i = 0; i < len && jv_dd != 0; ++i) {
                int value = JVal_getInt(jv_dd, &err);

                // check for errors
                if(JErr_isError(&err)) {        
                    DTXT("BMix_GetArrayByte(2): JSON decoder error; %s\n", JErr_getErrS(&err));
                    return -3;
                }

                //DTXT("BMix_GetArrayByte(): i = %d, value = %d\n", i, value);
                
                array[i] = 0xFF & value;
                
                jv_dd = JVal_getNextElem(jv_dd);
            }
            
            return i;
        }
    }
    else {
        // not found
        return -1;
    }
}
/**
 * 
 * @param name
 * @return 
 */
JVal* ICACHE_FLASH_ATTR bmixFind(const char* name)
{
    JVal* jv_dd = m_Jv_d;
    
    while(jv_dd != 0) {
        const char* _name = JVal_getName(jv_dd);

        if(_name != 0 && os_strcmp(_name, name) == 0) {
            return jv_dd;
        }
        
        jv_dd = JVal_getNextElem(jv_dd);
    }

    DTXT("bmixFind(): not found; %s\n", name);
    
    // didn't find it
    return NULL;
}
