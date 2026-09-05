#include "fsutil.h"
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

static void join_path(char *out, size_t outsz, const char *base, const char *name) {
	if (strcmp(base, "sdmc:/") == 0 || (base[0] && base[strlen(base)-1] == '/'))
		snprintf(out, outsz, "%s%s", base, name);
	else
		snprintf(out, outsz, "%s/%s", base, name);
}

void fs_browser_init(FsBrowser *b, const char *start_path) {
	memset(b, 0, sizeof(*b));
	strncpy(b->path, start_path ? start_path : "sdmc:/", FS_PATH_LEN - 1);
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
		if (strcmp(de->d_name, ".") == 0) continue;
		FsEntry *e = &b->entries[b->count];
		strncpy(e->name, de->d_name, FS_NAME_LEN - 1);
		e->name[FS_NAME_LEN - 1] = 0;
		char full[FS_PATH_LEN];
		join_path(full, sizeof(full), b->path, de->d_name);
		struct stat st;
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
	if (b->count <= 0 || b->sel < 0 || b->sel >= b->count) return;
	FsEntry *e = &b->entries[b->sel];
	if (!e->is_dir) return;
	char next[FS_PATH_LEN];
	join_path(next, sizeof(next), b->path, e->name);
	strncpy(b->path, next, FS_PATH_LEN - 1);
	fs_browser_refresh(b);
}

void fs_browser_up(FsBrowser *b) {
	if (strcmp(b->path, "sdmc:/") == 0 || strcmp(b->path, "sdmc:") == 0)
		return;
	char *slash = strrchr(b->path, '/');
	if (!slash) return;
	if (slash == b->path + 5) { /* sdmc:/ */
		slash[1] = 0;
	} else {
		*slash = 0;
	}
	if (strlen(b->path) < 6) strncpy(b->path, "sdmc:/", FS_PATH_LEN - 1);
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
