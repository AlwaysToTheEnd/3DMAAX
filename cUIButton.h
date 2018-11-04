#pragma once

class cUIButton : public cUIObject
{
public:
	cUIButton();
	virtual ~cUIButton();

	virtual void SetActiveFunc(function<void(int)> func) override { m_ActiveFunc = func; }
	void SetParameter(UINT value) { assert(value <= (UINT)INT_MAX); m_Param = value; }
	virtual bool IsMousePosInUI() override;

private:
	virtual void UIUpdate() override;

private:
	int					m_Param;
	RECT				m_buttonRect;
	function<void(int)>	m_ActiveFunc;
};

