#pragma once

class Control
{
public:
	virtual ~Control() {}
	virtual void Draw() {}

	void SetPos(const VEC2& pos);
	void SetSize(const VEC2& size);
	const VEC2& GetPos() const { return pos; }
	const VEC2& GetSize() const { return size; }

private:
	VEC2 pos, size;
};
