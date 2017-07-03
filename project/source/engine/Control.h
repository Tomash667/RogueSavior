#pragma once

class Container;

class Control
{
	friend class Container;
public:
	virtual ~Control() {}
	virtual void Draw() {}
	virtual void Update(float dt) {}

	Container* GetParent() const { return parent; }
	const VEC2& GetPos() const { return pos; }
	const VEC2& GetSize() const { return size; }
	void SetPos(const VEC2& pos);
	void SetSize(const VEC2& size);

protected:
	Container* parent;
	VEC2 pos, size;
};
