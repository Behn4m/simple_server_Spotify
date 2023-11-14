#include "SpotifyAuthorization.h"
#include <stdio.h>
#include <stdlib.h>
#include "HttpsRequests.h"
#include "cJSON.h"
#include"JsonExtraction.h"
extern  struct Token_ TokenParam;
extern  struct  UserInfo_ UserInfo;
static const char *TAG = "JsonExTraction";

/**
 * @brief This function extracts specific parameters from a JSON string and assigns them to corresponding fields in a TokenParam structure.
 * @param[in] Json The input JSON string.
 * @param[in] SizeJson The size of the JSON string.
 * @return This function does not return a value.
 */
void ExtractionJsonParamForFindAccessToken(char *Json, size_t SizeJson)
{
    cJSON *J_Token = cJSON_Parse(Json);
    if (J_Token == NULL)
    {
        ESP_LOGI(TAG,"Failed to parse JSON\n");
    }
    // Extract values from the cJSON object
    cJSON *accessTokenObj = cJSON_GetObjectItem(J_Token, "access_token");
    cJSON *tokenTypeObj = cJSON_GetObjectItem(J_Token, "token_type");
    cJSON *expiresInObj = cJSON_GetObjectItem(J_Token, "expires_in");
    cJSON *refreshTokenObj = cJSON_GetObjectItem(J_Token, "refresh_token");
    cJSON *scopeObj = cJSON_GetObjectItem(J_Token, "scope");
    if (accessTokenObj != NULL && accessTokenObj->type == cJSON_String)
    {
        strncpy(TokenParam.access_token, accessTokenObj->valuestring, sizeof(TokenParam.access_token) - 1);
        TokenParam.access_token[sizeof(TokenParam.access_token) - 1] = '\0';
    }
    if (tokenTypeObj != NULL && tokenTypeObj->type == cJSON_String)
    {
        strncpy(TokenParam.token_type, tokenTypeObj->valuestring, sizeof(TokenParam.token_type) - 1);
        TokenParam.token_type[sizeof(TokenParam.token_type) - 1] = '\0';
    }
    if (expiresInObj != NULL && expiresInObj->type == cJSON_Number)
    {
        TokenParam.expires_in = expiresInObj->valueint;
    }
    if (refreshTokenObj != NULL && refreshTokenObj->type == cJSON_String)
    {
        strncpy(TokenParam.refresh_token, refreshTokenObj->valuestring, sizeof(TokenParam.refresh_token) - 1);
        TokenParam.refresh_token[sizeof(TokenParam.refresh_token) - 1] = '\0';
    }
    if (scopeObj != NULL && scopeObj->type == cJSON_String)
    {
        strncpy(TokenParam.scope, scopeObj->valuestring, sizeof(TokenParam.scope) - 1);
        TokenParam.scope[sizeof(TokenParam.scope) - 1] = '\0';
    }
    // Print the values from the TokenParam struct
    ESP_LOGI(TAG,"Access Token: %s\n", TokenParam.access_token);
    ESP_LOGI(TAG,"Token Type: %s\n", TokenParam.token_type);
    ESP_LOGI(TAG,"Expires In: %d seconds\n", TokenParam.expires_in);
    ESP_LOGI(TAG,"Refresh Token: %s\n", TokenParam.refresh_token);
    ESP_LOGI(TAG,"Scope: %s\n", TokenParam.scope);
    cJSON_Delete(J_Token);
}

/**
 * @brief This function extracts specific parameters from a JSON string and assigns them to corresponding fields in a UserInfo structure.
 * @param[in] JsonUSerInfo The input JSON string containing user information.
 * @return Returns 0 if the JSON is parsed successfully, or 1 otherwise.
 */
