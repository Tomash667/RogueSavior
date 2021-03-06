#include "Pch.h"
#include "Core.h"

static const uint FORMAT_STRINGS = 8;
static const uint FORMAT_LENGTH = 2048;
static char format_buf[FORMAT_STRINGS][FORMAT_LENGTH];
static int format_marker;
static string g_escp;
string g_tmp_string;
static char escape_from[] = { '\n', '\t', '\r', ' ' };
static cstring escape_to[] = { "\\n", "\\t", "\\r", " " };

//=================================================================================================
cstring Format(cstring str, ...)
{
	assert(str);

	va_list list;
	va_start(list, str);
	char* cbuf = format_buf[format_marker];
	_vsnprintf_s(cbuf, FORMAT_LENGTH, FORMAT_LENGTH - 1, str, list);
	cbuf[FORMAT_LENGTH - 1] = 0;
	format_marker = (format_marker + 1) % FORMAT_STRINGS;
	va_end(list);

	return cbuf;
}

//=================================================================================================
cstring FormatList(cstring str, va_list list)
{
	assert(str);

	char* cbuf = format_buf[format_marker];
	_vsnprintf_s(cbuf, FORMAT_LENGTH, FORMAT_LENGTH - 1, str, list);
	cbuf[FORMAT_LENGTH - 1] = 0;
	format_marker = (format_marker + 1) % FORMAT_STRINGS;

	return cbuf;
}

//=================================================================================================
cstring Upper(cstring str)
{
	assert(str);

	char* cbuf = format_buf[format_marker];
	if(*str == 0)
		cbuf[0] = 0;
	else
	{
		cbuf[0] = toupper(str[0]);
		strcpy_s(cbuf + 1, FORMAT_LENGTH - 1, str + 1);
	}

	format_marker = (format_marker + 1) % FORMAT_STRINGS;
	return cbuf;
}

//=================================================================================================
void SplitText(char* buf, vector<cstring>& lines)
{
	cstring start = buf;
	int len = 0;

	while(true)
	{
		char c = *buf;
		if(c == 0x0D || c == 0x0A)
		{
			if(len)
			{
				lines.push_back(start);
				len = 0;
			}
			start = buf + 1;
			*buf = 0;
		}
		else if(c == 0)
		{
			if(len)
				lines.push_back(start);
			break;
		}
		else
			++len;
		++buf;
	}
}

//=================================================================================================
void Split(cstring str, vector<string>& splitted, char split_char, bool add_empty)
{
	assert(str);

	string s;
	char c;

	while((c = *str) != 0)
	{
		if(c == split_char)
		{
			if(!s.empty() || add_empty)
			{
				splitted.push_back(std::move(s));
				s.clear();
			}
		}
		else
			s.push_back(c);

		++str;
	}

	if(!s.empty() || add_empty)
		splitted.push_back(std::move(s));
}


//=================================================================================================
cstring Escape(cstring str, char quote)
{
	char* out = format_buf[format_marker];
	char* out_buf = out;
	cstring from = "\n\t\r";
	cstring to = "ntr";

	char c;
	while((c = *str) != 0)
	{
		int index = StrCharIndex(from, c);
		if(index == -1)
		{
			if(c == quote)
				*out++ = '\\';
			*out++ = c;
		}
		else
		{
			*out++ = '\\';
			*out++ = to[index];
		}
		++str;
	}

	*out = 0;
	out_buf[FORMAT_LENGTH - 1] = 0;
	format_marker = (format_marker + 1) % FORMAT_STRINGS;
	return out_buf;
}

//=================================================================================================
cstring Escape(cstring str, string& out, char quote)
{
	out.clear();
	cstring from = "\n\t\r";
	cstring to = "ntr";

	char c;
	while((c = *str) != 0)
	{
		int index = StrCharIndex(from, c);
		if(index == -1)
		{
			if(c == quote)
				out += '\\';
			out += c;
		}
		else
		{
			out += '\\';
			out += to[index];
		}
		++str;
	}

	return out.c_str();
}

//=================================================================================================
cstring EscapeChar(char c)
{
	char* out = format_buf[format_marker];
	for(int i = 0; i < countof(escape_from); ++i)
	{
		if(c == escape_from[i])
		{
			strcpy_s(out, FORMAT_LENGTH, escape_to[i]);
			format_marker = (format_marker + 1) % FORMAT_STRINGS;
			return out;
		}
	}

	if(isprint(c))
	{
		out[0] = c;
		out[1] = 0;
	}
	else
		_snprintf_s(out, FORMAT_LENGTH, FORMAT_LENGTH - 1, "0x%u", (uint)c);

	format_marker = (format_marker + 1) % FORMAT_STRINGS;
	return out;
}

