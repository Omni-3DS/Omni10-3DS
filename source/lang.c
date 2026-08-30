/*
 * Omni10-3DS – Multi-Language System
 */

#include "lang.h"
#include <string.h>

static lang_id_t current_lang = LANG_EN;

// Simple string tables (will later be loaded from files)

static const char* str_en[][2] = {
    { "APP_NAME",           "Omni10-3DS" },
    { "TAGLINE",            "Full Access. No Limits." },
    { "BOOTING",            "booting..." },
    { "YES",                "Yes" },
    { "NO",                 "No" },
    { "OK",                 "OK" },
    { "CANCEL",             "Cancel" },
    { "CONFIRM",            "Confirm" },
    { "BACK",               "Back" },
    { "CONTINUE",           "Continue" },
    { "FILE_BROWSER",       "File Browser" },
    { "SCRIPTS",            "Scripts" },
    { "LUA_SCRIPTS",        "Lua Scripts" },
    { "SETTINGS",           "Settings" },
    { "LANGUAGE",           "Language" },
    { "FTP_SERVER",         "FTP Server" },
    { "START_FTP",          "Start FTP Server" },
    { "STOP_FTP",           "Stop FTP Server" },
    { "FTP_RUNNING",        "FTP Server is running" },
    { "FTP_STOPPED",        "FTP Server stopped" },
    { "NAND_BACKUP",        "NAND Backup" },
    { "ARE_YOU_SURE",       "Are you sure?" },
    { "PLEASE_WAIT",        "Please wait..." },
    { "DONE",               "Done." },
    { "ERROR",              "Error" },
    { "SUCCESS",            "Success" },
    { NULL, NULL }
};

static const char* str_de[][2] = {
    { "APP_NAME",           "Omni10-3DS" },
    { "TAGLINE",            "Voller Zugriff. Keine Limits." },
    { "BOOTING",            "starte..." },
    { "YES",                "Ja" },
    { "NO",                 "Nein" },
    { "OK",                 "OK" },
    { "CANCEL",             "Abbrechen" },
    { "CONFIRM",            "Bestätigen" },
    { "BACK",               "Zurück" },
    { "CONTINUE",           "Weiter" },
    { "FILE_BROWSER",       "Dateibrowser" },
    { "SCRIPTS",            "Skripte" },
    { "LUA_SCRIPTS",        "Lua-Skripte" },
    { "SETTINGS",           "Einstellungen" },
    { "LANGUAGE",           "Sprache" },
    { "FTP_SERVER",         "FTP-Server" },
    { "START_FTP",          "FTP-Server starten" },
    { "STOP_FTP",           "FTP-Server stoppen" },
    { "FTP_RUNNING",        "FTP-Server läuft" },
    { "FTP_STOPPED",        "FTP-Server gestoppt" },
    { "NAND_BACKUP",        "NAND-Backup" },
    { "ARE_YOU_SURE",       "Bist du sicher?" },
    { "PLEASE_WAIT",        "Bitte warten..." },
    { "DONE",               "Fertig." },
    { "ERROR",              "Fehler" },
    { "SUCCESS",            "Erfolg" },
    { NULL, NULL }
};

static const char* str_it[][2] = {
    { "APP_NAME",           "Omni10-3DS" },
    { "TAGLINE",            "Accesso completo. Nessun limite." },
    { "BOOTING",            "avvio..." },
    { "YES",                "Sì" },
    { "NO",                 "No" },
    { "OK",                 "OK" },
    { "CANCEL",             "Annulla" },
    { "CONFIRM",            "Conferma" },
    { "BACK",               "Indietro" },
    { "CONTINUE",           "Continua" },
    { "FILE_BROWSER",       "Esplora file" },
    { "SCRIPTS",            "Script" },
    { "LUA_SCRIPTS",        "Script Lua" },
    { "SETTINGS",           "Impostazioni" },
    { "LANGUAGE",           "Lingua" },
    { "FTP_SERVER",         "Server FTP" },
    { "START_FTP",          "Avvia server FTP" },
    { "STOP_FTP",           "Ferma server FTP" },
    { "FTP_RUNNING",        "Server FTP in esecuzione" },
    { "FTP_STOPPED",        "Server FTP fermato" },
    { "NAND_BACKUP",        "Backup NAND" },
    { "ARE_YOU_SURE",       "Sei sicuro?" },
    { "PLEASE_WAIT",        "Attendere..." },
    { "DONE",               "Fatto." },
    { "ERROR",              "Errore" },
    { "SUCCESS",            "Successo" },
    { NULL, NULL }
};

