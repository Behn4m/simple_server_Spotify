#include "SpotifyInterface.h"
#include "SpotifyTypedef.h"
#include <stdio.h>
#include <stdlib.h>
#include "cJSON.h"
#include"JsonExtraction.h"

static const char *TAG = "JsonExTraction";

/**
 * @brief This function extracts specific parameters from a JSON string and assigns them to corresponding fields in a UserInfo structure.
 * @param[in] JsonUserInfo The input JSON string containing user information.
 * @param[in] userInfo The object needed to be filled with extarcted data
 * @return Returns 0 if the JSON is parsed successfully, or 1 otherwise.
 */
int ExtractUserInfoParamsfromJson(char *JsonUserInfo, UserInfo_t *userInfo)
{
    cJSON *J_UserInfo = cJSON_Parse(JsonUserInfo);

    if (J_UserInfo == NULL)
    {
        ESP_LOGE(TAG, "Failed to parse JSON: %s\n", cJSON_GetErrorPtr());
        return 1;
    }

    cJSON *displayNameItem = cJSON_GetObjectItemCaseSensitive(J_UserInfo, "display_name");
    if (cJSON_IsString(displayNameItem) && (displayNameItem->valuestring != NULL))
    {
        strncpy(userInfo->DisplayName, displayNameItem->valuestring, DISPLAY_NAME_STR_SIZE - 1);
        userInfo->DisplayName[DISPLAY_NAME_STR_SIZE - 1] = '\0';
    }

    cJSON *spotifyProfileURLItem = cJSON_GetObjectItemCaseSensitive(J_UserInfo, "external_urls");
    if (cJSON_IsObject(spotifyProfileURLItem))
    {
        cJSON *spotifyItem = cJSON_GetObjectItemCaseSensitive(spotifyProfileURLItem, "spotify");
        if (cJSON_IsString(spotifyItem) && (spotifyItem->valuestring != NULL))
        {
            strncpy(userInfo->ProfileURL, spotifyItem->valuestring, PROFILE_STR_SIZE - 1);
            userInfo->ProfileURL[PROFILE_STR_SIZE - 1] = '\0';
        }
    }

    cJSON *userInfoIDItem = cJSON_GetObjectItemCaseSensitive(J_UserInfo, "id");
    if (cJSON_IsString(userInfoIDItem) && (userInfoIDItem->valuestring != NULL))
    {
        strncpy(userInfo->UserID, userInfoIDItem->valuestring, USER_ID_SIZE - 1);
        userInfo->UserID[USER_ID_SIZE - 1] = '\0';
    }

    cJSON *countryItem = cJSON_GetObjectItemCaseSensitive(J_UserInfo, "country");
    if (cJSON_IsString(countryItem) && (countryItem->valuestring != NULL))
    {
        strncpy(userInfo->Country, countryItem->valuestring, COUNTRY_STR_SIZE - 1);
        userInfo->Country[COUNTRY_STR_SIZE - 1] = '\0';
    }

    cJSON *productItem = cJSON_GetObjectItemCaseSensitive(J_UserInfo, "product");
    if (cJSON_IsString(productItem) && (productItem->valuestring != NULL))
    {
        strncpy(userInfo->Product, productItem->valuestring, PRODUCT_STR_SIZE - 1);
        userInfo->Product[PRODUCT_STR_SIZE - 1] = '\0';
    }

    ESP_LOGI(TAG, "** USER INFO: ");
    ESP_LOGI(TAG,"DisplayName: %s", userInfo->DisplayName);
    ESP_LOGI(TAG,"ProfileURL: %s", userInfo->ProfileURL);
    ESP_LOGI(TAG,"UserID: %s", userInfo->UserID);
    ESP_LOGI(TAG,"Country: %s", userInfo->Country);
    ESP_LOGI(TAG,"Product: %s", userInfo->Product);
    ESP_LOGI(TAG, "************** ");

    cJSON_Delete(J_UserInfo);
    return 0;
}

/**
 * @brief This function extracts specific parameters from a JSON string and assigns them to corresponding fields in a PlaybackInfo structure.
 * @param[in] JsonPlaybackInfo The input JSON string containing playback information.
 * @param[in] playbackInfo The object needed to be filled with extarcted data
 * @return Returns 0 if the JSON is parsed successfully, or 1 otherwise.
 */
