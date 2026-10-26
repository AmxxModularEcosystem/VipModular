#include <amxmodx>
#include <reapi>
#include <json>
#include <ItemsController>

DefaultObjects_ItemType_RefillBpAmmo_Register() {
    IC_ItemType_SimpleRegister(
        .name = "RefillBpAmmo",
        .onGive = "@OnRefillBpAmmoGive"
    );
}

@OnRefillBpAmmoGive(const playerIndex, const Trie:p) {
    new iMaxAmmos[32] = {-1, ...};

    // Получение дефолтных значений
    for (new WeaponIdType:iWpnId = WEAPON_P228; iWpnId < WEAPON_P90; iWpnId++) {
        switch (iWpnId) {
            case WEAPON_KNIFE, WEAPON_HEGRENADE, WEAPON_SMOKEGRENADE, WEAPON_FLASHBANG, WEAPON_SHIELDGUN:
                iMaxAmmos[rg_get_weapon_info(iWpnId, WI_AMMO_TYPE)] = -1;
            default:
                iMaxAmmos[rg_get_weapon_info(iWpnId, WI_AMMO_TYPE)] = rg_get_weapon_info(iWpnId, WI_MAX_ROUNDS);
        }
    }

    // Получение актуальных значений исходя из пушек в инвентаре 
    for (new InventorySlotType:iSlot = PRIMARY_WEAPON_SLOT; iSlot <= PISTOL_SLOT; iSlot++) {
        new ItemId = get_member(playerIndex, m_rgpPlayerItems, iSlot);
        while (ItemId > 0 && !is_nullent(ItemId)) {
            new iAmmoType = get_member(ItemId, m_Weapon_iPrimaryAmmoType);
            if (iAmmoType >= 0) {
                iMaxAmmos[iAmmoType] = max(iMaxAmmos[iAmmoType], rg_get_iteminfo(ItemId, ItemInfo_iMaxAmmo1));
            }
            ItemId = get_member(ItemId, m_pNext);
        }
    }

    for (new iAmmoType = 0; iAmmoType < 32; iAmmoType++) {
        if (iMaxAmmos[iAmmoType] >= 0) {
            set_member(playerIndex, m_rgAmmo, iMaxAmmos[iAmmoType], iAmmoType);
        }
    }
}
