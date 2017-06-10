#include "Pch.h"
#include "Core.h"
#include "DirectXIncl.h"

void MATRIX::Identity()
{
	_11 = 1.f; _12 = 0.f; _13 = 0.f; _14 = 0.f;
	_21 = 0.f; _22 = 1.f; _23 = 0.f; _24 = 0.f;
	_31 = 0.f; _32 = 0.f; _33 = 1.f; _34 = 0.f;
	_41 = 0.f; _42 = 0.f; _43 = 0.f; _44 = 1.f;
}

bool MATRIX::Inverse(MATRIX& result) const
{
	D3DXMATRIX* result_ptr = D3DXMatrixInverse((D3DXMATRIX*)&result, nullptr, (D3DXMATRIX*)this);
	return result_ptr != nullptr;
}

void MATRIX::Multiply(MATRIX& mat1, MATRIX& mat2)
{
	D3DXMatrixMultiply((D3DXMATRIX*)this, (D3DXMATRIX*)&mat1, (D3DXMATRIX*)&mat2);
}
