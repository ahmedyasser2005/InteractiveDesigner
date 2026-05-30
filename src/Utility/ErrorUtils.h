#pragma once
#include "Platform/CustomWin.h"
#include <string>
#include <sstream>
#include <stdexcept>

inline void ShowErrorMessageBoxA( const std::string& title, const std::string& message ) noexcept
{
	MessageBoxA( nullptr, message.c_str(), title.c_str(), MB_ICONERROR | MB_OK );
}

inline std::string HrToStringA( HRESULT hr )
{
	char* errorMsg = nullptr;
	FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr, hr, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
		reinterpret_cast<LPSTR>(&errorMsg), 0, nullptr );
	std::string result = errorMsg ? errorMsg : "Unknown error";
	if( errorMsg ) LocalFree( errorMsg );
	return result;
}

#define DX_CALL(call) \
    do { \
        HRESULT _dx_hr_ = (call); \
        if (FAILED(_dx_hr_)) { \
            std::ostringstream _dx_ss_; \
            _dx_ss_ << #call << " failed.\nHRESULT: 0x" << std::hex << _dx_hr_ << "\n" << HrToStringA(_dx_hr_); \
            ShowErrorMessageBoxA("DirectX Error", _dx_ss_.str()); \
            throw std::runtime_error(std::string("DirectX call failed: ") + #call); \
        } \
    } while (false)