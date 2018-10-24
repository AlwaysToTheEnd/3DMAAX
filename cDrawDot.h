#pragma once

class cDot :public cObject
{
public:
	cDot();
	virtual ~cDot();

	virtual void XM_CALLCONV Update(FXMMATRIX mat) override;
	virtual bool XM_CALLCONV Picking(PICKRAY ray, float& distance) override;
	void SetHostObject(cObject* object) { m_hostObject = object; }
	cObject* GetHostObject() { return m_hostObject; }

private:
	cObject*		m_hostObject;
};

class cDrawDot : public cDrawElement
{
public:
	static void MeshSetUp();

private:
	static MeshGeometry* m_geo;

public:
	cDrawDot();
	virtual ~cDrawDot();

	virtual void SetRenderItem(shared_ptr<cRenderItem> renderItem) override;
	virtual cObject* AddElement() override;

private:
	
};

