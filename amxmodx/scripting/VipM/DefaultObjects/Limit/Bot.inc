#include <amxmodx>
#include <VipModular>

DefaultObjects_Limit_Bot_Register() {
    VipM_Limits_RegisterType("Bot", true, true);
}

DefaultObjects_Limit_Bot_OnClientAuth(const playerIndex) {
    VipM_Limits_SetStaticValue("Bot", is_user_bot(playerIndex), playerIndex);
}
