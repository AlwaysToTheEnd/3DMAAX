#pragma once

#define OBJCOORD cObjectCoordinator::Get()

class cObjectCoordinator
{
public:
	static void MeshSetUp(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList);
	static void DisPosUploaders() { m_geo->DisPosUploaders(); }

private:
	static unique_ptr<MeshGeometry>	m_geo;

public:
	~cObjectCoordinator() {}
	static cObjectCoordinator * Get()
	{
		if (instance == nullptr)
		{
			instance = new cObjectCoordinator;
		}

		return instance;
	}

	void SetUp();
	void Update();
	void ObjectRegistration(cObject* object) { m_controlObject = object; }

private:
	cObjectCoordinator();
	
private:
	static cObjectCoordinator*	instance;
	shared_ptr<cRenderItem>		m_arrowRenderItem;
	shared_ptr<cRenderItem>		m_sphereRenderItem;
	shared_ptr<RenderInstance>	m_arrowRenderInstance;
	shared_ptr<RenderInstance>	m_sphereRenderInstance;

	cObject*					m_controlObject;
	XMFLOAT3					m_prevVec;
	bool						m_isRotationControl;
};

