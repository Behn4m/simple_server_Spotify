#include "SpotifyInterface.h"
#include <stdio.h>
#include <stdlib.h>
#include "HttpsRequests.h"
#include "cJSON.h"
#include"JsonExtraction.h"

static const char *TAG = "JsonExTraction";

/**
 * @brief Extracts JSON content from an HTTP response string.
 * This function separates the JSON content from an HTTP response header and extracts the JSON string.
 * @param[in] HttpResponse The input string containing an HTTP response with JSON content.
 * @param[out] Json The output buffer to store the extracted JSON content.
 * @return Returns true if the JSON content is successfully extracted, otherwise false.
 */
bool ExtractJsonFromHttpResponse(char *HttpResponse, size_t SizeRes) 
{
 uint8_t FlgFindToken = 0;
    uint32_t SizeOfJson = 0;
    char json[MEDIUM_BUF] = {0};
    for (uint16_t i = 0; i < SizeRes; i++)
    {
        if (HttpResponse[i] == '{')
        {
            if (HttpResponse[i + 1] == '"' && HttpResponse[i + 2] == 'a' && HttpResponse[i + 3] == 'c' && HttpResponse[i + 4] == 'c' && HttpResponse[i + 5] == 'e' && HttpResponse[i + 6] == 's')
            {
                FlgFindToken = 1;
                SizeOfJson = i;
            }
        }
        if (HttpResponse[i] == '}')
        {
            for (uint16_t j = SizeOfJson; j <= i; j++)
            {
                json[j - SizeOfJson] = HttpResponse[j];
            }
            memset(HttpResponse, 0x000, SizeRes);
            for (uint16_t j = 0; j < sizeof(json); j++)
            {
                HttpResponse[j] = json[j];
            }
        }
    }
    return FlgFindToken;
}

/**
 * @brief This function extracts specific parameters from a JSON string and assigns them to corresponding fields in a TokenParam structure.
 * @param[in] Json The input JSON string.
 * @param[in] JsonBufSize The size of the JSON string.
 * @return false if fail, true if finish successful.
 */
bool ExtractionJsonParamForFindAccessToken(char *Json, size_t JsonBufSize,
                                           char *AccessToken,
                                           char *TokenType,
                                           char *RefreshToken,
                                           char *GrantedScope,
                                           int ExpiresInMS) 
    {
    
    cJSON *J_Token = cJSON_Parse(Json);
    if (J_Token == NULL) 
    {
        ESP_LOGE(TAG,"%s",Json);
        ESP_LOGE(TAG, "Failed to parse JSON\n");
        return false;
    }

    cJSON *accessTokenObj = cJSON_GetObjectItem(J_Token, "access_token");
    cJSON *tokenTypeObj = cJSON_GetObjectItem(J_Token, "token_type");
    cJSON *expiresInObj = cJSON_GetObjectItem(J_Token, "expires_in");
    cJSON *refreshTokenObj = cJSON_GetObjectItem(J_Token, "refresh_token");
    cJSON *scopeObj = cJSON_GetObjectItem(J_Token, "scope");

    if (accessTokenObj != NULL && accessTokenObj->type == cJSON_String) 
    {
        if (AccessToken != NULL && ACCESS_TOKEN_STR_SIZE > 0) 
        {
            strncpy(AccessToken, accessTokenObj->valuestring, ACCESS_TOKEN_STR_SIZE - 1);
            AccessToken[ACCESS_TOKEN_STR_SIZE - 1] = '\0';
        }
    }
    if (tokenTypeObj != NULL && tokenTypeObj->type == cJSON_String) 
    {
        if (TokenType != NULL && TOKEN_TYPE_STR_SIZE > 0) 
        {
            strncpy(TokenType, tokenTypeObj->valuestring, TOKEN_TYPE_STR_SIZE - 1);
            TokenType[TOKEN_TYPE_STR_SIZE - 1] = '\0';
        }
    }
    if (expiresInObj != NULL && expiresInObj->type == cJSON_Number) 
    {
        ExpiresInMS = expiresInObj->valueint;
    }
    if (refreshTokenObj != NULL && refreshTokenObj->type == cJSON_String) 
    {
        if (RefreshToken != NULL && REFRESH_TOKEN_STP_SIZE > 0) 
        {
            strncpy(RefreshToken, refreshTokenObj->valuestring, REFRESH_TOKEN_STP_SIZE - 1);
            RefreshToken[REFRESH_TOKEN_STP_SIZE - 1] = '\0';
        }
    }
    if (scopeObj != NULL && scopeObj->type == cJSON_String) 
    {
        if (GrantedScope != NULL && GRANTED_SCOP_STR_SIZE > 0) 
        {
            strncpy(GrantedScope, scopeObj->valuestring, GRANTED_SCOP_STR_SIZE - 1);
            GrantedScope[GRANTED_SCOP_STR_SIZE - 1] = '\0';
        }
    }

    ESP_LOGI(TAG, "Access Token: %s", AccessToken);
    ESP_LOGI(TAG, "Token Type: %s", TokenType);
    ESP_LOGI(TAG, "Expires In: %d seconds\n", ExpiresInMS);
    ESP_LOGI(TAG, "Refresh Token: %s", RefreshToken);
    ESP_LOGI(TAG, "Scope: %s", GrantedScope);

    cJSON_Delete(J_Token);
    return true;
}

