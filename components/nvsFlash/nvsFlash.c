#include <stdio.h>
#include "nvsFlash.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void nvsFlashInit(void) {

    esp_err_t err = nvs_flash_init();

    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK( err );
}

void nvsFlashDigitRead(
    const nvs_type_t valueType,
    const char* namespace_name,    
    const char* key, 
    uint64_t *uValue,
    int64_t *iValue) {

    esp_err_t err;
    nvs_handle_t my_handle;

    err = nvs_open(namespace_name, NVS_READONLY, &my_handle);

    if (err != ESP_OK) 
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    else 
    {
        switch (valueType)
        {
            case NVS_TYPE_U8:   
                uint8_t valuePrime;          
                err = nvs_get_u8(my_handle, key, &valuePrime);
                *uValue = valuePrime;
                break;

            case NVS_TYPE_I8:      
                int8_t valuePrime1;       
                err = nvs_get_i8(my_handle, key, &valuePrime1);
                *iValue = valuePrime1;
                break;   

            case NVS_TYPE_U16:   
                uint16_t valuePrime2;          
                err = nvs_get_u16(my_handle, key, &valuePrime2);
                *uValue = valuePrime2;
                break;

            case NVS_TYPE_I16:      
                int16_t valuePrime3;       
                err = nvs_get_i16(my_handle, key, &valuePrime3);
                *iValue = valuePrime3;
                break;   
                
            case NVS_TYPE_U32:   
                uint32_t valuePrime4;          
                err = nvs_get_u32(my_handle, key, &valuePrime4);
                *uValue = valuePrime4;
                break;

            case NVS_TYPE_I32:      
                int32_t valuePrime5;       
                err = nvs_get_i32(my_handle, key, &valuePrime5);
                *iValue = valuePrime5;
                break;   

            case NVS_TYPE_U64:                   
                err = nvs_get_u64(my_handle, key, uValue);                
                break;

            case NVS_TYPE_I64:                      
                err = nvs_get_i64(my_handle, key, iValue);                
                break;   

            default:
                break;
        }       

        if (err != ESP_OK) 
            printf("doneNvsFlashDigitRead Failed!\n"); 
    }        
    nvs_close(my_handle);
}

void nvsFlashDigitWrite(
    const nvs_type_t valueType,
    const char* namespace_name,    
    const char* key, 
    uint64_t value) {
    
    esp_err_t err;
    nvs_handle_t my_handle;

    err = nvs_open(namespace_name, NVS_READWRITE, &my_handle);

    if (err != ESP_OK) 
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    else 
    {
        switch (valueType)
        {
            case NVS_TYPE_U8:             
                err = nvs_set_u8(my_handle, key, (uint8_t) value);
                break;

            case NVS_TYPE_I8:             
                err = nvs_set_i8(my_handle, key, (int8_t) value);
                break;  

            case NVS_TYPE_U16:             
                err = nvs_set_u16(my_handle, key, (uint16_t) value);
                break;                

            case NVS_TYPE_I16:             
                err = nvs_set_i16(my_handle, key, (int16_t) value);
                break;

            case NVS_TYPE_U32:             
                err = nvs_set_u32(my_handle, key, (uint32_t) value);
                break;                

            case NVS_TYPE_I32:             
                err = nvs_set_i32(my_handle, key, (int32_t) value);
                break;

            case NVS_TYPE_U64:             
                err = nvs_set_u64(my_handle, key, (uint64_t) value);
                break;                

            case NVS_TYPE_I64:             
                err = nvs_set_i64(my_handle, key, (int64_t) value);
                break;

            default:
                break;            
        }        

        if (err != ESP_OK) 
            printf("doneNvsFlashDigitWrite Failed!\n");
    }        

    err = nvs_commit(my_handle);

    if (err != ESP_OK) 
        printf("commit Failed!\n");

    nvs_close(my_handle);
}

bool nvsFlashStringRead(   
    const char* namespace_name,         
    const char* key, 
    char* value,
    size_t* length) {

    esp_err_t err;
    nvs_handle_t my_handle;
    bool isExist = false;      

    err = nvs_open(namespace_name, NVS_READONLY, &my_handle);

    if (err != ESP_OK) 
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    else 
    {
        size_t required_size = 0;          

        err = nvs_get_str(my_handle, key, NULL, &required_size);//read size of
        if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) 
            printf("Error (%s) nvs_get_str \n", esp_err_to_name(err));        

        if (required_size > 0)                                    
        {                        
            isExist = true;
            err = nvs_get_str(my_handle, key, value, &required_size);        
            *length = required_size;                        

            if (err != ESP_OK)                                        
                printf("\nnvsFlashStringRead Failed!\n");                                                
            
        }        
    }  

    nvs_close(my_handle);

    return isExist;
}    

