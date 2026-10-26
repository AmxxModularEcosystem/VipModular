#include <amxmodx>
#include <reapi>
#include <json>
#include <ItemsController>

DefaultObjects_ItemType_InstantReloadAllWeapons_Register() {
    IC_ItemType_SimpleRegister(
        .name = "InstantReloadAllWeapons",
        .onGive = "@OnInstantReloadAllWeaponsGive"
    );
}

@OnInstantReloadAllWeaponsGive(const playerIndex, const Trie:p) {
    InstantReloadAllWeapons(playerIndex);
}
