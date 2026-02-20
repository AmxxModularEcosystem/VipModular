#if defined __vipm_weaponmenu_objects_menuitem_included
    #endinput
#endif
#define __vipm_weaponmenu_objects_menuitem_included

#include <amxmodx>
#include <VipModular>
#include <ParamsController>

enum MenuItemType {
    MenuItemType_Default = 0,
    MenuItemType_Text,
    MenuItemType_Blank,
}

enum _:S_MenuItem {
    MenuItemType:MenuItem_Type,

    MenuItem_Title[64],
    MenuItem_InactiveTitle[64],
    Array:MenuItem_Items, // T_IC_Item[]
    bool:MenuItem_UseCounter,
    bool:MenuItem_FakeInactive,
    
    Array:MenuItem_ShowLimits, // T_LimitUnit[]
    Array:MenuItem_ActiveLimits, // T_LimitUnit[]
    Array:MenuItem_Limits, // T_LimitUnit[]
}

bool:MenuItem_Read(const JSON:itemJson, item[S_MenuItem]) {
    if (!json_is_object(itemJson)) {
        PCJson_LogForFile(itemJson, "WARNING", "Invalid menu item format.");
        return false;
    }

    if (!PCSingle_ObjShortString(itemJson, "Title", item[MenuItem_Title], charsmax(item[MenuItem_Title]))) {
        item[MenuItem_Type] = MenuItemType_Blank;
        return true;
    }
    
    item[MenuItem_ShowLimits] = PCSingle_ObjVipmLimits(itemJson, "ShowLimits");
    item[MenuItem_ActiveLimits] = PCSingle_ObjVipmLimits(itemJson, "ActiveLimits");
    
    if (!PCSingle_ObjShortString(itemJson, "InactiveTitle", item[MenuItem_InactiveTitle], charsmax(item[MenuItem_InactiveTitle]))) {
        formatex(item[MenuItem_InactiveTitle], charsmax(item[MenuItem_InactiveTitle]), "\d%s", item[MenuItem_Title]);
    }

    item[MenuItem_Items] = PCSingle_ObjIcItems(itemJson, "Items");
    if (item[MenuItem_Items] == Invalid_Array) {
        item[MenuItem_Type] = MenuItemType_Text;
        return true;
    }
    
    item[MenuItem_Type] = MenuItemType_Default;
    item[MenuItem_Limits] = PCSingle_ObjVipmLimits(itemJson, "Limits");
    item[MenuItem_FakeInactive] = PCSingle_ObjBool(itemJson, "FakeInactive", false);
    item[MenuItem_UseCounter] = PCSingle_ObjBool(itemJson, "UseCounter", true);

    return true;
}

Array:MenuItem_ReadList(const JSON:itemsJson, &Array:items = Invalid_Array) {
    ArrayCreateIfNotCreated(items, S_MenuItem);
    
    new JSON:linkedJson;
    PCJson_HandleLinkedValue(itemsJson, linkedJson);

    switch (json_get_type(linkedJson)) {
        case JSONObject: {
            new MenuItem[S_MenuItem];
            if (MenuItem_Read(linkedJson, MenuItem)) {
                ArrayPushArray(items, MenuItem);
            }
        }
        case JSONArray: {
            json_array_foreach_value (linkedJson: i => itemJson) {
                MenuItem_ReadList(itemJson, items);
                json_free(itemJson);
            }
        }
        default: {
            PCJson_LogForFile(linkedJson, "WARNING", "Invalid menu items format.");
        }
    }

    PCJson_FreeLinked(linkedJson, itemsJson);
    return items;
}

Array:Json_Object_GetMenuItemsList(
    const JSON:jObj,
    const sKey[],
    &Array:aMenuItems = Invalid_Array,
    const bool:bDotNot = false
) {
    if (!json_object_has_value(jObj, sKey, .dot_not = bDotNot)) {
        return Invalid_Array;
    }

    new JSON:jMenuItems = json_object_get_value(jObj, sKey, bDotNot);
    aMenuItems = MenuItem_ReadList(jMenuItems, aMenuItems);
    json_free(jMenuItems);

    return aMenuItems;
}