static const char* str_fr[][2] = {
    { "APP_NAME",           "Omni10-3DS" },
    { "TAGLINE",            "Accès total. Sans limites." },
    { "BOOTING",            "démarrage..." },
    { "YES",                "Oui" },
    { "NO",                 "Non" },
    { "OK",                 "OK" },
    { "CANCEL",             "Annuler" },
    { "CONFIRM",            "Confirmer" },
    { "BACK",               "Retour" },
    { "CONTINUE",           "Continuer" },
    { "FILE_BROWSER",       "Explorateur" },
    { "SCRIPTS",            "Scripts" },
    { "LUA_SCRIPTS",        "Scripts Lua" },
    { "SETTINGS",           "Paramètres" },
    { "LANGUAGE",           "Langue" },
    { "FTP_SERVER",         "Serveur FTP" },
    { "START_FTP",          "Démarrer le serveur FTP" },
    { "STOP_FTP",           "Arrêter le serveur FTP" },
    { "FTP_RUNNING",        "Serveur FTP en cours" },
    { "FTP_STOPPED",        "Serveur FTP arrêté" },
    { "NAND_BACKUP",        "Sauvegarde NAND" },
    { "ARE_YOU_SURE",       "Êtes-vous sûr ?" },
    { "PLEASE_WAIT",        "Veuillez patienter..." },
    { "DONE",               "Terminé." },
    { "ERROR",              "Erreur" },
    { "SUCCESS",            "Succès" },
    { NULL, NULL }
};

static const char* str_es[][2] = {
    { "APP_NAME",           "Omni10-3DS" },
    { "TAGLINE",            "Acceso total. Sin límites." },
    { "BOOTING",            "iniciando..." },
    { "YES",                "Sí" },
    { "NO",                 "No" },
    { "OK",                 "OK" },
    { "CANCEL",             "Cancelar" },
    { "CONFIRM",            "Confirmar" },
    { "BACK",               "Atrás" },
    { "CONTINUE",           "Continuar" },
    { "FILE_BROWSER",       "Explorador" },
    { "SCRIPTS",            "Scripts" },
    { "LUA_SCRIPTS",        "Scripts Lua" },
    { "SETTINGS",           "Ajustes" },
    { "LANGUAGE",           "Idioma" },
    { "FTP_SERVER",         "Servidor FTP" },
    { "START_FTP",          "Iniciar servidor FTP" },
    { "STOP_FTP",           "Detener servidor FTP" },
    { "FTP_RUNNING",        "Servidor FTP en ejecución" },
    { "FTP_STOPPED",        "Servidor FTP detenido" },
    { "NAND_BACKUP",        "Copia de NAND" },
    { "ARE_YOU_SURE",       "¿Estás seguro?" },
    { "PLEASE_WAIT",        "Por favor espera..." },
    { "DONE",               "Hecho." },
    { "ERROR",              "Error" },
    { "SUCCESS",            "Éxito" },
    { NULL, NULL }
};

static const char* (*tables[])[2] = {
    str_en, str_de, str_it, str_fr, str_es, str_en, str_en  // NL + PT fallback to EN for now
};

static const char* lang_names[] = {
    "English", "Deutsch", "Italiano", "Français", "Español", "Nederlands", "Português"
};

static const char* lang_codes[] = {
    "EN", "DE", "IT", "FR", "ES", "NL", "PT"
};

omni_result_t lang_init(void)
{
    current_lang = LANG_EN;
    return OMNI_OK;
}

void lang_deinit(void)
{
}

omni_result_t lang_set(lang_id_t id)
{
    if (id < 0 || id >= LANG_COUNT)
        return OMNI_ERR_INVALID;
    current_lang = id;
    return OMNI_OK;
}

lang_id_t lang_get(void)
{
    return current_lang;
}

const char* lang_get_name(lang_id_t id)
{
    if (id < 0 || id >= LANG_COUNT) return "Unknown";
    return lang_names[id];
}

const char* lang_get_code(lang_id_t id)
{
    if (id < 0 || id >= LANG_COUNT) return "??";
    return lang_codes[id];
}

const char* lang_get_str(const char* key)
{
    if (!key) return "";

    const char* (*table)[2] = tables[current_lang];
    for (int i = 0; table[i][0] != NULL; i++) {
        if (strcmp(table[i][0], key) == 0)
            return table[i][1];
    }

    // Fallback to English
    if (current_lang != LANG_EN) {
        table = tables[LANG_EN];
        for (int i = 0; table[i][0] != NULL; i++) {
            if (strcmp(table[i][0], key) == 0)
                return table[i][1];
        }
    }

    return key; // return key itself if not found
}
