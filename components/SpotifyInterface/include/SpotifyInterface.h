#ifdef __cplusplus
extern "C"
{
#endif
#ifndef HTTPS_SPOTIFY_H
#define HTTPS_SPOTIFY_H

#include "SpotifyMakeRequest.h"
#include "SpotifyEventHandler.h"
#include "SpotifyMakeRequest.h"
#include "JsonExtraction.h"
#include "SpotifyTypedef.h"
#include "SpotifyHttpLocalServer.h"

#define NO_COMMAND 0
#define PLAY_PAUSE 1
#define PLAY_NEXT 2
#define PLAY_PREV 3
#define STOP 4
#define PLAY 5
#define PAUSE 6
#define GET_NOW_PLAYING 7
#define GET_USER_INFO 8
#define GET_SONG_IMAGE_URL 9
#define GET_ARTIST_IMAGE_URL 10

    typedef enum
    {
        NoCommand = 0,
        PlayPause = 1,
        PlayNext = 2,
        PlayPrev = 3,
        Stop = 4,
        Play = 5,
        Pause = 6,
        GetNowPlaying = 7,
        GetUserInfo = 8,
        GetSongImageUrl = 9,
        GetArtisImageUrl = 10,
    } Command_t;

    typedef struct
    {
        QueueHandle_t *HttpsBufQueue;
        SemaphoreHandle_t *HttpsResponseReadySemaphore;
        SemaphoreHandle_t *IsSpotifyAuthorizedSemaphore;
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
    bool Spotify_SendCommand(int command);
#endif

#ifdef __cplusplus
}
#endif
