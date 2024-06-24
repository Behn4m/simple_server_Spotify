#include <stdio.h>
#include <stdlib.h>
#include "authorization.h"
#include "OauthHttpLocalServer.h"
#include "SpiffsManger.h"
#include "OauthAPICall.h"
#include "cJSON.h"

// ****************************** Global Variables
ServiceInterfaceHandler_t *pInterfaceHandler;
PrivateHandler_t PrivateHandler;

// ****************************** Local Variables
static const char *TAG = "OAuthTask";

/**
 * @brief This function check Time for token validation
 * @return true if token expired, false otherwise
 * */
static bool Oauth_IsTokenExpired()
{
    bool tokenExpired = false;
    uint32_t elapsedTime = 
            (xTaskGetTickCount() - PrivateHandler.TokenLastUpdate) * portTICK_PERIOD_MS;             // Calculate the elapsed time since the last token was received
    elapsedTime = elapsedTime / 1000;                                                                               // Convert the elapsed time to seconds                                             
    if (elapsedTime > (HOUR - 300))
    {
        tokenExpired = true;  
        ESP_LOGW(TAG , "Token is Expired");                                                                 // If the elapsed time is greater than the expiration time, set tokenExpired to true
    }
    return tokenExpired;
}

/**
 * @brief This function extracts specific parameters from a JSON string and 
 *         assigns them to corresponding fields in a TokenParam structure.
 * @param[in] Json The input JSON string.
 * @param[in] token address to save tokin extracted information.
 * @return false if fail, true if finish successful.
 */
static bool ExtractTokenJson(char *Json, Token_t *token) 
{
    const char *TAG = "JsonExTraction";
    cJSON *J_Token = cJSON_Parse(Json);

    if (J_Token == NULL) 
    {
        ESP_LOGE(TAG,"%s",Json);
        ESP_LOGE(TAG, "Failed to parse JSON\n");
        return false;
    }

    cJSON *accessTokenObj = cJSON_GetObjectItem(J_Token, "access_token");
    if (accessTokenObj != NULL && accessTokenObj->type == cJSON_String) 
    {
        strncpy(token->AccessToken, accessTokenObj->valuestring, ACCESS_TOKEN_STR_SIZE - 1);
        token->AccessToken[ACCESS_TOKEN_STR_SIZE - 1] = '\0';
    }

    cJSON *tokenTypeObj = cJSON_GetObjectItem(J_Token, "token_type");
    if (tokenTypeObj != NULL && tokenTypeObj->type == cJSON_String) 
    {
        strncpy(token->TokenType, tokenTypeObj->valuestring, TOKEN_TYPE_STR_SIZE - 1);
        token->TokenType[TOKEN_TYPE_STR_SIZE - 1] = '\0';
    }

    cJSON *expiresInObj = cJSON_GetObjectItem(J_Token, "expires_in");
    if (expiresInObj->type == cJSON_Number) 
    {
        token->ExpiresInMS = expiresInObj->valueint;
    }

    cJSON *refreshTokenObj = cJSON_GetObjectItem(J_Token, "refresh_token");
    if (refreshTokenObj != NULL && refreshTokenObj->type == cJSON_String) 
    {
        strncpy(token->RefreshToken, refreshTokenObj->valuestring, REFRESH_TOKEN_STP_SIZE - 1);
        token->RefreshToken[REFRESH_TOKEN_STP_SIZE - 1] = '\0';
    }

    cJSON *scopeObj = cJSON_GetObjectItem(J_Token, "scope");
    if (scopeObj != NULL && scopeObj->type == cJSON_String) 
    {
        strncpy(token->GrantedScope, scopeObj->valuestring, GRANTED_SCOP_STR_SIZE - 1);
        token->GrantedScope[GRANTED_SCOP_STR_SIZE - 1] = '\0';
    }

    cJSON_Delete(J_Token);
    return true;
}

/**
 * @brief This function reads refresh token from spiffs and send request for new token
 * @return True if token received and saved, false for otherwise
 */
