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
	static void MeshSetUp(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList);
	static void DisPosUploaders() { m_geo->DisPosUploaders(); }

private:
	static unique_ptr<MeshGeometry> m_geo;

public:
	cDrawPlane();
	virtual ~cDrawPlane();

	virtual void Update() override;
	virtual void SetRenderItem(shared_ptr<cRenderItem> renderItem) override;
	virtual bool Picking(cObject** ppObject) override;
	virtual cObject* AddElement() override;

public:
	void DeletePlane();
	void XM_CALLCONV SetPlaneInfo(XMVECTOR plane);

private:
	vector<cPlane> m_planes;
};