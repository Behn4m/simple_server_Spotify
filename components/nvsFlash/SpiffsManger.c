/*
 * Mohammad Javad Abbasi (work.abbasii@gmail.com)
 * init 2023.11.27
 *
 * a component for work with Spiffs.
 */
#include "SpiffsManger.h"
#define InternalBufSize_ 2 * 1000
static const char *TAG = "Spiffs";

/**
 *@brief Perform a SPIFFS check on the specified partition.
 * @param conf The SPIFFS configuration.
 */
void SpiffsCheckingPerforming(esp_vfs_spiffs_conf_t conf)
{
    esp_err_t Ret;
    ESP_LOGI(TAG, "Performing SPIFFS_check().");
    Ret = esp_spiffs_check(conf.partition_label);
    if (Ret != ESP_OK)
    {
        ESP_LOGE(TAG, "SPIFFS_check() failed (%s)", esp_err_to_name(Ret));
        return;
    }
    else
    {
        ESP_LOGI(TAG, "SPIFFS_check() successful");
    }
}

/**
 *@brief Perform a SPIFFS check on the specified partition and initd globally
 * @param conf The SPIFFS configuration.
 */
void SpiffsInit()
{
    ESP_LOGI(TAG, "Initializing SPIFFS");
    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = NULL,
        .max_files = 15,
        .format_if_mount_failed = true};
    esp_err_t Ret = esp_vfs_spiffs_register(&conf);
    if (Ret != ESP_OK)
    {
        if (Ret == ESP_FAIL)
        {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        }
        else if (Ret == ESP_ERR_NOT_FOUND)
        {
            ESP_LOGE(TAG, "Failed to find SPIFFS partition");
        }
        else
        {
            ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(Ret));
        }
        return;
    }
    size_t total = 0, used = 0;
    Ret = esp_spiffs_info(conf.partition_label, &total, &used);
    if (Ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s). Formatting...", esp_err_to_name(Ret));
        esp_spiffs_format(conf.partition_label);
        return;
    }
    else
    {
        ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
    }
    if (used > total)
    {
        ESP_LOGW(TAG, "Number of used bytes cannot be larger than total. Performing SPIFFS_check().");
        Ret = esp_spiffs_check(conf.partition_label);
        if (Ret != ESP_OK)
        {
            ESP_LOGE(TAG, "SPIFFS_check() failed (%s)", esp_err_to_name(Ret));
            return;
        }
        else
        {
            ESP_LOGI(TAG, "SPIFFS_check() successful");
        }
    }
    SpiffsCheckingPerforming(conf);
}

/**
 *@brief Check if a file exists in the SPIFFS file system.
 * @param addressInSpiffs The address of the file in SPIFFS.
 * @return True if the file exists, false otherwise.
 */
bool SpiffsExistenceCheck(char *addressInSpiffs)
{
    bool retValue;
    FILE *file;
    file = fopen(addressInSpiffs, "r");
    if (file)
    {
        retValue = true;
    }
    else
    {
        retValue = false;
    }
    fclose(file);
    return retValue;
}
/**
 *@brief Write data to a new file or append to existing file
 *@param addressInSpiffs File path
 *@param data Data to write
 */
void SpiffsWrite(char *addressInSpiffs, char *data)
{
    if (SpiffsExistenceCheck(addressInSpiffs) == false)
    {
        ESP_LOGI(TAG, "Opening file for writing");
        FILE *file = fopen(addressInSpiffs, "w");
        if (file == NULL)
        {
            ESP_LOGE(TAG, "Failed to open file for writing");
            return;
        }
        fprintf(file, data);
        ESP_LOGI(TAG, "File written");
        fclose(file);
    }
    else
    {
        ESP_LOGI(TAG, "Opening file for appending");
        FILE *file = fopen(addressInSpiffs, "a");
        if (file == NULL)
        {
            ESP_LOGE(TAG, "Failed to open file for appending");
            return;
        }
        fprintf(file, "$%s", data);
        ESP_LOGI(TAG, "File appended");
        fclose(file);
    }
}

