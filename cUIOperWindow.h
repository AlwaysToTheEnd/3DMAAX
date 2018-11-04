#pragma once

enum UIOPERDATATYPE
{
	OPERDATATYPE_INDEX,
	OPERDATATYPE_BOOL,
	OPERDATATYPE_FLOAT,
	OPERDATATYPE_FUNC,
	OPERDATATYPE_FUNC_UINT_PARAM,
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

		OperParameter(wstring _name, UIOPERDATATYPE _format, function<void()> _func,
			function<void(UINT64)> _param_Func, void* _data, UINT64 _funcParam)
		{
			dataName = _name;
			dataFormat = _format;
			func = _func;
			param_Func = _param_Func;
			data = _data;
			funcParam = _funcParam;
		}
	};

public:
	cUIOperWindow();
	virtual ~cUIOperWindow();
	 
	void AddParameter(wstring dataName, UIOPERDATATYPE format, void* pData);
	void AddParameter(wstring dataName, UIOPERDATATYPE format, function<void()> func);
	void AddParameter(wstring dataName, UIOPERDATATYPE format, function<void(UINT64)> func, UINT64 param);
	void ClearParameters();

	void SetFontSize(int size) { m_fontSize = size; }
	virtual void InputDataUpdate() override;

private:
	virtual void UIUpdate() override;
	virtual void SetRenderState(bool value) override;
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
