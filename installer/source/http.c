#include "http.h"
#include <curl/curl.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SOC_ALIGN 0x1000
#define SOC_BUFFERSIZE 0x100000

static u32 *s_socBuf;
static bool s_ready;

bool httpInit(void) {
	if (s_ready) return true;
	s_socBuf = (u32 *)memalign(SOC_ALIGN, SOC_BUFFERSIZE);
	if (!s_socBuf) return false;
	if (R_FAILED(socInit(s_socBuf, SOC_BUFFERSIZE))) {
		free(s_socBuf);
		s_socBuf = NULL;
		return false;
	}
	curl_global_init(CURL_GLOBAL_DEFAULT);
	s_ready = true;
	return true;
}

void httpExit(void) {
	if (s_ready) {
		curl_global_cleanup();
		socExit();
		s_ready = false;
	}
	if (s_socBuf) {
		free(s_socBuf);
		s_socBuf = NULL;
	}
}

typedef struct {
	FILE *f;
	HttpProgressFn cb;
	void *ud;
} FileSink;

static size_t file_write(char *ptr, size_t size, size_t nmemb, void *ud) {
	FileSink *s = (FileSink *)ud;
	return fwrite(ptr, size, nmemb, s->f);
}

static int xfer_info(void *ud, curl_off_t dltotal, curl_off_t dlnow,
                     curl_off_t ultotal, curl_off_t ulnow) {
	FileSink *s = (FileSink *)ud;
	(void)ultotal;
	(void)ulnow;
	if (s->cb) s->cb((u32)dlnow, (u32)dltotal, s->ud);
	return 0;
}

Result httpDownloadToFile(const char *url, const char *path,
                          HttpProgressFn cb, void *userdata) {
	char tmp[512];
	FILE *f;
	CURL *c;
	FileSink sink;
	CURLcode res;
	long httpCode = 0;

	if (!s_ready) return -1;
	snprintf(tmp, sizeof(tmp), "%s.part", path);
	f = fopen(tmp, "wb");
	if (!f) return -2;
	c = curl_easy_init();
	if (!c) {
		fclose(f);
		remove(tmp);
		return -3;
	}
	sink.f = f;
	sink.cb = cb;
	sink.ud = userdata;
	curl_easy_setopt(c, CURLOPT_URL, url);
	curl_easy_setopt(c, CURLOPT_FOLLOWLOCATION, 1L);
	curl_easy_setopt(c, CURLOPT_MAXREDIRS, 10L);
	curl_easy_setopt(c, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt(c, CURLOPT_SSL_VERIFYHOST, 0L);
	curl_easy_setopt(c, CURLOPT_USERAGENT, "O10-Inst-Booter/1.0");
	curl_easy_setopt(c, CURLOPT_FAILONERROR, 1L);
	curl_easy_setopt(c, CURLOPT_NOSIGNAL, 1L);
	curl_easy_setopt(c, CURLOPT_CONNECTTIMEOUT, 30L);
	curl_easy_setopt(c, CURLOPT_WRITEFUNCTION, file_write);
	curl_easy_setopt(c, CURLOPT_WRITEDATA, &sink);
	curl_easy_setopt(c, CURLOPT_NOPROGRESS, 0L);
	curl_easy_setopt(c, CURLOPT_XFERINFOFUNCTION, xfer_info);
	curl_easy_setopt(c, CURLOPT_XFERINFODATA, &sink);
	res = curl_easy_perform(c);
	curl_easy_getinfo(c, CURLINFO_RESPONSE_CODE, &httpCode);
	curl_easy_cleanup(c);
	fclose(f);
	if (res != CURLE_OK) {
		remove(tmp);
		return (Result)res;
	}
	(void)httpCode;
	remove(path);
	if (rename(tmp, path) != 0) {
		remove(tmp);
		return -4;
	}
	return 0;
}
