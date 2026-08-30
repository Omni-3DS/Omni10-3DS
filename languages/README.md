# Omni10-3DS Languages

## Supported Languages

| Code | Language     | Status      |
|------|--------------|-------------|
| EN   | English      | Complete    |
| DE   | Deutsch      | Complete    |
| IT   | Italiano     | Complete    |
| FR   | Français     | Complete    |
| ES   | Español      | Complete    |
| NL   | Nederlands   | Planned     |
| PT   | Português    | Planned     |
| JA   | 日本語        | Future      |

## How it works

Currently strings are compiled into `source/lang.c`.

Later they will be loaded from external files (similar to GodMode9 TRF format) so users can add translations without recompiling.

## Adding a new language

1. Add the language to `lang_id_t` in `include/lang.h`
2. Create a new string table in `source/lang.c`
3. Add the name + code to the arrays
4. Update this README
