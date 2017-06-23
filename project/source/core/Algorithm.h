#pragma once

template<typename T>
struct TreeItem
{
	struct Iterator
	{
	private:
		T* current;
		int index, depth;
		bool up;

	public:
		Iterator(T* current, bool up) : current(current), index(-1), depth(0), up(up) {}

		bool operator != (const Iterator& it) const
		{
			return it.current != current;
		}

		void operator ++ ()
		{
			while(current)
			{
				if(index == -1)
				{
					if(current->childs.empty())
					{
						index = current->index;
						current = current->parent;
						--depth;
						if(up)
							break;
					}
					else
					{
						index = -1;
						current = current->childs.front();
						++depth;
						break;
					}
				}
				else
				{
					if(index + 1 == current->childs.size())
					{
						index = current->index;
						current = current->parent;
						--depth;
						if(up)
							break;
					}
					else
					{
						current = current->childs[index + 1];
						index = -1;
						++depth;
						break;
					}
				}
			}
		}

		T* GetCurrent() { return current; }
		int GetDepth() { return depth; }
		bool IsUp() { return index != -1; }
	};

	T* parent;
	vector<T*> childs;
	int index;

	Iterator begin(bool up = false)
	{
		return Iterator(static_cast<T*>(this), up);
	}

	Iterator end()
	{
		return Iterator(nullptr, false);
	}
};
