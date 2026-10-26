#include <amxmodx>
#include <reapi>
#include <json>
#include <ItemsController>

DefaultObjects_ItemType_DefuseKit_Register() {
    IC_ItemType_SimpleRegister(
        .name = "DefuseKit",
        .onGive = "@OnDefuseKitGive"
    );
}

@OnDefuseKitGive(const playerIndex, const Trie:p) {
    if (get_member(playerIndex, m_iTeam) == TEAM_CT) {
        rg_give_defusekit(playerIndex);
    }
}
