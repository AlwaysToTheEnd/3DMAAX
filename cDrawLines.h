#pragma once

class cPlane;

class cDrawLines : public cDrawElement
{
public:
	static void MeshSetUp(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList);
	static void DisPosUploaders() { m_geo->DisPosUploaders(); }

private:
	static unique_ptr<MeshGeometry> m_geo;

public:
	cDrawLines(cPlane* plane);
	~cDrawLines();

	virtual void Update() override;
	virtual void SetRenderItem(shared_ptr<cRenderItem> renderItem) override;

private:
	cPlane*	m_drawPlane;
};