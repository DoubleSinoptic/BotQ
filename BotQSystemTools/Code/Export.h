#pragma once
#ifndef KH_EXPORT_H
#define KH_EXPORT_H

#if defined(_WIN32) || defined(_WIN64)
#define KH_EXPORT __declspec(dllexport)
#else
#define KH_EXPORT
#endif

#endif