#ifndef _FAD_CONTROL_CONFIG_H_
#define _FAD_CONTROL_CONFIG_H_

#if defined(_WIN32)             // MS Compiler & MinGW GCC
#	if defined(_FADCTRLAPI_EXPORTS)
#       define _EXPORT_ADCTRL_ __declspec(dllexport)
#	else
#		define _EXPORT_ADCTRL_ __declspec(dllimport)
#	endif

#elif defined(__GNUG__)   // GCC
#   define _EXPORT_ADCTRL_

#elif defined(__ARMCC_VERSION)  // ARM Compiler (RVCT 3.1)
#	define _EXPORT_ADCTRL_

#else
#	define _EXPORT_ADCTRL_

#endif

#endif

