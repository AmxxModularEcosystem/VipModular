#if defined __items_controller_included
	#endinput
#endif
#define __items_controller_included

#include <amxmodx>
#include <json>
#include <ParamsController>

#define IC_VERSION "1.0.0-rc4f2"
stock const IC_LIBRARY[] = "items-controller";
stock const IC_VERSION_CVAR[] = "ic_version";

stock const IC_PARAM_TYPE_ITEM_NAME[] = "IC-Item";
stock const IC_PARAM_TYPE_ITEMS_NAME[] = "IC-Items";

#define IC_ITEM_TYPE_NAME_MAX_LEN 64
#define IC_EVENT_LISTENER_FUNCTION_NAME_MAX_LEN 64

#define IC_RET_READ_SUCCESS 0
#define IC_RET_READ_FAIL 1

#define IC_RET_GIVE_SUCCESS 0
#define IC_RET_GIVE_FAIL 1

enum T_IC_ItemType { Invalid_IC_ItemType = -1 }
enum T_IC_Item {Invalid_IC_Item = -1}

// Deprecated names!
#define VipM_IC_T_Item T_IC_Item
#define VipM_IC_Invalid_Item Invalid_IC_Item
#define VIPM_IC_ITEM_TYPE_NAME_MAX_LEN IC_ITEM_TYPE_NAME_MAX_LEN

enum E_ItemTypeEvent{
    /*
     * Описание:    Вызывается, когда читаются параметры предмета.
     * Возв. тип:   IC_RET_READ_*
     * Параметры:   (const JSON:instanceJson, Trie:params):
     *      instanceJson - JSON-обьект с параметрами предмета.
     *      params - key-value хранилище для записи прочитанных параметров.
     * 
     * Примечание: Вызывается в момент чтения оружия нативом IC_Item_ReadFromJson. Скорее всего в plugin_precache.
     * Примечание: Если возвращено IC_RET_READ_FAIL, предмет будет пропущен.
     */
    ItemType_OnRead,

    /*
     * Описание:    Вызывается, когда предмет выдаётся игроку (При вызове натива IC_Item_Give).
     * Возв. тип:   IC_RET_GIVE_*
     * Параметры:   (const playerIndex, const Trie:params):
     *      playerIndex - Индекс игрока, которому выдаётся предмет.
     *      params - key-value хранилище параметров.
     * 
     * Примечание: Если возвращено IC_RET_GIVE_FAIL, предмет не будет считаться выданным (Натив IC_Item_Give вернёт false).
     */
    ItemType_OnGive,

    /*
     * Описание:    Вызывается, при освобождении хендлера предмета.
     * Возв. тип:   void
     * Параметры:   (const T_IC_Item:item, const Trie:p):
     *      item - Хендлер предмета.
     *      p - key-value хранилище параметров.
     * 
     * // TODO
     */
    ItemType_OnFree,
}

/**
 * Вызывается после инициализации компонента типов предметов.
 * После вызова этого форварда можно регистрировать типы предметов.
 *
 * @note Вызывается не позже plugin_precache.
 *
 * @noreturn
 */
forward IC_ItemType_OnInited();

/**
 * Вызывается после инициализации компонента предметов.
 * После вызова этого форварда можно создавать экземпляры предметов (в т.ч. читать их из JSON).
 *
 * @note Вызывается не позже plugin_precache.
 *
 * @noreturn
 */
forward IC_Item_OnInited();

/**
 * Принудительно инициализирует контроллер предметов.
 *
 * @noreturn
 */
native IC_Init();

/**
 * Регистрирует новый тип предмета.
 *
 * @param name Название типа.
 *
 * @return Хендлер зарегистрированного типа.
 */
native T_IC_ItemType:IC_ItemType_Register(const name[]);

/**
 * Регистрирует новый тип предмета.
 *
 * @param type         Хендлер типа.
 * @param event        Событие, для которого устанавливается обработчик.
 * @param functionName Название функции-обработчика.
 *
 * @noreturn
 */
native IC_ItemType_SetEventListener(const T_IC_ItemType:type, const E_ItemTypeEvent:event, const functionName[]);

/**
 * Добавляет параметры для типа предмета.
 *
 * @param type      Хендлер типа.
 * @param any:...   Перечисление параметров.
 * 
 * @note            Формат перечисления параметров:
 *                      IC_ItemType_AddParams(..., const paramName[], const paramType[], const bool:paramRequired, ...);
 * 
 *                  Где:
 *                  - paramName - Название параметра;
 *                  - paramType - Название типа параметра;
 *                  - paramRequired - Обязателен ли параметр.
 * 
 *                  Пример:
 *                      IC_ItemType_AddParams(type,
 *                          "Param1", "String", true,
 *                          "Param2", "Integer", false
 *                      );
 * 
 * @noreturn
 */
native IC_ItemType_AddParams(const T_IC_ItemType:type, any:...);

/**
 * Регистрирует новый тип предмета с обработчиками событий.
 *
 * @param name   Название типа.
 * @param onRead Название функции-обработчика для события ItemType_OnRead.
 * @param onGive Название функции-обработчика для события ItemType_OnGive.
 *
 * @return Хендлер зарегистрированного типа.
 */
