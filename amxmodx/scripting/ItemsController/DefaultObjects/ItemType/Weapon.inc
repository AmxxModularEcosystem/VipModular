#include <amxmodx>
#include <reapi>
#include <json>
#include <ItemsController>

DefaultObjects_ItemType_Weapon_Register() {
    new T_IC_ItemType:type = IC_ItemType_SimpleRegister(
        .name = "Weapon",
        .onRead = "@OnWeaponRead",
        .onGive = "@OnWeaponGive"
    );
    IC_ItemType_AddParams(type,
        PCParam("Name", DEFAULT_PARAMS_STR_NAME, true),
        PCParam("BpAmmo", DEFAULT_PARAMS_INT_NAME)
    );
}

@OnWeaponRead(const JSON:instanceJson, const Trie:p) {
    TrieSetCell(p, "GiveType", _:Json_Object_GetGiveType(instanceJson, "GiveType"));

    return IC_RET_READ_SUCCESS;
}

@OnWeaponGive(const playerIndex, const Trie:p) {
    static WeaponName[32];
    PCGet_Str(p, "Name", WeaponName, charsmax(WeaponName));
    if (!WeaponName[0]) {
        return IC_RET_GIVE_FAIL;
    }
    
    if (!get_weaponid(WeaponName)) {
        log_amx("[WARNING] Default weapon `%s` not found.", WeaponName);
        return IC_RET_GIVE_FAIL;
    }
    
    new GiveType:iGiveType = GiveType:PCGet_Int(p, "GiveType", _:GT_DROP_AND_REPLACE);
    new iBpAmmo = PCGet_Int(p, "BpAmmo", -1);

    new ItemId = rg_give_item(playerIndex, WeaponName, iGiveType);
    if (ItemId < 0) {
        return IC_RET_GIVE_FAIL;
    }

    new WeaponIdType:iWpnId = rg_get_weapon_info(WeaponName, WI_ID);
    new iWpnSlot = rg_get_iteminfo(ItemId, ItemInfo_iSlot);

    if (
        iBpAmmo < 0
        && !(
            iWpnSlot == 0
            || iWpnSlot == 1
        )
    ) {
        return IC_RET_GIVE_SUCCESS;
    }

    new def_BpAmmo = (rg_get_weapon_info(iWpnId, WI_MAX_ROUNDS));
    if (def_BpAmmo >= 0) {
        rg_set_user_bpammo(playerIndex, iWpnId, iBpAmmo < 0 ? def_BpAmmo : iBpAmmo);
    }

    return IC_RET_GIVE_SUCCESS;
}

GiveType:Json_Object_GetGiveType(const JSON:Obj, const Key[], const bool:DotNot = false) {
    new Str[32];
    json_object_get_string(Obj, Key, Str, charsmax(Str), DotNot);
    return ItemType_Weapon_StrToGiveType(Str);
}

GiveType:ItemType_Weapon_StrToGiveType(const Str[]) {
    if (equali(Str, "GT_APPEND") || equali(Str, "Append") || equali(Str, "Add")) {
        return GT_APPEND;
    } else if (equali(Str, "GT_REPLACE") || equali(Str, "Replace")) {
        return GT_REPLACE;
    } else if (equali(Str, "GT_DROP_AND_REPLACE") || equali(Str, "Drop") || equali(Str, "DropAndReplace")) {
        return GT_DROP_AND_REPLACE;
    } else {
        return GT_DROP_AND_REPLACE;
    }
}
