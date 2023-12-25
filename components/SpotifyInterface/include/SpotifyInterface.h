#ifdef __cplusplus
extern "C"
{
#endif
#ifndef HTTPS_SPOTIFY_H
#define HTTPS_SPOTIFY_H

#include "MakeSpotifyRequest.h"
#include "SpotifyEventHandler.h"
#include "MakeSpotifyRequest.h"
#include "JsonExtraction.h"
#include "SpotifyTypedef.h"

    typedef struct
    {
        QueueHandle_t *HttpsBufQueue;
        SemaphoreHandle_t *HttpsResponseReadySemaphore;
        char *ConfigAddressInSpiffs;
        ReadTxtFileFromSpiffsPtr ReadTxtFileFromSpiffs;
        WriteTxtFileToSpiffsPtr WriteTxtFileToSpiffs;
        CheckAddressInSpiffsPtr CheckAddressInSpiffs;
        EventHandlerCallBackPtr EventHandlerCallBackFunction;
    } SpotifyInterfaceHandler_t;

    /**
     * @brief This function initiates the Spotify authorization process.
     * @param SpotifyInterfaceHandler as the handler
     * @return true if task run to the end
     */
    bool Spotify_TaskInit(SpotifyInterfaceHandler_t *SpotifyInterfaceHandler, uint16_t SpotifyTaskStackSize);

    /**
     * @brief This checks if the applciaiton is initiated and connected to Spotify web service
     * @param SpotifyInterfaceHandler as the handler
     * @return true if everything is OK, flase for ERROR
     */
    bool Spotify_IsConnected(SpotifyInterfaceHandler_t *SpotifyInterfaceHandler);

    /**
     * @brief This function get and apply the command to Spotify service.
     * @param command could be play, pause, stop, next, previous, user_info, song_img, artist_img, etc.
     * @return true if function successfully sent the command to Spotify
     */
    bool Spotify_PlaybackCommand(char *command);
#endif

#ifdef __cplusplus
}
#endif
