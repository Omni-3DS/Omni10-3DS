#include "omni.h"
#include "http.h"
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
	if (!f) {
		f = fopen("sdmc:/omni10/version.dat", "rb");
	}
	if (!f) {
		if (outsz) out[0] = 0;
		return false;
	}
	{
		size_t n = fread(out, 1, outsz ? outsz - 1 : 0, f);
		fclose(f);
		if (outsz) {
			out[n] = 0;
			while (n > 0 && (out[n - 1] == '\n' || out[n - 1] == '\r' || out[n - 1] == ' '))
				out[--n] = 0;
		}
		return n > 0;
	}
}

void omni_ensure_dirs(void) {
	mkdir("sdmc:/luma", 0777);
	mkdir(LUMA_PAYLOADS, 0777);
	mkdir(OMNI_DIR, 0777);
	mkdir("sdmc:/o10/scripts", 0777);
	mkdir("sdmc:/o10/slr", 0777);
}

typedef struct {
	void (*cb)(const char *, u32, u32, void *);
	void *ud;
	const char *label;
} ProgWrap;

static void http_prog(u32 done, u32 total, void *ud) {
	ProgWrap *w = (ProgWrap *)ud;
	if (w->cb) w->cb(w->label, done, total, w->ud);
}

Result omni_install_latest(void (*progress)(const char *label, u32 done, u32 total, void *ud),
                           void *ud) {
	ProgWrap w;
	Result r;
	char tmpFirm[] = "sdmc:/o10/Omni10.firm.part";

	omni_ensure_dirs();
	w.cb = progress;
	w.ud = ud;
	w.label = "Downloading Omni10.firm...";
	if (progress) progress(w.label, 0, 0, ud);

	r = httpDownloadToFile(OMNI_FIRM_URL, "sdmc:/o10/Omni10.firm.download", http_prog, &w);
	if (R_FAILED(r)) {
		/* offline fallback: copy from SD root */
		FILE *in = fopen("sdmc:/Omni10.firm", "rb");
		FILE *out;
		char *buf;
		long sz;
		if (!in) return r;
		fseek(in, 0, SEEK_END);
		sz = ftell(in);
		fseek(in, 0, SEEK_SET);
		if (sz <= 0 || sz > 8 * 1024 * 1024) {
			fclose(in);
			return -10;
		}
		buf = (char *)malloc((size_t)sz);
		if (!buf) {
			fclose(in);
			return -11;
		}
		if (fread(buf, 1, (size_t)sz, in) != (size_t)sz) {
			free(buf);
			fclose(in);
			return -12;
		}
		fclose(in);
		out = fopen(OMNI_FIRM_PATH, "wb");
		if (!out) {
			free(buf);
			return -13;
		}
		fwrite(buf, 1, (size_t)sz, out);
		fclose(out);
		free(buf);
		if (progress) progress("Installed from SD copy", (u32)sz, (u32)sz, ud);
		return 0;
	}

	/* Move download into luma/payloads */
	{
		FILE *in = fopen("sdmc:/o10/Omni10.firm.download", "rb");
		FILE *out;
		char *buf;
		long sz;
		if (!in) return -20;
		fseek(in, 0, SEEK_END);
		sz = ftell(in);
		fseek(in, 0, SEEK_SET);
		buf = (char *)malloc((size_t)sz);
		if (!buf) {
			fclose(in);
			return -21;
		}
		fread(buf, 1, (size_t)sz, in);
		fclose(in);
		out = fopen(OMNI_FIRM_PATH, "wb");
		if (!out) {
			free(buf);
			return -22;
		}
		fwrite(buf, 1, (size_t)sz, out);
		fclose(out);
		free(buf);
		remove("sdmc:/o10/Omni10.firm.download");
		(void)tmpFirm;
	}

	w.label = "Downloading version.dat...";
	httpDownloadToFile(OMNI_VER_URL, OMNI_VER_PATH, http_prog, &w);
	if (progress) progress("Done", 1, 1, ud);
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