int ExtractionJsonParamForFindUserInfo(char *JsonUSerInfo)
{
    cJSON *J_UsserInfo = cJSON_Parse(JsonUSerInfo);
    if (J_UsserInfo == NULL)
    {
        ESP_LOGI(TAG,"Failed to parse JSON: %s\n", cJSON_GetErrorPtr());
        return 1;
    }
    cJSON *displayNameItem = cJSON_GetObjectItemCaseSensitive(J_UsserInfo, "DisplayName");
    if (cJSON_IsString(displayNameItem) && (displayNameItem->valuestring != NULL))
    {
        strncpy(UserInfo.DisplayName, displayNameItem->valuestring, sizeof(UserInfo.DisplayName) - 1);
        UserInfo.DisplayName[sizeof(UserInfo.DisplayName) - 1] = '\0';
    }
    cJSON *spotifyProfileURLItem = cJSON_GetObjectItemCaseSensitive(J_UsserInfo, "SpotifyProfileURL");
    if (cJSON_IsString(spotifyProfileURLItem) && (spotifyProfileURLItem->valuestring != NULL))
    {
        strncpy(UserInfo.SpotifyProfileURL, spotifyProfileURLItem->valuestring, sizeof(UserInfo.SpotifyProfileURL) - 1);
        UserInfo.SpotifyProfileURL[sizeof(UserInfo.SpotifyProfileURL) - 1] = '\0';
    }
    cJSON *UserInfoIDItem = cJSON_GetObjectItemCaseSensitive(J_UsserInfo, "UserID");
    if (cJSON_IsString(UserInfoIDItem) && (UserInfoIDItem->valuestring != NULL))
    {
        strncpy(UserInfo.UserID, UserInfoIDItem->valuestring, sizeof(UserInfo.UserID) - 1);
        UserInfo.UserID[sizeof(UserInfo.UserID) - 1] = '\0';
    }
    cJSON *image1Item = cJSON_GetObjectItemCaseSensitive(J_UsserInfo, "Image1");
    if (cJSON_IsString(image1Item) && (image1Item->valuestring != NULL))
    {
        strncpy(UserInfo.Image1, image1Item->valuestring, sizeof(UserInfo.Image1) - 1);
        UserInfo.Image1[sizeof(UserInfo.Image1) - 1] = '\0';
    }
    cJSON *image2Item = cJSON_GetObjectItemCaseSensitive(J_UsserInfo, "Image2");
    if (cJSON_IsString(image2Item) && (image2Item->valuestring != NULL))
    {
        strncpy(UserInfo.Image2, image2Item->valuestring, sizeof(UserInfo.Image2) - 1);
        UserInfo.Image2[sizeof(UserInfo.Image2) - 1] = '\0';
    }
    cJSON *followerItem = cJSON_GetObjectItemCaseSensitive(J_UsserInfo, "Follower");
    if (cJSON_IsNumber(followerItem))
    {
        UserInfo.Follower = followerItem->valueint;
    }
    cJSON *countryItem = cJSON_GetObjectItemCaseSensitive(J_UsserInfo, "Country");
    if (cJSON_IsString(countryItem) && (countryItem->valuestring != NULL))
    {
        strncpy(UserInfo.Country, countryItem->valuestring, sizeof(UserInfo.Country) - 1);
        UserInfo.Country[sizeof(UserInfo.Country) - 1] = '\0';
    }
    cJSON *productItem = cJSON_GetObjectItemCaseSensitive(J_UsserInfo, "Product");
    if (cJSON_IsString(productItem) && (productItem->valuestring != NULL))
    {
        strncpy(UserInfo.Product, productItem->valuestring, sizeof(UserInfo.Product) - 1);
        UserInfo.Product[sizeof(UserInfo.Product) - 1] = '\0';
    }
    cJSON_Delete(J_UsserInfo);
    ESP_LOGI(TAG,"DisplayName: %s\n", UserInfo.DisplayName);
    ESP_LOGI(TAG,"SpotifyProfileURL: %s\n", UserInfo.SpotifyProfileURL);
    ESP_LOGI(TAG,"UserID: %s\n", UserInfo.UserID);
    ESP_LOGI(TAG,"Image1: %s\n", UserInfo.Image1);
    ESP_LOGI(TAG,"Image2: %s\n", UserInfo.Image2);
    ESP_LOGI(TAG,"Follower: %d\n", UserInfo.Follower);
    ESP_LOGI(TAG,"Country: %s\n", UserInfo.Country);
    ESP_LOGI(TAG,"Product: %s\n", UserInfo.Product);
    return 0;
}
