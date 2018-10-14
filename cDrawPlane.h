#pragma once

class cPlane :public cObject
{
public:
	cPlane();
	cPlane(XMFLOAT4 plane);
	virtual ~cPlane();

	void Update();
	void SetPlane(XMVECTOR plane);
	bool XM_CALLCONV Picking(PICKRAY ray, float& distance);
private:
	XMFLOAT4 m_plane;
};

class cDrawPlane : public cDrawElement
{
public:
	cDrawPlane();
	virtual ~cDrawPlane();

	bool XM_CALLCONV Picking(PICKRAY ray, float& distance, cPlane** ppPlane);
private:

};