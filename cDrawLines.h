#pragma once

class cPlane;

class cLine :public cObject
{
public:
	cLine();
	virtual ~cLine();

	virtual void XM_CALLCONV Update(FXMMATRIX mat) override;
	virtual bool XM_CALLCONV Picking(PICKRAY ray, float& distance) override;
	void SetFirstDot(cDot* object) { m_hostObject[0] = object; }
	void SetSecendDot(cDot* object) { m_hostObject[0]->SetNextDots(object); m_hostObject[1] = object; }
private:
	cDot *	m_hostObject[2];
	float	m_lineDistacne;
};

class cDrawLines : public cDrawElement
{
public:
	static void MeshSetUp();

private:
	static MeshGeometry* m_geo;

public:
	cDrawLines();
	virtual ~cDrawLines();

	virtual void SetRenderItem(shared_ptr<cRenderItem> renderItem) override;
	virtual bool Picking(cObject** ppObject) override;
	virtual cObject* AddElement() override;

private:
};