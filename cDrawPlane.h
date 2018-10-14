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

	virtual void Update() override;
	virtual void SetRenderItem(shared_ptr<cRenderItem> renderItem) override;
	bool XM_CALLCONV Picking(PICKRAY ray, float& distance, cPlane** ppPlane);
private:
	vector<cPlane> m_planes;
};