#pragma once

namespace unicode
{
	inline UNICODE_STRING init_string(const wchar_t* source)
	{
		SIZE_T size = 0;
		UNICODE_STRING result = {};

		if (source)
		{
			size = (wcslen(source) * sizeof(wchar_t));
			result.Length = static_cast<USHORT>(size);
			result.MaximumLength = static_cast<USHORT>(size) + sizeof(WCHAR);
		}
		else
		{
			result.Buffer[0] = '\0';
			result.Length = 0;
			result.MaximumLength = 0;
		}
		result.Buffer = const_cast<PWCHAR>(source);
		return result;
	}

	inline void clear_string(UNICODE_STRING* string)
	{
		string->Buffer[0] = L'\0';
		string->Length = 0;
		string->MaximumLength = 0;
	}


	inline bool compare_strings(UNICODE_STRING* string1, UNICODE_STRING* string2) { return RtlEqualUnicodeString(string1, string2, TRUE); }
}