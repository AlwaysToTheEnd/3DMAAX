#pragma once

class cUIButton : public cUIObject
{
public:
	cUIButton();
	virtual ~cUIButton();

	virtual void SetActiveFunc(function<void(int)> func) { m_ActiveFunc = func; }

private:
	virtual void UIUpdate() override;

private:
	function<void(int)> m_ActiveFunc;
	int m_Param;
};

