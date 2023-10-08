/*
 * SEZ@Done(ehsan.ziyaee@gmail.com) 
 * init 2023.10.02
 *
 * a component for nvs flash access.
 */
#ifndef NVS_FLASH_H_
#define NVS_FLASH_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"

/**
 * @brief  we bring below parameter from nvs.h
 * 
#define NVS_PART_NAME_MAX_SIZE              16   
#define NVS_KEY_NAME_MAX_SIZE               16   
#define NVS_NS_NAME_MAX_SIZE                NVS_KEY_NAME_MAX_SIZE 
      

typedef enum {
        NVS_READONLY,  // Read only
        NVS_READWRITE  // Read and write
} nvs_open_mode_t;

typedef enum {
    NVS_TYPE_U8    = 0x01,  // Type uint8_t
    NVS_TYPE_I8    = 0x11,  // Type int8_t 
    NVS_TYPE_U16   = 0x02,  // Type uint16_t 
    NVS_TYPE_I16   = 0x12,  // Type int16_t 
    NVS_TYPE_U32   = 0x04,  // Type uint32_t
    NVS_TYPE_I32   = 0x14,  // Type int32_t 
    NVS_TYPE_U64   = 0x08,  // Type uint64_t 
    NVS_TYPE_I64   = 0x18,  // Type int64_t 
    NVS_TYPE_STR   = 0x21,  // Type string 
    NVS_TYPE_BLOB  = 0x42,  // Type blob 
    NVS_TYPE_ANY   = 0xff   // Must be last
} nvs_type_t;
*/

/**
 * @brief      initialize nvs flash partition.
 * @param[in]  void   nothing
 * @param[out] void   nothing
 * @return nothing
 */
void nvsFlashInit(void);

/**
 * @brief      read 8/16/32/64 bit-digit from nvs
 * @param[in]   valueType        type value of digit from nvs_type_t enum.
 * @param[in]   namespace_name   Namespace name. Maximum length is (NVS_KEY_NAME_MAX_SIZE-1) characters. Shouldn't be empty.
 * @param[in]   key              Key name. Maximum length is (NVS_KEY_NAME_MAX_SIZE-1) characters. Shouldn't be empty.
 * @param[out]  uvalue           poiner to output value and it is valid if type is unsigned.
 * @param[out]  ivalue           poiner to output value and it is valid if type is signed.
 * @return nothing
 */
void nvsFlashDigitRead(
    const nvs_type_t valueType,
    const char* namespace_name,    
    const char* key, 
    uint64_t *uValue,
    int64_t *iValue);

/**
 * @brief      write 8/16/32/64 bit-digit to nvs
 * @param[in]   valueType        type value of digit from nvs_type_t enum.
 * @param[in]   namespace_name   Namespace name. Maximum length is (NVS_KEY_NAME_MAX_SIZE-1) characters. Shouldn't be empty.
 * @param[in]   key              Key name. Maximum length is (NVS_KEY_NAME_MAX_SIZE-1) characters. Shouldn't be empty.
 * @param[out]  value           the value shold be witten.
 * @return nothing
 */
void nvsFlashDigitWrite(
    const nvs_type_t valueType,
    const char* namespace_name,    
    const char* key, 
    uint64_t value);

/**
 * @brief      read string from nvs
 * @param[in]   namespace_name   Namespace name. Maximum length is (NVS_KEY_NAME_MAX_SIZE-1) characters. Shouldn't be empty.
 * @param[in]   key              Key name. Maximum length is (NVS_KEY_NAME_MAX_SIZE-1) characters. Shouldn't be empty.
 * @param[out]  value           poiner to char array (cstring) that read from nvs
 * @param[inout]  length        A non-zero pointer to the variable holding the length of out_value.
 * @return[out] isExist         return true if the key is exist and value and length have valid values;
 *                              if return false the value and length not valid
 */
bool nvsFlashStringRead(  
    const char* namespace_name,          
    const char* key, 
    char* value,
    size_t* length);    

/**
 * @brief      write string to nvs
 * @param[in]   namespace_name   Namespace name. Maximum length is (NVS_KEY_NAME_MAX_SIZE-1) characters. Shouldn't be empty.
 * @param[in]   key              Key name. Maximum length is (NVS_KEY_NAME_MAX_SIZE-1) characters. Shouldn't be empty.
 * @param[out]  value           poiner to char array (cstring) that should write to nvs.
 * @return nothing
 */
void nvsFlashStringWrite(  
    const char* namespace_name,          
    const char* key, 
    const char* value);    

/**
 * @brief      read blob from nvs
 * @param[in]   namespace_name   Namespace name. Maximum length is (NVS_KEY_NAME_MAX_SIZE-1) characters. Shouldn't be empty.
 * @param[in]   key              Key name. Maximum length is (NVS_KEY_NAME_MAX_SIZE-1) characters. Shouldn't be empty.
 * @param[out]  value           void poiner that read from nvs 
 * @param[inout]  length        A non-zero pointer to the variable holding the length of out_value.
 * @return[out]  isExist        return true if the key is exist and value and length have valid values;
 *                              if return false the value and length not valid
 */
bool nvsFlashBlobRead( 
    const char* namespace_name,           
    const char* key, 
    void* value,
    size_t* length);    

/**
 * @brief      write blob to nvs
 * @param[in]   namespace_name   Namespace name. Maximum length is (NVS_KEY_NAME_MAX_SIZE-1) characters. Shouldn't be empty.
 * @param[in]   key              Key name. Maximum length is (NVS_KEY_NAME_MAX_SIZE-1) characters. Shouldn't be empty.
 * @param[out]  value           void poiner that write to nvs
 * @param[in]  length        the variable holding the length of out_value.
 * @return nothing
 */
void nvsFlashBlobWrite(  
    const char* namespace_name,          
    const char* key, 
    const void* value,
    size_t length);


void TestAllNvsFunction();

#ifdef __cplusplus
}
#endif

#endif /* NVS_FLASH_H_ */
