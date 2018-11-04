#pragma once

class cUIObject : public cObject
{
	friend class cUIObjectMG;
public:
	cUIObject();
	virtual ~cUIObject();

	virtual void XM_CALLCONV Update(FXMMATRIX mat) override;
	virtual void SetActiveFunc(function<void(int)> func) {};

	void SetPos(XMFLOAT3 pos);
	void SetSize(XMFLOAT2 size);
	
	vector<unique_ptr<cUIObject>>& GetChilds() { return m_ChildObject; }
	const string& GetUIName() { return m_UIName; } 
	void SetUIName(string name) { m_UIName = name; }
	virtual bool IsMousePosInUI() = 0;
	virtual bool InputDataUpdate() = 0;

protected:
	virtual void UIUpdate() = 0;
	virtual void SetRenderState(bool value) override;

protected:
	string							m_UIName;
	vector<unique_ptr<cUIObject>>	m_ChildObject;
};