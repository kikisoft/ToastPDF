#pragma once

#define PRINTER_NAME		 "Toast PDF"
#define PORT_NAME			 "ToastPDFPort"
#define MONITOR_NAME	     "ToastPDF_Redmon"
#define PROGRESS_STATUS_NAME "ProgressStatus"

#ifdef _UNICODE
#define tstring wstring
#else
#define tstring string
#endif

#include "Registry.h"
#include "TTUtil.h"
#include "TTPDFWorker.h"