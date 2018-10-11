#pragma once

class cMirror:public cObject
{
public:
	static void SetReflectionRenderPipeLine(string name)
	{
		ReflectionRenderPipeLineName = name;
	}

	cMirror();
	virtual ~cMirror();
	virtual void Build(string subMeshName, MeshGeometry* geo,
		Material* mater, D3D12_GPU_DESCRIPTOR_HANDLE tex, string piplineName = "") override;
	void Update(cObject* renderObj);
	void SetPos(XMFLOAT3 pos) { m_pos = pos; }
	void SetRotX(float x) { m_rotX = x; }
	void SetRotY(float y) { m_rotY = y; }

private:
	static const UINT MaxReflectNum=10;
	static string ReflectionRenderPipeLineName;
	XMFLOAT3 m_pos;
	float m_rotY;
	float m_rotX;
	vector<cObject*> m_reflectObjects;
	shared_ptr<RenderItem> m_reflectionRender[MaxReflectNum] = {};
};

