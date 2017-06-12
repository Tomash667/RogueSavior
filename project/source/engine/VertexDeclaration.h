#pragma once

//-----------------------------------------------------------------------------
enum VertexDeclarationId
{
	VDI_DEFAULT, // Pos Tex Normal
	VDI_ANIMATED, // Pos Weights Indices Tex Normal
	VDI_TANGENT, // Pos Tex Normal Tangent Binormal
	VDI_ANIMATED_TANGENT, // Pos Weights Indices Tex Normal Tangent Binormal
	VDI_POS, // Pos
	VDI_MAX
};

//-----------------------------------------------------------------------------
struct VDefault
{
	VEC3 pos;
	VEC3 normal;
	VEC2 tex;
};

//-----------------------------------------------------------------------------
struct VAnimated
{
	VEC3 pos;
	float weights;
	uint indices;
	VEC3 normal;
	VEC2 tex;
};

//-----------------------------------------------------------------------------
struct VTangent
{
	VEC3 pos;
	VEC3 normal;
	VEC2 tex;
	VEC3 tangent;
	VEC3 binormal;
};

//-----------------------------------------------------------------------------
struct VAnimatedTangent
{
	VEC3 pos;
	float weights;
	uint indices;
	VEC3 normal;
	VEC2 tex;
	VEC3 tangent;
	VEC3 binormal;
};

//-----------------------------------------------------------------------------
struct VPos
{
	VEC3 pos;
};
