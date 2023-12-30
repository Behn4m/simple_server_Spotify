#ifdef __cplusplus
extern "C" {
#endif
#ifndef   JSON_EXTRACTION_H_
#define  JSON_EXTRACTION_H_

/**
 * @brief Extracts JSON content from an HTTP response string.
 * This function separates the JSON content from an HTTP response header and extracts the JSON string.
 * @param[in] HttpResponse The input string containing an HTTP response with JSON content.
 * @param[out] Json The output buffer to store the extracted JSON content.
 * @return Returns true if the JSON content is successfully extracted, otherwise false.
 */
bool ExtractJsonFromHttpResponse(char *HttpResponse, size_t SizeRes) ;
/**
 * @brief This function extracts specific parameters from a JSON string and assigns them to corresponding fields in a TokenParam structure.
 * @param[in] Json The input JSON string.
 * @param[in] SizeJson The size of the JSON string.
 * @return fasle if fail, true if finish successfull.
 */
bool ExtractionJsonParamForFindAccessToken(char *Json, size_t SizeJson, char *Token, char *TokenType, char *RefreshToken, char *GrantedScope, int ExpiresInMS);

/**
 * @brief This function extracts specific parameters from a JSON string and assigns them to corresponding fields in a UserInfo structure.
 * @param[in] all Json parameters has a @param to write on
 * @return Returns 0 if the JSON is parsed successfully, or 1 otherwise.
 */
int ExtractionJsonParamForFindUserInfo(char *JsonUSerInfo, char *DisplayName, char *ProfileURL, char *UserID, char *Image1, char *Image2, int *Follower, char *Country, char *Product);
#endif
#ifdef __cplusplus
}
#endif