#include <amxmodx>
#include <reapi>
#include <VipModular>
#include "VipM/Utils"
#include "VipM/DebugMode"
#include "VipM/ArrayTrieUtils"

#pragma semicolon 1
#pragma compress 1

public stock const PluginName[] = "[VipM-M] Spawn Health";
public stock const PluginVersion[] = _VIPM_VERSION;
public stock const PluginAuthor[] = "ArKaNeMaN";
public stock const PluginURL[] = _VIPM_PLUGIN_URL;
public stock const PluginDescription[] = "Vip modular`s module - SpawnHealth";

new const MODULE_NAME[] = "SpawnHealth";

public VipM_Modules_OnInited() {
    register_plugin(PluginName, PluginVersion, PluginAuthor);

    VipM_Modules_Register(MODULE_NAME);
    VipM_Modules_AddParamsEx(MODULE_NAME,
        "Health", DEFAULT_PARAMS_INT_NAME, false,
        "SetHealth", DEFAULT_PARAMS_BOOL_NAME, false,
        "MaxHealth", DEFAULT_PARAMS_INT_NAME, false
    );
    VipM_Modules_AddParamsEx(MODULE_NAME,
        "Armor", DEFAULT_PARAMS_INT_NAME, false,
        "SetArmor", DEFAULT_PARAMS_BOOL_NAME, false,
        "MaxArmor", DEFAULT_PARAMS_INT_NAME, false
    );
    VipM_Modules_AddParamsEx(MODULE_NAME,
        "Helmet", DEFAULT_PARAMS_BOOL_NAME, false,
        "Limits", VIPM_PARAM_TYPE_LIMITS_NAME, false
    );
    VipM_Modules_RegisterEvent(MODULE_NAME, Module_OnActivated, "@Event_ModuleActivate");
}

@Event_ModuleActivate() {
    RegisterHookChain(RG_CBasePlayer_Spawn, "@Event_PlayerSpawned", true);
}

@Event_PlayerSpawned(const UserId) {
    if (!is_user_alive(UserId)) {
        return;
    }
    
    if (!VipM_Modules_HasModule(MODULE_NAME, UserId)) {
        return;
    }

    new Trie:p = VipM_Modules_GetParams(MODULE_NAME, UserId);
    
    if (!PCGet_VipmLimitsCheck(p, "Limits", UserId, Limit_Exec_AND)) {
        return;
    }

    new health = PCGet_Int(p, "Health", 0);
    new maxHealth = PCGet_Int(p, "MaxHealth", floatround(Float:get_entvar(UserId, var_max_health)));
    if (health > 0) {
        if (!PCGet_Bool(p, "SetHealth", true)) {
            health = min(floatround(get_entvar(UserId, var_health)) + health, maxHealth);
        }
        set_entvar(UserId, var_health, float(health));
    }

    new armor = PCGet_Int(p, "Armor", 0);
    if (armor > 0) {
        if (!PCGet_Bool(p, "SetArmor", true)) {
            armor = min(rg_get_user_armor(UserId) + armor, PCGet_Int(p, "MaxArmor", 100));
        }
        rg_set_user_armor(UserId, armor, PCGet_Bool(p, "Helmet", false) ? ARMOR_VESTHELM : ARMOR_KEVLAR);
    }
}
