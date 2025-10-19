#include amxmodx
#include VipModular
#include "VipM/Utils"
#include "VipM/ArrayTrieUtils"
#include "VipM/DebugMode"

#include "VipM/WeaponMenu/Objects/WeaponMenu"

Menu_GetUserMenusCount(const UserId) {
    return ArraySizeSafe(VipM_Params_GetArr(VipM_Modules_GetParams(MODULE_NAME, UserId), "Menus"));
}

Menu_MainMenu(const UserId, const MainMenuTitle[], const Array:aMenus) {
    new iMenu = menu_create(MainMenuTitle, "@MenuHandler_Command");

    ArrayForeachArray2 (aMenus: i => Menu[S_WeaponMenu]) {
        menu_additem(iMenu, Menu[WeaponMenu_Name], fmt("%s %d", VIPM_M_WEAPONMENU_CMD_MENU, __i__));
    }

    menu_setprop(iMenu, MPROP_BACKNAME, Lang("VIPM_M_WM_MENU_BACK"));
    menu_setprop(iMenu, MPROP_NEXTNAME, Lang("VIPM_M_WM_MENU_NEXT"));
    menu_setprop(iMenu, MPROP_EXITNAME, Lang("VIPM_M_WM_MENU_EXIT"));

    menu_display(UserId, iMenu);
}

Menu_WeaponsMenu(const UserId, const MenuId, const Menu[S_WeaponMenu]) {
    new iItemsLeft = GetUserLeftItems(UserId, MenuId, Menu);
    new iMenusCount = Menu_GetUserMenusCount(UserId);

    static sItemsLeft[128] = "";
    if (iItemsLeft >= 0) {
        formatex(sItemsLeft, charsmax(sItemsLeft), "\w%L^n", UserId, "VIPM_M_WM_MENU_MAIN_ITEMS_LEFT", iItemsLeft);
    } else {
        sItemsLeft[0] = 0;
    }

    static sExpireStatus[128];
    if (gUserExpireStatus[UserId][0]) {
        formatex(sExpireStatus, charsmax(sExpireStatus), "\w%L^n", UserId, "VIPM_M_WM_MENU_EXPIRE_STATUS", gUserExpireStatus[UserId]);
    } else {
        sExpireStatus[0] = 0;
    }

    static sMenu[512];
    formatex(sMenu, charsmax(sMenu), "%s^n%s%s", Menu[WeaponMenu_Title], sExpireStatus, sItemsLeft);

    new iMenu = menu_create(
        sMenu,
        Menu[WeaponMenu_BackOnExit] && iMenusCount > 1
            ? "@MenuHandler_Weapons"
            : "@MenuHandler_Command"
    );

    if (Menu[WeaponMenu_BackOnExit] && iMenusCount > 1) {
        menu_setprop(iMenu, MPROP_EXITNAME, uLang(UserId, "VIPM_M_WM_MENU_WEAPONS_BACK_TO_MAIN_TEXT"));
    }

    if (Menu[WeaponMenu_PerPage] >= 0) {
        menu_setprop(iMenu, MPROP_PERPAGE, Menu[WeaponMenu_PerPage]);
    }

    menu_setprop(iMenu, MPROP_SHOWPAGE, Menu[WeaponMenu_ShowPage]);

    menu_setprop(iMenu, MPROP_BACKNAME, Lang("VIPM_M_WM_MENU_BACK"));
    menu_setprop(iMenu, MPROP_NEXTNAME, Lang("VIPM_M_WM_MENU_NEXT"));
    menu_setprop(iMenu, MPROP_EXITNAME, Lang("VIPM_M_WM_MENU_EXIT"));

    static iInactiveItemHandler;
    if (!iInactiveItemHandler) {
        iInactiveItemHandler = menu_makecallback("@MenuItemHandler_Inactive");
    }

    new iItemIndex = 0;
    ArrayForeachArray2 (Menu[WeaponMenu_Items]: i => MenuItem[S_MenuItem]) {
        if (
            MenuItem[MenuItem_ShowLimits] != Invalid_Array
            && !VipM_Limits_ExecuteList(MenuItem[MenuItem_ShowLimits], UserId, Limit_Exec_AND)
        ) {
            continue;
        }

        switch (MenuItem[MenuItem_Type]) {
            case MenuItemType_Default: {
                new bool:bIsActive = MenuItem[MenuItem_ShowLimits] == Invalid_Array
                    || VipM_Limits_ExecuteList(MenuItem[MenuItem_ActiveLimits], UserId, Limit_Exec_AND);

                menu_additem(iMenu, fmt("%s%s", bIsActive ? "" : "\d", MenuItem[MenuItem_Title]), fmt("%s %d %d", VIPM_M_WEAPONMENU_CMD_MENU, MenuId, __i__));
                if (!bIsActive && !MenuItem[MenuItem_FakeInactive]) {
                    menu_item_setcall(iMenu, iItemIndex, iInactiveItemHandler);
                }
            }
            
            case MenuItemType_Text: {
                menu_addtext2(iMenu, MenuItem[MenuItem_Title]);
            }

            case MenuItemType_Blank: {
                menu_addblank2(iMenu);
            }
        }
        iItemIndex++;
    }
    menu_display(UserId, iMenu);
}

@MenuItemHandler_Inactive() {
    return ITEM_DISABLED;
}

@MenuHandler_Weapons(const UserId, const MenuId, const ItemId) {
    @MenuHandler_Command(UserId, MenuId, ItemId);
    
    if (ItemId == MENU_EXIT) {
        client_cmd(UserId, "%s", VIPM_M_WEAPONMENU_CMD_MENU);
    }
}

@MenuHandler_Command(const UserId, const MenuId, const ItemId){
    AbortAutoCloseMenu(UserId);

    if (ItemId == MENU_EXIT) {
        menu_destroy(MenuId);
        return;
    }

    static sCmd[128];
    menu_item_getinfo(MenuId, ItemId, _, sCmd, charsmax(sCmd));

    if (sCmd[0]) {
        client_cmd(UserId, sCmd);
    }

    menu_destroy(MenuId);
}