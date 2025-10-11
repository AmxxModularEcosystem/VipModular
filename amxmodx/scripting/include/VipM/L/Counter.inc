#if defined _vipmodular_l_counter_included
	#endinput
#endif
#define _vipmodular_l_counter_included

#define VIPM_L_COUNTER_KEY_MAX_LEN 64

new const VIPM_L_COUNTER_PARAM_TYPE[] = "VipM-L-CounterType";

enum VipM_L_Counter_Type {
    VipM_L_Counter_PerLife,
    VipM_L_Counter_PerRound,
    VipM_L_Counter_PerSession,
    VipM_L_Counter_PerGame,
    VipM_L_Counter_PerMap,
}

/**
 * Получение текущего значения счётчика.
 * 
 * @param type Тип счётчика (см. VipM_L_Counter_Type).
 * @param key Ключ счётчика, уникальный для каждого типа.
 * @param playerIndex Индекс игрока.
 * 
 * @return Текущее значение счётчика.
 */
native VipM_L_Counter_Get(const VipM_L_Counter_Type:type, const key[], const playerIndex);

/**
 * Перезапись текущего значения счётчика.
 * 
 * @param type Тип счётчика (см. VipM_L_Counter_Type).
 * @param key Ключ счётчика, уникальный для каждого типа.
 * @param playerIndex Индекс игрока.
 * @param value Новое значение счётчика.
 * 
 * @return Новое значение счётчика.
 */
native VipM_L_Counter_Set(const VipM_L_Counter_Type:type, const key[], const playerIndex, const value);

/**
 * Перезапись текущего значения счётчика.
 * 
 * @param type Тип счётчика (см. VipM_L_Counter_Type).
 * @param key Ключ счётчика, уникальный для каждого типа.
 * @param playerIndex Индекс игрока.
 * 
 * @return Новое значение счётчика.
 */
stock VipM_Counter_Inc(const VipM_L_Counter_Type:type, const key[], const playerIndex) {
    return VipM_L_Counter_Set(type, key, playerIndex, VipM_L_Counter_Get(type, key) + 1);
}
