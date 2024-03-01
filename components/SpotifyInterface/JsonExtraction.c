#include "SpotifyInterface.h"
#include "SpotifyTypedef.h"
#include <stdio.h>
#include <stdlib.h>
#include "cJSON.h"
#include"JsonExtraction.h"

static const char *TAG = "JsonExTraction";


/**
 * @brief This function extracts specific parameters from a JSON string and assigns them to corresponding fields in a TokenParam structure.
 * @param[in] Json The input JSON string.
 * @param[in] token address to save tokin extracted information.
 * @return false if fail, true if finish successful.
 */
bool ExtractAccessTokenParamsTokenFromJson(char *Json, Token_t *token) 
    {
    
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
    cJSON_Delete(accessTokenObj);

    cJSON *tokenTypeObj = cJSON_GetObjectItem(J_Token, "token_type");
    if (tokenTypeObj != NULL && tokenTypeObj->type == cJSON_String) 
    {
        strncpy(token->TokenType, tokenTypeObj->valuestring, TOKEN_TYPE_STR_SIZE - 1);
        token->TokenType[TOKEN_TYPE_STR_SIZE - 1] = '\0';
    }
    cJSON_Delete(tokenTypeObj);

    cJSON *expiresInObj = cJSON_GetObjectItem(J_Token, "expires_in");
    if (expiresInObj->type == cJSON_Number) 
    {
        token->ExpiresInMS = expiresInObj->valueint;
    }
    cJSON_Delete(expiresInObj);

    cJSON *refreshTokenObj = cJSON_GetObjectItem(J_Token, "refresh_token");
    if (refreshTokenObj != NULL && refreshTokenObj->type == cJSON_String) 
    {
        strncpy(token->RefreshToken, refreshTokenObj->valuestring, REFRESH_TOKEN_STP_SIZE - 1);
        token->RefreshToken[REFRESH_TOKEN_STP_SIZE - 1] = '\0';
    }
    cJSON_Delete(refreshTokenObj);

    cJSON *scopeObj = cJSON_GetObjectItem(J_Token, "scope");
    if (scopeObj != NULL && scopeObj->type == cJSON_String) 
    {
        strncpy(token->GrantedScope, scopeObj->valuestring, GRANTED_SCOP_STR_SIZE - 1);
        token->GrantedScope[GRANTED_SCOP_STR_SIZE - 1] = '\0';
    }
    cJSON_Delete(scopeObj);

    // ESP_LOGI(TAG, "Access Token: %s", AccessToken);
    // ESP_LOGI(TAG, "Token Type: %s", TokenType);
    // ESP_LOGI(TAG, "Expires In: %d seconds\n", ExpiresInMS);
    // ESP_LOGI(TAG, "Refresh Token: %s", RefreshToken);
    // ESP_LOGI(TAG, "Scope: %s", GrantedScope);

    cJSON_Delete(J_Token);
    return true;
}

/**
 * @brief This function extracts specific parameters from a JSON string and assigns them to corresponding fields in a UserInfo structure.
 * @param[in] JsonUserInfo The input JSON string containing user information.
 * @param[in] userInfo The object needed to be filled with extarcted data
 * @return Returns 0 if the JSON is parsed successfully, or 1 otherwise.
 */
