#include <amxmodx>
#include <reapi>
#include <hamsandwich>
#include <ItemsController>
#include <ParamsController>

DefaultObjects_ItemType_Health_Register() {
    new T_IC_ItemType:type = IC_ItemType_SimpleRegister(
        .name = "Health",
        .onGive = "@OnHealthGive"
    );
    IC_ItemType_AddParams(type,
        "Health", DEFAULT_PARAMS_FLOAT_NAME, true,
        "MaxHealth", DEFAULT_PARAMS_FLOAT_NAME, false,
        "SetHealth", DEFAULT_PARAMS_BOOL_NAME, false
    );
}

@OnHealthGive(const playerIndex, const Trie:p) {
    if (PCGet_Bool(p, "SetHealth", false)) {
        set_entvar(playerIndex, var_health, PCGet_Float(p, "Health"));
    } else {
        new Float:fHealth = Float:get_entvar(playerIndex, var_health);
        new Float:fMaxHealth = PCGet_Float(p, "MaxHealth", 100.0);
        new Float:fAddHealth = floatclamp(PCGet_Float(p, "Health"), 0.0, floatmax(0.0, fMaxHealth - fHealth));
        new Float:fMaxHealthCurrent = Float:get_entvar(playerIndex, var_max_health);
        new bool:bNeedOverrideMaxHealth = (fHealth < fMaxHealth && fMaxHealthCurrent < fMaxHealth);

        if (bNeedOverrideMaxHealth) {
            set_entvar(playerIndex, var_max_health, fMaxHealth);
        }

        ExecuteHamB(Ham_TakeHealth, playerIndex, fAddHealth, DMG_GENERIC);

        if (bNeedOverrideMaxHealth) {
            set_entvar(playerIndex, var_max_health, fMaxHealthCurrent);
        }
    }
}
