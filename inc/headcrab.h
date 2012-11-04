/*
    Headcrab.js -- Embeddable HTML GUI framework for C.
*/

#pragma once
#ifndef _HEADCRAB_H
#define _HEADCRAB_H

#include <jansson.h> /* json_t type */
#include <string.h>

typedef enum HEADCRAB_ERROR
{
    HC_SUCCESS,
    HC_FAIL,
    HC_SIZE
} HEADCRAB_ERROR;

/*
    HC_LogFunciton is a function pointer to a printf-like function we use for internal logging.
*/
typedef int (*HC_LogFunction)(const char* fmt, ...);

/*
    HC_PreOpFunction is a function pointer that will be called before accessing an object, and passed arguments needed to do its work.
    This is useful, for example, when needing to acquire a mutex for safety in accessing a variable.
*/
typedef HEADCRAB_ERROR (*HC_PreOpFunction)(void* _args);

/*
    HC_PostOpFunction is a function pointer that will be called after accessing an object, and passed arguments needed to do its work.
    This is useful, for example, when needing to release a previously acquired mutex.
*/
typedef HEADCRAB_ERROR (*HC_PostOpFunction)(void* _args);

/*
    HC_MutatorFunction is a function pointer that will be called to change an object when prompted by the web interface.
    It is passed the target to be manipulated and a JSON object of the arguments sent from the web frontend.
    Note that the _args parameter should not be depended upon to exist once function leaves scope--so, copy what you need.
*/
typedef void (*HC_MutatorFunction)(void* _target, const json_t* _args);

/*
    log_call is the function to be used for logging.
    It can be specified by the user for custom logging.
    By default, it points to printf().
*/
extern HC_LogFunction log_call;

/*
    LOG_MSG logs a message to the log_call function, which is printf by default.
    It is used for informational log messages only.
*/
#ifdef DEBUG    
#define LOG_MSG(...) (log_call(__VA_ARGS__))
#else
#define LOG_MSG(...)
#endif

/*
    LOG_ERROR logs messages out to stderr.
    It is used in exceptional situations, when things have gone horribly wrong.
*/
#ifdef DEBUG
#define LOG_ERROR(...) (fprintf(stderr, __VA_ARGS__))
#else
#define LOG_ERROR(...)
#endif

/*
    Function to initialize headcrab--this will startup the web server and point it at the directory to serve assets from.

    @param _assetDir - null-terminated string of a path (relative or absolute) to host files from.
    @return HC_SUCCESS on success, HC_ERROR otherwise.
*/
HEADCRAB_ERROR headcrab_init(const char* _assetDir);

/*
    Function to close headcrab--this will clean up the web server and disconnect clients.

    @return void.
*/
void headcrab_shutdown();

/*
    Function to set the logging callback if desired.
    This function will be used internally in the same fashion as printf().

    @param _callback - HC_LogFunction function pointer taking a null-terminated format string and varargs.
    @return void.
*/
void headcrab_set_logging_callback( HC_LogFunction _callback);

/*
    Function to bind an object for access by the GUI.

    @param _target - a pointer to the object that will be manipulated by the function
    @param _name - null-terminated string giving the name the object will be referred to as by the frontend.
    @param _verb - null-terminated string giving the name used by the frontend to invoke a command on the object.
    @param _preOp - HC_PreOpFunction that will be called before attempting to change the object.
    @param _preOpArgs - void pointer to the arguments that will be passed in to the pre-op function.
    @param _op - HC_MutatorFunction that will be called on the object.
    @param _postOp - HC_PostOpFunction that will be called once the mutator has been run on the object.
    @param _postOpArgs - void pointer to the arguments that will be passed in to the post-op function.
*/
void headcrab_bind_object(  void* _target,
                            const char* _name,
                            const char* _verb,
                            HC_PreOpFunction _preOp,
                            const void* _preOpArgs,
                            HC_MutatorFunction _op,
                            HC_PostOpFunction _postOp,
                            const void* _postOpArgs
                        );

/*
    Function to clear all verb bindings for an object. Call this will make the object inaccessible to the frontend.

    @param _name - null-terminated string of the object to remove the bindings of.
    @return void.
*/
void headcrab_clear_all_object_bindings( const char* _name );

/*
    Function to change the object memory binding--useful, for example, during reallocations or garbage collection.

    @param _name - null-terminated string of the object to rebind.
    @param _object - void* to the location of the object in memory.
    @return void.
*/
void headcrab_rebind_object( const char* _name, void* _object);

/*
    Handles all commands received via the websocket server since the last call.
*/
void headcrab_handle_commands();

#endif