#pragma once

enum UIOPERDATATYPE
{
	OPERDATATYPE_INDEX,
	OPERDATATYPE_BOOL,
	OPERDATATYPE_FLOAT,
	OPERDATATYPE_FUNC,
	OPERDATATYPE_FUNC_INT_PARAM,
	OPERDATATYPE_NONE,
};

class cUIOperWindow : public cUIObject
{
public:
	struct OperParameter
	{
		wstring dataName;
		UIOPERDATATYPE dataFormat;
		function<void()> func = nullptr;
		function<void(UINT64)> param_Func = nullptr;
		void* data = nullptr;
		UINT64 funcParam = 0;
	};

public:
	cUIOperWindow();
	virtual ~cUIOperWindow();

	virtual void Build(shared_ptr<cRenderItem> renderItem) override;
	virtual void SetRenderState(bool value) override;
	 
	void AddParameter(wstring dataName, UIOPERDATATYPE format, void* pData);
	void AddParameter(wstring dataName, UIOPERDATATYPE format, function<void()> func);
	void AddParameter(wstring dataName, UIOPERDATATYPE format, function<void(UINT64)> func, UINT64 param);
	void ClearParameters();

	void SetFontSize(int size) { m_fontSize = size; }
	bool IsMousePosInUIWindow();
private:
	virtual void UIUpdate() override;
	void ButtonSet();
	void KeyboardDataInput();
	void EnterAction();

private:
	vector<OperParameter>			m_operParameters;
	vector<shared_ptr<RenderFont>>	m_operFonts;
	wstring							m_currInputData;
	int								m_currParameterIndex;
	int								m_fontSize;
};
