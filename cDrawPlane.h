#pragma once

class cPlane :public cObject
{
public:
	cPlane();
	cPlane(XMFLOAT4 plane);
	virtual ~cPlane();

	virtual void Update() override;

public:
	void SetPlane(XMVECTOR plane) { XMStoreFloat4(&m_plane, plane); }
	bool XM_CALLCONV Picking(PICKRAY ray, float& distance);

private:
	XMFLOAT4 m_plane;
	XMFLOAT2 m_viewSize;
	cDrawLines m_lines;
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

public:
	void AddPlanes();
	bool XM_CALLCONV Picking(PICKRAY ray, float& distance, cPlane** ppPlane);
private:
	vector<cPlane> m_planes;
};