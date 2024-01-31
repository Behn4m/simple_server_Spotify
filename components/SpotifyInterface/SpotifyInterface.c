#include <stdio.h>
#include "SpotifyTypedef.h"
#include "SpotifyInterface.h"
#include "SpotifyAPICall.h"
#include "SpotifyHttpLocalServer.h"
#include "SpiffsManger.h"
#include "JsonExtraction.h"
#include "SpotifyTypedef.h"

// ****************************** Global Variables
SpotifyInterfaceHandler_t *InterfaceHandler;
SpotifyPrivateHandler_t PrivateHandler;
bool SaveExistence = 0;

// ****************************** Local Variables
static const char *TAG = "SpotifyTask";

// ****************************** Local Functions
static void Spotify_MainTask(void *pvparameters);
static bool Spotify_TokenRenew(void);
static bool Spotify_IsTokenExpired();
bool Spotify_ExtractAccessToken(char *receivedData, size_t SizeOfReceivedData);

// ******************************
void Spotify_CheckRefreshTokenExistence()
{
    if (xSemaphoreTake(*(InterfaceHandler->WorkWithStorageInSpotifyComponentSemaphore), 1) == pdTRUE)
    {
        PrivateHandler.Status = EXPIRED;
        SaveExistence = true;
    }
}

/**
 * @brief This function initiates the Spotify authorization process.
 * @param SpotifyInterfaceHandler as the handler
 * @return true if task run to the end
 */
bool Spotify_TaskInit(SpotifyInterfaceHandler_t *SpotifyInterfaceHandler)
{
    InterfaceHandler = SpotifyInterfaceHandler;
    PrivateHandler.Status = INIT;
    Spotify_CheckRefreshTokenExistence();
    if (InterfaceHandler->ConfigAddressInSpiffs != NULL &&
        InterfaceHandler->HttpsResponseReadySemaphore != NULL &&
        InterfaceHandler->IsSpotifyAuthorizedSemaphore != NULL &&
        InterfaceHandler->HttpsBufQueue != NULL)
    {
        StaticTask_t *xTaskBuffer = (StaticTask_t *)malloc(sizeof(StaticTask_t));
        StackType_t *xStack = (StackType_t *)malloc(SPOTIFY_TASK_STACK_SIZE * sizeof(StackType_t));                 // Assuming a stack size of 400 words (adjust as needed)
        if (xTaskBuffer == NULL || xStack == NULL)
        {
            ESP_LOGE(TAG, "Memory allocation failed!\n");
            free(xTaskBuffer);
            free(xStack);
            return false; // Exit with an error code
        }
        xTaskCreateStatic(
            Spotify_MainTask,                    // Task function
            "Spotify_MainTask",                  // Task name (for debugging)
            SPOTIFY_TASK_STACK_SIZE,             // Stack size (in words)
            NULL,                                // Task parameters (passed to the task function)
            tskIDLE_PRIORITY + SPOTIFY_PRIORITY, // Task priority (adjust as needed)
            xStack,                              // Stack buffer
            xTaskBuffer                          // Task control block
        );

        // create queue to transfer data between the http and interface tasks
        httpToSpotifyDataQueue = xQueueCreate(1, sizeof(char) * sizeof(char[LONG_BUF]));

        ESP_LOGI(TAG, "Spotify app initiated successfully");
    }
    else
    {
        ESP_LOGE(TAG, "SpotifyIntefaceHandler is is missing some pointers, can not run the app");
        return false;
    }
    return true;
}

/**
 * @brief Run Http local service
 */
bool Spotify_HttpServerServiceInit()
{
    SendCodeFromHttpToSpotifyTask = xQueueCreate(1, sizeof(char) * sizeof(char[MEDIUM_BUF]));
    
    httpd_handle_t spotifyLocalServer = Spotify_StartWebServer();
    if (spotifyLocalServer == NULL)
    {
        ESP_LOGE(TAG, "Creating Spotify local server failed!");
        return false;
    }
    
    if (Spotify_StartMDNSService() == false)
    {
        ESP_LOGE(TAG, "Running mDNS failed!");
        return false;
    };

    ESP_LOGI(TAG, "** Spotify local server created, mDNS is running! **");
    return true;
}

/**
 * @brief This function is the entry point for handling HTTPS requests for Spotify authorization.
 * @param[in] parameters because it is a Task!
 */
