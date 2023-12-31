#include "SpotifyEventHandler.h"

/* *****************************************************************/
static const char *TAG = "Spotify_EventHandler";
esp_event_loop_handle_t Spotify_EventLoopHandle;
const esp_event_base_t BASE_SPOTIFY_EVENTS = "BASE_SPOTIFY_EVENTS";
ESP_EVENT_DECLARE_BASE(BASE_SPOTIFY_EVENTS);
/* *****************************************************************/
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
        case SpotifyEventSendRequestForNext:
        {
            Spotify_SendRequestForNext(EventData_t->token);
            if (xQueueReceive((*EventData_t->HttpsBufQueue), TempBuffer, portMAX_DELAY) == pdTRUE)
            {
                ESP_LOGI(TAG, "Data received in Event handler by queue ");
            }
            EventData_t->EventHandlerCallBackFunction(TempBuffer);

            break;
        }
        case SpotifyEventSendRequestForPrevious:
        {
            Spotify_SendRequestForPrevious(EventData_t->token);
            if (xQueueReceive((*EventData_t->HttpsBufQueue), TempBuffer, portMAX_DELAY) == pdTRUE)
            {
                ESP_LOGI(TAG, "Data received in Event handler by queue ");
            }
            EventData_t->EventHandlerCallBackFunction(TempBuffer);
            break;
        }
        case SpotifyEventSendRequestForPlay:
        {
            Spotify_SendRequestForPlay(EventData_t->token);
            if (xQueueReceive((*EventData_t->HttpsBufQueue), TempBuffer, portMAX_DELAY) == pdTRUE)
            {
                ESP_LOGI(TAG, "Data received in Event handler by queue ");
            }
            EventData_t->EventHandlerCallBackFunction(TempBuffer);
            break;
        }
        case SpotifyEventSendRequestForPause:
        {
            Spotify_SendRequestForPause(EventData_t->token);
            if (xQueueReceive((*EventData_t->HttpsBufQueue), TempBuffer, portMAX_DELAY) == pdTRUE)
            {
                ESP_LOGI(TAG, "Data received in Event handler by queue ");
            }
            EventData_t->EventHandlerCallBackFunction(TempBuffer);
            break;
        }
        case SpotifyEventGetUserStatus:
        {
            Spotify_GetUserStatus(EventData_t->token);
            if (xQueueReceive((*EventData_t->HttpsBufQueue), TempBuffer, portMAX_DELAY) == pdTRUE)
            {
                ESP_LOGI(TAG, "Data received in Event handler by queue ");
            }
            EventData_t->EventHandlerCallBackFunction(TempBuffer);
            break;
        }
        case SpotifyEventGetUserTopItems:
        {
            Spotify_GetUserTopItems(EventData_t->token);
            if (xQueueReceive((*EventData_t->HttpsBufQueue), TempBuffer, portMAX_DELAY) == pdTRUE)
            {
                ESP_LOGI(TAG, "Data received in Event handler by queue ");
            }
            EventData_t->EventHandlerCallBackFunction(TempBuffer);
            break;
        }
        case SpotifyEventGetUserProfile:
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
                ESP_LOGI(TAG, "Data received in Event handler by queue ");
            }
            EventData_t->EventHandlerCallBackFunction(TempBuffer);
            break;
        }
        case SpotifyEventGetCurrentPlaying:
        {
            Spotify_GetCurrentPlaying(EventData_t->token);
            if (xQueueReceive((*EventData_t->HttpsBufQueue), TempBuffer, portMAX_DELAY) == pdTRUE)
            {
                ESP_LOGI(TAG, "Data received in Event handler by queue ");
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
        .queue_size = EVENT_LOOP_QUEUE,
        .task_name = "Spotify_Event_Task", // task will be created
        .task_priority = uxTaskPriorityGet(NULL),
        .task_stack_size = SPOTIFY_EVENT_STACK_SIZE,
        .task_core_id = tskNO_AFFINITY};
    esp_event_loop_create(&Spotify_EventLoopArgs, &Spotify_EventLoopHandle);
    ESP_ERROR_CHECK(esp_event_handler_instance_register_with(Spotify_EventLoopHandle, BASE_SPOTIFY_EVENTS, ESP_EVENT_ANY_ID, Spotify_EventHandler, Spotify_EventLoopHandle, NULL));
     ESP_LOGI(TAG, "Register Event Handler for Spotify ");
}

/**
 * @brief This function register event handler for spotify and delete event loop
 */
void Spotify_UnRegisterEventHandler(void)
{
    esp_event_handler_unregister_with(Spotify_EventLoopHandle, BASE_SPOTIFY_EVENTS, ESP_EVENT_ANY_ID, &Spotify_EventHandler);
    esp_event_loop_delete(Spotify_EventLoopHandle);
}