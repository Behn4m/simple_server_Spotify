#ifdef __cplusplus
extern "C"
{
#endif
#ifndef HTTPS_SPOTIFY_H
#define HTTPS_SPOTIFY_H

#include <esp_event.h>
#include <esp_log.h>
#include <esp_system.h>
#include <sys/param.h>
#include "freertos/queue.h"
#include "esp_psram.h"

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

#define DISPLAY_NAME_STR_SIZE 100
#define PROFILE_STR_SIZE 100
#define USER_ID_SIZE 100
#define IMAGE_STR_SIZE 100
#define COUNTRY_STR_SIZE 100
#define PRODUCT_STR_SIZE 100
#define DISPLAY_NAME_STR_SIZE 100


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
    GetUserTopItems = 9
} Command_t;

typedef struct UserInfo_t
{
    char DisplayName[DISPLAY_NAME_STR_SIZE];
    char ProfileURL[PROFILE_STR_SIZE];
    char UserID[USER_ID_SIZE];
    char Country[COUNTRY_STR_SIZE];
    char Product[PRODUCT_STR_SIZE];
} UserInfo_t;

typedef struct PlaybackInfo_t
{
    char SongName[DISPLAY_NAME_STR_SIZE];
    char ArtistName[DISPLAY_NAME_STR_SIZE];
    char AlbumName[DISPLAY_NAME_STR_SIZE];
    char SongImageURL[IMAGE_STR_SIZE];
    char ArtistImageURL[IMAGE_STR_SIZE];
    int Duration;
    int Progress;
    int IsPlaying;
} PlaybackInfo_t;
typedef struct SpotifyInterfaceHandler_t
{
    UserInfo_t *UserInfo;                   // Nested struct for user information
    PlaybackInfo_t *PlaybackInfo;           // Nested struct for now playing song information
    char *ConfigAddressInSpiffs;
    SemaphoreHandle_t *IsSpotifyAuthorizedSemaphore;
} SpotifyInterfaceHandler_t;

/**
 * @brief This function initiates the Spotify authorization process.
 * @param SpotifyInterfaceHandler as the handler
 * @return true if task run to the end
 */
bool Spotify_TaskInit(SpotifyInterfaceHandler_t *SpotifyInterfaceHandler);

/**
 * @brief This function deinitiates the Spotify authorization process.
 * @param SpotifyInterfaceHandler as the handler
 * @return true if task run to the end
 */
void Spotify_TaskDeinit(SpotifyInterfaceHandler_t *SpotifyInterfaceHandler);

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
bool Spotify_SendCommand(SpotifyInterfaceHandler_t SpotifyInterfaceHandler, int Command);
#endif

#ifdef __cplusplus
}
#endif
