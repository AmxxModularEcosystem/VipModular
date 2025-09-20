#include <amxmodx>
#include <ParamsController>
#include <VipModular>

new bool:IsSteamLimitAvailable = false;

DefaultObjects_Limit_Steam_Register() {
    VipM_Limits_RegisterType("Steam", true, true);
}

DefaultObjects_Limit_Steam_NativeFilter(const name[], const bool:trap) {
    if (equal(name, "REU_GetAuthtype")) {
        IsSteamLimitAvailable = trap;
        return true;
    }

    return false;
}

DefaultObjects_Limit_Steam_OnClientAuth(const playerIndex) {
    if (IsSteamLimitAvailable) {
        VipM_Limits_SetStaticValue("Steam", is_user_steam(playerIndex), playerIndex);
    } else {
        VipM_Limits_SetStaticValue("Steam", true, playerIndex);
    }
}
