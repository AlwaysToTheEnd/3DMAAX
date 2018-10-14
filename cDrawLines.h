#pragma once

class cDrawLines : public cDrawElement
{
public:
	static void LineMeshSetUp(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList);
	static void DisPosUploaders();

public:
	cDrawLines(cPlane* plane);
	~cDrawLines();

	void Update();

private:
	static unique_ptr<MeshGeometry> m_geo;
	cPlane*	m_drawPlane;
};