/**
 *@brief Read the contents of a file in the SPIFFS file system and store it in a buffer.
 * @param addressInSpiffs The address of the file in SPIFFS.
 * @param Buffer The buffer to store the file contents.
 * @param SizeOfBuffer The size of the buffer.
 */
void SpiffsRead(char *addressInSpiffs, char *Buffer, size_t SizeOfBuffer)
{
    char *InternalBuf = (char *)malloc(SizeOfBuffer * sizeof(char));
    memset(Buffer, 0x00, SizeOfBuffer);
    FILE *file;
    if (SpiffsExistenceCheck(addressInSpiffs) == 0)
    {
        return;
    }
    file = fopen(addressInSpiffs, "r");
    static int Counter = 0;
    memset(InternalBuf, 0x00, SizeOfBuffer);
    while (fgets(InternalBuf, SizeOfBuffer, file) != NULL)
    {
        strcat(Buffer, InternalBuf);
        Counter = strlen(InternalBuf) + Counter;
    }
    Buffer[Counter + 1] = '\0';
    ESP_LOGI(TAG, "File red");
    fclose(file);
}

/**
 *@brief This function renames a file in SPIFFS.
 *@param[in] oldName The name of the file to be renamed.
 *@param[in] newName The new name for the file.
 *@return Returns true if the rename operation is successful, and false otherwise.
 */
void SpiffsRename(char *OldAddress, char *NewAddress)
{

    // FILE *file = fopen(OldAddress, "r");
    if (SpiffsExistenceCheck(OldAddress) == 0)
    {
        return;
    }
    struct stat st;
    if (stat(OldAddress, &st) == 0)
    {
        // Delete it if it exists
        unlink(OldAddress);
    }
    // Rename original file
    ESP_LOGI(TAG, "Renaming file");
    if (rename(OldAddress, NewAddress) != 0)
    {
        ESP_LOGE(TAG, "Rename failed");
        return;
    }
    ESP_LOGE(TAG, "Rename success!");
    // fclose(file);
}

/**
 *@brief This function removes a file from SPIFFS.
 *@param[in] filename The name of the file to be removed.
 *@return Returns true if the file removal is successful, and false otherwise.
 */
bool SpiffsRemoveFile(char *addressInSpiffs)
{
    if (SpiffsExistenceCheck(addressInSpiffs) == 1)
    {
        if (remove(addressInSpiffs) == 0)
        {
            ESP_LOGE(TAG, "File removed successfully.");
            return 0;
        }
        else
        {
            ESP_LOGE(TAG, "Unable to remove the file.");
            return 1;
        }
    }
    else
    {
        return 0;
    }
}

/**
 *@brief This function saves key-value pairs in a file in SPIFFS with a text format.
 *@param[in] filename The name of the file to save the key-value pairs.
 *@param[in] ... The variable arguments containing key-value pairs. The last argument must be NULL.
 *@return Returns true if the file is successfully saved, and false otherwise.
 */
void SaveFileInSpiffsWithTxtFormat(char *addressInSpiffs, char *key, char *value, ...)
{
    va_list args;
    va_start(args, value);
    cJSON *root = cJSON_CreateObject();
    while (key != NULL && value != NULL)
    {
        cJSON_AddStringToObject(root, key, value);
        key = va_arg(args, char *);
        value = va_arg(args, char *);
    }
    va_end(args);
    char *jsonStr = cJSON_Print(root);
    cJSON_Delete(root);
    SpiffsWrite(addressInSpiffs, jsonStr);
    free(jsonStr);
}

/**
 *@brief This function reads key-value pairs from a file in SPIFFS with a text format.
 *@param[in] addressInSpiffs The address of the file to read the key-value pairs.
 *@param[out] ... The variable arguments to store the retrieved values. The last argument must be NULL.
 *@return Returns true if the file is successfully read and key-value pairs are retrieved, and false otherwise.
 */
