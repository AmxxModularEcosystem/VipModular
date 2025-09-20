#include <ItemsController>

#include "VipM/DefaultObjects/ParamType/Limit"
#include "VipM/DefaultObjects/ParamType/Limits"

public ParamsController_OnRegisterTypes() {
    DefaultObjects_ParamType_Limit_Register();
    DefaultObjects_ParamType_Limits_Register();
}

#include <VipModular>

#include "VipM/DefaultObjects/Limit/Always"
#include "VipM/DefaultObjects/Limit/GameTime"
#include "VipM/DefaultObjects/Limit/InFreezyTime"
#include "VipM/DefaultObjects/Limit/Ip"
#include "VipM/DefaultObjects/Limit/Logic"
#include "VipM/DefaultObjects/Limit/Name"
#include "VipM/DefaultObjects/Limit/Never"
#include "VipM/DefaultObjects/Limit/OncePer"
#include "VipM/DefaultObjects/Limit/Round"
#include "VipM/DefaultObjects/Limit/RoundTime"
#include "VipM/DefaultObjects/Limit/Steam"
#include "VipM/DefaultObjects/Limit/SteamId"
#include "VipM/DefaultObjects/Limit/Time"
#include "VipM/DefaultObjects/Limit/WeekDay"

public VipM_Limits_OnInited() {
    DefaultObjects_Limit_Always_Register();
    DefaultObjects_Limit_GameTime_Register();
    DefaultObjects_Limit_InFreezyTime_Register();
    DefaultObjects_Limit_Ip_Register();
    DefaultObjects_Limit_Logic_Register();
    DefaultObjects_Limit_Name_Register();
    DefaultObjects_Limit_Never_Register();
    DefaultObjects_Limit_OncePer_Register();
    DefaultObjects_Limit_Round_Register();
    DefaultObjects_Limit_RoundTime_Register();
    DefaultObjects_Limit_Steam_Register();
    DefaultObjects_Limit_SteamId_Register();
    DefaultObjects_Limit_Time_Register();
    DefaultObjects_Limit_WeekDay_Register();
}

bool:DefaultObjects_NativeFilter(const name[], const bool:trap) {
    new bool:ret = false;

    if (DefaultObjects_Limit_Steam_NativeFilter(name, trap)) {
        ret = true;
    }

    return ret;
}

DefaultObjects_OnClientAuth(const playerIndex, const steamId[]) {
    DefaultObjects_Limit_SteamId_OnClientAuth(playerIndex, steamId);
    DefaultObjects_Limit_Ip_OnClientAuth(playerIndex);
    DefaultObjects_Limit_Steam_OnClientAuth(playerIndex);
}