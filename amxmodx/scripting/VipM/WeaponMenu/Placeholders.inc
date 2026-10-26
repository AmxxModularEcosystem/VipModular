#if defined __vipm_weaponmenu_placeholders_included
    #endinput
#endif
#define __vipm_weaponmenu_placeholders_included

#include <amxmodx>
#include <ParamsController>

/**
 * Подставляет плейсхолдеры в строку с контекстом указанного игрока.
 *
 * Доступны плейсхолдеры глобальной группы (`{map}`, `{time}`, ...) и группы
 * игрока `p` (`{p:name}`, `{p:health}`, ...). Перед форматированием индекс игрока
 * кладётся на стек контекстов группы `p`, после — снимается.
 *
 * @param playerIndex   Индекс игрока, чей контекст подставляется в плейсхолдеры группы `p`.
 * @param out           Буфер для результирующей строки.
 * @param outLen        Размер буфера.
 * @param format        Исходная строка с плейсхолдерами.
 *
 * @noreturn
 */
Menu_FormatPlaceholders(const playerIndex, out[], const outLen, const format[]) {
    static T_PHGroup:playerGroup = Invalid_PHGroup;
    if (playerGroup == Invalid_PHGroup) {
        playerGroup = PCPH_FindGroup(DEFAULT_PH_PLAYER_GROUP_PREFIX);
    }

    if (playerGroup != Invalid_PHGroup) {
        PCPH_Group_PushIntContext(playerGroup, playerIndex);
    }

    PCPH_Format(out, outLen, format);

    if (playerGroup != Invalid_PHGroup) {
        PCPH_Group_PopContext(playerGroup);
    }
}
