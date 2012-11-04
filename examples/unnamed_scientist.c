#include "headcrab.h"
#include <stdarg.h>
#include <stdio.h>

int customLog(const char* fmt, ...)
{
    int count;
    va_list args;
    
    va_start(args,fmt);
    vfprintf(stderr,"HEADCRAB SAYS: ");
    count = vfprintf(stderr, fmt, args);
    va_end(args);

    return count;
}

int preHook(void* _target)
{
    printf("Preop tripped!\n");
    return HC_SUCCESS;
}

double accum = 0;
int opAccum(void* _target, const json_t* _args)
{
    json_t* incval;

    incval = json_object_get(_args, "incval");
    if (incval == NULL || !json_is_number(incval) )
    {
        printf("Malformed increment op arg!\n");
        return HC_FAIL;
    }

    *((double*)_target) += json_number_value(incval);

    return HC_SUCCESS;
}

int postHook(void* _target)
{
    printf("Accum has been set to %f!\n", *((double*)_target));
    return HC_SUCCESS;
}

static int shouldShutdown = 0;

int opShutdown(void* _target, const json_t* _args)
{
    shouldShutdown = 1;
    return HC_SUCCESS;
}

int opGet(void* _target, const json_t* _args)
{
    
    // TODO: send value back to server.

    return HC_SUCCESS;
}

int main(int argc, char ** argv)
{
    /* Startup headcrab and host the gui. */
	headcrab_init("./");

    /* Configure basic options. */
    headcrab_set_logging_callback(customLog);

    headcrab_bind_object( NULL,         /* object */
                          "",           /* javascript name */
                          "shutdown",   /* javascript verb */
                          NULL,         /* preop function */
                          NULL,         /* preop arg */
                          opShutdown,   /* op function */
                          NULL,         /* postop function */
                          NULL);        /* postop arg */
    headcrab_bind_object(   &accum,
                            "accum",
                            "add",
                            preHook,
                            NULL,
                            opAccum,
                            postHook,
                            &accum
                         );
    headcrab_bind_object(   &accum,
                            "accum",
                            "read",
                            NULL,
                            NULL,
                            opGet,
                            NULL,
                            NULL
                        );

    /* spin in an update loop until told to shutdown. */
	while(!shouldShutdown)
	{
        /* pump the command loop */
		headcrab_handle_commands();
	}

    headcrab_shutdown();
	return 0;
}
