#include "SpotifyEventHandler.h"
static const char *TAG = "Spotify_EventHandler";

esp_event_loop_handle_t Spotify_EventLoopHandle;
const esp_event_base_t BASE_SPOTIFY_EVENTS = "BASE_SPOTIFY_EVENTS";
ESP_EVENT_DECLARE_BASE(BASE_SPOTIFY_EVENTS);

static void Spotify_EventHandler(void *Arg, esp_event_base_t EventBase,
                               int32_t EventId, void *EventData)
{
    ESP_LOGI(TAG,"we are in Spotify event handler");
    if (EventBase == BASE_SPOTIFY_EVENTS)
    {
        switch (EventId)
        {
        case SpotifyEventSendRequestForNext_:
        {
            Spotify_SendRequestForNext();
            // if (xQueueReceive(BufQueue1, EventData, portMAX_DELAY) == pdTRUE)
            // ESP_LOGI(TAG, "Spotify Event handler is working !");
            // {
            // ESP_LOGI(TAG, "Received TOKEN by Queue: %s\n", (char *)EventData);
            // }
            break;
        }
        case SpotifyEventSendRequestForPrevious_:
        {

            Spotify_SendRequestForPrevious();
            break;
        }
        case SpotifyEventSendRequestForPlay_:
        {
            Spotify_SendRequestForPlay();
            break;
        }
        case SpotifyEventSendRequestForPause_:
        {
            Spotify_SendRequestForPause();
            break;
        }
        case SpotifyEventGetUserStatus_:
        {
            Spotify_GetUserStatus();
            break;
        }
        case SpotifyEventGetUserTopItems_:
        {
            Spotify_GetUserTopItems();
            break;
        }
        case SpotifyEventGetUserProfile_:
        {
            // Spotify_GetUserProfile(UserInfo.UserID);
            break;
        }
        case SpotifyEventGetCurrentPlaying_:
        {
            Spotify_GetCurrentPlaying();
            break;
        }
        }
    }
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