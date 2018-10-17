#pragma once


class cUIOperWindow : public cUIObject
{
public:
	struct OperParameter
	{
		string dataName;
		DXGI_FORMAT dataFormat;
		void* data;
	};

public:
	cUIOperWindow();
	virtual ~cUIOperWindow();

	virtual void Build(shared_ptr<cRenderItem> renderItem) override;
	 
	void AddParameter(string dataName, DXGI_FORMAT format,void* pData);
	void ClearParameters() { m_operParameters.clear(); }

private:
	virtual void UIUpdate() override;

private:
	shared_ptr<cRenderItem>			m_renderItem;
	vector<OperParameter>			m_operParameters;
	vector<shared_ptr<RenderFont>>	m_operFonts;
};
