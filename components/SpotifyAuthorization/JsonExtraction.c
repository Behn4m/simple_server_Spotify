#include "SpotifyAuthorization.h"
#include <stdio.h>
#include <stdlib.h>
#include "HttpsRequests.h"
#include "cJSON.h"
#include"JsonExtraction.h"
extern  struct Token_t TokenParam;
extern  struct  UserInfo_t UserInfo;
static const char *TAG = "JsonExTraction";

/**
 * @brief This function extracts specific parameters from a JSON string and assigns them to corresponding fields in a TokenParam structure.
 * @param[in] Json The input JSON string.
 * @param[in] SizeJson The size of the JSON string.
 * @return This function does not return a value.
 */
void ExtractionJsonParamForFindAccessToken(char *Json, size_t SizeJson, char *access_token, char *token_type, char *refresh_token, char *granted_scope, int *expires_in_ms)
{
    cJSON *J_Token = cJSON_Parse(Json);
    if (J_Token == NULL)
    {
        ESP_LOGE(TAG,"Failed to parse JSON\n");
    }
    // Extract values from the cJSON object
    cJSON *accessTokenObj = cJSON_GetObjectItem(J_Token, "access_token");
    cJSON *tokenTypeObj = cJSON_GetObjectItem(J_Token, "token_type");
    cJSON *expiresInObj = cJSON_GetObjectItem(J_Token, "expires_in_ms");
    cJSON *refreshTokenObj = cJSON_GetObjectItem(J_Token, "refresh_token");
    cJSON *scopeObj = cJSON_GetObjectItem(J_Token, "scope");
    if (accessTokenObj != NULL && accessTokenObj->type == cJSON_String)
    {
        strncpy(access_token, accessTokenObj->valuestring, sizeof(access_token) - 1);
        access_token[sizeof(access_token) - 1] = '\0';
    }
    if (tokenTypeObj != NULL && tokenTypeObj->type == cJSON_String)
    {
        strncpy(token_type, tokenTypeObj->valuestring, sizeof(token_type) - 1);
        token_type[sizeof(token_type) - 1] = '\0';
    }
    if (expiresInObj != NULL && expiresInObj->type == cJSON_Number)
    {
        expires_in_ms = expiresInObj->valueint;
    }
    if (refreshTokenObj != NULL && refreshTokenObj->type == cJSON_String)
    {
        strncpy(refresh_token, refreshTokenObj->valuestring, sizeof(refresh_token) - 1);
        refresh_token[sizeof(refresh_token) - 1] = '\0';
    }
    if (scopeObj != NULL && scopeObj->type == cJSON_String)
    {
        strncpy(granted_scope, scopeObj->valuestring, sizeof(granted_scope) - 1);
        granted_scope[sizeof(granted_scope) - 1] = '\0';
    }
    // Print the values from the TokenParam struct
    ESP_LOGI(TAG,"Access Token: %s\n", access_token);
    ESP_LOGI(TAG,"Token Type: %s\n", token_type);
    ESP_LOGI(TAG,"Expires In: %d seconds\n", *expires_in_ms);
    ESP_LOGI(TAG,"Refresh Token: %s\n", refresh_token);
    ESP_LOGI(TAG,"Scope: %s\n", granted_scope);
    cJSON_Delete(J_Token);
}

/**
 * @brief This function extracts specific parameters from a JSON string and assigns them to corresponding fields in a UserInfo structure.
 * @param[in] JsonUSerInfo The input JSON string containing user information.
 * @return Returns 0 if the JSON is parsed successfully, or 1 otherwise.
 */
int ExtractionJsonParamForFindUserInfo(char *JsonUSerInfo, char *DisplayName, char *SpotifyProfileURL, char *UserID, char *Image1, char *Image2, int *Follower, char *Country, char *Product)
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
        strncpy(DisplayName, displayNameItem->valuestring, sizeof(DisplayName) - 1);
        DisplayName[sizeof(DisplayName) - 1] = '\0';
    }
    cJSON *spotifyProfileURLItem = cJSON_GetObjectItemCaseSensitive(J_UsserInfo, "SpotifyProfileURL");
    if (cJSON_IsString(spotifyProfileURLItem) && (spotifyProfileURLItem->valuestring != NULL))
    {
        strncpy(SpotifyProfileURL, spotifyProfileURLItem->valuestring, sizeof(SpotifyProfileURL) - 1);
        SpotifyProfileURL[sizeof(SpotifyProfileURL) - 1] = '\0';
    }
    cJSON *UserInfoIDItem = cJSON_GetObjectItemCaseSensitive(J_UsserInfo, "UserID");
    if (cJSON_IsString(UserInfoIDItem) && (UserInfoIDItem->valuestring != NULL))
    {
        strncpy(UserID, UserInfoIDItem->valuestring, sizeof(UserID) - 1);
        UserID[sizeof(UserID) - 1] = '\0';
    }
    cJSON *image1Item = cJSON_GetObjectItemCaseSensitive(J_UsserInfo, "Image1");
    if (cJSON_IsString(image1Item) && (image1Item->valuestring != NULL))
    {
        strncpy(Image1, image1Item->valuestring, sizeof(Image1) - 1);
        Image1[sizeof(Image1) - 1] = '\0';
    }
    cJSON *image2Item = cJSON_GetObjectItemCaseSensitive(J_UsserInfo, "Image2");
    if (cJSON_IsString(image2Item) && (image2Item->valuestring != NULL))
    {
        strncpy(Image2, image2Item->valuestring, sizeof(Image2) - 1);
        Image2[sizeof(Image2) - 1] = '\0';
    }
    cJSON *followerItem = cJSON_GetObjectItemCaseSensitive(J_UsserInfo, "Follower");
    if (cJSON_IsNumber(followerItem))
    {
        Follower = followerItem;
    }
    cJSON *countryItem = cJSON_GetObjectItemCaseSensitive(J_UsserInfo, "Country");
    if (cJSON_IsString(countryItem) && (countryItem->valuestring != NULL))
    {
        strncpy(Country, countryItem->valuestring, sizeof(Country) - 1);
        Country[sizeof(Country) - 1] = '\0';
    }
    cJSON *productItem = cJSON_GetObjectItemCaseSensitive(J_UsserInfo, "Product");
    if (cJSON_IsString(productItem) && (productItem->valuestring != NULL))
    {
        strncpy(Product, productItem->valuestring, sizeof(Product) - 1);
        Product[sizeof(Product) - 1] = '\0';
    }
    cJSON_Delete(J_UsserInfo);
    ESP_LOGI(TAG,"DisplayName: %s\n", DisplayName);
    ESP_LOGI(TAG,"SpotifyProfileURL: %s\n", SpotifyProfileURL);
    ESP_LOGI(TAG,"UserID: %s\n", UserID);
    ESP_LOGI(TAG,"Image1: %s\n", Image1);
    ESP_LOGI(TAG,"Image2: %s\n", Image2);
    ESP_LOGI(TAG,"Follower: %d\n", *Follower);
    ESP_LOGI(TAG,"Country: %s\n", Country);
    ESP_LOGI(TAG,"Product: %s\n", Product);
    return 0;
}
