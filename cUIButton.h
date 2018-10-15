#pragma once

class cUIButton : public cUIObject
{
public:
	cUIButton();
	virtual ~cUIButton();

	virtual void SetActiveFunc(function<void()> func) { m_ActiveFunc = func; }

private:
	virtual void UIUpdate() override;

private:
	function<void()> m_ActiveFunc;
};

