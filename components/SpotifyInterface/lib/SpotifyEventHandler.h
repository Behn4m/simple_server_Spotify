#ifdef __cplusplus
extern "C" {
#endif
#ifndef  EVENT_HANDLER_H
#define  EVENT_HANDLER_H
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_event_base.h"
#include "MakeSpotifyRequest.h"
#include "SpotifyInterface.h"
 extern esp_event_loop_handle_t Spotify_EventLoopHandle;
#define SpotifyEventStack (uint32_t)20*1000


enum
{
    SpotifyEventSendRequestForNext_,
    SpotifyEventSendRequestForPrevious_,
    SpotifyEventSendRequestForPlay_,
    SpotifyEventSendRequestForPause_,
    SpotifyEventGetUserStatus_,
    SpotifyEventGetUserTopItems_,
    SpotifyEventGetUserProfile_,
    SpotifyEventGetCurrentPlaying_,
};
/**
 * @brief This function register event handler for spotify , this event is task and need
 *        long stack ,because spotify functions need long stack 
 */
void Spotify_RegisterEventHandler(void);
/**
 * @brief This function register event handler for spotify and delete event loop
 */
void Spotify_UnRegisterEventHandler(void);

#endif
#ifdef __cplusplus
}
#endif
