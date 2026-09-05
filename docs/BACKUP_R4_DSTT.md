# R4 / DSTT backup & restore

Before the flashcart **warning** screen, Omni10 always writes a safety backup.

## Paths

| Cart | Backup file |
|------|-------------|
| **R4** | `sdmc:/o10/r4/backup.bin` |
| **DSTT** | `sdmc:/o10/dstt/backup.bin` |

## Contents

`O10BK1` header + kind + version string + **Omni10.firm** + `version.dat`

## Flow

1. Other options → **Flashcart R4 / DSTT**
2. Choose **R4 warn** or **DSTT warn**
3. **Backup runs first** → then warning (magnet / sleep / http://ip:1089)
4. **A** → color sequence
5. **Restore R4 / Restore DSTT** puts firm back from `backup.bin`
