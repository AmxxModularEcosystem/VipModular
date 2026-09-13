#if defined _vipmodular_l_counter_included
	#endinput
#endif
#define _vipmodular_l_counter_included

#include <ParamsController>

#define VIPM_L_COUNTER_KEY_MAX_LEN 64

new const VIPM_L_COUNTER_PARAM_TYPE[] = "VipM-L-CounterType";

enum VipM_L_Counter_Type {
    /**
     * Сбрасывается при спавне игрока
     */
    VipM_L_Counter_PerLife,

    /**
     * Сбрасывается в начале раунда
     */
    VipM_L_Counter_PerRound,
    
    /**
     * Сбрасывается при звходе игрока на сервер
     */
    VipM_L_Counter_PerSession,
    
    /**
     * Сбрасывается в начале игры (начало карты или sv_restart)
     */
    VipM_L_Counter_PerGame,
    
    /**
     * Сбрасывается в начале карты
     */
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
 * Инкрементация значения счётчика.
 * 
 * @param type Тип счётчика (см. VipM_L_Counter_Type).
 * @param key Ключ счётчика, уникальный для каждого типа.
 * @param playerIndex Индекс игрока.
 * @param add Добавляемое значение.
 * 
 * @return Новое значение счётчика.
 */
stock VipM_L_Counter_Inc(const VipM_L_Counter_Type:type, const key[], const playerIndex, const add = 1) {
    return VipM_L_Counter_Set(type, key, playerIndex, VipM_L_Counter_Get(type, key, playerIndex) + add);
}

stock VipM_L_Counter_Type:PCGet_VipmCounterType(const Trie:p, const key[], const VipM_L_Counter_Type:def = VipM_L_Counter_PerLife) {
    return PCGet_Cell(p, key, def);
}

stock VipM_L_Counter_Type:PCSingle_VipmCounterType(const JSON:valueJson, const VipM_L_Counter_Type:def = VipM_L_Counter_PerLife, const orFailKey[] = "") {
    return PCSingle_Cell(valueJson, VIPM_L_COUNTER_PARAM_TYPE, def, orFailKey);
}

stock VipM_L_Counter_Type:PCSingle_ObjVipmCounterType(const JSON:objectJson, const key[], const VipM_L_Counter_Type:def = VipM_L_Counter_PerLife, const bool:dotNot = false, const bool:orFail = false) {
    return PCSingle_ObjCell(objectJson, key, VIPM_L_COUNTER_PARAM_TYPE, def, dotNot, orFail);
}