static void Spotify_MainTask(void *pvparameters)
{
    while (1)
    {
        char receivedData[MEDIUM_BUF];
        switch (PrivateHandler.Status)
        {
            case INIT:
            {
                if (Spotify_HttpServerServiceInit() == true)                                                        // Init the webserver in initialization
                {
                    PrivateHandler.Status = LOGIN;                                                                   // if webserver and mDNS run successfully, go to LOGIN
                }
                else
                {
                    ESP_LOGE(TAG, "Authorization initialization failed!");
                }
                break;
            }
            case LOGIN:
            {
                if (xQueueReceive(SendCodeFromHttpToSpotifyTask, receivedData, pdMS_TO_TICKS(SEC)) == pdTRUE)       // Waiting for Code to be recieved by queue
                {
                    ESP_LOGI(TAG, "Received CODE by queue: %s\n", receivedData);
                    Spotify_SendTokenRequest(receivedData);                                                         // send request for Token
                    PrivateHandler.Status = AUTHENTICATED;                                                          // Code received and checked, so update Status to AUTHENTICATED         
                }
                break;
            }
            case AUTHENTICATED:
            {
                ESP_LOGI(TAG, "AUTHENTICATED");
                if (xQueueReceive(httpToSpotifyDataQueue, receivedData, pdMS_TO_TICKS(SEC)) == pdTRUE)              // Waiting for Token to be recieved by queue
                {
                    if (Spotify_ExtractAccessToken(receivedData, sizeof(receivedData)) == true)                     // extract all keys from spotify server response
                    {
                        ESP_LOGI(TAG, "Token found!");
                        PrivateHandler.TokenLastUpdate = xTaskGetTickCount();
                        PrivateHandler.Status = AUTHORIZED;                                                         // Token recieved and checked, so update Status to AUTHORIZED
                    }
                    else
                    {
                        ESP_LOGW(TAG, "Token not found!");
                        PrivateHandler.Status = LOGIN;                                                               // the reponse did not include all needed keys, so set Status back to LOGIN
                    }            
                }
                else
                {
                    PrivateHandler.Status = LOGIN;
                    ESP_LOGW(TAG, "Timeout - Spotify did not respond within the expected time.!");
                }
                break;
            }
            case AUTHORIZED:
            {
                ESP_LOGI(TAG, "AUTHORIZED");
                xSemaphoreGive((*InterfaceHandler->IsSpotifyAuthorizedSemaphore));                                  // give IsSpotifyAuthorizedSemaphore semaphore in the inteface handler
                                                                                                                    // so applicaiton can know the Spotify Module is authorized 
                                                                                                                    // and ready for sending commands 
                if (SaveExistence != 1)
                {
                    PrivateHandler.Status = SAVE_NEW_TOKEN;
                }
                else
                {
                    PrivateHandler.Status = CHECK_TIME;
                }
                break;
            }
            case CHECK_TIME:
            {
                if (Spotify_IsTokenExpired() == true)                                                                 // Check if the expiration time has elapsed since the last received token
                {
                    PrivateHandler.Status = EXPIRED;
                }            
                break;
            }
            case SAVE_NEW_TOKEN:
            {
                ESP_LOGI(TAG, "SAVE_NEW_TOKEN");
                SpiffsRemoveFile(InterfaceHandler->ConfigAddressInSpiffs);                                          // Delete old value at the directory
                SaveFileInSpiffsWithTxtFormat(InterfaceHandler->ConfigAddressInSpiffs,                              // Save new file in the directory
                                            "refresh_token", PrivateHandler.token.RefreshToken, 
                                            NULL, NULL);
                PrivateHandler.Status = CHECK_TIME;                                                                 // Set back the Status to the CHECK_TIME
                break;
            }
            case EXPIRED:
            {
                ESP_LOGW(TAG, "token is expired");
                if (Spotify_TokenRenew() == true)                                                                   // Run function to renew the token
                {
                    PrivateHandler.Status = AUTHORIZED;                                                             // set Status to AUTHORIZED if token renewed successfully
                }
                else
                {
                    PrivateHandler.Status = LOGIN;                                                                   // set Status to ILDLE if token renewed unsuccessfully
                }
                break;
            }
        }   
        vTaskDelay(pdMS_TO_TICKS(100));                                                                             // Task delay at the end of while(1) loop
    }
}

/**
 * @brief This function check Time for
 * @return true if token expired, false otherwise
 * */