bool ReadTxtFileFromSpiffs(char *addressInSpiffs, char *key, char *value, ...)
{
    char *InternalBuf = (char *)malloc(InternalBufSize_ * sizeof(char));
    SpiffsRead(addressInSpiffs, InternalBuf, InternalBufSize_);
    cJSON *root = cJSON_Parse(InternalBuf);
    if (root == NULL)
    {
        printf("Failed to parse JSON.\n");
        free(InternalBuf);
        return false;
    }
    va_list args;
    va_start(args, value);
    char *currentKey = key;
    char *currentValue = value;
    bool keyFound = false;
    while (currentKey != NULL && currentValue != NULL)
    {
        cJSON *item = cJSON_GetObjectItemCaseSensitive(root, currentKey);
        if (item != NULL && cJSON_IsString(item))
        {
            const char *itemValue = item->valuestring;
            snprintf(currentValue, strlen(itemValue) + 1, "%s", itemValue);
            keyFound = true;
        }
        currentKey = va_arg(args, char *);
        currentValue = va_arg(args, char *);
    }
    va_end(args);
    cJSON_Delete(root);
    free(InternalBuf);
    return keyFound;
}

/**
 *@brief This function does global initialization for Spiffs and checks for save existence, and sends a signal if it exists
 */
void SpiffsGlobalConfig()
{
    SpiffsInit();
    if (SpiffsExistenceCheck(WifiConfigDirectoryAddressInSpiffs) == 1)
    {
        xSemaphoreGive(WifiParamExistenceCheckerSemaphore);
    }
}
#ifdef TEST
/**
 *@brief This function is a test scenario that demonstrates the usage of the SPIFFS and JSON-related functions.
 */
void SpiffsTest(void)
{
    SpiffsInit();
    SpiffsRemoveFile("/spiffs/hello.txt");
    SpiffsWrite("/spiffs/hello.txt", "This is Test!");
    char buf[1000];
    SpiffsRead("/spiffs/hello.txt", buf, sizeof(buf));
    SpiffsWrite("/spiffs/hello.txt", "QABtmfRQr3dQABtmfRQr3dAr_QABtmfRQr3dQABtmfRQr3dAr_QABtmfRQr3dQABtmfRQr3dAr_QABtmfRQr3dQABtmfRQr3dAr_QABtmfRQr3dQABtmfRQr3dAr_QABtmfRQr3dQABtmfRQr3dAr_QABtmfRQr3dQABtmfRQr3dAr_QABtmfRQr3dQABtmfRQr3dAr_QABtmfRQr3dQABtmfRQr3dAr_QABtmfRQr3dQABtmfRQr3dAr_QABtmfRQr3dQABtmfRQr3dAr_QABtmfRQr3dQABtmfRQr3dAr_QABtmfRQr3dQABtmfRQr3dAr_QABtmfRQr3dQABtmfRQr3dAr_QABtmfRQr3dQABtmfRQr3dAr_QABtmfRQr3dQABtmfRQr3dAr_QABtmfRQr3dAr_QABtmfRQr3d");
    SpiffsRead("/spiffs/hello.txt", buf, sizeof(buf));
    printf("\n\n\n\n%s\n\n\n", buf);
    SaveFileInSpiffsWithTxtFormat("/spiffs/hello.txt", "Key1", "test", "Key2", "544", "Key3", "bibibi", NULL, NULL);
    char value1[20];
    char value2[20];
    char value3[20];

    ReadFileFromSpiffsWithTxtFormat("/spiffs/hello.txt", "Key1", value1, "Key2", value2, "Key3", value3, NULL, NULL);

    printf("Value1: %s\n", value1);
    printf("Value2: %s\n", value2);
    printf("Value3: %s\n", value3);
}
#endif