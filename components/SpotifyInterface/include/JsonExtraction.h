#ifdef __cplusplus
extern "C" {
#endif
#ifndef   JSON_EXTRACTION_H_
#define  JSON_EXTRACTION_H_

/**
 * @brief This function extracts specific parameters from a JSON string and assigns them to corresponding fields in a UserInfo structure.
 * @param[in] all Json parameters has a @param to write on
 * @return Returns 0 if the JSON is parsed successfully, or 1 otherwise.
 */
int ExtractUserInfoParamsfromJson(char *JsonUserInfo, UserInfo_t *userInfo);

/**
 * @brief This function extracts specific parameters from a JSON string and assigns them to corresponding fields in a PlaybackInfo structure.
 * @param[in] JsonPlaybackInfo The input JSON string containing playback information.
 * @param[in] playbackInfo The object needed to be filled with extarcted data
 * @return Returns 0 if the JSON is parsed successfully, or 1 otherwise.
 */
int ExtractPlaybackInfoParamsfromJson(char *JsonPlaybackInfo, PlaybackInfo_t *playbackInfo);

#endif
#ifdef __cplusplus
}
#endif
