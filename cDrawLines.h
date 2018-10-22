#pragma once

class cPlane;

class cLine :public cObject
{
public:
	cLine();
	virtual ~cLine();

	virtual void XM_CALLCONV Update(FXMMATRIX mat) override;
	virtual bool XM_CALLCONV Picking(PICKRAY ray, float& distance) override;
	void SetFirstDot(cObject* object) { m_hostObject[0] = object; }
	void SetSecendDot(cObject* object) { m_hostObject[1] = object; }
private:
	cObject*	m_hostObject[2];
	float		m_lineDistacne;
};

class cDrawLines : public cDrawElement
{
public:
	static void MeshSetUp(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList);
	static void DisPosUploaders() { m_geo->DisPosUploaders(); }

private:
	static unique_ptr<MeshGeometry> m_geo;

public:
	cDrawLines();
	~cDrawLines();

	virtual void SetRenderItem(shared_ptr<cRenderItem> renderItem) override;
	virtual bool Picking(cObject** ppObject) override;
	virtual cObject* AddElement() override;

private:
};