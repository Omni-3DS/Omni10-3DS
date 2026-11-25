# Omni10 filesystem API

`include/fs.h` and `source/fs.c` implement a POSIX-backed filesystem API for
host-side tools and modular development. The current FIRM build does not link
`source/`; this backend does not replace or claim to implement bare-metal
SDMMC/FAT access.

## Root and paths

`fs_init()` uses `OMNI_SD_ROOT` when set, otherwise `./sdmc`. It creates the
root directory if needed. All paths are relative to that root. Both
`sdmc:/o10/slr/demo.slr` and `o10/slr/demo.slr` address the same file.
Separators may be `/` or `\\`. Parent traversal (`..`), other drive prefixes,
and symlink components are rejected so a path cannot escape the configured
root. Parent directories must already exist for file creation.

## Operations

- `fs_list()` returns sorted entry names, excluding `.` and `..`. Free the
  result with `fs_list_free()`.
- `fs_read()` allocates the complete regular file. Free the returned buffer
  with `fs_free()`.
- `fs_write()` writes a temporary file in the SD root and renames it over the
  target after a successful write. Existing regular files are replaced; a
  directory target is rejected.
- `fs_copy()` refuses to replace an existing destination and removes a
  partial destination if copying fails. `fs_move()` also refuses an existing
  destination.
- `fs_delete()` removes a file or an empty directory. It cannot delete the SD
  root or follow symlinks.
- `fs_mkdir()` creates one directory level and succeeds if that directory
  already exists.
- `fs_hash()` supports `sha256` and writes a lowercase, 64-character hex digest
  plus a NUL terminator.

The API reports errors using `omni_result_t`; boolean queries return `false`
for invalid paths and I/O errors as well as for missing entries.
