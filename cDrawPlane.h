#pragma once

class cDrawPlane :public cObject
{
public:
	cDrawPlane();
	cDrawPlane(XMFLOAT4 plane);
	virtual ~cDrawPlane();

	void SetPlane(XMVECTOR plane);
	bool XM_CALLCONV Picking(PICKRAY ray, float& distance);
private:
	XMFLOAT4 m_plane;
};