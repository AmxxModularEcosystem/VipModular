#if defined __vipm_weaponmenu_objects_weaponmenu_included
    #endinput
#endif
#define __vipm_weaponmenu_objects_weaponmenu_included

#include <amxmodx>
#include <json>
#include <VipModular>
#include <ParamsController>
#include <VipM/L/Counter>

#include "VipM/WeaponMenu/Objects/MenuItem"

enum _:S_WeaponMenu {
    WeaponMenu_Name[64],
    WeaponMenu_Title[128],

    Array:WeaponMenu_Items, // S_MenuItem[]
    WeaponMenu_Count,
    VipM_L_Counter_Type:WeaponMenu_CounterType,
    WeaponMenu_CounterKey[VIPM_L_COUNTER_KEY_MAX_LEN],

    Array:WeaponMenu_Limits, // T_LimitUnit[]

    bool:WeaponMenu_BackOnExit,
    WeaponMenu_PerPage,
    bool:WeaponMenu_ShowPage,
    
    WeaponMenu_FakeMessage[256],
}

static WeaponMenuCounterKeysCount = 0;

bool:WeaponMenu_Read(const JSON:menuJson, menuObject[S_WeaponMenu]) {
    PCSingle_ObjString(menuJson, "Name", menuObject[WeaponMenu_Name], charsmax(menuObject[WeaponMenu_Name]));

    if (PCSingle_ObjChatMessage(
        menuJson, "FakeMessage",
        menuObject[WeaponMenu_FakeMessage], charsmax(menuObject[WeaponMenu_FakeMessage])
    )) {
        return true;
    }
    
    menuObject[WeaponMenu_Items] = Json_Object_GetMenuItemsList(menuJson, "Items");
    if (menuObject[WeaponMenu_Items] == Invalid_Array) {
        PCJson_LogForFile(menuJson, "WARNING", "menuObject items list are empty.");
        return false;
    }

    PCSingle_ObjString(menuJson, "Title", menuObject[WeaponMenu_Title], charsmax(menuObject[WeaponMenu_Title]), menuObject[WeaponMenu_Name]);

    menuObject[WeaponMenu_BackOnExit] = PCSingle_ObjBool(menuJson, "BackOnExit", false);
    menuObject[WeaponMenu_PerPage] = PCSingle_ObjInt(menuJson, "PerPage", -1);
    menuObject[WeaponMenu_ShowPage] = PCSingle_ObjBool(menuJson, "ShowPage", true);
    menuObject[WeaponMenu_Limits] = PCSingle_ObjVipmLimits(menuJson, "Limits");

    menuObject[WeaponMenu_Count] = PCSingle_ObjInt(menuJson, "Count", -1);
    menuObject[WeaponMenu_CounterType] = PCSingle_ObjVipmCounterType(menuJson, "CounterType");
    if (!PCSingle_ObjShortString(menuJson, "CounterKey", menuObject[WeaponMenu_CounterKey], charsmax(menuObject[WeaponMenu_CounterKey]))) {
        formatex(menuObject[WeaponMenu_CounterKey], charsmax(menuObject[WeaponMenu_CounterKey]), "%s-%d", VIPM_M_WEAPONMENU_MENU_COUNTER_KEY_PREFIX, ++WeaponMenuCounterKeysCount);
    }

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

    PCJson_FreeLinked(linkedJson, jWeaponMenus);
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
