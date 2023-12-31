#include "SpotifyEventHandler.h"
// ****************************************************************
static const char *TAG = "Spotify_EventHandler";
esp_event_loop_handle_t Spotify_EventLoopHandle;
const esp_event_base_t BASE_SPOTIFY_EVENTS = "BASE_SPOTIFY_EVENTS";
ESP_EVENT_DECLARE_BASE(BASE_SPOTIFY_EVENTS);
// ****************************************************************
/**
 * @brief when this function call that somebody post event with Spotify event base
 * @param[in]  EventData is struct that has all things you need include pointer of callback function
 *             queue for receive data and user info and token info
 */
static void Spotify_EventHandler(void *Arg, esp_event_base_t EventBase,
                                 int32_t EventId, void *EventData)
{
    EventHandlerDataStruct_t *EventData_t;
    EventData_t = (EventHandlerDataStruct_t *)EventData;
    char *TempBuffer;
    TempBuffer = (char *)malloc(LONG_BUF * sizeof(char));
    if (TempBuffer == NULL)
    {
        ESP_LOGE(TAG, "Failed to allocate memory for the array.");
        return;
    }
    memset(TempBuffer, 0x0, LONG_BUF);
    if (EventBase == BASE_SPOTIFY_EVENTS)
    {
        switch (EventId)
        {
        case SpotifyEventSendRequestForNext_:
        {
            Spotify_SendRequestForNext(EventData_t->token);
            ESP_LOGW(TAG, "Spotify Event handler is working !");
            if (xQueueReceive((*EventData_t->HttpsBufQueue), TempBuffer, portMAX_DELAY) == pdTRUE)
            {
                ESP_LOGI(TAG, "Receive data in Event handler by queue ");
            }
            EventData_t->EventHandlerCallBackFunction(TempBuffer);

            break;
        }
        case SpotifyEventSendRequestForPrevious_:
        {
            Spotify_SendRequestForPrevious(EventData_t->token);
            if (xQueueReceive((*EventData_t->HttpsBufQueue), TempBuffer, portMAX_DELAY) == pdTRUE)
            {
                ESP_LOGI(TAG, "Receive data in Event handler by queue ");
            }
            EventData_t->EventHandlerCallBackFunction(TempBuffer);
            break;
        }
        case SpotifyEventSendRequestForPlay_:
        {
            Spotify_SendRequestForPlay(EventData_t->token);
            if (xQueueReceive((*EventData_t->HttpsBufQueue), TempBuffer, portMAX_DELAY) == pdTRUE)
            {
                ESP_LOGI(TAG, "Receive data in Event handler by queue ");
            }
            EventData_t->EventHandlerCallBackFunction(TempBuffer);
            break;
        }
        case SpotifyEventSendRequestForPause_:
        {
            Spotify_SendRequestForPause(EventData_t->token);
            if (xQueueReceive((*EventData_t->HttpsBufQueue), TempBuffer, portMAX_DELAY) == pdTRUE)
            {
                ESP_LOGI(TAG, "Receive data in Event handler by queue ");
            }
            EventData_t->EventHandlerCallBackFunction(TempBuffer);
            break;
        }
        case SpotifyEventGetUserStatus_:
        {
            Spotify_GetUserStatus(EventData_t->token);
            if (xQueueReceive((*EventData_t->HttpsBufQueue), TempBuffer, portMAX_DELAY) == pdTRUE)
            {
                ESP_LOGI(TAG, "Receive data in Event handler by queue ");
            }
            EventData_t->EventHandlerCallBackFunction(TempBuffer);
            break;
        }
        case SpotifyEventGetUserTopItems_:
        {
            Spotify_GetUserTopItems(EventData_t->token);
            if (xQueueReceive((*EventData_t->HttpsBufQueue), TempBuffer, portMAX_DELAY) == pdTRUE)
            {
                ESP_LOGI(TAG, "Receive data in Event handler by queue ");
            }
            EventData_t->EventHandlerCallBackFunction(TempBuffer);
            break;
        }
        case SpotifyEventGetUserProfile_:
        {
            if ((EventData_t->UserInfo.UserID[0]) == 0)
            {
                ESP_LOGE(TAG, "you should run 'GetUserInfo' before this !");
                free(TempBuffer);
                return;
            }
            Spotify_GetUserProfile(EventData_t->UserInfo.UserID, EventData_t->token);
            if (xQueueReceive((*EventData_t->HttpsBufQueue), TempBuffer, portMAX_DELAY) == pdTRUE)
            {
                ESP_LOGI(TAG, "Receive data in Event handler by queue ");
            }
            EventData_t->EventHandlerCallBackFunction(TempBuffer);
            break;
        }
        case SpotifyEventGetCurrentPlaying_:
        {
            Spotify_GetCurrentPlaying(EventData_t->token);
            if (xQueueReceive((*EventData_t->HttpsBufQueue), TempBuffer, portMAX_DELAY) == pdTRUE)
            {
                ESP_LOGI(TAG, "Receive data in Event handler by queue ");
            }
            EventData_t->EventHandlerCallBackFunction(TempBuffer);
            break;
        }
        }
    }
    free(TempBuffer);
}

/**
 * @brief This function register event handler for spotify , this event is task and need
 *        long stack ,because spotify functions need long stack
 */
void Spotify_RegisterEventHandler(void)
{
    esp_event_loop_args_t Spotify_EventLoopArgs = {
        .queue_size = 5,
        .task_name = "Spotify_Event_Task", // task will be created
        .task_priority = uxTaskPriorityGet(NULL),
        .task_stack_size = SpotifyEventStack,
        .task_core_id = tskNO_AFFINITY};
    esp_event_loop_create(&Spotify_EventLoopArgs, &Spotify_EventLoopHandle);
    ESP_ERROR_CHECK(esp_event_handler_instance_register_with(Spotify_EventLoopHandle, BASE_SPOTIFY_EVENTS, ESP_EVENT_ANY_ID, Spotify_EventHandler, Spotify_EventLoopHandle, NULL));
}

/**
 * @brief This function register event handler for spotify and delete event loop
 */
void Spotify_UnRegisterEventHandler(void)
{
    esp_event_handler_unregister_with(Spotify_EventLoopHandle, BASE_SPOTIFY_EVENTS, ESP_EVENT_ANY_ID, &Spotify_EventHandler);
    esp_event_loop_delete(Spotify_EventLoopHandle);
}