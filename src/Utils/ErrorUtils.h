#pragma once
#include "../Platform/CustomWin.h"
#include <string>
#include <sstream>

inline void ShowErrorMessageBoxA( const std::string& title, const std::string& message )
{
	MessageBoxA( nullptr, message.c_str(), title.c_str(), MB_ICONERROR | MB_OK );
}

inline std::string HrToStringA( HRESULT hr )
{
	char* errorMsg = nullptr;
	FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr, hr, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
		(LPSTR)&errorMsg, 0, nullptr );
	std::string result = errorMsg ? errorMsg : "Unknown error";
	if( errorMsg ) LocalFree( errorMsg );
	return result;
}

#define DX_CALL(call) \
    { \
        HRESULT _hr = (call); \
        if (FAILED(_hr)) { \
            std::ostringstream _ss; \
            _ss << #call << " failed.\nHRESULT: 0x" << std::hex << _hr << "\n" << HrToStringA(_hr); \
            ShowErrorMessageBoxA("DirectX Error", _ss.str()); \
            throw std::runtime_error("DirectX call failed: " #call); \
        } \
    }