/**
 * @brief This function extracts specific parameters from a JSON string and assigns them to corresponding fields in a UserInfo structure.
 * @param[in] JsonUSerInfo The input JSON string containing user information.
 * @return Returns 0 if the JSON is parsed successfully, or 1 otherwise.
 */
int ExtractionJsonParamForFindUserInfo(char *JsonUSerInfo, char *DisplayName, char *ProfileURL, char *UserID, char *Image1, char *Image2, int *Follower, char *Country, char *Product)
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
        strncpy(DisplayName, displayNameItem->valuestring, DISPLAY_NAME_STR_SIZE - 1);
        DisplayName[DISPLAY_NAME_STR_SIZE - 1] = '\0';
    }
    cJSON *spotifyProfileURLItem = cJSON_GetObjectItemCaseSensitive(J_UsserInfo, "ProfileURL");
    if (cJSON_IsString(spotifyProfileURLItem) && (spotifyProfileURLItem->valuestring != NULL))
    {
        strncpy(ProfileURL, spotifyProfileURLItem->valuestring, PROFILE_STR_SIZE - 1);
        ProfileURL[PROFILE_STR_SIZE - 1] = '\0';
    }
    cJSON *UserInfoIDItem = cJSON_GetObjectItemCaseSensitive(J_UsserInfo, "UserID");
    if (cJSON_IsString(UserInfoIDItem) && (UserInfoIDItem->valuestring != NULL))
    {
        strncpy(UserID, UserInfoIDItem->valuestring, USER_ID_SIZE - 1);
        UserID[USER_ID_SIZE - 1] = '\0';
    }
    cJSON *image1Item = cJSON_GetObjectItemCaseSensitive(J_UsserInfo, "Image1");
    if (cJSON_IsString(image1Item) && (image1Item->valuestring != NULL))
    {
        strncpy(Image1, image1Item->valuestring, IMAGE1_STR_SIZE - 1);
        Image1[IMAGE1_STR_SIZE - 1] = '\0';
    }
    cJSON *image2Item = cJSON_GetObjectItemCaseSensitive(J_UsserInfo, "Image2");
    if (cJSON_IsString(image2Item) && (image2Item->valuestring != NULL))
    {
        strncpy(Image2, image2Item->valuestring, IMAGE2_STR_SIZE - 1);
        Image2[IMAGE2_STR_SIZE - 1] = '\0';
    }
    cJSON *followerItem = cJSON_GetObjectItemCaseSensitive(J_UsserInfo, "Follower");
    if (cJSON_IsNumber(followerItem))
    {
        //Follower = followerItem;
    }
    cJSON *countryItem = cJSON_GetObjectItemCaseSensitive(J_UsserInfo, "Country");
    if (cJSON_IsString(countryItem) && (countryItem->valuestring != NULL))
    {
        strncpy(Country, countryItem->valuestring, COUNTERY_STR_SIZE - 1);
        Country[COUNTERY_STR_SIZE - 1] = '\0';
    }
    cJSON *productItem = cJSON_GetObjectItemCaseSensitive(J_UsserInfo, "Product");
    if (cJSON_IsString(productItem) && (productItem->valuestring != NULL))
    {
        strncpy(Product, productItem->valuestring, PRODUCT_STR_SIZE - 1);
        Product[PRODUCT_STR_SIZE - 1] = '\0';
    }
    cJSON_Delete(J_UsserInfo);
    ESP_LOGI(TAG,"DisplayName: %s\n", DisplayName);
    ESP_LOGI(TAG,"ProfileURL: %s\n", ProfileURL);
    ESP_LOGI(TAG,"UserID: %s\n", UserID);
    ESP_LOGI(TAG,"Image1: %s\n", Image1);
    ESP_LOGI(TAG,"Image2: %s\n", Image2);
    ESP_LOGI(TAG,"Follower: %d\n", *Follower);
    ESP_LOGI(TAG,"Country: %s\n", Country);
    ESP_LOGI(TAG,"Product: %s\n", Product);
    return 0;
}
