#include "Pch.h"
#include "Core.h"
#include "Config.h"
#include "Tokenizer.h"

struct ConfigItemInfo
{
	cstring name;
};

static ConfigItemInfo config_item_infos[] = {
	"unknown",
	"container",
	"bool",
	"int",
	"float",
	"string",
	"container types",
	"INT2"
};

ConfigItem::~ConfigItem()
{
	DeleteElements(childs);
	if(!backing_field && type == String && value != 0)
	{
		string* s = (string*)value;
		delete s;
	}
}

ConfigItem* ConfigItem::FindItem(const string& id)
{
	for(auto item : childs)
	{
		if(item->id == id)
			return item;
	}

	return nullptr;
}

// poor performance, use only in exceptions
string ConfigItem::GetPath()
{
	vector<ConfigItem*> link;
	auto item = this;
	while(item)
	{
		link.push_back(item);
		item = item->parent;
	}

	string path;
	bool first = true;
	for(auto it = link.rbegin(), end = link.rend(); it != end; ++it)
	{
		if(first)
			first = false;
		else
			path += '.';
		path += (*it)->id;
	}

	return path;
}

Config::Config(cstring _path)
{
	assert(_path);
	path = _path;
	type = Container;
}

ConfigItem* Config::Add(cstring id)
{
	assert(id);

	splitted.clear();
	Split(id, splitted, '.');

	ConfigItem* top = this;
	for(uint i = 0; i < splitted.size() - 1; ++i)
	{
		auto& part_id = splitted[i];
		auto item = top->FindItem(part_id);
		if(!item)
		{
			item = new ConfigItem;
			item->backing_field = false;
			item->id = splitted[i];
			item->parent = top;
			item->index = (int)top->childs.size();
			item->type = ConfigItem::Container;
			item->value = 0;
			top->childs.push_back(item);
		}
		top = item;
	}

	auto item = new ConfigItem;
	item->backing_field = false;
	item->id = splitted.back();
	item->parent = top;
	item->index = (int)top->childs.size();
	item->type = ConfigItem::Unknown;
	item->value = 0;
	top->childs.push_back(item);

	return item;
}

bool Config::Load()
{
	t.FromFile(path.c_str());

	try
	{
		ConfigItem* parent = this;
		t.Next();
		t.AssertSymbol('{');
		t.Next();

		while(true)
		{
			// closing brace
			if(t.IsSymbol('}'))
			{
				parent = parent->parent;
				if(!parent)
					break;
				continue;
			}

			// item id
			const string& id = t.MustGetString();
			auto item = parent->FindItem(id);
			if(!item)
			{
				item = new ConfigItem;
				item->id = id;
				item->type = ConfigItem::Unknown;
				item->parent = parent;
				item->value = 0;
				item->backing_field = false;
				parent->childs.push_back(item);
			}
			t.Next();

			// :
			t.AssertSymbol(':');
			t.Next();

			// item or {
			auto token = t.GetToken();
			ConfigItem::Type type;
			switch(token)
			{
			case tokenizer::T_BOOL:
				type = ConfigItem::Bool;
				break;
			case tokenizer::T_INT:
				type = ConfigItem::Int;
				break;
			case tokenizer::T_FLOAT:
				type = ConfigItem::Float;
				break;
			case tokenizer::T_STRING:
				type = ConfigItem::String;
				break;
			case tokenizer::T_SYMBOL:
				if(t.GetSymbol() == '{')
					type = ConfigItem::Container;
				else
					t.Unexpected();
				break;
			default:
				t.Unexpected();
				break;
			}

			// verify type
			if(item->type == ConfigItem::Unknown)
				item->type = type;
			else if(item->type < ConfigItem::ContainerTypes && type != ConfigItem::Container)
				t.Throw("Config item '%s' has invalid type %s (expecting %s).", item->GetPath().c_str(), config_item_infos[type].name, config_item_infos[item->type].name);

			// set value
			switch(item->type)
			{
			case ConfigItem::Container:
				parent = item;
				break;
			case ConfigItem::Bool:
				item->Get<bool>() = t.GetBool();
				break;
			case ConfigItem::Int:
				item->Get<int>() = t.GetInt();
				break;
			case ConfigItem::Float:
				item->Get<float>() = t.GetFloat();
				break;
			case ConfigItem::String:
				item->Get<string>() = t.GetString();
				break;
			case ConfigItem::Int2:
				{
					auto& i2 = item->Get<INT2>();
					t.Next();
					i2.x = t.MustGetInt();
					t.Next();
					i2.y = t.MustGetInt();
					t.Next();
					t.AssertSymbol('}');
				}
				break;
			}
			t.Next();
		}

		return true;
	}
	catch(Tokenizer::Exception&)
	{
		return false;
	}
}

bool Config::Save()
{
	TextWriter f(path);
	if(!f)
		return false;

	f << "{\n";
	ConfigItem::Type prev_type;
	
	for(auto it = begin(true), it_end = end(); it != it_end; ++it)
	{
		auto current = it.GetCurrent();
		if(current == this)
			continue;

		int depth = it.GetDepth();
		if(it.IsUp())
		{
			if(prev_type == ConfigItem::Container)
			{
				f << '\n';
				for(int i = 0; i < depth - 1; ++i)
					f << '\t';
				f << "}\n";
			}
			else
				f << '\n';
			prev_type = ConfigItem::Container;
			continue;
		}

		for(int i = 0; i < depth; ++i)
			f << '\t';

		f << Format("\"%s\": ", current->id.c_str());
		prev_type = current->type;
		switch(current->type)
		{
		case ConfigItem::Container:
			f << "{\n";
			break;
		case ConfigItem::Bool:
			f << (current->Get<bool>() ? "true" : "false");
			break;
		case ConfigItem::Int:
			f << Format("%d", current->Get<int>());
			break;
		case ConfigItem::Float:
			f << Format("%g", current->Get<float>());
			break;
		case ConfigItem::String:
			f << Format("\"%s\"", Escape(current->Get<string>().c_str()));
			break;
		case ConfigItem::Int2:
			{
				auto& i2 = current->Get<INT2>();
				f << Format("{ %d %d }", i2.x, i2.y);
			}
			break;
		}
	}

	f << "\n}\n";

	return true;
}
