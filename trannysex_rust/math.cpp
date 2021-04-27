#include "stdafx.h"

atan_t math::atan;
atan2_t math::atan2;
cos_t math::cos;
sin_t math::sin;
asin_t math::asin;

void math::init()
{
	HMODULE ntdll = GetModuleHandleA(s("ntdll.dll"));
	HMODULE msvcrt = GetModuleHandleA(s("msvcrt.dll"));

	math::atan = reinterpret_cast<atan_t>(GetProcAddress(ntdll, s("atan")));
	math::atan2 = reinterpret_cast<atan2_t>(GetProcAddress(ntdll, s("atan2")));
	math::cos = reinterpret_cast<cos_t>(GetProcAddress(ntdll, s("cos")));
	math::sin = reinterpret_cast<sin_t>(GetProcAddress(ntdll, s("sin")));
	math::asin = reinterpret_cast<asin_t>(GetProcAddress(msvcrt, s("asin")));
}