#ifndef _APP_INFO_H_
#define _APP_INFO_H_

/*
    To generate the base64 credentials using the client_id and secret_id, 
    you can use an online web tool or a base64 encoding library in your programming language. 
    The client_id and secret_id should be concatenated with a colon (:) in between, 
    and then the resulting string should be encoded using base64 encoding.
*/

#define CLIENT_ID "xxxxxx"                                          // update it with your Spotify Web App client_id
#define BASE64_CREDINTIALS "xxxxxx="                                 // update it with your Spotify Web App base64 credentials
#define REDIRECT_URI "http%3A%2F%2Fdeskhub.local%2Fcallback%2f"     // leave it as it is

#endif // _APP_INFO_H_
