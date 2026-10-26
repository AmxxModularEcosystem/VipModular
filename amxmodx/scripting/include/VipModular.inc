#if defined _vipmodular_included
	#endinput
#endif
#define _vipmodular_included

#include amxmodx
#include json

stock const VIPM_LIBRARY[] = "VipModular";
#define _VIPM_VERSION "5.0.0-rc4f2"
stock const VIPM_VERSION[] = _VIPM_VERSION;
#define _VIPM_PLUGIN_URL "https://github.com/AmxxModularEcosystem/VipModular"

// deprecated
stock const VIPM_CFG_PATH[] = "plugins/VipModular";
// new
stock const VIPM_CONFIGS_FOLDER_NAME[] = "VipModular";
stock const VIPM_VIPS_CONFIG_FILE_PATH[] = "Vips.json";
stock const VIPM_VIPS_CONFIG_DIR_PATH[] = "Vips";

/**
 * Получение пути, относительно папки с конфигами вип-системы или AmxModX.
 *
 * @note    Если название файла начинается со слеша `/`, файл будет взят из папки amxmodx/configs.
 *          Иначе из папки вип-системы (см. константу VIPM_CFG_PATH).
 *
 * @param Path      Путь / Название файла.
 * @param sOut      Буффер для записи полученного пути.
 * @param iOutLen   Размер буфера.
 * 
 * @deprecated Use PCPath_MakePath() from ParamsController instead
 *
 * @return  Кол-во записанных в буфер ячеек.
 */
#pragma deprecated Use PCPath_MakePath() from ParamsController instead
stock VipM_GetCfgPath(const sPath[] = "", sOut[], const iOutLen){
    static __amxx_configsdir[PLATFORM_MAX_PATH];
    if (!__amxx_configsdir[0]) {
        get_localinfo("amxx_configsdir", __amxx_configsdir, charsmax(__amxx_configsdir));
    }

    if (!sPath[0]) {
        return formatex(sOut, iOutLen, "%s/%s", __amxx_configsdir, VIPM_CFG_PATH);
    }
    
    return (sPath[0] == '/')
        ? formatex(sOut, iOutLen, "%s%s", __amxx_configsdir, sPath)
        : formatex(sOut, iOutLen, "%s/%s/%s", __amxx_configsdir, VIPM_CFG_PATH, sPath);
}

/**
 * Получение пути, относительно папки с конфигами вип-системы или AmxModX.
 *
 * @note    Если название файла начинается со слеша `/`, файл будет взят из папки amxmodx/configs.
 *          Иначе из папки вип-системы (см. константу VIPM_CFG_PATH).
 *
 * @param Path      Путь / Название файла.
 * 
 * @deprecated Use PCPath_iMakePath() from ParamsController instead
 *
 * @return  Полученный путь.
 */
#pragma deprecated Use PCPath_iMakePath() from ParamsController instead
stock VipM_iGetCfgPath(const sPath[] = "") {
    new sRetPath[PLATFORM_MAX_PATH];
    VipM_GetCfgPath(sPath, sRetPath, charsmax(sRetPath));
    return sRetPath;
}
// inl_VipM_GetCfgPath is deprecated
#define inl_VipM_GetCfgPath VipM_iGetCfgPath

enum _:VipM_FwdReturn{
    /*
     * Продолжить действие
     */
    VIPM_CONTINUE = 0,
    
    /*
     * Прервать действие
     */
    VIPM_STOP = 1,
}

/**
 * Вызывается после инициалзиации модулей и лимитов.
 *
 * @note    Вызывается из plugin_precache
 *
 * @noreturn
 */
#pragma deprecated Use VipM_Modules_OnInited() or VipM_Limits_OnInited() instead.
forward VipM_OnInitModules();

/**
 * Вызывается после полной загрузки системы привилегий.
 *
 * @note    Вызывается из plugin_precache
 *
 * @noreturn
 */
forward VipM_OnLoaded();

/**
 * Обновляет привилегии указанного пользователя.
 *
 * @param UserId    Индекс игрока, чьи привилегии надо обновить.
 *
 * @noreturn
 */
native VipM_UserUpdate(const UserId);

/**
 * Вызывается после загрузки привилегий игрока.
 *
 * @param UserId    Индекс игрока, чьи привилегии были загружены.
 *
 * @note            Можно использовать для кэширования параметров привилегий.
 *
 * @noreturn
 */
forward VipM_OnUserUpdated(const UserId);

/**
 * Выводит лог с указанием JSON-файла, из которого было прочитано указанное значене.
 * 
 * @deprecated
 *
 * @param jValue    Значение, для которого надо вывести лог.
 * @param sPrefix   Префикс лога. Например: ERROR, WARNING, INFO
 * @param sMessage  Текст сообщения.
 * @param any:...   Параметры форматирования сообщения.
 *
 * @noreturn
 */
#pragma deprecated Use PCJson_*ForFile() instead.
native VipM_Json_LogForFile(const JSON:jValue, const sPrefix[], const sMessage[], any:...);

#include <VipM/Modules>
#include <VipM/Limits>
#include <ItemsController>
#include <VipM/Params>
#include <VipM/M/WeaponMenu>
