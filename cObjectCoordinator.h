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
	cObject* GetCurrObject() { return m_controlObject; }

private:
	cObjectCoordinator();
	void ObjectTranslation();
	
private:
	enum CLICKMOVEDIR
	{
		AXIS_X,
		AXIS_Y,
		AXIS_Z,
		AXIS_NONE,
	}m_controlState;

private:
	static cObjectCoordinator*	instance;
	BoundingBox					m_arrowBoundingBox[AXIS_NONE] = {};
	shared_ptr<cRenderItem>		m_arrowRenderItem;
	shared_ptr<RenderInstance>	m_arrowRenderInstance[AXIS_NONE] = {};
	shared_ptr<cRenderItem>		m_sphereRenderItem;
	shared_ptr<RenderInstance>	m_sphereRenderInstance;

	XMFLOAT3					m_prevVec;
	bool						m_isRotationControl;
	float						m_prevTranslationValue;

	cObject*					m_controlObject;
};

