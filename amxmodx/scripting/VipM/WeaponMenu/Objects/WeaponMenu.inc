#if defined __vipm_weaponmenu_objects_weaponmenu_included
    #endinput
#endif
#define __vipm_weaponmenu_objects_weaponmenu_included

#include <amxmodx>
#include <json>
#include <VipModular>
#include <ParamsController>

#include "VipM/WeaponMenu/Objects/MenuItem"

enum _:S_WeaponMenu {
    WeaponMenu_Name[64],
    WeaponMenu_Title[128],

    Array:WeaponMenu_Items, // S_MenuItem[]
    WeaponMenu_Count,

    Array:WeaponMenu_Limits, // T_LimitUnit[]

    bool:WeaponMenu_BackOnExit,
    WeaponMenu_PerPage,
    bool:WeaponMenu_ShowPage,
    
    WeaponMenu_FakeMessage[256],
}

bool:WeaponMenu_Read(const JSON:menuJson, Menu[S_WeaponMenu]) {
    PCSingle_ObjString(menuJson, "Name", Menu[WeaponMenu_Name], charsmax(Menu[WeaponMenu_Name]));

    if (PCSingle_ObjChatMessage(
        menuJson, "FakeMessage",
        Menu[WeaponMenu_FakeMessage], charsmax(Menu[WeaponMenu_FakeMessage])
    )) {
        return true;
    }
    
    Menu[WeaponMenu_Items] = Json_Object_GetMenuItemsList(menuJson, "Items");
    if (Menu[WeaponMenu_Items] == Invalid_Array) {
        PCJson_LogForFile(menuJson, "WARNING", "Menu items list are empty.");
        return false;
    }

    PCSingle_ObjString(menuJson, "Title", Menu[WeaponMenu_Title], charsmax(Menu[WeaponMenu_Title]), Menu[WeaponMenu_Name]);

    Menu[WeaponMenu_Count] = PCSingle_ObjInt(menuJson, "Count", -1);
    Menu[WeaponMenu_BackOnExit] = PCSingle_ObjBool(menuJson, "BackOnExit", false);
    Menu[WeaponMenu_PerPage] = PCSingle_ObjInt(menuJson, "PerPage", -1);
    Menu[WeaponMenu_ShowPage] = PCSingle_ObjBool(menuJson, "ShowPage", true);
    Menu[WeaponMenu_Limits] = Json_Object_GetLimits(menuJson, "Limits");

    return true;
}

Array:WeaponMenu_ReadList(const JSON:jWeaponMenus, &Array:aWeaponMenus = Invalid_Array) {
    ArrayCreateIfNotCreated(aWeaponMenus, S_WeaponMenu);

    new JSON:linkedJson;
    PCJson_HandleLinkedValue(jWeaponMenus, linkedJson);

    switch (json_get_type(linkedJson)) {
        case JSONObject: {
            new WeaponMenu[S_WeaponMenu];
            if (WeaponMenu_Read(linkedJson, WeaponMenu)) {
                ArrayPushArray(aWeaponMenus, WeaponMenu);
            }
        }
        case JSONArray: {
            json_array_foreach_value (linkedJson: i => jWeaponMenu) {
                aWeaponMenus = WeaponMenu_ReadList(jWeaponMenu, aWeaponMenus);
                json_free(jWeaponMenu);
            }
        }
        default: {
            PCJson_LogForFile(linkedJson, "WARNING", "Invalid weapon menus format.");
        }
    }

    PCJson_FreeLinked(linkedJson);
    return aWeaponMenus;
}

Array:Json_Object_GetWeaponMenusList(
    const JSON:jObj,
    const sKey[],
    &Array:aWeaponMenus = Invalid_Array,
    const bool:bDotNot = false
) {
    if (!json_object_has_value(jObj, sKey, .dot_not = bDotNot)) {
        return Invalid_Array;
    }

    new JSON:jWeaponMenus = json_object_get_value(jObj, sKey, bDotNot);
    aWeaponMenus = WeaponMenu_ReadList(jWeaponMenus, aWeaponMenus);
    json_free(jWeaponMenus);

    return aWeaponMenus;
}
