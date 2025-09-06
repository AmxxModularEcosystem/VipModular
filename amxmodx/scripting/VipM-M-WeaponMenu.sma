#include <amxmodx>
#include <reapi>
#include <VipModular>
#include <ParamsController>
#include "VipM/Utils"
#include "VipM/DebugMode"

#include "VipM/WeaponMenu/Objects/WeaponMenu"
#include "VipM/WeaponMenu/Objects/MenuItem"

public stock const PluginName[] = "[VipM-M] Weapon Menu";
public stock const PluginVersion[] = _VIPM_VERSION;
public stock const PluginAuthor[] = "ArKaNeMaN";
public stock const PluginURL[] = _VIPM_PLUGIN_URL;
public stock const PluginDescription[] = "Vip modular`s module - Weapon Menu";

new const MODULE_NAME[] = "WeaponMenu";

enum {
    TASK_OFFSET_AUTO_OPEN = 100,
    TASK_OFFSET_AUTO_CLOSE = 200,
}

new bool:gUserShouldResetCounters[MAX_PLAYERS + 1] = {true, ...};
new gUserLeftItems[MAX_PLAYERS + 1] = {0, ...};
new Trie:g_tUserMenuItemsCounter[MAX_PLAYERS + 1] = {Invalid_Trie, ...};

new bool:gUserAutoOpen[MAX_PLAYERS + 1] = {true, ...};
new gUserExpireStatus[MAX_PLAYERS + 1][VIPM_M_WEAPONMENU_EXPIRE_STATUS_MAX_LEN];

#include "VipM/WeaponMenu/KeyValueCounter"
#include "VipM/WeaponMenu/Menus"

public VipM_Modules_OnInited() {
    register_plugin(PluginName, PluginVersion, PluginAuthor);
    register_dictionary("VipM-WeaponMenu.ini");
    IC_Init();

    VipM_Modules_Register(MODULE_NAME);
    VipM_Modules_AddParamsEx(MODULE_NAME,
        // TODO Read "Menus" as param
        "MainMenuTitle", "String", false,
        "Count", "Integer", false,
        "Limits", "VipM-Limits", false
    );
    VipM_Modules_AddParamsEx(MODULE_NAME,
        "AutoopenLimits", "VipM-Limits", false,
        "AutoopenDelay", "Float", false,
        "AutoopenCloseDelay", "Float", false,
        "AutoopenMenuNum", "Integer", false
    );
    VipM_Modules_AddParamsEx(MODULE_NAME,
        "StayOpen", "Boolean", false,
        "StayOpen_CheckCounter", "Boolean", false
    );
    VipM_Modules_RegisterEvent(MODULE_NAME, Module_OnActivated, "@OnModuleActivate");
    VipM_Modules_RegisterEvent(MODULE_NAME, Module_OnRead, "@OnReadConfig");
}

@OnReadConfig(const JSON:jCfg, Trie:tParams) {
    if (!json_object_has_value(jCfg, "Menus")) {
        PCJson_LogForFile(jCfg, "WARNING", "Param 'Menus' required for module '%s'.", MODULE_NAME);
        return VIPM_STOP;
    }
    
    TrieSetCell(tParams, "Menus", Json_Object_GetWeaponMenusList(jCfg, "Menus"));

    if (!TrieKeyExists(tParams, "MainMenuTitle")) {
        TrieSetString(tParams, "MainMenuTitle", Lang("MENU_MAIN_TITLE"));
    }

    return VIPM_CONTINUE;
}

@OnModuleActivate() {
    RegisterHookChain(RG_CBasePlayer_Spawn, "@OnPlayerSpawn", true);
    RegisterHookChain(RG_CSGameRules_RestartRound, "@OnRestartRound", false);

    register_clcmd(VIPM_M_WEAPONMENU_CMD_MENU, "@Cmd_Menu");
    register_clcmd(VIPM_M_WEAPONMENU_CMD_MENU_SILENT, "@Cmd_MenuSilent");
    register_clcmd(VIPM_M_WEAPONMENU_CMD_AUTOOPEN_TOGGLE, "@Cmd_SwitchAutoOpen");
}

