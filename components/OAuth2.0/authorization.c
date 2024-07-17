#include <stdio.h>
#include <stdlib.h>
#include "authorization.h"
#include "OauthHttpLocalServer.h"
#include "SpiffsManger.h"
#include "OauthAPICall.h"
#include "cJSON.h"

// ****************************** Global Variables
OAuthInterfaceHandler_t *AuthInterfaceHandler;
OAuthPrivateHandler_t AuthPrivateHandler;

// ****************************** Local Variables
static const char *TAG = "OAuthTask";

/**
 * @brief This function check Time for token validation
 * @return true if token expired, false otherwise
 * */
static bool Oauth_IsTokenExpired()
{
    bool tokenExpired = false;
    uint32_t elapsedTime = (xTaskGetTickCount() - AuthPrivateHandler.TokenLastUpdate) 
                            * portTICK_PERIOD_MS;             
    elapsedTime = elapsedTime / 1000;                                           
    if (elapsedTime > (HOUR - 300))
    {
        tokenExpired = true;  
        ESP_LOGW(TAG , "Token is Expired");
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
    ReadTxtFileFromSpiffs(AuthInterfaceHandler->ConfigAddressInSpiffs, 
                            "refresh_token", receivedData, NULL, NULL);
    SendRequest_ExchangeTokenWithRefreshToken(receivedData, &AuthInterfaceHandler->ClientConfig);
    memset(receivedData, 0x0, LONG_BUF);
    
    bool IsResponseReady = 
            xSemaphoreTake(AuthPrivateHandler.OAuthBuffer.ResponseReadyFlag, 
                            pdMS_TO_TICKS(SEC));
    if (!IsResponseReady)
    {
        ESP_LOGW(TAG, "timeout - Service not responded!");
        return false;
    }

    bool IsTokenExtracted = 
            ExtractTokenJson(AuthPrivateHandler.OAuthBuffer.MessageBuffer,
                                                    &AuthPrivateHandler.token);
    if (!IsTokenExtracted)
    {
        ESP_LOGW(TAG, "new Token not found!");
        return false;
    }

    ESP_LOGI(TAG, "new Token found!");
    AuthPrivateHandler.TokenLastUpdate = xTaskGetTickCount();
    return true;
}

/**
 * @brief This function sends the token request to the Service server.
 * @param[in] Code The authorization code to be sent.
 * @param[in] ClientConfig The client configuration.
 * @return This function does not return a value.
*/
static void Oauth_MainTask(void *pvparameters)
{
    while (true)
    {
        char receivedData[MEDIUM_BUF];
        switch (AuthPrivateHandler.Status)
        {
            case INIT:
            {
                bool IsServerInit = Oauth_HttpServerServiceInit(AuthInterfaceHandler->ClientConfig);
                if (!IsServerInit)
                {
                    ESP_LOGE(TAG, "Authorization initialization failed!");
                    break;
                }
                bool IsSpiffExists = SpiffsExistenceCheck(AuthInterfaceHandler->ConfigAddressInSpiffs);
                if (!IsSpiffExists)
                {
                    ESP_LOGE(TAG, "Autorization is needed");
                    AuthPrivateHandler.Status = LOGIN;
                    break;
                }
                ESP_LOGI(TAG, "RefreshToken found");
                AuthPrivateHandler.Status = EXPIRED;
                break;
            }
            case LOGIN:
            {
                bool IsCodeReceived = xQueueReceive(SendCodeFromHttpToTask, receivedData, 
                                                    pdMS_TO_TICKS(SEC));
                if (!IsCodeReceived)
                {
                    // stay in this state until receive the Code
                    break;
                }
                SendTokenRequest(receivedData, &AuthInterfaceHandler->ClientConfig);
                AuthPrivateHandler.Status = AUTHENTICATED;         
                break;
            }
            case AUTHENTICATED:
            {
                ESP_LOGI(TAG, "AUTHENTICATED");
                bool IsServiceResponded = xSemaphoreTake(AuthPrivateHandler.OAuthBuffer.ResponseReadyFlag, 
                                                         pdMS_TO_TICKS(SEC));
                if (!IsServiceResponded)
                {
                    AuthPrivateHandler.Status = LOGIN;
                    ESP_LOGW(TAG, "Timeout - Service did not respond within the expected time.!");
                    break;
                }

                bool IsTokenExtracted =
                 ExtractTokenJson(AuthPrivateHandler.OAuthBuffer.MessageBuffer,&AuthPrivateHandler.token);
                if (!IsTokenExtracted)
                {
                    ESP_LOGW(TAG, "Token not found!");
                    AuthPrivateHandler.Status = LOGIN;
                    break;
                }

                ESP_LOGI(TAG, "Token found!");
                AuthPrivateHandler.TokenLastUpdate = xTaskGetTickCount();
                AuthPrivateHandler.Status = AUTHORIZED;
                break;
            }
            case AUTHORIZED:
            {
                ESP_LOGI(TAG, "AUTHORIZED");
                SpiffsRemoveFile(AuthInterfaceHandler->ConfigAddressInSpiffs);
                SaveFileInSpiffsWithTxtFormat(AuthInterfaceHandler->ConfigAddressInSpiffs,
                                              "refresh_token", AuthPrivateHandler.token.RefreshToken, 
                                              NULL, NULL);
                xSemaphoreGive((*AuthInterfaceHandler->IsServiceAuthorizedSemaphore));                 
                AuthPrivateHandler.Status = CHECK_TIME;   
                break;
            }
            case CHECK_TIME:
            {
                bool IsTokenExpired = Oauth_IsTokenExpired(); 
                if (!IsTokenExpired)
                {
                    // keep state
                    break;
                }  
                AuthPrivateHandler.Status = EXPIRED;
                break;
            }
            case EXPIRED:
            {
                ESP_LOGW(TAG, "token is expired");
                bool IsTokenRenewed = Oauth_TokenRenew();
                if (!IsTokenRenewed)
                {
                    AuthPrivateHandler.Status = LOGIN;
                    break;
                }

                xSemaphoreGive((*AuthInterfaceHandler->IsServiceAuthorizedSemaphore));                              
                AuthPrivateHandler.TokenLastUpdate = xTaskGetTickCount();                                           
                AuthPrivateHandler.Status = CHECK_TIME;                                                            
                break;
            }
        }   
        vTaskDelay(pdMS_TO_TICKS(100));            // Task delay at the end of while(1) loop
    }
}

/**
 * @brief This function initiates the authorization process.
 * @param InterfaceHandler as the handler
 * @return true if task run to the end, false if any error occurs
 */
bool Oauth_TaskInit(OAuthInterfaceHandler_t *InterfaceHandler)
{
    AuthInterfaceHandler = InterfaceHandler;
    AuthPrivateHandler.Status = INIT;
    if (AuthInterfaceHandler->ConfigAddressInSpiffs != NULL &&
        AuthInterfaceHandler->IsServiceAuthorizedSemaphore != NULL)
    {
        StaticTask_t *xTaskBuffer = (StaticTask_t *)malloc(sizeof(StaticTask_t));
        StackType_t *xStack = (StackType_t *)malloc(SERVICE_TASK_STACK_SIZE * sizeof(StackType_t));                 
        if (xTaskBuffer == NULL || xStack == NULL)
        {
            ESP_LOGE(TAG, "Memory allocation failed!\n");
            free(xTaskBuffer);
            free(xStack);
            return false; // Exit with an error code
        }
        xTaskCreateStatic(
            Oauth_MainTask,                         // Task function
            "Oauth_MainTask",                       // Task name (for debugging)
            SERVICE_TASK_STACK_SIZE,                // Stack size (in words)
            NULL,                                   // Task parameters (passed to the task function)
            tskIDLE_PRIORITY + SERVICE_PRIORITY,    // Task priority (adjust as needed)
            xStack,                                 // Stack buffer
            xTaskBuffer                             // Task control block
        );
        // Allocate buffer and Initialize the service API call //TODO change dynamic mem to static
        AuthPrivateHandler.OAuthBuffer.MessageBuffer = (char *)malloc(SUPER_BUF * sizeof(char));    
        AuthPrivateHandler.OAuthBuffer.ResponseReadyFlag = xSemaphoreCreateBinary();
        APICallInit(&AuthPrivateHandler.OAuthBuffer, AuthInterfaceHandler->ClientConfig.base64Credintials);

        ESP_LOGI(TAG, "App initiated successfully");
    }
    else
    {
        ESP_LOGE(TAG, "IntefaceHandler is is missing some pointers, can not run the app");
        return false;
    }
    return true;
}

/**
 * @brief This function returns the access token.
 * @param OAuthInterfaceHandler as the handler
 * @return the access token
 */
char* Oauth_GetAccessToken(void)
{
    return AuthPrivateHandler.token.AccessToken;
}
