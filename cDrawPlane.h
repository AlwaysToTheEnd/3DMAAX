#pragma once

class cPlane :public cObject
{
public:
	cPlane();
	virtual ~cPlane();

	virtual bool XM_CALLCONV Picking(PICKRAY ray, float& distance) override;

private:
};

class cDrawPlane : public cDrawElement
{
public:
	static void MeshSetUp();

private:
	static MeshGeometry* m_geo;

public:
	cDrawPlane();
	virtual ~cDrawPlane();

	virtual void SetRenderItem(shared_ptr<cRenderItem> renderItem) override;
	virtual cObject* AddElement() override;

private:
};