#include <ItemsController>

DefaultObjects_HandleNativeFilter(const name[], const trap) {
    new handled = false;
    
    if (DefaultObjects_ItemType_CustomWeapon_NativeFilter(name, trap)) {
        handled = true;
    }

    if (DefaultObjects_ItemType_If_NativeFilter(name, trap)) {
        handled = true;
    }

    return handled;
}

#include "ItemsController/DefaultObjects/ItemType/Armor"
#include "ItemsController/DefaultObjects/ItemType/Command"
#include "ItemsController/DefaultObjects/ItemType/DamageMult"
#include "ItemsController/DefaultObjects/ItemType/DefuseKit"
#include "ItemsController/DefaultObjects/ItemType/Health"
#include "ItemsController/DefaultObjects/ItemType/InstantReload"
#include "ItemsController/DefaultObjects/ItemType/InstantReloadAllWeapons"
#include "ItemsController/DefaultObjects/ItemType/ItemsList"
#include "ItemsController/DefaultObjects/ItemType/Money"
#include "ItemsController/DefaultObjects/ItemType/Random"
#include "ItemsController/DefaultObjects/ItemType/RefillBpAmmo"
#include "ItemsController/DefaultObjects/ItemType/Speed"
#include "ItemsController/DefaultObjects/ItemType/Weapon"
#include "ItemsController/DefaultObjects/ItemType/CustomWeapon"
#include "ItemsController/DefaultObjects/ItemType/Function"
#include "ItemsController/DefaultObjects/ItemType/If"

public IC_ItemType_OnInited() {
    // Затычка для тех мест, где при отсутствии предметов вылезает ошибка/варн
    IC_ItemType_SimpleRegister(.name = "None");

    DefaultObjects_ItemType_Armor_Register();
    DefaultObjects_ItemType_Command_Register();
    DefaultObjects_ItemType_DamageMult_Register();
    DefaultObjects_ItemType_DefuseKit_Register();
    DefaultObjects_ItemType_Health_Register();
    DefaultObjects_ItemType_InstantReload_Register();
    DefaultObjects_ItemType_InstantReloadAllWeapons_Register();
    DefaultObjects_ItemType_ItemsList_Register();
    DefaultObjects_ItemType_Money_Register();
    DefaultObjects_ItemType_Random_Register();
    DefaultObjects_ItemType_RefillBpAmmo_Register();
    DefaultObjects_ItemType_Speed_Register();
    DefaultObjects_ItemType_Weapon_Register();
    DefaultObjects_ItemType_CustomWeapon_Register();
    DefaultObjects_ItemType_Function_Register();
    DefaultObjects_ItemType_If_Register();
}

#include "ItemsController/DefaultObjects/ParamType/Item"
#include "ItemsController/DefaultObjects/ParamType/Items"

public ParamsController_OnRegisterTypes() {
    DefaultObjects_ParamType_Item_Register();
    DefaultObjects_ParamType_Items_Register();
}