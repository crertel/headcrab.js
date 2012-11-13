#pragma once
#ifndef _HEADCRAB_SERVER_H
#define _HEADCRAB_SERVER_H

int websocket_initialize(const char * _assetDir);
void websocket_notify();
void websocket_shutdown();

#endif//_HEADCRAB_SERVER_H