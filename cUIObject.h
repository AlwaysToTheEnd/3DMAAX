#pragma once

class cUIObject : public cObject
{
public:
	static void MeshSetUp(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList);
	static void DisPosUploaders() { m_geo->DisPosUploaders(); }

protected:
	static unique_ptr<MeshGeometry> m_geo;

public:
	cUIObject();
	virtual ~cUIObject();

	virtual void Build(shared_ptr<cRenderItem> renderItem) override;
	virtual void Update(FXMMATRIX mat) override;
	virtual void UIUpdate() {}

	void AddChild(cUIObject* ui) { m_ChildObject.push_back(unique_ptr<cUIObject>(ui)); }
	void SetPos(XMFLOAT3 pos) { m_Pos = pos; }
	void SetSize(XMFLOAT2 size) { m_Size.x = size.x / 2; m_Size.y = size.y / 2; }
	void RenderUI(bool value);

protected:
	XMFLOAT3 m_Pos;
	XMFLOAT2 m_Size;
	shared_ptr<cRenderItem> m_renderItem;
	vector<unique_ptr<cUIObject>> m_ChildObject;
};