#pragma once

class cMesh :cObject
{
public:
	virtual void Build(shared_ptr<cRenderItem> renderItem) override;
	virtual void XM_CALLCONV Update(FXMMATRIX mat) override;
	virtual bool XM_CALLCONV Picking(PICKRAY ray, float& distance) override;
	virtual void IsRenderState(bool value) { m_renderInstance->m_isRenderOK = value; }

private:
	vector<unique_ptr<MeshGeometry>> m_geo;
};



class cDrawMesh : public cDrawElement
{
public:
	cDrawMesh();
	virtual ~cDrawMesh();

	virtual void SetRenderItem(shared_ptr<cRenderItem> renderItem) override;
	virtual bool Picking(cObject** ppObject) override;
	virtual cObject* AddElement() override;

private:

};