ResetUserMenuCounters(const UserId) {
    new Trie:Params = VipM_Modules_GetParams(MODULE_NAME, UserId);

    gUserLeftItems[UserId] = PCGet_Int(Params, "Count", -1);
    g_tUserMenuItemsCounter[UserId] = KeyValueCounter_Reset(g_tUserMenuItemsCounter[UserId]);

    gUserShouldResetCounters[UserId] = false;

    Dbg_Log("ResetUserMenuCounters(%n): gUserLeftItems[UserId] = %d", UserId, gUserLeftItems[UserId]);
}

public client_putinserver(UserId) {
    gUserShouldResetCounters[UserId] = true;
    gUserExpireStatus[UserId][0] = 0;
}

public client_disconnected(UserId) {
    AbortAutoCloseMenu(UserId);
}

@OnRestartRound() {
    for (new UserId = 1; UserId <= MAX_PLAYERS; UserId++) {
        gUserShouldResetCounters[UserId] = true;
    }
    Dbg_Log("@OnRestartRound(): Should reset all counters.");
}

@OnPlayerSpawn(const UserId) {
    if (!IsPlayerAlive(UserId)) {
        Dbg_Log("@OnPlayerSpawn(%n): Invalid (or dead) player", UserId);
        return;
    }

    if (gUserShouldResetCounters[UserId]) {
        ResetUserMenuCounters(UserId);
    } else {
        Dbg_Log("@OnPlayerSpawn(%n): Shouldn't reset counter", UserId);
    }

    // TODO: Добавить квар для отключения авто-открытия

    new Trie:Params = VipM_Modules_GetParams(MODULE_NAME, UserId);

    if (!gUserAutoOpen[UserId]) {
        return;
    }

    if (VipM_Params_GetArr(Params, "Menus") == Invalid_Array) {
        return;
    }

    if (!PCGet_VipmLimitsCheck(Params, "AutoopenLimits", UserId, Limit_Exec_AND)) {
        return;
    }

    set_task(PCGet_Float(Params, "AutoopenDelay", 0.0), "@Task_AutoOpen", TASK_OFFSET_AUTO_OPEN + UserId);
}

@Task_AutoOpen(UserId) {
    UserId -= TASK_OFFSET_AUTO_OPEN;

    new Trie:tParams = VipM_Modules_GetParams(MODULE_NAME, UserId);
    new Float:fAutoCloseDelay = PCGet_Float(tParams, "AutoopenCloseDelay", 0.0);
    new iMenuNum = PCGet_Int(tParams, "AutoopenMenuNum", -1);

    if (iMenuNum > 0) {
        client_cmd(UserId, "%s %d", VIPM_M_WEAPONMENU_CMD_MENU_SILENT, iMenuNum - 1);
    } else {
        client_cmd(UserId, VIPM_M_WEAPONMENU_CMD_MENU_SILENT);
    }
    
    Dbg_Log("@Task_AutoOpen(%d): fAutoCloseDelay = %.2f", UserId, fAutoCloseDelay);
    if (fAutoCloseDelay > 0.0) {
        Dbg_Log("@Task_AutoOpen(%d): Start auto close task", UserId);
        set_task(fAutoCloseDelay, "@Task_AutoClose", TASK_OFFSET_AUTO_CLOSE + UserId);
    }
}

@Task_AutoClose(UserId) {
    Dbg_Log("@Task_AutoClose(%d)", UserId);
    UserId -= TASK_OFFSET_AUTO_CLOSE;
    menu_cancel(UserId);
    show_menu(UserId, 0, "");
}

AbortAutoCloseMenu(const UserId) {
    remove_task(TASK_OFFSET_AUTO_CLOSE + UserId);
}

@Cmd_SwitchAutoOpen(const UserId) {
    gUserAutoOpen[UserId] = !gUserAutoOpen[UserId];
    ChatPrintL(UserId, gUserAutoOpen[UserId] ? "MSG_AUTOOPEN_TURNED_ON" : "MSG_AUTOOPEN_TURNED_OFF");
    return PLUGIN_HANDLED;
}

