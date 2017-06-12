#pragma once

namespace Color
{
	inline uint RGBA(uint r, uint g, uint b, uint a)
	{
		return ((a & 0xFF) << 24)
			| ((r & 0xFF) << 16)
			| ((g & 0xFF) << 8)
			| (b & 0xFF);
	}

	inline uint RGB(uint r, uint g, uint b)
	{
		return RGBA(r, g, b, 0xFF);
	}

	static const uint Black = RGB(0, 0, 0);
	static const uint White = RGB(255, 255, 255);
	static const uint Red = RGB(255, 0, 0);
	static const uint Green = RGB(0, 255, 0);
	static const uint Blue = RGB(0, 0, 255);
}