stock T_IC_ItemType:IC_ItemType_SimpleRegister(const name[], const onRead[] = "", const onGive[] = "") {
    new T_IC_ItemType:type = IC_ItemType_Register(name);

    if (onRead[0] != EOS) {
        IC_ItemType_SetEventListener(type, ItemType_OnRead, onRead);
    }

    if (onGive[0] != EOS) {
        IC_ItemType_SetEventListener(type, ItemType_OnGive, onGive);
    }

    return type;
}

/**
 * Читает предмет из JSON-значения.
 *
 * @param isntanceJson JSON-значение, содержажее предмет или ссылку на него.
 *
 * @return Хендлер прочитанного экземпляра предмета. Invalid_IC_Item в случае ошибки.
 */
native T_IC_Item:IC_Item_ReadFromJson(const JSON:isntanceJson);

/**
 * Читает массив предметов из JSON-значения.
 *
 * @param isntancesJson JSON-значение, содержажее массив предметов, один предмет или ссылку.
 * @param array         Хендлер динамического массива, в который нужно дописать прочитанные предметы.
 *
 * @return Хендлер динамического массива с прочитанными предметами. Если был передан параметр array, то вернётся то же значение.
 */
native Array:IC_Item_ReadArrayFromJson(const JSON:isntancesJson, &Array:array = Invalid_Array);

#pragma deprecated Use PCSingle_ObjIcItems() instead.
stock Array:Json_Object_IC_GetItems(const JSON:jObj, const sKey[], const bool:bDotNot = false) {
    return PCSingle_ObjIcItems(jObj, sKey, .dotNot = bDotNot);
}

/**
 * Выдача предмета игроку.
 *
 * @param playerIndex Индекс игрока.
 * @param item        Хендлер экземпляра предмета.
 *
 * @return true, если предмет успешно выдан, иначе false.
 */
native bool:IC_Item_Give(const playerIndex, const T_IC_Item:item);

/**
 * Удаление экземпляра предмета из памяти (освобождение хендлера).
 * 
 * @note На данный момент может вызывать утечку памяти при некоторых обстоятельствах.
 *       Если в параметрах предмета находятся хендлеры каких-либо хранилищ (Array, Trie и т.п.), они не будут очищены.
 *       В будущем планируется решить это через ParamsController, но там пока нет нужного функционала.
 *
 * @param item        Хендлер экземпляра предмета. Значение будет изменено на Invalid_IC_Item.
 *
 * @return Всегда Invalid_IC_Item.
 */
native T_IC_Item:IC_Item_Free(&T_IC_Item:item);

/**
 * Выдача игроку всех предметов из массива.
 *
 * @param playerIndex Индекс игрока.
 * @param array       Хендлер динамического массива с хендлерами экземпляров предметов.
 *
 * @return true, если хотя бы один предмет успешно выдан, иначе false.
 */
stock bool:IC_Item_GiveArray(const playerIndex, const Array:array) {
    if (array == Invalid_Array) {
        return false;
    }

    new bool:success = false;
    for (new i = 0; i < ArraySize(array); i++) {
        if (IC_Item_Give(playerIndex, ArrayGetCell(array, i))) {
            success = true;
        }
    }

    return success;
}

/**
 * Вызывается в момент инициалзиации типов предметов.
 *
 * @note Вызывается не позже plugin_precache.
 *
 * @noreturn
 * 
 * @deprecated Use IC_ItemType_OnInited() instead.
 */
#pragma deprecated Use IC_ItemType_OnInited() instead.
forward VipM_IC_OnInitTypes();

/**
 * Инициализирует контроллер предметов.
 *
 * @noreturn
 * 
 * @deprecated Use IC_Init() instead.
 */
#pragma deprecated Use IC_Init() instead.
native VipM_IC_Init();

/**
 * Регистрирует новый тип предмета.
 *
 * @param Type    Название типа.
 *
 * @return Индекс зарегистрированного типа
 * 
 * @deprecated Use IC_ItemType_Register() instead.
 */
#pragma deprecated Use IC_ItemType_Register() instead.
native VipM_IC_RegisterType(const Type[]);

/**
 * Регистрирует обработчик события для указанного типа предмета.
 *
 * @param Type      Название типа.
 * @param Event     Событие.
 * @param Func      Название функции-обработчика.
 *
 * @return  Вернёт true, если обработчик успешно зарегистрирован.
 * 
 * @deprecated Use IC_ItemType_SetEventListener() instead.
 */
#pragma deprecated Use IC_ItemType_SetEventListener() instead.
native VipM_IC_RegisterTypeEvent(const Type[], const E_ItemTypeEvent:Event, const Func[]);

/**
 * Загрузка предмета из JSON-обьекта.
 *
 * @param jItem     JSON-объект, содержащий информацию о предмете.
 *
 * @note    После загрузки предмета, JSON-объект уничтожается.
 * @note    Структура объекта:
 *              {
 *                  "Item": "НазваниеТипа",
 *                  "НазваниеПараметра1": "ЗначениеПараметра1",
 *                  "НазваниеПараметра2": "ЗначениеПараметра2",
 *                  ...
 *              }
 *
 * @return  Индекс загруженного предмета. Invalid_IC_Item, если что-то пошло не так.
 * 
 * @deprecated Use IC_Item_ReadFromJson() instead.
 */
