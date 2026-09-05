#pragma once
#include <3ds.h>
#include <stdbool.h>
#include <stddef.h>

#define BACKUP_MAGIC "O10BK1"
#define BACKUP_PATH_R4   "sdmc:/o10/r4/backup.bin"
#define BACKUP_PATH_DSTT "sdmc:/o10/dstt/backup.bin"
#define BACKUP_DIR_R4    "sdmc:/o10/r4"
#define BACKUP_DIR_DSTT  "sdmc:/o10/dstt"

typedef enum {
	BACKUP_KIND_R4 = 1,
	BACKUP_KIND_DSTT = 2
} BackupKind;

/* Create dirs + write backup.bin (firm + version + meta). */
Result backup_create(BackupKind kind, char *msg, size_t msgsz);

/* Restore from backup.bin to luma/payloads + omni10 version. */
Result backup_restore(BackupKind kind, char *msg, size_t msgsz);

bool backup_exists(BackupKind kind);
const char *backup_path(BackupKind kind);
