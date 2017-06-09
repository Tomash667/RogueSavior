#include "Pch.h"
#include "Core.h"
#include "WindowsIncl.h"
#include <chrono>
#include <Shellapi.h>

static_assert(INVALID_HANDLE_VALUE == INVALID_FILE, "INVALID_HANDLE_VALUE is not -1");

char BUF[256];

//=================================================================================================
bool DeleteDirectory(cstring dir)
{
	assert(dir);

	char* s = BUF;
	char c;
	while((c = *dir++) != 0)
		*s++ = c;
	*s++ = 0;
	*s = 0;

	SHFILEOPSTRUCT op = {
		nullptr,
		FO_DELETE,
		BUF,
		nullptr,
		FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT,
		FALSE,
		nullptr,
		nullptr
	};

	return SHFileOperation(&op) == 0;
}

//=================================================================================================
bool DirectoryExists(cstring dir)
{
	assert(dir);

	DWORD attrib = GetFileAttributes(dir);
	if(attrib == INVALID_FILE_ATTRIBUTES)
		return false;

	return IS_SET(attrib, FILE_ATTRIBUTE_DIRECTORY);
}

//=================================================================================================
bool FileExists(cstring filename)
{
	assert(filename);

	DWORD attrib = GetFileAttributes(filename);
	if(attrib == INVALID_FILE_ATTRIBUTES)
		return false;

	return !IS_SET(attrib, FILE_ATTRIBUTE_DIRECTORY);
}

//=================================================================================================
bool FindFiles(cstring pattern, delegate<bool(const FindData&)> action, bool exclude_special)
{
	assert(pattern);

	static FindData user_find_data;

	WIN32_FIND_DATA find_data;
	HANDLE find = FindFirstFile(pattern, &find_data);
	if(find == INVALID_HANDLE_VALUE)
		return false;

	do
	{
		// exclude special files or directories
		if((exclude_special && (strcmp(find_data.cFileName, ".") == 0 || strcmp(find_data.cFileName, "..") == 0))
			|| IS_SET(find_data.dwFileAttributes, FILE_ATTRIBUTE_DIRECTORY))
			continue;

		// callback
		user_find_data.filename = find_data.cFileName;
		user_find_data.is_dir = IS_SET(find_data.dwFileAttributes, FILE_ATTRIBUTE_DIRECTORY);
		if(!action(user_find_data))
			break;

	} while(FindNextFile(find, &find_data) != 0);

	DWORD result = GetLastError();
	FindClose(find);
	return (result == ERROR_NO_MORE_FILES);
}

//=================================================================================================
void Execute(cstring file)
{
	assert(file);

	ShellExecute(nullptr, "open", file, nullptr, nullptr, SW_SHOWNORMAL);
}

