/*
 * Omni10 flashcart safety backup: sdmc:/o10/r4/backup.bin or .../dstt/backup.bin
 * Format: magic[6] + kind u8 + ver[16] + firm_size u32 LE + firm bytes + version.dat bytes
 */
#include "backup.h"
#include "omni.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

const char *backup_path(BackupKind kind) {
	return kind == BACKUP_KIND_DSTT ? BACKUP_PATH_DSTT : BACKUP_PATH_R4;
}

static const char *backup_dir(BackupKind kind) {
	return kind == BACKUP_KIND_DSTT ? BACKUP_DIR_DSTT : BACKUP_DIR_R4;
}

bool backup_exists(BackupKind kind) {
	struct stat st;
	return stat(backup_path(kind), &st) == 0 && st.st_size > 12;
}

static void mkdirs(void) {
	mkdir("sdmc:/o10", 0777);
	mkdir(BACKUP_DIR_R4, 0777);
	mkdir(BACKUP_DIR_DSTT, 0777);
	mkdir(OMNI_DIR, 0777);
	mkdir("sdmc:/luma", 0777);
	mkdir(LUMA_PAYLOADS, 0777);
}

static int read_file(const char *path, char **out, u32 *out_sz) {
	FILE *f;
	long sz;
	char *buf;
	*out = NULL;
	*out_sz = 0;
	f = fopen(path, "rb");
	if (!f) return -1;
	fseek(f, 0, SEEK_END);
	sz = ftell(f);
	fseek(f, 0, SEEK_SET);
	if (sz < 0 || sz > 8 * 1024 * 1024) { fclose(f); return -2; }
	buf = (char *)malloc((size_t)sz);
	if (!buf) { fclose(f); return -3; }
	if (fread(buf, 1, (size_t)sz, f) != (size_t)sz) {
		free(buf);
		fclose(f);
		return -4;
	}
	fclose(f);
	*out = buf;
	*out_sz = (u32)sz;
	return 0;
}

Result backup_create(BackupKind kind, char *msg, size_t msgsz) {
	char *firm = NULL, *ver = NULL;
	u32 firm_sz = 0, ver_sz = 0;
	FILE *out;
	u8 kind_b = (u8)kind;
	char verbuf[16];
	int i;

	mkdirs();
	memset(verbuf, 0, sizeof(verbuf));

	if (read_file(OMNI_FIRM_PATH, &firm, &firm_sz) != 0) {
		/* try alternate locations */
		if (read_file("sdmc:/Omni10.firm", &firm, &firm_sz) != 0) {
			snprintf(msg, msgsz, "No Omni10.firm to back up");
			return -1;
		}
	}
	if (read_file(OMNI_VER_PATH, &ver, &ver_sz) != 0)
		read_file("sdmc:/version.dat", &ver, &ver_sz);
	if (ver && ver_sz > 0) {
		for (i = 0; i < 15 && i < (int)ver_sz; i++) {
			if (ver[i] == '\n' || ver[i] == '\r') break;
			verbuf[i] = ver[i];
		}
	}

	out = fopen(backup_path(kind), "wb");
	if (!out) {
		free(firm);
		free(ver);
		snprintf(msg, msgsz, "Cannot write %s", backup_path(kind));
		return -2;
	}
	fwrite(BACKUP_MAGIC, 1, 6, out);
	fwrite(&kind_b, 1, 1, out);
	fwrite(verbuf, 1, 16, out);
	{
		u8 le[4];
		le[0] = (u8)(firm_sz);
		le[1] = (u8)(firm_sz >> 8);
		le[2] = (u8)(firm_sz >> 16);
		le[3] = (u8)(firm_sz >> 24);
		fwrite(le, 1, 4, out);
	}
	fwrite(firm, 1, firm_sz, out);
	if (ver && ver_sz)
		fwrite(ver, 1, ver_sz, out);
	fclose(out);
	free(firm);
	free(ver);
	snprintf(msg, msgsz, "Backup OK → %s (%u bytes firm)",
	         backup_path(kind), (unsigned)firm_sz);
	return 0;
}

Result backup_restore(BackupKind kind, char *msg, size_t msgsz) {
	FILE *in;
	char magic[7];
	u8 kind_b;
	char verbuf[16];
	u8 le[4];
	u32 firm_sz;
	char *firm;
	size_t rest;
	char *verdata;

	mkdirs();
	in = fopen(backup_path(kind), "rb");
	if (!in) {
		snprintf(msg, msgsz, "No backup at %s", backup_path(kind));
		return -1;
	}
	if (fread(magic, 1, 6, in) != 6) { fclose(in); snprintf(msg, msgsz, "Short file"); return -2; }
	magic[6] = 0;
	if (memcmp(magic, BACKUP_MAGIC, 6) != 0) {
		fclose(in);
		snprintf(msg, msgsz, "Bad magic (not O10 backup)");
		return -3;
	}
	if (fread(&kind_b, 1, 1, in) != 1 || fread(verbuf, 1, 16, in) != 16 ||
	    fread(le, 1, 4, in) != 4) {
		fclose(in);
		snprintf(msg, msgsz, "Corrupt header");
		return -4;
	}
	firm_sz = (u32)le[0] | ((u32)le[1] << 8) | ((u32)le[2] << 16) | ((u32)le[3] << 24);
	if (firm_sz == 0 || firm_sz > 8 * 1024 * 1024) {
		fclose(in);
		snprintf(msg, msgsz, "Bad firm size");
		return -5;
	}
	firm = (char *)malloc(firm_sz);
	if (!firm) { fclose(in); return -6; }
	if (fread(firm, 1, firm_sz, in) != firm_sz) {
		free(firm);
		fclose(in);
		snprintf(msg, msgsz, "Truncated firm data");
		return -7;
	}
	{
		FILE *fo = fopen(OMNI_FIRM_PATH, "wb");
		if (!fo) {
			free(firm);
			fclose(in);
			snprintf(msg, msgsz, "Cannot write Omni10.firm");
			return -8;
		}
		fwrite(firm, 1, firm_sz, fo);
		fclose(fo);
	}
	free(firm);

	/* remaining bytes = version.dat */
	fseek(in, 0, SEEK_END);
	{
		long end = ftell(in);
		long hdr = 6 + 1 + 16 + 4 + (long)firm_sz;
		rest = (end > hdr) ? (size_t)(end - hdr) : 0;
	}
	if (rest > 0 && rest < 64) {
		verdata = (char *)malloc(rest + 1);
		if (verdata) {
			fseek(in, 6 + 1 + 16 + 4 + (long)firm_sz, SEEK_SET);
			if (fread(verdata, 1, rest, in) == rest) {
				FILE *vo = fopen(OMNI_VER_PATH, "wb");
				if (vo) { fwrite(verdata, 1, rest, vo); fclose(vo); }
			}
			free(verdata);
		}
	} else if (verbuf[0]) {
		FILE *vo = fopen(OMNI_VER_PATH, "wb");
		if (vo) {
			fwrite(verbuf, 1, strlen(verbuf), vo);
			fwrite("\n", 1, 1, vo);
			fclose(vo);
		}
	}
	fclose(in);
	snprintf(msg, msgsz, "Restored Omni10.firm from %s", backup_path(kind));
	return 0;
}
