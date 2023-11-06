#ifdef __cplusplus
extern "C" {
#endif


#ifndef   MAKE_SPOTIFY_REQUEST_H_
#define   MAKE_SPOTIFY_REQUEST_H_

#include"SpotifyAuthorization.h"



void GetCurrentPlaying();
void SendRequest_AndGiveTokenWithRefreshToken(char *Buf, size_t SizeBuf, char *RefreshToken_);

bool FindToken(char *Res, uint16_t SizeRes);
bool FindCode(char *Res, uint16_t SizeRes);
void SendRequest_AndGiveToken(char *Buf, size_t SizeBuf, char *code, size_t SizeCode);
void MakePlayerCommand_AndSendIt(const char *Method_, const char *Command_, char *Buf, size_t SizeBuf);
void SendRequestForPause();
void SendRequestForPlay();
void SendRequestForPrevious();
void SendRequestForNext();
void GetUserStatus();
void GetUserTopItems();
void GetUserProfile(char *UserId_);
void ExtractionJsonParamForFindAccessToken(char *Json, size_t SizeJson);







#endif
#ifdef __cplusplus
}
#endif

