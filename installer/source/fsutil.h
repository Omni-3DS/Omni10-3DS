#pragma once
#include <3ds.h>
#include <stdbool.h>

#define FS_MAX_ENTRIES 64
#define FS_NAME_LEN    64
#define FS_PATH_LEN    512

typedef struct {
	char name[FS_NAME_LEN];
	bool is_dir;
	u64 size;
} FsEntry;

typedef struct {
	char path[FS_PATH_LEN];
	FsEntry entries[FS_MAX_ENTRIES];
	int count;
	int sel;
	int scroll;
} FsBrowser;

void fs_browser_init(FsBrowser *b, const char *start_path);
bool fs_browser_refresh(FsBrowser *b);
void fs_browser_enter(FsBrowser *b);
void fs_browser_up(FsBrowser *b);
const char *fs_browser_selected_path(FsBrowser *b, char *out, size_t outsz);
