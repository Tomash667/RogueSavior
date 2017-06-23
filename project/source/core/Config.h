#pragma once

#include "Algorithm.h"

struct ConfigItem : TreeItem<ConfigItem>
{
	enum Type
	{
		Unknown,
		Container,
		Bool,
		Int,
		Float,
		String,
		ContainerTypes,
		Int2
	};

	template<typename T>
	struct Helper {};

#define Map(type, e) template<> struct Helper<type> { static const Type value = e; }
	Map(bool, Bool);
	Map(int, Int);
	Map(float, Float);
	Map(string, String);
	Map(INT2, Int2);
#undef  Map

	string id;
	Type type;
	int value;
	bool backing_field;

	~ConfigItem();

	template<typename T>
	void Set(T& val)
	{
		value = (int)&val;
		type = Helper<T>::value;
		backing_field = true;
	}

	template<typename T>
	T& Get()
	{
		if(backing_field)
			return *(T*)(int*)value;
		else
		{
			if(value == 0 && type == String)
			{
				string* s = new string;
				value = (int)s;
			}
			return *(T*)&value;
		}
	}

	ConfigItem* FindItem(const string& id);

	string GetPath();
};

class Config : ConfigItem
{
public:
	Config(cstring path);

	bool Save();
	bool Load();

	template<typename T>
	void Add(cstring id, T& value)
	{
		auto item = Add(id);
		item->Set(value);
	}

private:
	ConfigItem* Add(cstring id);

	Tokenizer t;
	string path;
	vector<string> splitted;
};