#pragma deprecated Use IC_Item_ReadFromJson() instead.
native VipM_IC_T_Item:VipM_IC_JsonGetItem(&JSON:jItem);

/**
 * Выдача предмета игроку.
 *
 * @param UserId     Индекс игрока.
 * @param ItemId     Индекс предмета, возвращённый нативом VipM_IC_JsonGetItem.
 *
 * @return  true, если предмет успешно выдан, иначе false.
 * 
 * @deprecated Use IC_Item_Give() instead.
 */
#pragma deprecated Use IC_Item_Give() instead.
native bool:VipM_IC_GiveItem(const UserId, const VipM_IC_T_Item:ItemId);

#pragma deprecated Use IC_Item_ReadArrayFromJson() instead.
stock Array:VipM_IC_JsonGetItems(JSON:jItems) {
    // TODO: Не удалять хендлер (то же относится и к нативу)

    new Array:aItems = Invalid_Array;
    if (jItems == Invalid_JSON) {
        return aItems;
    }

    switch (json_get_type(jItems)) {
        case JSONArray: {
            new cItems = json_array_get_count(jItems);
            if (cItems < 1) {
                return Invalid_Array;
            }

            aItems = ArrayCreate(1, cItems);
            for (new i = 0; i < cItems; i++) {
                new JSON:jItem = json_array_get_value(jItems, i);
                new VipM_IC_T_Item:ItemId = VipM_IC_JsonGetItem(jItem);
                if (ItemId != Invalid_IC_Item) {
                    ArrayPushCell(aItems, ItemId);
                }
            }
            json_free(jItems);
        }

        case JSONObject, JSONString: {
            new VipM_IC_T_Item:ItemId = VipM_IC_JsonGetItem(jItems);
            if (ItemId == Invalid_IC_Item) {
                return Invalid_Array;
            }

            aItems = ArrayCreate(1, 1);
            ArrayPushCell(aItems, ItemId);
        }
    }
    return aItems;
}

#pragma deprecated Use IC_Item_GiveArray() instead.
stock bool:VipM_IC_GiveItems(const UserId, const Array:aItems) {
    if (aItems == Invalid_Array) {
        return false;
    }

    new bool:bRes = false;
    for (new i = 0; i < ArraySize(aItems); i++) {
        if (VipM_IC_GiveItem(UserId, VipM_IC_T_Item:ArrayGetCell(aItems, i))) {
            bRes = true;
        }
    }

    return bRes;
}

stock bool:VipM_Params_GiveItems(const UserId, const Trie:tParams, const sParam[]) {
    return IC_Item_GiveArray(UserId, VipM_Params_GetArr(tParams, sParam));
}

stock bool:VipM_Params_GiveItem(const UserId, const Trie:tParams, const sParam[]) {
    return IC_Item_Give(UserId, PCGet_Cell(tParams, sParam));
}

#pragma deprecated Use Json_Object_IC_GetItems() instead.
stock Array:Json_Object_GetItemsIC(const JSON:jObj, const sKey[], const bool:bDotNot = false) {
    if (!json_object_has_value(jObj, sKey, .dot_not = bDotNot)) {
        return Invalid_Array;
    }

    new JSON:jItems = json_object_get_value(jObj, sKey, bDotNot);
    new Array:aItems = VipM_IC_JsonGetItems(jItems);
    json_free(jItems);

    return aItems;
}

stock T_IC_Item:PCSingle_ObjIcItem(const JSON:objectJson, const key[], const T_IC_Item:def = Invalid_IC_Item, const bool:dotNot = false, const bool:orFail = false) {
    return PCSingle_ObjCell(objectJson, key, IC_PARAM_TYPE_ITEM_NAME, def, dotNot, orFail);
}

stock Array:PCSingle_ObjIcItems(const JSON:objectJson, const key[], const Array:def = Invalid_Array, const bool:dotNot = false, const bool:orFail = false) {
    return PCSingle_ObjCell(objectJson, key, IC_PARAM_TYPE_ITEMS_NAME, def, dotNot, orFail);
}

stock T_IC_Item:PCGet_IcItem(const Trie:p, const key[], const T_IC_Item:def = Invalid_IC_Item) {
    return PCGet_Cell(p, key, def);
}

stock Array:PCGet_IcItems(const Trie:p, const key[], const Array:def = Invalid_Array) {
    return PCGet_Cell(p, key, def);
}

stock bool:PCGet_IcItemGive(const Trie:p, const key[], const playerIndex) {
    new T_IC_Item:item = PCGet_IcItem(p, key);
    return item != Invalid_IC_Item && IC_Item_Give(0playerIndex, item);
}

stock bool:PCGet_IcItemsGive(const Trie:p, const key[], const playerIndex) {
    return IC_Item_GiveArray(playerIndex, PCGet_IcItems(p, key));
}
