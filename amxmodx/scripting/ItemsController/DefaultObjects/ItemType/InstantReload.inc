#include <amxmodx>
#include <reapi>
#include <json>
#include <ItemsController>

DefaultObjects_ItemType_InstantReload_Register() {
    IC_ItemType_SimpleRegister(
        .name = "InstantReload",
        .onGive = "@OnInstantReloadGive"
    );
}

@OnInstantReloadGive(const playerIndex, const Trie:p) {
    InstantReloadActiveWeapon(playerIndex);
}