int ExtractPlaybackInfoParamsfromJson(char *JsonPlaybackInfo, PlaybackInfo_t *playbackInfo)
{

    cJSON *J_PlaybackInfo = cJSON_Parse(JsonPlaybackInfo);
    if (J_PlaybackInfo == NULL) {
        // Handle parsing failure
        ESP_LOGE(TAG, "Failed to parse JSON: %s\n", cJSON_GetErrorPtr());
        return 1; // Or any other appropriate error handling
    }

    cJSON *IsPlaying = cJSON_GetObjectItemCaseSensitive(J_PlaybackInfo, "is_playing");
    if (cJSON_IsBool(IsPlaying)) {
        playbackInfo->IsPlaying = cJSON_IsTrue(IsPlaying);
    }  

    cJSON *ProgressMS = cJSON_GetObjectItemCaseSensitive(J_PlaybackInfo, "progress_ms");
    if (cJSON_IsNumber(ProgressMS)) {
        playbackInfo->Progress = ProgressMS->valueint;
    }
    
    cJSON *Item = cJSON_GetObjectItemCaseSensitive(J_PlaybackInfo, "item");
    if (cJSON_IsObject(Item)) {
        cJSON *Album = cJSON_GetObjectItemCaseSensitive(Item, "album");
        cJSON *Artists = cJSON_GetObjectItemCaseSensitive(Item, "artists");
        cJSON *Name = cJSON_GetObjectItemCaseSensitive(Item, "name");
        cJSON *DurationMS = cJSON_GetObjectItemCaseSensitive(Item, "duration_ms");

        if (cJSON_IsObject(Album) && cJSON_IsArray(Artists) && cJSON_IsString(Name)) {
            // Extract album information
            cJSON *AlbumName = cJSON_GetObjectItemCaseSensitive(Album, "name");
            if (cJSON_IsString(AlbumName) && (AlbumName->valuestring != NULL)) {
                strncpy(playbackInfo->AlbumName, AlbumName->valuestring, DISPLAY_NAME_STR_SIZE - 1);
                playbackInfo->AlbumName[DISPLAY_NAME_STR_SIZE - 1] = '\0';
            }

            // Extract artist information
            cJSON *FirstArtist = cJSON_GetArrayItem(Artists, 0); // Assuming there's at least one artist
            cJSON *ArtistName = cJSON_GetObjectItemCaseSensitive(FirstArtist, "name");
            if (cJSON_IsString(ArtistName) && (ArtistName->valuestring != NULL)) {
                strncpy(playbackInfo->ArtistName, ArtistName->valuestring, DISPLAY_NAME_STR_SIZE - 1);
                playbackInfo->ArtistName[DISPLAY_NAME_STR_SIZE - 1] = '\0';
            }

            // Extract track name
            strncpy(playbackInfo->SongName, Name->valuestring, DISPLAY_NAME_STR_SIZE - 1);
            playbackInfo->SongName[DISPLAY_NAME_STR_SIZE - 1] = '\0';

            // Extract song image URL
            cJSON *SongImageURLs = cJSON_GetObjectItemCaseSensitive(Album, "images");
            if (cJSON_IsArray(SongImageURLs) && cJSON_GetArraySize(SongImageURLs) > 0) {
                cJSON *ThirdImage = cJSON_GetArrayItem(SongImageURLs, 2);
                cJSON *SongImageURL = cJSON_GetObjectItemCaseSensitive(ThirdImage, "url");
                if (cJSON_IsString(SongImageURL) && SongImageURL->valuestring != NULL) {
                    strncpy(playbackInfo->SongImageURL, SongImageURL->valuestring, IMAGE_STR_SIZE - 1);
                    playbackInfo->SongImageURL[IMAGE_STR_SIZE - 1] = '\0';
                }
            }

            playbackInfo->Duration = DurationMS->valueint;

        }
    }
    ESP_LOGI(TAG, "** PLAYBACK INFO: ");
    ESP_LOGI(TAG, "IsPlaying: %d", playbackInfo->IsPlaying);
    ESP_LOGI(TAG, "ArtistName: %s", playbackInfo->ArtistName);
    ESP_LOGI(TAG, "AlbumName: %s", playbackInfo->AlbumName);
    ESP_LOGI(TAG, "SongName: %s", playbackInfo->SongName);
    ESP_LOGI(TAG, "Duration: %d", playbackInfo->Duration);
    ESP_LOGI(TAG, "ProgressMS: %d", playbackInfo->Progress);
    ESP_LOGI(TAG, "SongImageURL: %s", playbackInfo->SongImageURL);
    ESP_LOGI(TAG, "************** ");

 
    cJSON_Delete(J_PlaybackInfo);
    return 0;
}