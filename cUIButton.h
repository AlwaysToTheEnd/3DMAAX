#pragma once

class cUIButton : public cUIObject
{
public:
	cUIButton();
	virtual ~cUIButton();

	virtual void SetActiveFunc(function<void(int)> func) override { m_ActiveFunc = func; }
	void SetParameter(UINT value) { assert(value <= (UINT)INT_MAX); m_Param = value; }

private:
	virtual void UIUpdate() override;

private:
	function<void(int)> m_ActiveFunc;
	int m_Param;
};

