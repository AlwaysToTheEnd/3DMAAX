#pragma once

class cObject
{
public:
	cObject();
	virtual ~cObject();

	virtual void Build(shared_ptr<cRenderItem> renderItem);
	virtual void Update(FXMMATRIX mat) {}
	virtual void Update() {}

	void SetMatrialIndex(UINT index) { m_renderInstance->instanceData.MaterialIndex = index; }

protected:
	shared_ptr<RenderInstance> m_renderInstance;
};
