#ifndef _SPIFFS_MANAGER_H_
#define _SPIFFS_MANAGER_H_
#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_err.h"
#include "esp_log.h"
#include "esp_spiffs.h"
#include <stdarg.h>
#include "cJSON.h"

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
void ReadFileFromSpiffsWithTxtFormat(char *addressInSpiffs, char *key, char *value, ...);

#endif