static bool Oauth_TokenRenew(void)
{
    char receivedData[LONG_BUF];
    ReadTxtFileFromSpiffs(pInterfaceHandler->ConfigAddressInSpiffs, 
                            "refresh_token", receivedData, NULL, NULL);
    SendRequest_ExchangeTokenWithRefreshToken(receivedData);
    memset(receivedData, 0x0, LONG_BUF);
    
    bool IsResponseReady = 
            xSemaphoreTake(PrivateHandler.ServiceBuffer.ResponseReadyFlag, 
                            pdMS_TO_TICKS(SEC));
    if (!IsResponseReady)
    {
        ESP_LOGW(TAG, "timeout - Service not responded!");
        return false;
    }

    bool IsTokenExtracted = 
            ExtractTokenJson(PrivateHandler.ServiceBuffer.MessageBuffer,
                                                    &PrivateHandler.token);
    if (!IsTokenExtracted)
    {
        ESP_LOGW(TAG, "new Token not found!");
        return false;
    }

    ESP_LOGI(TAG, "new Token found!");
    PrivateHandler.TokenLastUpdate = xTaskGetTickCount();
    return true;
}

static void Oauth_MainTask(void *pvparameters)
{
    while (1)
    {
        char receivedData[MEDIUM_BUF];
        switch (PrivateHandler.Status)
        {
            case INIT:
            {
                bool IsServerInit = HttpServerServiceInit();                                                     // Init the webserver in initialization
                if (!IsServerInit)
                {
                    ESP_LOGE(TAG, "Authorization initialization failed!");
                    break;
                }

                bool IsSpiffExists = SpiffsExistenceCheck(pInterfaceHandler->ConfigAddressInSpiffs);                      // Check if the refresh token exists in the spiffs
                if (!IsSpiffExists)
                {
                    ESP_LOGE(TAG, "Autorization is needed");
                    PrivateHandler.Status = LOGIN;                                                              // If the refresh token does not exist, set the status to LOGIN 
                    break;
                }

                ESP_LOGI(TAG, "RefreshToken found");
                PrivateHandler.Status = EXPIRED;                                                            // If the refresh token exists, set the status to EXPIRED
                break;
            }
            case LOGIN:
            {
                bool IsCodeReceived = xQueueReceive(SendCodeFromHttpToTask, receivedData, pdMS_TO_TICKS(SEC));       // Waiting for Code to be recieved by queue
                if (!IsCodeReceived)
                {
                    // stay in this state until receive the Code
                    break;
                }
                SendTokenRequest(receivedData);                                                         // send request for Token
                PrivateHandler.Status = AUTHENTICATED;                                                          // Code received and checked, so update Status to AUTHENTICATED         
                break;
            }
            case AUTHENTICATED:
            {
                ESP_LOGI(TAG, "AUTHENTICATED");
                bool IsServiceResponded = xSemaphoreTake(PrivateHandler.ServiceBuffer.ResponseReadyFlag, pdMS_TO_TICKS(SEC));              // Waiting for Token to be recieved by queue
                if (!IsServiceResponded)
                {
                    PrivateHandler.Status = LOGIN;                                                                  // if the response did not come within the expected time, set Status back to LOGIN
                    ESP_LOGW(TAG, "Timeout - Service did not respond within the expected time.!");
                    break;
                }

                bool IsTokenExtracted =
                 ExtractTokenJson(PrivateHandler.ServiceBuffer.MessageBuffer,
                                     &PrivateHandler.token);                       // extract all keys from Service server response
                if (!IsTokenExtracted)
                {
                    ESP_LOGW(TAG, "Token not found!");
                    PrivateHandler.Status = LOGIN;                                                               // the reponse did not include all needed keys, so set Status back to LOGIN
                    break;
                }

                ESP_LOGI(TAG, "Token found!");
                PrivateHandler.TokenLastUpdate = xTaskGetTickCount();                                       // Save the time when the token was received
                PrivateHandler.Status = AUTHORIZED;                                                         // Token recieved and checked, so update Status to AUTHORIZED
                break;
            }
            case AUTHORIZED:
            {
                ESP_LOGI(TAG, "AUTHORIZED");
                SpiffsRemoveFile(pInterfaceHandler->ConfigAddressInSpiffs);                                          // Delete old value at the directory
                                                                                                                    // so applicaiton can know the Service Module is authorized 
                                                                                                                    // and ready for sending commands 
                SaveFileInSpiffsWithTxtFormat(pInterfaceHandler->ConfigAddressInSpiffs,                              // Save new file in the directory
                                              "refresh_token", PrivateHandler.token.RefreshToken, 
                                              NULL, NULL);
                xSemaphoreGive((pInterfaceHandler->IsServiceAuthorizedSemaphore));                                  // give IsServiceAuthorizedSemaphore semaphore in the IntefaceHandler                
                PrivateHandler.Status = CHECK_TIME;                                                                 // set Status to CHECK_TIME     
                break;
            }
            case CHECK_TIME:
            {
                bool IsTokenExpired = Oauth_IsTokenExpired();                                                               // Check if the expiration time has elapsed since the last received token
                if (!IsTokenExpired)
                {
                    // keep state
                    break;
                }  
                PrivateHandler.Status = EXPIRED;                                                                // set Status to EXPIRED if token expired
          
                break;
            }
            case EXPIRED:
            {
                ESP_LOGW(TAG, "token is expired");
                bool IsTokenRenewed = Oauth_TokenRenew();
                if (!IsTokenRenewed)                                                                   // Run function to renew the token
                {
                    PrivateHandler.Status = LOGIN;
                    break;                                                                   // set Status to ILDLE if token renewed unsuccessfully
                }

                xSemaphoreGive((pInterfaceHandler->IsServiceAuthorizedSemaphore));                              // give IsServiceAuthorizedSemaphore semaphore in the IntefaceHandler
                PrivateHandler.TokenLastUpdate = xTaskGetTickCount();                                           // Save the time when the token was received
                PrivateHandler.Status = CHECK_TIME;                                                             // set Status to CHECK_TIME if token renewed successfully
                break;
            }
        }   
        vTaskDelay(pdMS_TO_TICKS(100)); //FIXME magic number                                                                            // Task delay at the end of while(1) loop
    }
}

