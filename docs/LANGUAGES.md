# Omni10-3DS Language System

## Supported Languages

| Code | Language   | Status   |
|------|------------|----------|
| EN   | English    | ✓ Complete |
| DE   | Deutsch    | ✓ Complete |
| IT   | Italiano   | ✓ Complete |
| FR   | Français   | ✓ Complete |
| ES   | Español    | ✓ Complete |
| NL   | Nederlands | Planned  |
| PT   | Português  | Planned  |

## Usage in Code

```c
#include "lang.h"

// Get translated string
const char* text = _("YES");          // "Yes" / "Ja" / "Sì" / ...
const char* title = _("APP_NAME");    // always "Omni10-3DS"

// Change language
lang_set(LANG_DE);
ui_echo(_("TAGLINE"));                // "Voller Zugriff. Keine Limits."
```

## Available Keys (selection)

- `APP_NAME`, `TAGLINE`, `BOOTING`
- `YES`, `NO`, `OK`, `CANCEL`, `CONFIRM`, `BACK`, `CONTINUE`
- `FILE_BROWSER`, `SCRIPTS`, `LUA_SCRIPTS`, `SETTINGS`, `LANGUAGE`
- `FTP_SERVER`, `START_FTP`, `STOP_FTP`, `FTP_RUNNING`, `FTP_STOPPED`
- `NAND_BACKUP`, `ARE_YOU_SURE`, `PLEASE_WAIT`, `DONE`, `ERROR`, `SUCCESS`

## Future

External language files (similar to GodMode9 TRF) so the community can add translations without recompiling.
