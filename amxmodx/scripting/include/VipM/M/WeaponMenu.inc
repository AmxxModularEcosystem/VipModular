#if defined _vipmodular_m_weaponmenu_included
	#endinput
#endif
#define _vipmodular_m_weaponmenu_included

#define VIPM_M_WEAPONMENU_EXPIRE_STATUS_MAX_LEN 64

stock const VIPM_M_WEAPONMENU_CMD_MENU[] = "vipm_m_weaponmenu_menu";
stock const VIPM_M_WEAPONMENU_CMD_MENU_SILENT[] = "vipm_m_weaponmenu_menu_silent";
stock const VIPM_M_WEAPONMENU_CMD_AUTOOPEN_TOGGLE[] = "vipm_m_weaponmenu_autoopen_toggle";

stock const VIPM_M_WEAPONMENU_PLAYER_COUNTER_KEY[] = "VipM-M-WeaponMenu-Player";
stock const VIPM_M_WEAPONMENU_MENU_COUNTER_KEY_PREFIX[] = "VipM-M-WeaponMenu-Menu";

/**
 * Устанавливает статус окончания привилегий, выводимый игроку в оружейном меню.
 * 
 * @param UserId        Индекс игрока, которому будет установлен статус.
 * @param sNewStatus    Устанавливаемый статус.
 * 
 * @note    При установке пустого статуса, соответствующий пункт в меню будет скрыт.
 * @note    По умолчанию статус пустой.
 * 
 * @noreturn
 */
native VipM_WeaponMenu_SetExpireStatus(const UserId, const sNewStatus[]);