static bool Spotify_IsTokenExpired()
{
    bool tokenExpired = false;
    uint32_t elapsedTime = (xTaskGetTickCount() - PrivateHandler.TokenLastUpdate) * portTICK_PERIOD_MS;
    elapsedTime = elapsedTime / 1000;
    if (elapsedTime > (HOUR - 300))
    {
        tokenExpired = true;
    }
    return tokenExpired;
}

/**
 * @brief This function reads refresh token from spiffs and send request for new token
 * @return True if token received and saved, false for otherwise
 */
static bool Spotify_TokenRenew(void)
{
    char receivedData[LONG_BUF];
    ReadTxtFileFromSpiffs(InterfaceHandler->ConfigAddressInSpiffs, "refresh_token", receivedData, NULL, NULL);
    ESP_LOGI(TAG, "RefreshToken=%s", receivedData);
    SendRequest_ExchangeTokenWithRefreshToken(receivedData);
    memset(receivedData, 0x0, LONG_BUF);

    if (xQueueReceive(httpToSpotifyDataQueue, receivedData, pdMS_TO_TICKS(SEC)) == pdTRUE)
    {
        if (Spotify_ExtractAccessToken(receivedData, sizeof(receivedData)) == true)
        {
            ESP_LOGI(TAG, "new Token found!");
            PrivateHandler.TokenLastUpdate = xTaskGetTickCount();
            return true;
        }
        else
        {
            ESP_LOGW(TAG, "new Token not found!");
            return false;
        }
    }
    else
    {
        ESP_LOGW(TAG, "timeout - Spotify not responded!");
        return false;
    }
}

/**
 * @brief This function  Extracts JSON content from an HTTP response string
 *        and give value to privet handler variable , if all things was true , spotify event handler
 *        register
 * @return True if token received and saved, false for otherwise
 */
bool Spotify_ExtractAccessToken(char *ReceivedData, size_t SizeOfReceivedData)
{
    // extract keys from JSON
    if (ExtractAccessTokenParamsTokenFromJson(ReceivedData, SizeOfReceivedData,
                                              PrivateHandler.token.AccessToken,
                                              PrivateHandler.token.TokenType,
                                              PrivateHandler.token.RefreshToken,
                                              PrivateHandler.token.GrantedScope,
                                              PrivateHandler.token.ExpiresInMS) == true)
    {
        ESP_LOGW(TAG, "%s", PrivateHandler.token.AccessToken );
        return true;
    }
    else
    {
        ESP_LOGE(TAG, "TOKEN extraction failed, back to LOGIN state");
        return false;
        // TO DO: the handler should reset here
    }
}

/**
 * @brief Sends a command to control Spotify.
 * This function sends various commands to control the Spotify application based on the given command value.
 * @param command A pointer to the command to be sent to Spotify.
 * @return true if the command is successfully sent, false otherwise.
 *
 * @note Possible command values include:
 * - NO_COMMAND: Waits for a command.
 * - PLAY: Sends the PLAY command to Spotify.
 * - PAUSE: Sends the PAUSE command to Spotify.
 * - PLAY_NEXT: Sends the PLAY_NEXT command to Spotify.
 * - PLAY_PREV: Sends the PLAY_PREV command to Spotify.
 * - GET_NOW_PLAYING: Sends the GET_NOW_PLAYING command to Spotify.
 * - GET_USER_INFO: Sends the GET_USER_INFO command to Spotify.
 * - GET_SONG_IMAGE_URL: Sends the GET_SONG_IMAGE_URL command to Spotify.
 * - GET_ARTIST_IMAGE_URL: Sends the GET_ARTIST_IMAGE_URL command to Spotify.
 */
bool Spotify_SendCommand(int Command)
{
    ESP_LOGI(TAG, "user Command is %d", Command);
    if (PrivateHandler.Status == LOGIN || PrivateHandler.Status == AUTHENTICATED)
    {
        ESP_LOGE(TAG, "You are not authorized !");
        return false;
    }
    switch (CCOMPARE_1)
    {
        case Play:
        case Pause:
        case PlayNext:
        case PlayPrev:
            Spotify_ControlPlayback(Command, PrivateHandler.token.AccessToken);
            // TO DO: process and store Spotify api service response
            break;
        case GetNowPlaying:
        case GetUserInfo:
        case GetUserTopItems:
            Spotify_GetInfo(Command, PrivateHandler.token.AccessToken);
            // TO DO: process and store Spotify api service response 
            break;
        case GetSongImageUrl:
            // TO DO: Implement this feature
            break;
        case GetArtisImageUrl:
            break;
            // TO DO: Implement this feature
    }
    return true;
}
