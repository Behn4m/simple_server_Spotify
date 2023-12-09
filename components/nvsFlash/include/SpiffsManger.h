#ifndef _SPIFFS_MANAGER_H_
#define _SPIFFS_MANAGER_H_
#ifdef __cplusplus
extern "C"
{
#endif
#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_err.h"
#include "esp_log.h"
#include "esp_spiffs.h"
#include <stdarg.h>
#include "cJSON.h"
#include "GlobalInit.h"

/**
 *@brief This function does global initialization for Spiffs, checks for save existence, and sends a signal if it exists
 */
void SpiffsGlobalConfig();

/**
 *@brief Perform a SPIFFS check on the specified partition and initd globally
 * @param conf The SPIFFS configuration.
 */
void SpiffsInit();

/**
 *@brief Read the contents of a file in the SPIFFS file system and store it in a buffer.
 * @param addressInSpiffs The address of the file in SPIFFS.
 * @param Buffer The buffer to store the file contents.
 * @param SizeOfBuffer The size of the buffer.
 */
void SpiffsRead(char *addressInSpiffs, char *Buffer, size_t SizeOfBuffer);

/**
 *@brief Write data to a new file or append to existing file
 *@param addressInSpiffs File path
 *@param data Data to write
 */
void SpiffsWrite(char *addressInSpiffs, char *data);

/**
 *@brief This function saves key-value pairs in a file in SPIFFS with a text format.
 *@param[in] filename The name of the file to save the key-value pairs.
 *@param[in] ... The variable arguments containing key-value pairs. The last argument must be NULL.
 *@return Returns true if the file is successfully saved, and false otherwise.
 */
void SaveFileInSpiffsWithTxtFormat(char *addressInSpiffs, char *key, char *value, ...);

/**
 *@brief This function reads key-value pairs from a file in SPIFFS with a text format.
 *@param[in] filename The name of the file to read the key-value pairs.
 *@param[out] ... The variable arguments to store the retrieved values. The last argument must be NULL.
 *@return Returns true if the file is successfully read and key-value pairs are retrieved, and false otherwise.
 */
void ReadTxtFileFromSpiffs(char *addressInSpiffs, char *key, char *value, ...);

/**
 *@brief Check if a file exists in the SPIFFS file system.
 * @param addressInSpiffs The address of the file in SPIFFS.
 * @return True if the file exists, false otherwise.
 */
bool SpiffsExistenceCheck(char *addressInSpiffs);

/**
 *@brief This function removes a file from SPIFFS.
 *@param[in] filename The name of the file to be removed.
 *@return Returns true if the file removal is successful, and false otherwise.
 */
bool SpiffsRemoveFile(char *addressInSpiffs);
#endif
#ifdef __cplusplus
}
#endif