void nvsFlashStringWrite(   
    const char* namespace_name,         
    const char* key, 
    const char* value) {    

    esp_err_t err;
    nvs_handle_t my_handle;

    err = nvs_open(namespace_name, NVS_READWRITE, &my_handle);

    if (err != ESP_OK) 
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    else 
    {
        err = nvs_set_str(my_handle, key, value);        
       
        if (err != ESP_OK) 
            printf("doneNvsFlashStringWrite Failed!\n");
    }  

    err = nvs_commit(my_handle);

    if (err != ESP_OK) 
        printf("commit Failed!\n");

    nvs_close(my_handle);        
}

bool nvsFlashBlobRead(    
    const char* namespace_name,        
    const char* key, 
    void* value,
    size_t *length) {

    esp_err_t err;
    nvs_handle_t my_handle;
    bool isExist = false;      

    err = nvs_open(namespace_name, NVS_READONLY, &my_handle);

    if (err != ESP_OK) 
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    else 
    {
        size_t required_size = 0;  
        
        err = nvs_get_blob(my_handle, key, NULL, &required_size);
        if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) 
            printf("Error (%s) nvs_get_blob \n", esp_err_to_name(err));

        if (required_size > 0)                                    
        {
            isExist = true;
            err = nvs_get_blob(my_handle, key, value, length);
            *length = required_size;                        
            
            if (err != ESP_OK) 
                printf("doneNvsFlashBlobRead Failed!\n");
        }
    }  

    nvs_close(my_handle);    

    return isExist;    
}    

void nvsFlashBlobWrite(    
    const char* namespace_name,        
    const char* key, 
    const void* value,
    size_t length) {

    esp_err_t err;
    nvs_handle_t my_handle;

    err = nvs_open(namespace_name, NVS_READWRITE, &my_handle);

    if (err != ESP_OK) 
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    else 
    {
        err = nvs_set_blob(my_handle, key, value, length);
       
        if (err != ESP_OK) 
            printf("doneNvsFlashBlobWrite Failed!\n");
    }  

    err = nvs_commit(my_handle);

    if (err != ESP_OK) 
        printf("commit Failed!\n");

    nvs_close(my_handle);       
}

void TestAllNvsFunction()
{
    char* namespaceName = "spotifyEra";   
    char* idKey = "userId";
    //char* idValue = "Done.Technology";
    char* idValue = "QABtmfRQr3dACl7BjnPG9UaVSIgd6LuxrorVty3cvzTRtXTer_Hk7SKnmzIbthiwedLyaseu3Ljp1F4kkHonhX64FxQU4eXLCBF8XqrJ-wQFx5xtC0xUjblQkFuoTYf15TGidV9xwyx7XiFfjQ4LNZ3R7Y1071ynjqu-aqFJV8125sGqxwg31ue0OMwv-wOmcLFgp6HbW8bVG0NpwsT0wc0uREniIAOvTDWIcX38KnY-b3Ps5-K17CnIJOA_x7oCgf_QWHp1yBfS_BDZan4BrdtTPE7b_94Fzu8sTBqrUhRjT-zMIAEuw";
    char* idValue1 = "";
    size_t len=0;

    uint64_t value;
    int64_t iValue;
    value =24;

    vTaskDelay(1000 / portTICK_PERIOD_MS);
    nvsFlashDigitWrite(NVS_TYPE_U8, "storage", "wifi-pass", value);
    printf("\nwrite value= %lld", value);

    value = 12;
    printf("\nchange value= %lld", value);

    vTaskDelay(1000 / portTICK_PERIOD_MS);
    nvsFlashDigitRead(NVS_TYPE_U8, "storage", "wifi-pass", &value, &iValue);
    printf("\nread value= %lld", value);        

    vTaskDelay(1000 / portTICK_PERIOD_MS);
    nvsFlashStringWrite(namespaceName, idKey, idValue);
    printf("\nwrite idValue = %s", idValue);    

    vTaskDelay(1000 / portTICK_PERIOD_MS);
    if(nvsFlashStringRead(namespaceName, idKey, idValue1, &len))            
        printf("\nread idValue = %s", idValue1);        
        
}