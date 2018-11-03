#pragma once


class cUIOperWindow : public cUIObject
{
public:
	struct OperParameter
	{
		wstring dataName;
		DXGI_FORMAT dataFormat;
		void* data;
	};

public:
	cUIOperWindow();
	virtual ~cUIOperWindow();

	virtual void Build(shared_ptr<cRenderItem> renderItem) override;
	virtual void SetRenderState(bool value) override;
	 
	void AddParameter(wstring dataName, DXGI_FORMAT format,void* pData);
	void ClearParameters();

	void SetFontSize(int size) { m_fontSize = size; }
	bool IsMousePosInUIWindow();
private:
	virtual void UIUpdate() override;

private:
	vector<OperParameter>			m_operParameters;
	vector<shared_ptr<RenderFont>>	m_operFonts;
	wstring							m_currInputData;
	int								m_currParameterIndex;
	int								m_fontSize;
};