@Cmd_Menu(const UserId) {
    _Cmd_Menu(UserId);
    return PLUGIN_HANDLED;
}

@Cmd_MenuSilent(const UserId) {
    _Cmd_Menu(UserId, true);
    return PLUGIN_HANDLED;
}

_Cmd_Menu(const UserId, const bool:bSilent = false) {
    if (!IsPlayerValid(UserId)) {
        Dbg_Log("_Cmd_Menu(%d, %s): Invalid player", UserId, bSilent ? "true" : "false");
        return;
    }

    if (!is_user_alive(UserId)) {
        ChatPrintLIf(!bSilent, UserId, "MSG_YOU_DEAD");

        Dbg_Log("_Cmd_Menu(%n, %s): Player is dead", UserId, bSilent ? "true" : "false");
        return;
    }

    new Trie:Params = VipM_Modules_GetParams(MODULE_NAME, UserId);
    new Array:aMenus = VipM_Params_GetArr(Params, "Menus");

    if (ArraySizeSafe(aMenus) < 1) {
        ChatPrintLIf(!bSilent, UserId, "MSG_NO_ACCESS");

        Dbg_Log("_Cmd_Menu(%n, %s): No access", UserId, bSilent ? "true" : "false");
        return;
    }
    
    if (!PCGet_VipmLimitsCheck(Params, "Limits", UserId, Limit_Exec_AND)) {
        ChatPrintLIf(!bSilent, UserId, "MSG_MAIN_NOT_PASSED_LIMIT");

        Dbg_Log("_Cmd_Menu(%n, %s): Not passed main limits", UserId, bSilent ? "true" : "false");
        return;
    }

    if (read_argc() < 2) {
        if (ArraySizeSafe(aMenus) == 1) {
            client_cmd(UserId, "%s %d", VIPM_M_WEAPONMENU_CMD_MENU, 0);
        } else {
            static MainMenuTitle[128];
            PCGet_Str(Params, "MainMenuTitle", MainMenuTitle, charsmax(MainMenuTitle));
            Menu_MainMenu(UserId, MainMenuTitle, aMenus);
        }
        return;
    }

    new MenuId = read_argv_int(2);
    if (
        ArraySizeSafe(aMenus) <= MenuId
        || MenuId < 0
    ) {
        Dbg_Log("_Cmd_Menu(%n, %s): Invalid menu id (%d)", UserId, bSilent ? "true" : "false", MenuId);
        return;
    }

    static Menu[S_WeaponMenu];
    ArrayGetArray(aMenus, MenuId, Menu);

    if (Menu[WeaponMenu_Limits] != Invalid_Array && !VipM_Limits_ExecuteList(Menu[WeaponMenu_Limits], UserId, Limit_Exec_AND)) {
        ChatPrintLIf(!bSilent, UserId, "MSG_MENU_NOT_PASSED_LIMIT");
        Dbg_Log("_Cmd_Menu(%n, %s): Not passed menu limits", UserId, bSilent ? "true" : "false");
        return;
    }
    
    if (Menu[WeaponMenu_FakeMessage][0]) {
        ChatPrint(UserId, Menu[WeaponMenu_FakeMessage]);
        Dbg_Log("_Cmd_Menu(%n, %s): Fake menu (%s)", UserId, bSilent ? "true" : "false", Menu[WeaponMenu_FakeMessage]);
        return;
    }

    if (read_argc() < 3) {
        Menu_WeaponsMenu(UserId, MenuId, Menu);
        return;
    }

    new ItemId = read_argv_int(3);
    if (
        ArraySizeSafe(Menu[WeaponMenu_Items]) <= ItemId
        || ItemId < 0
    ) {
        Dbg_Log("_Cmd_Menu(%n, %s): Invalid item id (%d)", UserId, bSilent ? "true" : "false", ItemId);
        return;
    }

    static MenuItem[S_MenuItem];
    ArrayGetArray(Menu[WeaponMenu_Items], ItemId, MenuItem);

    new iItemsLeft = GetUserLeftItems(UserId, MenuId, Menu);

    if (
        MenuItem[MenuItem_UseCounter]
        && iItemsLeft == 0
    ) {
        ChatPrintLIf(!bSilent, UserId, "MSG_NO_LEFT_ITEMS");

        Dbg_Log("_Cmd_Menu(%n, %s): No left items", UserId, bSilent ? "true" : "false");
        return;
    }

    if (
        !VipM_Limits_ExecuteList(MenuItem[MenuItem_ShowLimits], UserId, Limit_Exec_AND)
        || !VipM_Limits_ExecuteList(MenuItem[MenuItem_ActiveLimits], UserId, Limit_Exec_AND)
        || !VipM_Limits_ExecuteList(MenuItem[MenuItem_Limits], UserId, Limit_Exec_AND)
    ) {
        ChatPrintLIf(!bSilent, UserId, "MSG_MENUITEM_NOT_PASSED_LIMIT");

        Dbg_Log("_Cmd_Menu(%n, %s): Not passed item limits", UserId, bSilent ? "true" : "false");
        return;
    }
    
    if (
        IC_Item_GiveArray(UserId, MenuItem[MenuItem_Items])
        && MenuItem[MenuItem_UseCounter]
    ) {
        gUserLeftItems[UserId]--;

        if (Menu[WeaponMenu_Count]) {
            KeyValueCounter_Inc(g_tUserMenuItemsCounter[UserId], IntToStr(MenuId));
        }
    }

    if (
        PCGet_Bool(Params, "StayOpen", false)
        && (
            !PCGet_Bool(Params, "StayOpen_CheckCounter", true)
            || iItemsLeft != 0
        )
    ) {
        client_cmd(UserId, "%s %d", VIPM_M_WEAPONMENU_CMD_MENU, MenuId);
    }
}

