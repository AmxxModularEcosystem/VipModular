#include <amxmodx>
#include <reapi>
#include <VipModular>
#include <ParamsController>
#include "VipM/Utils"

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
        // TODO: Read "Menus" as param
        "MainMenuTitle", DEFAULT_PARAMS_STR_NAME, false,
        "Count", DEFAULT_PARAMS_INT_NAME, false,
        "Limits", VIPM_PARAM_TYPE_LIMITS_NAME, false,
        "ResetCountOnSpawn", DEFAULT_PARAMS_BOOL_NAME, false
    );
    VipM_Modules_AddParamsEx(MODULE_NAME,
        "AutoopenLimits", VIPM_PARAM_TYPE_LIMITS_NAME, false,
        "AutoopenDelay", DEFAULT_PARAMS_FLOAT_NAME, false,
        "AutoopenCloseDelay", DEFAULT_PARAMS_FLOAT_NAME, false,
        "AutoopenMenuNum", DEFAULT_PARAMS_INT_NAME, false
    );
    VipM_Modules_AddParamsEx(MODULE_NAME,
        "StayOpen", DEFAULT_PARAMS_BOOL_NAME, false,
        "StayOpen_CheckCounter", DEFAULT_PARAMS_BOOL_NAME, false
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

ResetUserMenuCounters(const playerIndex) {
    new Trie:Params = VipM_Modules_GetParams(MODULE_NAME, playerIndex);

    gUserLeftItems[playerIndex] = PCGet_Int(Params, "Count", -1);
    g_tUserMenuItemsCounter[playerIndex] = KeyValueCounter_Reset(g_tUserMenuItemsCounter[playerIndex]);

    gUserShouldResetCounters[playerIndex] = false;
}

public client_putinserver(playerIndex) {
    gUserShouldResetCounters[playerIndex] = true;
    gUserExpireStatus[playerIndex][0] = 0;
}

public client_disconnected(playerIndex) {
    AbortAutoCloseMenu(playerIndex);
}

@OnRestartRound() {
    for (new playerIndex = 1; playerIndex <= MAX_PLAYERS; playerIndex++) {
        gUserShouldResetCounters[playerIndex] = true;
    }
}

@OnPlayerSpawn(const playerIndex) {
    if (!is_user_alive(playerIndex)) {
        return;
    }

    new Trie:p = VipM_Modules_GetParams(MODULE_NAME, playerIndex);

    if (gUserShouldResetCounters[playerIndex] || PCGet_Bool(p, "ResetCountOnSpawn", false)) {
        ResetUserMenuCounters(playerIndex);
    }

    // TODO: Добавить квар для отключения авто-открытия

    if (!gUserAutoOpen[playerIndex]) {
        return;
    }

    if (VipM_Params_GetArr(p, "Menus") == Invalid_Array) {
        return;
    }

    if (!PCGet_VipmLimitsCheck(p, "AutoopenLimits", playerIndex, Limit_Exec_AND)) {
        return;
    }

    set_task(PCGet_Float(p, "AutoopenDelay", 0.0), "@Task_AutoOpen", TASK_OFFSET_AUTO_OPEN + playerIndex);
}

@Task_AutoOpen(playerIndex) {
    playerIndex -= TASK_OFFSET_AUTO_OPEN;

    new Trie:tParams = VipM_Modules_GetParams(MODULE_NAME, playerIndex);
    new Float:fAutoCloseDelay = PCGet_Float(tParams, "AutoopenCloseDelay", 0.0);
    new iMenuNum = PCGet_Int(tParams, "AutoopenMenuNum", -1);

    if (iMenuNum > 0) {
        client_cmd(playerIndex, "%s %d", VIPM_M_WEAPONMENU_CMD_MENU_SILENT, iMenuNum - 1);
    } else {
        client_cmd(playerIndex, VIPM_M_WEAPONMENU_CMD_MENU_SILENT);
    }
    
    if (fAutoCloseDelay > 0.0) {
        set_task(fAutoCloseDelay, "@Task_AutoClose", TASK_OFFSET_AUTO_CLOSE + playerIndex);
    }
}

@Task_AutoClose(playerIndex) {
    playerIndex -= TASK_OFFSET_AUTO_CLOSE;
    menu_cancel(playerIndex);
    show_menu(playerIndex, 0, "");
}

AbortAutoCloseMenu(const playerIndex) {
    remove_task(TASK_OFFSET_AUTO_CLOSE + playerIndex);
}

@Cmd_SwitchAutoOpen(const playerIndex) {
    gUserAutoOpen[playerIndex] = !gUserAutoOpen[playerIndex];
    ChatPrintL(playerIndex, gUserAutoOpen[playerIndex] ? "MSG_AUTOOPEN_TURNED_ON" : "MSG_AUTOOPEN_TURNED_OFF");
    return PLUGIN_HANDLED;
}

@Cmd_Menu(const playerIndex) {
    _Cmd_Menu(playerIndex);
    return PLUGIN_HANDLED;
}

@Cmd_MenuSilent(const playerIndex) {
    _Cmd_Menu(playerIndex, true);
    return PLUGIN_HANDLED;
}

_Cmd_Menu(const playerIndex, const bool:bSilent = false) {
    if (!is_user_connected(playerIndex)) {
        return;
    }

    if (!is_user_alive(playerIndex)) {
        ChatPrintLIf(!bSilent, playerIndex, "MSG_YOU_DEAD");
        return;
    }

    new Trie:p = VipM_Modules_GetParams(MODULE_NAME, playerIndex);
    new Array:aMenus = VipM_Params_GetArr(p, "Menus");

    if (ArraySizeSafe(aMenus) < 1) {
        ChatPrintLIf(!bSilent, playerIndex, "MSG_NO_ACCESS");
        return;
    }
    
    if (!PCGet_VipmLimitsCheck(p, "Limits", playerIndex, Limit_Exec_AND)) {
        ChatPrintLIf(!bSilent, playerIndex, "MSG_MAIN_NOT_PASSED_LIMIT");
        return;
    }

    if (read_argc() < 2) {
        if (ArraySizeSafe(aMenus) == 1) {
            client_cmd(playerIndex, "%s %d", VIPM_M_WEAPONMENU_CMD_MENU, 0);
        } else {
            Menu_MainMenu(playerIndex, PCGet_iStr(p, "MainMenuTitle", uLang(playerIndex, "MENU_MAIN_TITLE")), aMenus);
        }

        return;
    }

    new MenuId = read_argv_int(2);
    if (
        ArraySizeSafe(aMenus) <= MenuId
        || MenuId < 0
    ) {
        return;
    }

    static Menu[S_WeaponMenu];
    ArrayGetArray(aMenus, MenuId, Menu);

    if (Menu[WeaponMenu_Limits] != Invalid_Array && !VipM_Limits_ExecuteList(Menu[WeaponMenu_Limits], playerIndex, Limit_Exec_AND)) {
        ChatPrintLIf(!bSilent, playerIndex, "MSG_MENU_NOT_PASSED_LIMIT");
        return;
    }
    
    if (Menu[WeaponMenu_FakeMessage][0]) {
        ChatPrint(playerIndex, Menu[WeaponMenu_FakeMessage]);
        return;
    }

    if (read_argc() < 3) {
        Menu_WeaponsMenu(playerIndex, MenuId, Menu);
        return;
    }

    new ItemId = read_argv_int(3);
    if (
        ArraySizeSafe(Menu[WeaponMenu_Items]) <= ItemId
        || ItemId < 0
    ) {
        return;
    }

    static MenuItem[S_MenuItem];
    ArrayGetArray(Menu[WeaponMenu_Items], ItemId, MenuItem);

    new iItemsLeft = GetUserLeftItems(playerIndex, MenuId, Menu);

    if (
        MenuItem[MenuItem_UseCounter]
        && iItemsLeft == 0
    ) {
        ChatPrintLIf(!bSilent, playerIndex, "MSG_NO_LEFT_ITEMS");
        return;
    }

    if (
        !VipM_Limits_ExecuteList(MenuItem[MenuItem_ShowLimits], playerIndex, Limit_Exec_AND)
        || !VipM_Limits_ExecuteList(MenuItem[MenuItem_ActiveLimits], playerIndex, Limit_Exec_AND)
        || !VipM_Limits_ExecuteList(MenuItem[MenuItem_Limits], playerIndex, Limit_Exec_AND)
    ) {
        ChatPrintLIf(!bSilent, playerIndex, "MSG_MENUITEM_NOT_PASSED_LIMIT");
        return;
    }
    
    if (
        IC_Item_GiveArray(playerIndex, MenuItem[MenuItem_Items])
        && MenuItem[MenuItem_UseCounter]
    ) {
        gUserLeftItems[playerIndex]--;

        if (Menu[WeaponMenu_Count]) {
            KeyValueCounter_Inc(g_tUserMenuItemsCounter[playerIndex], IntToStr(MenuId));
        }
    }

    if (
        PCGet_Bool(p, "StayOpen", false)
        && (
            !PCGet_Bool(p, "StayOpen_CheckCounter", true)
            || iItemsLeft != 0
        )
    ) {
        client_cmd(playerIndex, "%s %d", VIPM_M_WEAPONMENU_CMD_MENU, MenuId);
    }
}

GetUserLeftItems(const playerIndex, const MenuId, const Menu[S_WeaponMenu]) {
    new iUserItemsLeft = gUserLeftItems[playerIndex];
    new iMenuItemsLeft = Menu[WeaponMenu_Count] - KeyValueCounter_Get(g_tUserMenuItemsCounter[playerIndex], IntToStr(MenuId));
    
    if (iUserItemsLeft < 0) {
        return iMenuItemsLeft;
    }

    if (Menu[WeaponMenu_Count] < 0) {
        return iUserItemsLeft;
    }
    
    return min(iUserItemsLeft, iMenuItemsLeft);
}

#include "VipM/WeaponMenu/Natives"