/**
 * @brief This function initiates the authorization process.
 * @param InterfaceHandler as the handler
 * @return true if task run to the end
 */
bool Oauth_TaskInit(ServiceInterfaceHandler_t *serviceInterfaceHandler)
{
    pInterfaceHandler = serviceInterfaceHandler;
    PrivateHandler.Status = INIT;
    if (pInterfaceHandler->ConfigAddressInSpiffs != NULL &&
        pInterfaceHandler->IsServiceAuthorizedSemaphore != NULL)
    {
        StaticTask_t *xTaskBuffer = (StaticTask_t *)malloc(sizeof(StaticTask_t));
        StackType_t *xStack = 
                (StackType_t *)malloc(SERVICE_TASK_STACK_SIZE * sizeof(StackType_t));                 // Assuming a stack size of 400 words (adjust as needed)
        if (xTaskBuffer == NULL || xStack == NULL)
        {
            ESP_LOGE(TAG, "Memory allocation failed!\n");
            free(xTaskBuffer);
            free(xStack);
            return false; // Exit with an error code
        }
        xTaskCreateStatic(
            Oauth_MainTask,                    // Task function
            "Oauth_MainTask",                  // Task name (for debugging)
            SERVICE_TASK_STACK_SIZE,             // Stack size (in words)
            NULL,                                // Task parameters (passed to the task function)
            tskIDLE_PRIORITY + SERVICE_PRIORITY, // Task priority (adjust as needed)
            xStack,                              // Stack buffer
            xTaskBuffer                          // Task control block
        );

        // Allocate buffer and Initialize the service API call //TODO change dynamic mem to static
        PrivateHandler.ServiceBuffer.MessageBuffer =
                (char *)malloc(SUPER_BUF * sizeof(char));    
        PrivateHandler.ServiceBuffer.ResponseReadyFlag = xSemaphoreCreateBinary();
        APICallInit(&PrivateHandler.ServiceBuffer);

        ESP_LOGI(TAG, "App initiated successfully");
    }
    else
    {
        ESP_LOGE(TAG, "IntefaceHandler is is missing some pointers, can not run the app");
        return false;
    }
    return true;
}