GetUserLeftItems(const UserId, const MenuId, const Menu[S_WeaponMenu]) {
    new iUserItemsLeft = gUserLeftItems[UserId];
    new iMenuItemsLeft = Menu[WeaponMenu_Count] - KeyValueCounter_Get(g_tUserMenuItemsCounter[UserId], IntToStr(MenuId));
    
    Dbg_Log("GetUserLeftItems(%n, %d, %d): iUserItemsLeft = %d", UserId, MenuId, Menu[WeaponMenu_Name], iUserItemsLeft);
    Dbg_Log("GetUserLeftItems(%n, %d, %d): iMenuItemsLeft = %d", UserId, MenuId, Menu[WeaponMenu_Name], iMenuItemsLeft);
    Dbg_Log("GetUserLeftItems(%n, %d, %d): Menu[WeaponMenu_Count] = %d", UserId, MenuId, Menu[WeaponMenu_Name], Menu[WeaponMenu_Count]);
    Dbg_Log("GetUserLeftItems(%n, %d, %d): g_tUserMenuItemsCounter[UserId] = %d", UserId, MenuId, Menu[WeaponMenu_Name], KeyValueCounter_Get(g_tUserMenuItemsCounter[UserId], IntToStr(MenuId)));

    if (iUserItemsLeft < 0) {
        Dbg_Log("GetUserLeftItems(%n, %d, %d): return %d (no global limit)", UserId, MenuId, Menu[WeaponMenu_Name], iMenuItemsLeft);
        return iMenuItemsLeft;
    }

    if (Menu[WeaponMenu_Count] < 0) {
        Dbg_Log("GetUserLeftItems(%n, %d, %d): return %d (no menu limit)", UserId, MenuId, Menu[WeaponMenu_Name], iUserItemsLeft);
        return iUserItemsLeft;
    }
    
    Dbg_Log("GetUserLeftItems(%n, %d, %d): return %d (has both limits)", UserId, MenuId, Menu[WeaponMenu_Name], min(iUserItemsLeft, iMenuItemsLeft));
    return min(iUserItemsLeft, iMenuItemsLeft);
}

#include "VipM/WeaponMenu/Natives"
