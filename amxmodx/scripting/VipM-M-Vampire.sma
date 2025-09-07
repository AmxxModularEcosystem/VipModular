#include <amxmodx>
#include <reapi>
#include <VipModular>
#include "VipM/Utils"

#pragma semicolon 1
#pragma compress 1

public stock const PluginName[] = "[VipM-M] Vampire";
public stock const PluginVersion[] = _VIPM_VERSION;
public stock const PluginAuthor[] = "ArKaNeMaN";
public stock const PluginURL[] = _VIPM_PLUGIN_URL;
public stock const PluginDescription[] = "Vip modular`s module - Vampire";

new const MODULE_NAME[] = "Vampire";

public VipM_Modules_OnInited() {
    register_plugin(PluginName, PluginVersion, PluginAuthor);
    register_dictionary("VipM-Vampire.ini");

    VipM_Modules_Register(MODULE_NAME);
    VipM_Modules_AddParamsEx(MODULE_NAME,
        "ByKill", "Integer", false,
        "ByHead", "Integer", false,
        "ByKnife", "Integer", false,
        "ByGrenade", "Integer", false
    );
    VipM_Modules_AddParamsEx(MODULE_NAME,
        "MaxHealth", "Integer", false,
        "Limits", "VipM-Limits", false
    );
    VipM_Modules_RegisterEvent(MODULE_NAME, Module_OnActivated, "@Event_ModuleActivate");
}

@Event_ModuleActivate() {
    RegisterHookChain(RG_CBasePlayer_Killed, "@Event_PlayerKilled", true);
}

@Event_PlayerKilled(const VictimId, UserId, InflictorId) {
    if (
        UserId == VictimId
        || !is_user_alive(UserId)
        || !is_user_connected(VictimId)
    ) {
        return;
    }

    new Trie:p = VipM_Modules_GetParams(MODULE_NAME, UserId);
    if (p == Invalid_Trie) {
        return;
    }

    if (!PCGet_VipmLimitsCheck(p, "Limits", UserId, Limit_Exec_AND)) {
        return;
    }

    new MaxHealth = PCGet_Int(p, "MaxHealth", 100);
    new Health = floatround(get_entvar(UserId, var_health));
    if (Health >= MaxHealth) {
        return;
    }
    
    new ByKill = PCGet_Int(p, "ByKill", 0);
    new VampHealth = 0;
    new ActiveItem; ActiveItem = get_member(UserId, m_pActiveItem);
    if (
        !(get_member(VictimId, m_bitsDamageType) & DMG_SLASH)
        && is_entity(ActiveItem)
        && rg_get_iteminfo(ActiveItem, ItemInfo_iId) == CSW_KNIFE
    ) {
        VampHealth = PCGet_Int(p, "ByKnife", ByKill);
    } else if(get_member(VictimId, m_bHeadshotKilled)) {
        VampHealth = PCGet_Int(p, "ByHead", ByKill);
    } else if (get_member(VictimId, m_bKilledByGrenade)) {
        VampHealth = PCGet_Int(p, "ByGrenade", ByKill);
    } else {
        VampHealth = ByKill;
    }

    if (ByKill <= 0) {
        return;
    }

    client_print(UserId, print_center, "%L", UserId, "VAMPIRE_HEALTH_MESSAGE", VampHealth);
    Health = clamp(Health + VampHealth, 1, MaxHealth < 1 ? cellmax : MaxHealth);
    set_entvar(UserId, var_health, float(Health));
}
