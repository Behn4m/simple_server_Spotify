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
 * @brief Extracts JSON content from an HTTP response string.
 * This function separates the JSON content from an HTTP response header and extracts the JSON string.
 * @param[in] HttpResponse The input string containing an HTTP response with JSON content.
 * @param[out] Json The output buffer to store the extracted JSON content.
 * @return Returns true if the JSON content is successfully extracted, otherwise false.
 */
bool ExtractJsonFromHttpResponse(char *HttpResponse, char *Json) 
{
    const char *jsonPrefix = "\r\n\r\n";
    char *jsonStart = strstr(HttpResponse, jsonPrefix);

    if (jsonStart != NULL) 
    {
        // Move the pointer to the start of JSON content
        jsonStart += strlen(jsonPrefix);
        strcpy(Json, jsonStart);
        return true;
    }

    return false;
}

/**
 * @brief This function extracts specific parameters from a JSON string and assigns them to corresponding fields in a TokenParam structure.
 * @param[in] Json The input JSON string.
 * @param[in] SizeJson The size of the JSON string.
 * @return fasle if fail, true if finish successfull.
 */
bool ExtractionJsonParamForFindAccessToken(char *Json, size_t SizeJson,
                                           char *access_token,
                                           char *token_type,
                                           char *refresh_token,
                                           char *granted_scope,
                                           int *expires_in_ms) 
    {
    cJSON *J_Token = cJSON_Parse(Json);
    if (J_Token == NULL) 
    {
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
        if (access_token != NULL && access_token_str_size > 0) 
        {
            strncpy(access_token, accessTokenObj->valuestring, access_token_str_size - 1);
            access_token[access_token_str_size - 1] = '\0';
        }
    }
    if (tokenTypeObj != NULL && tokenTypeObj->type == cJSON_String) 
    {
        if (token_type != NULL && token_type_str_size > 0) 
        {
            strncpy(token_type, tokenTypeObj->valuestring, token_type_str_size - 1);
            token_type[token_type_str_size - 1] = '\0';
        }
    }
    if (expiresInObj != NULL && expiresInObj->type == cJSON_Number) 
    {
        // *expires_in_ms = expiresInObj->valueint;
    }
    if (refreshTokenObj != NULL && refreshTokenObj->type == cJSON_String) 
    {
        if (refresh_token != NULL && refresh_token_str_size > 0) 
        {
            strncpy(refresh_token, refreshTokenObj->valuestring, refresh_token_str_size - 1);
            refresh_token[refresh_token_str_size - 1] = '\0';
        }
    }
    if (scopeObj != NULL && scopeObj->type == cJSON_String) 
    {
        if (granted_scope != NULL && granted_scope_str_size > 0) 
        {
            strncpy(granted_scope, scopeObj->valuestring, granted_scope_str_size - 1);
            granted_scope[granted_scope_str_size - 1] = '\0';
        }
    }

    ESP_LOGI(TAG, "Access Token: %s", access_token);
    ESP_LOGI(TAG, "Token Type: %s", token_type);
    // ESP_LOGI(TAG, "Expires In: %d seconds\n", *expires_in_ms);
    ESP_LOGI(TAG, "Refresh Token: %s", refresh_token);
    ESP_LOGI(TAG, "Scope: %s", granted_scope);

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
        strncpy(DisplayName, displayNameItem->valuestring, DisplayName_str_size - 1);
        DisplayName[DisplayName_str_size - 1] = '\0';
    }
    cJSON *spotifyProfileURLItem = cJSON_GetObjectItemCaseSensitive(J_UsserInfo, "ProfileURL");
    if (cJSON_IsString(spotifyProfileURLItem) && (spotifyProfileURLItem->valuestring != NULL))
    {
        strncpy(ProfileURL, spotifyProfileURLItem->valuestring, ProfileURL_str_size - 1);
        ProfileURL[ProfileURL_str_size - 1] = '\0';
    }
    cJSON *UserInfoIDItem = cJSON_GetObjectItemCaseSensitive(J_UsserInfo, "UserID");
    if (cJSON_IsString(UserInfoIDItem) && (UserInfoIDItem->valuestring != NULL))
    {
        strncpy(UserID, UserInfoIDItem->valuestring, UserID_str_size - 1);
        UserID[UserID_str_size - 1] = '\0';
    }
    cJSON *image1Item = cJSON_GetObjectItemCaseSensitive(J_UsserInfo, "Image1");
    if (cJSON_IsString(image1Item) && (image1Item->valuestring != NULL))
    {
        strncpy(Image1, image1Item->valuestring, Image1_str_size - 1);
        Image1[Image1_str_size - 1] = '\0';
    }
    cJSON *image2Item = cJSON_GetObjectItemCaseSensitive(J_UsserInfo, "Image2");
    if (cJSON_IsString(image2Item) && (image2Item->valuestring != NULL))
    {
        strncpy(Image2, image2Item->valuestring, Image2_str_size - 1);
        Image2[Image2_str_size - 1] = '\0';
    }
    cJSON *followerItem = cJSON_GetObjectItemCaseSensitive(J_UsserInfo, "Follower");
    if (cJSON_IsNumber(followerItem))
    {
        //Follower = followerItem;
    }
    cJSON *countryItem = cJSON_GetObjectItemCaseSensitive(J_UsserInfo, "Country");
    if (cJSON_IsString(countryItem) && (countryItem->valuestring != NULL))
    {
        strncpy(Country, countryItem->valuestring, Country_str_size - 1);
        Country[Country_str_size - 1] = '\0';
    }
    cJSON *productItem = cJSON_GetObjectItemCaseSensitive(J_UsserInfo, "Product");
    if (cJSON_IsString(productItem) && (productItem->valuestring != NULL))
    {
        strncpy(Product, productItem->valuestring, Product_str_size - 1);
        Product[Product_str_size - 1] = '\0';
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
