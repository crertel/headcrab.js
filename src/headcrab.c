#include "headcrab.h"
#include "mongoose.h"
#include "headcrab_dispatch.h"

struct mg_context * ctx;

HEADCRAB_ERROR headcrab_init(const char* _assetDir)
{
	const char * options[] = {
		"listening_ports", "8080",
		"document_root", _assetDir,
		NULL
	};
	ctx = mg_start(&callback, NULL, options);
}

void headcrab_shutdown()
{
	mg_stop(ctx);
}