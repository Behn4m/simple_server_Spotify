#ifdef __cplusplus
extern "C" {
#endif
#ifndef   JSON_EXTRACTION_H_
#define  JSON_EXTRACTION_H_

/**
 * @brief This function extracts specific parameters from a JSON string and assigns them to corresponding fields in a TokenParam structure.
 * @param[in] Json The input JSON string.
 * @param[in] SizeJson The size of the JSON string.
 * @return This function does not return a value.
 */
void ExtractionJsonParamForFindAccessToken(char *Json, size_t SizeJson);

/**
 * @brief This function extracts specific parameters from a JSON string and assigns them to corresponding fields in a UserInfo structure.
 * @param[in] JsonUSerInfo The input JSON string containing user information.
 * @return Returns 0 if the JSON is parsed successfully, or 1 otherwise.
 */
int ExtractionJsonParamForFindUserInfo(char *JsonUSerInfo);
#endif
#ifdef __cplusplus
}
#endif