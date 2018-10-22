#pragma once

class cUIObject : public cObject
{
public:
	static void MeshSetUp(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList);
	static void DisPosUploaders() { m_geo->DisPosUploaders(); }
	static void SetGeoAtRenderItem(shared_ptr<cRenderItem> renderItem);

private:
	static unique_ptr<MeshGeometry> m_geo;

public:
	cUIObject();
	virtual ~cUIObject();

	virtual void XM_CALLCONV Update(FXMMATRIX mat) override;
	virtual void SetActiveFunc(function<void(int)> func) {};

	void SetPos(XMFLOAT3 pos);
	void SetSize(XMFLOAT2 size);
	
	vector<unique_ptr<cUIObject>>& GetChilds() { return m_ChildObject; }

protected:
	virtual void UIUpdate() = 0;

protected:
	vector<unique_ptr<cUIObject>> m_ChildObject;
};