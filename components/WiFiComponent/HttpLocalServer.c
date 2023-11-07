#include "WiFiConfig.h"

char UserWifiPassWord[100];
char UserWifiSSID[100];

bool FindWifiSSIDAndPassword(char *Res, uint16_t SizeRes)
{
    uint8_t flg_findSSID = 0;
    uint8_t flg_findPassword = 0;
    for (uint16_t i = 0; i < SizeRes; i++)
    {
        if (Res[i] == 'u')
        {
            if (Res[i + 1] == 's' && Res[i + 2] == 'e' && Res[i + 3] == 'r' && Res[i + 4] == 'n' && Res[i + 5] == 'a' && Res[i + 6] == 'm' && Res[i + 7] == 'e')
            {

                for (int j = i + 9; j < SizeRes; j++)
                {
                    UserWifiSSID[j - (i + 9)] = Res[j];
                    if (Res[j] == '&')
                    {
                        break;
                        flg_findSSID = 1;
                        printf("\twe find SSID !\n");
                    }
                }
            }
        }
        if (flg_findSSID == 1)
        {
            if (Res[i] == 'l')
            {
                if (Res[i + 1] == 'o' && Res[i + 2] == 'g' && Res[i + 3] == 'i' && Res[i + 4] == 'n' && Res[i + 5] == '_' )
                {
                    for (int j = i + 15; j < SizeRes; j++)
                    {
                        UserWifiPassWord[j - (i + 9)] = Res[j];
                        break;
                        printf("\twe find Password!\n");
                        flg_findPassword = 1;
                    }
                }
            }
        }
    }
    if (flg_findSSID == 1 || flg_findPassword == 1)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}