//=================================================================================================
cstring EscapeChar(char c, string& out)
{
	cstring esc = EscapeChar(c);
	out = esc;
	return out.c_str();
}

//=================================================================================================
bool Unescape(const string& str_in, uint pos, uint size, string& str_out)
{
	str_out.clear();
	str_out.reserve(str_in.length());

	cstring unesc = "nt\\\"'";
	cstring esc = "\n\t\\\"'";
	uint end = pos + size;

	for(; pos < end; ++pos)
	{
		if(str_in[pos] == '\\')
		{
			++pos;
			if(pos == size)
			{
				// character \ at end of string
				assert_once(0);
				str_out += '\\';
				return true;
			}

			int index = StrCharIndex(unesc, str_in[pos]);
			assert_once(index != -1); // unknown escape sequences are removed
			if(index != -1)
				str_out += esc[index];
		}
		else
			str_out += str_in[pos];
	}

	return true;
}

//=================================================================================================
bool StringInString(cstring s1, cstring s2)
{
	while(true)
	{
		if(*s1 == *s2)
		{
			++s1;
			++s2;
			if(*s2 == 0)
				return true;
		}
		else
			return false;
	}
}

//=================================================================================================
int ToNumber(cstring s, __int64& i, float& f)
{
	assert(s);

	i = 0;
	f = 0;
	uint diver = 10;
	uint digits = 0;
	char c;
	bool sign = false;
	if(*s == '-')
	{
		sign = true;
		++s;
	}

	while((c = *s) != 0)
	{
		if(c == '.')
		{
			++s;
			break;
		}
		else if(c >= '0' && c <= '9')
		{
			i *= 10;
			i += (int)c - '0';
		}
		else
			return 0;
		++s;
	}

	if(c == 0)
	{
		if(sign)
			i = -i;
		f = (float)i;
		return 1;
	}

	while((c = *s) != 0)
	{
		if(c == 'f')
		{
			if(digits == 0)
				return 0;
			break;
		}
		else if(c >= '0' && c <= '9')
		{
			++digits;
			f += ((float)((int)c - '0')) / diver;
			diver *= 10;
		}
		else
			return 0;
		++s;
	}
	f += (float)i;
	if(sign)
	{
		f = -f;
		i = -i;
	}
	return 2;
}

//=================================================================================================
bool ToInt(cstring s, int& result)
{
	__int64 i;
	float f;
	if(ToNumber(s, i, f) != 0 && InRange<int>(i))
	{
		result = (int)i;
		return true;
	}
	else
		return false;
}

//=================================================================================================
bool ToUint(cstring s, uint& result)
{
	__int64 i;
	float f;
	if(ToNumber(s, i, f) != 0 && InRange<uint>(i))
	{
		result = (uint)i;
		return true;
	}
	else
		return false;
}

//=================================================================================================
bool ToFloat(cstring s, float& result)
{
	__int64 i;
	float f;
	if(ToNumber(s, i, f) != 0)
	{
		result = f;
		return true;
	}
	else
		return false;
}

//=================================================================================================
bool ToBool(cstring s, bool& result)
{
	if(_stricmp(s, "true") == 0)
	{
		result = true;
		return true;
	}
	else if(_stricmp(s, "false") == 0)
	{
		result = false;
		return true;
	}
	else
	{
		int value;
		if(!ToInt(s, value) && value != 0 && value != 1)
			return false;
		result = (value == 1);
		return true;
	}
}

//=================================================================================================
int StrCharIndex(cstring chrs, char c)
{
	assert(chrs);

	int index = 0;
	do
	{
		if(*chrs == c)
			return index;
		++index;
		++chrs;
	} while(*chrs);

	return -1;
}

//=================================================================================================
char StrContains(cstring s, cstring chrs)
{
	assert(s && chrs);

	while(true)
	{
		char c = *s++;
		if(c == 0)
			return 0;
		cstring ch = chrs;
		while(true)
		{
			char c2 = *ch++;
			if(c2 == 0)
				break;
			if(c == c2)
				return c;
		}
	}
}

//=================================================================================================
char CharInStr(char c, cstring chrs)
{
	assert(chrs);

	while(true)
	{
		char c2 = *chrs++;
		if(c2 == 0)
			return 0;
		if(c == c2)
			return c;
	}
}
