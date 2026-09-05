#include "omni.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

bool omni_firm_exists(void) {
	struct stat st;
	return stat(OMNI_FIRM_PATH, &st) == 0 && st.st_size > 0;
}

bool omni_read_local_version(char *out, size_t outsz) {
	FILE *f = fopen(OMNI_VER_PATH, "rb");
	if (!f) { if (outsz) out[0] = 0; return false; }
	size_t n = fread(out, 1, outsz ? outsz - 1 : 0, f);
	fclose(f);
	if (outsz) {
		out[n] = 0;
		while (n > 0 && (out[n-1]=='\n'||out[n-1]=='\r'||out[n-1]==' ')) out[--n]=0;
	}
	return n > 0;
}

u32 omni_wifi_status(void) {
	return (u32)(osGetWifiStrength() > 0);
}

static void ensure_dirs(void) {
	mkdir("sdmc:/luma", 0777);
	mkdir(LUMA_PAYLOADS, 0777);
	mkdir(OMNI_DIR, 0777);
}

Result omni_install_from_urls(void (*progress)(const char*, int, int, u32, u32, void*), void *ud) {
	ensure_dirs();
	if (progress) progress("Looking for Omni10.firm on SD...", 1, 2, 0, 1, ud);

	const char *candidates[] = {
		"sdmc:/Omni10.firm",
		"sdmc:/omni10/Omni10.firm",
		"sdmc:/3ds/Omni10.firm",
		NULL
	};
	FILE *in = NULL;
	for (int i = 0; candidates[i]; i++) {
		in = fopen(candidates[i], "rb");
		if (in) break;
	}
	if (!in) {
		if (progress) progress("Place Omni10.firm on SD root, then retry", 1, 2, 0, 1, ud);
		return MAKERESULT(RL_PERMANENT, RS_NOTFOUND, RM_APPLICATION, 1);
	}
	fseek(in, 0, SEEK_END);
	long sz = ftell(in);
	fseek(in, 0, SEEK_SET);
	if (sz <= 0 || sz > 8*1024*1024) { fclose(in); return -2; }
	char *buf = (char*)malloc((size_t)sz);
	if (!buf) { fclose(in); return -3; }
	if (fread(buf, 1, (size_t)sz, in) != (size_t)sz) { free(buf); fclose(in); return -4; }
	fclose(in);

	if (progress) progress("Writing sdmc:/luma/payloads/Omni10.firm", 2, 2, (u32)sz, (u32)sz, ud);
	FILE *out = fopen(OMNI_FIRM_PATH, "wb");
	if (!out) { free(buf); return -5; }
	fwrite(buf, 1, (size_t)sz, out);
	fclose(out);
	free(buf);

	FILE *vf = fopen("sdmc:/version.dat", "rb");
	if (!vf) vf = fopen("sdmc:/omni10/version.dat", "rb");
	if (vf) {
		char vbuf[64];
		size_t vn = fread(vbuf, 1, sizeof(vbuf)-1, vf);
		fclose(vf);
		FILE *vo = fopen(OMNI_VER_PATH, "wb");
		if (vo) { fwrite(vbuf, 1, vn, vo); fclose(vo); }
	}
	return 0;
}

Result omni_uninstall(void) {
	unlink(OMNI_FIRM_PATH);
	unlink(OMNI_VER_PATH);
	return 0;
}

Result omni_boot_reboot(void) {
	Result r = nsInit();
	if (R_SUCCEEDED(r)) {
		r = NS_RebootSystem();
		nsExit();
	}
	return r;
}
