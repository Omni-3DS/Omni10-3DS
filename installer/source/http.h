#pragma once
#include <3ds.h>
#include <stdbool.h>

typedef void (*HttpProgressFn)(u32 done, u32 total, void *ud);

bool httpInit(void);
void httpExit(void);
Result httpDownloadToFile(const char *url, const char *path,
                          HttpProgressFn cb, void *userdata);
