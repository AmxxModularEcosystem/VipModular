#include <amxmodx>
#include <VipModular>
#include <VipM/L/Counter>

#include "VipM/Utils"
#include "VipM/ArrayTrieUtils"

#include "VipM/WeaponMenu/Objects/WeaponMenu"

Menu_GetUserMenusCount(const playerIndex) {
    return ArraySizeSafe(VipM_Params_GetArr(VipM_Modules_GetParams(MODULE_NAME, playerIndex), "Menus"));
}

Menu_MainMenu(const playerIndex, const MainMenuTitle[], const Array:menus) {
    new menuHandler = menu_create(MainMenuTitle, "@MenuHandler_Command");

    ArrayForeachArray2 (menus: i => menuObject[S_WeaponMenu]) {
        menu_additem(menuHandler, menuObject[WeaponMenu_Name], fmt("%s %d", VIPM_M_WEAPONMENU_CMD_MENU, i));
    }

    menu_setprop(menuHandler, MPROP_BACKNAME, Lang("VIPM_M_WM_MENU_BACK"));
    menu_setprop(menuHandler, MPROP_NEXTNAME, Lang("VIPM_M_WM_MENU_NEXT"));
    menu_setprop(menuHandler, MPROP_EXITNAME, Lang("VIPM_M_WM_MENU_EXIT"));

    menu_display(playerIndex, menuHandler);
}

Menu_WeaponsMenu(const playerIndex, const menuIndex, const menuObject[S_WeaponMenu]) {
    new iItemsLeft = GetUserLeftItems(playerIndex, menuObject);
    new iMenusCount = Menu_GetUserMenusCount(playerIndex);

    static sItemsLeft[128] = "";
    if (iItemsLeft >= 0) {
        formatex(sItemsLeft, charsmax(sItemsLeft), "\w%L^n", playerIndex, "VIPM_M_WM_MENU_MAIN_ITEMS_LEFT", iItemsLeft);
    } else {
        sItemsLeft[0] = 0;
    }

    static sExpireStatus[128];
    if (gUserExpireStatus[playerIndex][0]) {
        formatex(sExpireStatus, charsmax(sExpireStatus), "\w%L^n", playerIndex, "VIPM_M_WM_MENU_EXPIRE_STATUS", gUserExpireStatus[playerIndex]);
    } else {
        sExpireStatus[0] = 0;
    }

    static sMenu[512];
    formatex(sMenu, charsmax(sMenu), "%s^n%s%s", menuObject[WeaponMenu_Title], sExpireStatus, sItemsLeft);

    new menuHandler = menu_create(sMenu, "@MenuHandler_Command");

    if (menuObject[WeaponMenu_BackOnExit] && iMenusCount > 1) {
        menu_setprop(menuHandler, MPROP_EXITNAME, uLang(playerIndex, "VIPM_M_WM_MENU_WEAPONS_BACK_TO_MAIN_TEXT"));
    }

    if (menuObject[WeaponMenu_PerPage] >= 0) {
        menu_setprop(menuHandler, MPROP_PERPAGE, menuObject[WeaponMenu_PerPage]);
    }

    menu_setprop(menuHandler, MPROP_SHOWPAGE, menuObject[WeaponMenu_ShowPage]);

    menu_setprop(menuHandler, MPROP_BACKNAME, Lang("VIPM_M_WM_MENU_BACK"));
    menu_setprop(menuHandler, MPROP_NEXTNAME, Lang("VIPM_M_WM_MENU_NEXT"));
    menu_setprop(menuHandler, MPROP_EXITNAME, Lang("VIPM_M_WM_MENU_EXIT"));

    static iInactiveItemHandler;
    if (!iInactiveItemHandler) {
        iInactiveItemHandler = menu_makecallback("@MenuItemHandler_Inactive");
    }

    new itemIndex = 0;
    ArrayForeachArray2 (menuObject[WeaponMenu_Items]: i => itemObject[S_MenuItem]) {
        if (
            itemObject[MenuItem_ShowLimits] != Invalid_Array
            && !VipM_Limits_ExecuteList(itemObject[MenuItem_ShowLimits], playerIndex, Limit_Exec_AND)
        ) {
            continue;
        }
        
        new bool:isItemActive = (
            itemObject[MenuItem_ShowLimits] == Invalid_Array
            || VipM_Limits_ExecuteList(itemObject[MenuItem_ActiveLimits], playerIndex, Limit_Exec_AND)
        );

        static title[64];
        if (isItemActive) {
            copy(title, charsmax(title), itemObject[MenuItem_Title]);
        } else {
            copy(title, charsmax(title), itemObject[MenuItem_InactiveTitle]);
        }

        switch (itemObject[MenuItem_Type]) {
            case MenuItemType_Default: {
                static cmd[128];
                formatex(cmd, charsmax(cmd), "%s %d %d", VIPM_M_WEAPONMENU_CMD_MENU, menuIndex, i);
                if (menuObject[WeaponMenu_BackOnExit]) {
                    add(cmd, charsmax(cmd), fmt(";%s %d", VIPM_M_WEAPONMENU_CMD_MENU, menuIndex));
                }

                menu_additem(menuHandler, title, cmd);

                if (!isItemActive && !itemObject[MenuItem_FakeInactive]) {
                    menu_item_setcall(menuHandler, itemIndex, iInactiveItemHandler);
                }
            }
            
            case MenuItemType_Text: {
                menu_addtext2(menuHandler, itemObject[MenuItem_Title]);
            }

            case MenuItemType_Blank: {
                menu_addblank2(menuHandler);
            }
        }
        itemIndex++;
    }
    menu_display(playerIndex, menuHandler);
}

@MenuItemHandler_Inactive() {
    return ITEM_DISABLED;
}

@MenuHandler_Command(const playerIndex, const menuIndex, const ItemId){
    AbortAutoCloseMenu(playerIndex);

    if (ItemId == MENU_EXIT) {
        menu_destroy(menuIndex);
        return;
    }

    static sCmd[128];
    menu_item_getinfo(menuIndex, ItemId, _, sCmd, charsmax(sCmd));

    if (sCmd[0]) {
        client_cmd(playerIndex, sCmd);
    }

    menu_destroy(menuIndex);
}