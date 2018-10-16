#pragma once

class cOper_Add_Plane : public cOperation
{
public:
	cOper_Add_Plane();
	virtual ~cOper_Add_Plane();

	virtual void DrawElementOperation(vector<unique_ptr<cDrawElement>>& draw) override;

private:

};

