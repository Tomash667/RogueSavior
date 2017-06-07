#pragma once

//-----------------------------------------------------------------------------
// Global temporary string
extern string g_tmp_string;

//-----------------------------------------------------------------------------
// Input string that can take cstring or const string&
template<bool AllowNull, uint MinLength>
struct _InString
{
	_InString(cstring s) : s(s)
	{
		if(!AllowNull)
			assert(s);
		if(MinLength > 0u)
			assert(s && strlen(s) >= MinLength);
	}

	_InString(const string& str) : s(str.c_str())
	{
		if(MinLength > 0u)
			assert(str.length() >= MinLength);
	}

	cstring s;
};

typedef _InString<false, 0> InString;
typedef _InString<false, 1> InString1;
typedef _InString<true, 0> InStringN;

//-----------------------------------------------------------------------------
// Compare cstrings functor
struct CstringComparer
{
	bool operator() (cstring s1, cstring s2) const
	{
		return _stricmp(s1, s2) > 0;
	}
};

//-----------------------------------------------------------------------------
// Text functions
//-----------------------------------------------------------------------------
// C format string to global bufer
cstring Format(cstring fmt, ...);
// C format string to global bufer using va_list
cstring FormatList(cstring fmt, va_list lis);
// Return string with first character uper-case, use global bufer
cstring Upper(cstring str);
// Split text on end of line, modify passed string
void SplitText(char* buf, vector<cstring>& lines);
// Split text on character, store splitted strings in vector
void Split(cstring str, vector<string>& splitted, char split_char, bool add_empty = false);
// Escape special characters and quote, use global bufer
cstring Escape(cstring str, char quote = '"');
// Escape special characters and qoute
cstring Escape(cstring str, string& out, char quote = '"');
// Escape single character, use global bufer
cstring EscapeChar(char c);
// Escape single character
cstring EscapeChar(char c, string& out);
// Reverse escape string operation on part of string
bool Unescape(const string& str_in, uint pos, uint length, string& str_out);
// Reverse escape string operation
inline bool Unescape(const string& str_in, string& str_out)
{
	return Unescape(str_in, 0u, str_in.length(), str_out);
}
// Return true if string contains other string
bool StringInString(cstring s1, cstring s2);
// Convert string to number, returns type 0-broken, 1-int, 2-float
int ToNumber(cstring s, __int64& i, float& f);
// Convert string to int
bool ToInt(cstring s, int& result);
// Convert string to uint
bool ToUint(cstring s, uint& result);
// Convert string to float
bool ToFloat(cstring s, float& result);
// Convert string to bool
bool ToBool(cstring s, bool& result);
// Left trim string
inline string& LTrim(string& str)
{
	str.erase(str.begin(), find_if(str.begin(), str.end(), [](char& ch)->bool { return !isspace(ch); }));
	return str;
}
// Right trim string
inline string& RTrim(string& str)
{
	str.erase(find_if(str.rbegin(), str.rend(), [](char& ch)->bool { return !isspace(ch); }).base(), str.end());
	return str;
}
// Trim string from both sides
inline string& Trim(string& str)
{
	return LTrim(RTrim(str));
}
// Return trimmed string
inline string Trimmed(const string& str)
{
	string s = str;
	Trim(s);
	return s;
}
// Join objects to string, Action return object converted to string
template<typename T, class Action>
inline void Join(const vector<T>& v, string& s, cstring separator, Action action)
{
	assert(separator);

	if(v.empty())
		return;
	if(v.size() == 1)
		s += action(v.front());
	else
	{
		for(vector<T>::const_iterator it = v.begin(), end = v.end() - 1; it != end; ++it)
		{
			s += action(*it);
			s += separator;
		}
		s += action(*(v.end() - 1));
	}
}
// Return index of first matching character in string or -1
int StrCharIndex(cstring chrs, char c);
// Checks if string contains any character and return it or 0
char StrContains(cstring s, cstring chrs);
// Return char contained in string or 0
char CharInStr(char c, cstring chrs);
