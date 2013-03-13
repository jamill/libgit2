/*
 * Copyright (C) the libgit2 contributors. All rights reserved.
 *
 * This file is part of libgit2, distributed under the GNU GPL v2 with
 * a Linking Exception. For full terms see the included COPYING file.
 */

#include "common.h"
#include "error.h"

char *git_win32_get_error_message(DWORD error_code)
{
	LPWSTR lpMsgBuf = NULL;
	DWORD flags =
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS;
	LPCVOID source = NULL;

	if (!error_code)
		return NULL;

	/** 
	 * Error codes 12000 - 12999 are ERROR_INTERNET_*. The following link
	 * indicates that 12000 - 12175 are for ERROR_INTERNET, but it appears
	 * there are also internet erros between 12175 - 12999.
	 * http://msdn.microsoft.com/en-us/library/windows/desktop/ms681384.aspx
	 */
	if ((error_code >= 12000) && (error_code <= 12999)) {
		flags = flags | FORMAT_MESSAGE_FROM_HMODULE;
		source = GetModuleHandle("winhttp");
	}

	if (FormatMessageW(
				flags,
				source, error_code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				(LPWSTR)&lpMsgBuf, 0, NULL)) {
		int utf8_size = WideCharToMultiByte(CP_UTF8, 0, lpMsgBuf, -1, NULL, 0, NULL, NULL);

		char *lpMsgBuf_utf8 = git__malloc(utf8_size * sizeof(char));
		if (lpMsgBuf_utf8 == NULL) {
			LocalFree(lpMsgBuf);
			return NULL;
		}
		if (!WideCharToMultiByte(CP_UTF8, 0, lpMsgBuf, -1, lpMsgBuf_utf8, utf8_size, NULL, NULL)) {
			LocalFree(lpMsgBuf);
			git__free(lpMsgBuf_utf8);
			return NULL;
		}

		LocalFree(lpMsgBuf);
		return lpMsgBuf_utf8;
	}
	return NULL;
}
