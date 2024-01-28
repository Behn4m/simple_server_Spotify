#ifdef __cplusplus
extern "C" {
#endif
#ifndef   JSON_EXTRACTION_H_
#define  JSON_EXTRACTION_H_

/**
 * @brief This function extracts specific parameters from a JSON string and assigns them to corresponding fields in a TokenParam structure.
 * @param[in] Json The input JSON string.
 * @param[in] SizeJson The size of the JSON string.
 * @return fasle if fail, true if finish successfull.
 */
bool ExtractAccessTokenParamsTokenFromJson(char *Json, size_t SizeJson, char *Token, char *TokenType, char *RefreshToken, char *GrantedScope, int ExpiresInMS);

/**
 * @brief This function extracts specific parameters from a JSON string and assigns them to corresponding fields in a UserInfo structure.
 * @param[in] all Json parameters has a @param to write on
 * @return Returns 0 if the JSON is parsed successfully, or 1 otherwise.
 */
int ExtractUserInfoParamsfromJson(char *JsonUSerInfo, char *DisplayName, char *ProfileURL, char *UserID, char *Image1, char *Image2, int *Follower, char *Country, char *Product);
#endif
#ifdef __cplusplus
}
#endif