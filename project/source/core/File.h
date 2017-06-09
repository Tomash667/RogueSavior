#pragma once

#include "ForwardInternal.h"

//-----------------------------------------------------------------------------
// Temporary buffer
extern char BUF[256];

//-----------------------------------------------------------------------------
static const HANDLE INVALID_FILE = (HANDLE)-1;

//-----------------------------------------------------------------------------
// File functions
//-----------------------------------------------------------------------------
// Delete directory.
bool DeleteDirectory(cstring dir);
// Check if directory exists.
bool DirectoryExists(cstring dir);
// Check if file exists.
bool FileExists(cstring filename);
// Find files matching pattern, return false from func to stop.
struct FindData
{
	string filename;
	bool is_dir;
};
bool FindFiles(cstring pattern, delegate<bool(const FindData&)> action, bool exclude_special = true);
// Call ShellExecute on file
void Execute(cstring file);
// Get filename from path, returned string use same string as argument
cstring FilenameFromPath(const string& path);
cstring FilenameFromPath(cstring path);
// Load text file to string (whole or up to max size)
bool LoadFileToString(cstring path, string& str, uint max_size = (uint)-1);
// Simple encryption (pass encrypted to decrypt data)
void Crypt(char* inp, uint inplen, cstring key, uint keylen);
// Read compile time from exe
string GetCompileTime();

//-----------------------------------------------------------------------------
// Binary file reader
class FileReader
{
public:
	FileReader() : file(INVALID_FILE), own_handle(false)
	{
	}
	explicit FileReader(HANDLE file) : file(file), own_handle(false)
	{
	}
	explicit FileReader(cstring filename) : own_handle(false)
	{
		Open(filename);
	}
	explicit FileReader(const string& filename) : own_handle(false)
	{
		Open(filename);
	}

	~FileReader();

	bool Open(cstring filename);
	bool Open(const string& filename)
	{
		return Open(filename.c_str());
	}
	uint GetSize() const;
	bool Read(void* ptr, uint size);
	void Skip(int bytes);

	bool IsOpen() const
	{
		return file != INVALID_FILE;
	}

	template<typename T>
	T Read()
	{
		T a;
		Read(&a, sizeof(T));
		return a;
	}

	template<typename T>
	bool Read(T& a)
	{
		return Read(&a, sizeof(a));
	}

	template<typename T, typename T2>
	bool ReadCasted(T2& a)
	{
		T b;
		if(!Read<T>(b))
			return false;
		a = (T2)b;
		return true;
	}

	bool ReadStringBUF()
	{
		byte len = Read<byte>();
		if(len == 0)
		{
			BUF[0] = 0;
			return true;
		}
		else
		{
			BUF[len] = 0;
			return Read(BUF, len);
		}
	}


	template<typename T>
	void Skip()
	{
		Skip(sizeof(T));
	}

	bool ReadString1(string& s)
	{
		byte len;
		if(!Read(len))
			return false;
		s.resize(len);
		return Read((char*)s.c_str(), len);
	}

	bool ReadString2(string& s)
	{
		word len;
		if(!Read(len))
			return false;
		s.resize(len);
		return Read((char*)s.c_str(), len);
	}

	template<typename T>
	bool operator >> (T& a)
	{
		return Read(&a, sizeof(a));
	}

	bool operator >> (string& s)
	{
		return ReadString1(s);
	}

	operator bool() const
	{
		return IsOpen();
	}

	bool ReadToString(string& s)
	{
		uint size = GetSize();
		s.resize(size);
		return Read((char*)s.c_str(), size);
	}

	template<typename T>
	void ReadVector1(vector<T>& v)
	{
		byte count;
		Read(count);
		v.resize(count);
		if(count)
			Read(&v[0], sizeof(T)*count);
	}

	template<typename T>
	void ReadVector2(vector<T>& v)
	{
		word count;
		Read(count);
		v.resize(count);
		if(count)
			Read(&v[0], sizeof(T)*count);
	}

private:
	HANDLE file;
	bool own_handle;
};

//-----------------------------------------------------------------------------
// Binary file writer
class FileWriter
{
public:
	FileWriter() : file(INVALID_FILE), own_handle(false)
	{
	}
	explicit FileWriter(HANDLE file) : file(file), own_handle(false)
	{
	}
	explicit FileWriter(cstring filename) : own_handle(false)
	{
		Open(filename);
	}
	explicit FileWriter(const string& filename) : own_handle(false)
	{
		Open(filename);
	}

	~FileWriter();

	bool Open(cstring filename);
	bool Open(const string& filename)
	{
		return Open(filename.c_str());
	}
	void Write(const void* ptr, uint size);
	void Flush();
	uint GetSize() const;

	bool IsOpen() const
	{
		return file != INVALID_FILE;
	}

	template<typename T>
	void operator << (const T& a)
	{
		Write(&a, sizeof(a));
	}

	template<typename T>
	void Write(const T& a)
	{
		Write(&a, sizeof(a));
	}

	template<typename T, typename T2>
	void WriteCasted(const T2& a)
	{
		Write(&a, sizeof(T));
	}

	void WriteString1(const string& s)
	{
		int length = s.length();
		assert(length < 256);
		WriteCasted<byte>(length);
		if(length)
			Write(s.c_str(), length);
	}

	void WriteString1(cstring str)
	{
		assert(str);
		int length = strlen(str);
		assert(length < 256);
		WriteCasted<byte>(length);
		if(length)
			Write(str, length);
	}

	void WriteString2(const string& s)
	{
		int length = s.length();
		assert(length < 256 * 256);
		WriteCasted<word>(length);
		if(length)
			Write(s.c_str(), length);
	}

	void WriteString2(cstring str)
	{
		assert(str);
		int length = strlen(str);
		assert(length < 256 * 256);
		Write<word>(length);
		if(length)
			Write(str, length);
	}

	void operator << (const string& s)
	{
		WriteString1(s);
	}

	void operator << (cstring str)
	{
		assert(str);
		WriteString1(str);
	}

	operator bool() const
	{
		return IsOpen();
	}

	void Write0()
	{
		WriteCasted<byte>(0);
	}

	template<typename T>
	void WriteVector1(const vector<T>& v)
	{
		WriteCasted<byte>(v.size());
		if(!v.empty())
			Write(&v[0], sizeof(T)*v.size());
	}

	template<typename T>
	void WriteVector2(const vector<T>& v)
	{
		WriteCasted<word>(v.size());
		if(!v.empty())
			Write(&v[0], sizeof(T)*v.size());
	}

private:
	HANDLE file;
	bool own_handle;
};

//-----------------------------------------------------------------------------
// Text writer to file
class TextWriter
{
public:
	TextWriter(cstring filename) : file(filename)
	{
	}
	TextWriter(const string& filename) : file(filename)
	{
	}
	
	operator bool() const
	{
		return file.IsOpen();
	}

	void operator << (const string& str)
	{
		file.Write(str.data(), str.length());
	}

	void operator << (cstring str)
	{
		file.Write(str, strlen(str));
	}

	void operator << (char c)
	{
		file.Write(&c, 1);
	}

private:
	FileWriter file;
};