int ExtractUserInfoParamsfromJson(char *JsonUserInfo, UserInfo_t *userInfo)
{
    cJSON *J_UsserInfo = cJSON_Parse(JsonUserInfo);
    if (J_UsserInfo == NULL)
    {
        ESP_LOGI(TAG,"Failed to parse JSON: %s\n", cJSON_GetErrorPtr());
        return 1;
    }
    cJSON_Delete(J_UsserInfo);

    cJSON *displayNameItem = cJSON_GetObjectItemCaseSensitive(J_UsserInfo, "DisplayName");
    if (cJSON_IsString(displayNameItem) && (displayNameItem->valuestring != NULL))
    {
        strncpy(userInfo->DisplayName, displayNameItem->valuestring, DISPLAY_NAME_STR_SIZE - 1);
        userInfo->DisplayName[DISPLAY_NAME_STR_SIZE - 1] = '\0';
    }
    cJSON_Delete(displayNameItem);

    cJSON *spotifyProfileURLItem = cJSON_GetObjectItemCaseSensitive(J_UsserInfo, "ProfileURL");
    if (cJSON_IsString(spotifyProfileURLItem) && (spotifyProfileURLItem->valuestring != NULL))
    {
        strncpy(userInfo->ProfileURL, spotifyProfileURLItem->valuestring, PROFILE_STR_SIZE - 1);
        userInfo->ProfileURL[PROFILE_STR_SIZE - 1] = '\0';
    }
    cJSON_Delete(spotifyProfileURLItem);

    cJSON *UserInfoIDItem = cJSON_GetObjectItemCaseSensitive(J_UsserInfo, "UserID");
    if (cJSON_IsString(UserInfoIDItem) && (UserInfoIDItem->valuestring != NULL))
    {
        strncpy(userInfo->UserID, UserInfoIDItem->valuestring, USER_ID_SIZE - 1);
        userInfo->UserID[USER_ID_SIZE - 1] = '\0';
    }
    cJSON_Delete(UserInfoIDItem);

    cJSON *image1Item = cJSON_GetObjectItemCaseSensitive(J_UsserInfo, "Image1");
    if (cJSON_IsString(image1Item) && (image1Item->valuestring != NULL))
    {
        strncpy(userInfo->Image1, image1Item->valuestring, IMAGE1_STR_SIZE - 1);
        userInfo->Image1[IMAGE1_STR_SIZE - 1] = '\0';
    }
    cJSON_Delete(image1Item);

    cJSON *image2Item = cJSON_GetObjectItemCaseSensitive(J_UsserInfo, "Image2");
    if (cJSON_IsString(image2Item) && (image2Item->valuestring != NULL))
    {
        strncpy(userInfo->Image2, image2Item->valuestring, IMAGE2_STR_SIZE - 1);
        userInfo->Image2[IMAGE2_STR_SIZE - 1] = '\0';
    }
    cJSON_Delete(image2Item);

    cJSON *countryItem = cJSON_GetObjectItemCaseSensitive(J_UsserInfo, "Country");
    if (cJSON_IsString(countryItem) && (countryItem->valuestring != NULL))
    {
        strncpy(userInfo->Country, countryItem->valuestring, COUNTERY_STR_SIZE - 1);
        userInfo->Country[COUNTERY_STR_SIZE - 1] = '\0';
    }
    cJSON_Delete(countryItem);

    cJSON *productItem = cJSON_GetObjectItemCaseSensitive(J_UsserInfo, "Product");
    if (cJSON_IsString(productItem) && (productItem->valuestring != NULL))
    {
        strncpy(userInfo->Product, productItem->valuestring, PRODUCT_STR_SIZE - 1);
        userInfo->Product[PRODUCT_STR_SIZE - 1] = '\0';
    }
    cJSON_Delete(productItem);

    cJSON_Delete(J_UsserInfo);
    // ESP_LOGI(TAG,"DisplayName: %s\n", userInfo->DisplayName);
    // ESP_LOGI(TAG,"ProfileURL: %s\n", userInfo->ProfileURL);
    // ESP_LOGI(TAG,"UserID: %s\n", userInfo->UserID);
    // ESP_LOGI(TAG,"Image1: %s\n", userInfo->Image1);
    // ESP_LOGI(TAG,"Image2: %s\n", userInfo->Image2);
    // ESP_LOGI(TAG,"Country: %s\n", userInfo->Country);
    // ESP_LOGI(TAG,"Product: %s\n", userInfo->Product);
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
    if (J_PlaybackInfo == NULL)
    {
        ESP_LOGI(TAG,"Failed to parse JSON: %s\n", cJSON_GetErrorPtr());
        return 1;
    }

    cJSON *displayNameItem = cJSON_GetObjectItemCaseSensitive(J_PlaybackInfo, "DisplayName");
    if (cJSON_IsString(displayNameItem) && (displayNameItem->valuestring != NULL))
    {
        strncpy(playbackInfo->AlbumName, displayNameItem->valuestring, DISPLAY_NAME_STR_SIZE - 1);
        playbackInfo->AlbumName[DISPLAY_NAME_STR_SIZE - 1] = '\0';
    }
    cJSON_Delete(displayNameItem);
 
    cJSON_Delete(J_PlaybackInfo);
    ESP_LOGI(TAG,"DisplayName: %s\n", playbackInfo->AlbumName);

    return 0;
}