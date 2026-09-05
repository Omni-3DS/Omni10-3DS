#include "fsutil.h"
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

static void copy_name(char *dst, size_t dstsz, const char *src) {
	size_t i = 0;
	if (dstsz == 0) return;
	while (i + 1 < dstsz && src[i]) {
		dst[i] = src[i];
		i++;
	}
	dst[i] = 0;
}

static void join_path(char *out, size_t outsz, const char *base, const char *name) {
	if (strcmp(base, "sdmc:/") == 0 || (base[0] && base[strlen(base) - 1] == '/'))
		snprintf(out, outsz, "%s%s", base, name);
	else
		snprintf(out, outsz, "%s/%s", base, name);
	out[outsz - 1] = 0;
}

void fs_browser_init(FsBrowser *b, const char *start_path) {
	memset(b, 0, sizeof(*b));
	copy_name(b->path, FS_PATH_LEN, start_path ? start_path : "sdmc:/");
	fs_browser_refresh(b);
}

bool fs_browser_refresh(FsBrowser *b) {
	DIR *d = opendir(b->path);
	b->count = 0;
	b->sel = 0;
	b->scroll = 0;
	if (!d) return false;
	struct dirent *de;
	while ((de = readdir(d)) != NULL && b->count < FS_MAX_ENTRIES) {
		FsEntry *e;
		char full[FS_PATH_LEN];
		struct stat st;
		if (strcmp(de->d_name, ".") == 0) continue;
		e = &b->entries[b->count];
		copy_name(e->name, FS_NAME_LEN, de->d_name);
		join_path(full, sizeof(full), b->path, de->d_name);
		if (stat(full, &st) == 0) {
			e->is_dir = S_ISDIR(st.st_mode);
			e->size = (u64)st.st_size;
		} else {
			e->is_dir = false;
			e->size = 0;
		}
		b->count++;
	}
	closedir(d);
	return true;
}

void fs_browser_enter(FsBrowser *b) {
	FsEntry *e;
	char next[FS_PATH_LEN];
	if (b->count <= 0 || b->sel < 0 || b->sel >= b->count) return;
	e = &b->entries[b->sel];
	if (!e->is_dir) return;
	join_path(next, sizeof(next), b->path, e->name);
	copy_name(b->path, FS_PATH_LEN, next);
	fs_browser_refresh(b);
}

void fs_browser_up(FsBrowser *b) {
	char *slash;
	if (strcmp(b->path, "sdmc:/") == 0 || strcmp(b->path, "sdmc:") == 0)
		return;
	slash = strrchr(b->path, '/');
	if (!slash) return;
	if (slash == b->path + 5)
		slash[1] = 0;
	else
		*slash = 0;
	if (strlen(b->path) < 6)
		copy_name(b->path, FS_PATH_LEN, "sdmc:/");
	fs_browser_refresh(b);
}

const char *fs_browser_selected_path(FsBrowser *b, char *out, size_t outsz) {
	if (b->count <= 0 || b->sel < 0 || b->sel >= b->count) {
		if (outsz) out[0] = 0;
		return out;
	}
	join_path(out, outsz, b->path, b->entries[b->sel].name);
	return out;
}