//=================================================================================================
bool LoadFileToString(cstring path, string& str, uint max_size)
{
	HANDLE file = CreateFile(path, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if(file == INVALID_HANDLE_VALUE)
		return false;

	uint file_size = (uint)GetFileSize(file, nullptr);
	uint size = min(file_size, max_size);
	str.resize(size);

	DWORD read;
	ReadFile(file, (char*)str.c_str(), size, &read, nullptr);

	CloseHandle(file);

	return size == read;
}

//=================================================================================================
void Crypt(char* inp, uint inplen, cstring key, uint keylen)
{
	//we will consider size of sbox 256 bytes
	//(extra byte are only to prevent any mishep just in case)
	char Sbox[257], Sbox2[257];
	uint i, j, t, x;

	//this unsecured key is to be used only when there is no input key from user
	char temp, k;
	i = j = t = x = 0;
	temp = k = 0;

	//always initialize the arrays with zero
	ZeroMemory(Sbox, sizeof(Sbox));
	ZeroMemory(Sbox2, sizeof(Sbox2));

	//initialize sbox i
	for(i = 0; i < 256U; i++)
	{
		Sbox[i] = (char)i;
	}

	//initialize the sbox2 with user key
	for(i = 0; i < 256U; i++)
	{
		if(j == keylen)
		{
			j = 0;
		}
		Sbox2[i] = key[j++];
	}

	j = 0; //Initialize j
		   //scramble sbox1 with sbox2
	for(i = 0; i < 256; i++)
	{
		j = (j + (uint)Sbox[i] + (uint)Sbox2[i]) % 256U;
		temp = Sbox[i];
		Sbox[i] = Sbox[j];
		Sbox[j] = temp;
	}

	i = j = 0;
	for(x = 0; x < inplen; x++)
	{
		//increment i
		i = (i + 1U) % 256U;
		//increment j
		j = (j + (uint)Sbox[i]) % 256U;

		//Scramble SBox #1 further so encryption routine will
		//will repeat itself at great interval
		temp = Sbox[i];
		Sbox[i] = Sbox[j];
		Sbox[j] = temp;

		//Get ready to create pseudo random  byte for encryption key
		t = ((uint)Sbox[i] + (uint)Sbox[j]) % 256U;

		//get the random byte
		k = Sbox[t];

		//xor with the data and done
		inp[x] = (inp[x] ^ k);
	}
}

//=================================================================================================
cstring FilenameFromPath(const string& path)
{
	uint pos = path.find_last_of('/');
	if(pos == string::npos)
		return path.c_str();
	else
		return path.c_str() + pos + 1;
}

//=================================================================================================
cstring FilenameFromPath(cstring path)
{
	assert(path);
	cstring filename = strrchr(path, '/');
	if(filename)
		return filename + 1;
	else
		return path;
}

//=================================================================================================
string GetCompileTime()
{
	// open exe
	int len = GetModuleFileName(nullptr, BUF, 256);
	HANDLE file;
	if(len == 256)
	{
		char* b = new char[2048];
		GetModuleFileName(nullptr, b, 2048);
		file = CreateFile(b, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
		delete[] b;
	}
	else
		file = CreateFile(BUF, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if(file == INVALID_HANDLE_VALUE)
		return "0";

	// read position of header
	int offset;
	DWORD read;
	SetFilePointer(file, 0x3C, nullptr, FILE_BEGIN);
	ReadFile(file, &offset, sizeof(offset), &read, nullptr);
	SetFilePointer(file, offset + 8, nullptr, FILE_BEGIN);

	// read time
	static_assert(sizeof(time_t) == 8, "time_t must be 64 bit");
	union TimeUnion
	{
		time_t t;
		struct
		{
			uint low;
			uint high;
		};
	};
	TimeUnion datetime = { 0 };
	ReadFile(file, &datetime.low, sizeof(datetime.low), &read, nullptr);
	CloseHandle(file);

	// format date
	tm t;
	errno_t err = gmtime_s(&t, &datetime.t);
	if(err != 0)
		return "0";
	strftime(BUF, 256, "%Y-%m-%d %H:%M:%S", &t);
	return BUF;
}


FileReader::~FileReader()
{
	if(own_handle && file != INVALID_FILE)
	{
		CloseHandle((HANDLE)file);
		file = INVALID_FILE;
	}
}

bool FileReader::Open(cstring filename)
{
	assert(filename);
	if(file != INVALID_FILE)
		CloseHandle((HANDLE)file);
	file = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	own_handle = true;
	return (file != INVALID_FILE);
}

bool FileReader::Read(void* ptr, uint size)
{
	DWORD read;
	ReadFile((HANDLE)file, ptr, size, &read, nullptr);
	return (size == read);
}

void FileReader::Skip(int bytes)
{
	SetFilePointer((HANDLE)file, bytes, nullptr, FILE_CURRENT);
}

uint FileReader::GetSize() const
{
	return GetFileSize((HANDLE)file, nullptr);
}

FileWriter::~FileWriter()
{
	if(own_handle && file != INVALID_FILE)
	{
		CloseHandle((HANDLE)file);
		file = INVALID_FILE;
	}
}

bool FileWriter::Open(cstring filename)
{
	assert(filename);
	if(file != INVALID_FILE)
		CloseHandle((HANDLE)file);
	file = CreateFile(filename, GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
	return (file != INVALID_FILE);
}

void FileWriter::Write(const void* ptr, uint size)
{
	DWORD written;
	WriteFile((HANDLE)file, ptr, size, &written, nullptr);
	assert(size == written);
}

void FileWriter::Flush()
{
	FlushFileBuffers((HANDLE)file);
}

uint FileWriter::GetSize() const
{
	return GetFileSize((HANDLE)file, nullptr);
}
