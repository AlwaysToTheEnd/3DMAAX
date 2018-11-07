#include "stdafx.h"

MeshGeometry* cObjectCoordinator::m_geo = nullptr;
cObjectCoordinator* cObjectCoordinator::instance = nullptr;

cObjectCoordinator::cObjectCoordinator()
	: m_controlState(AXIS_NONE)
	, m_controlObject(nullptr)
	, m_arrowRenderItem(nullptr)
	, m_sphereRenderItem(nullptr)
	, m_sphereRenderInstance(nullptr)
	, m_prevTranslationValue(0)
	, m_prevVec(0, 0, 0)
	, m_isRotationControl(false)
{
}

void cObjectCoordinator::MeshSetUp()
{
#pragma region VertexAndIndexSetting
	GeometryGenerator gen;
	vector<C_Vertex> vertices;
	vector<UINT16> indices;

	GeometryGenerator::MeshData sphere = gen.CreateSphere(0.2f, 20, 20);

	vertices.resize(sphere.Vertices.size());

	for (int i = 0; i < sphere.Vertices.size(); i++)
	{
		vertices[i].pos = sphere.Vertices[i].Position;
		XMStoreFloat4(&vertices[i].color, Colors::Gray.v);
	}

	indices = sphere.GetIndices16();

	vector<UINT16> arrowindices;
	vector<C_Vertex> arrowVertex;

	arrowVertex.push_back({ {0,1,0}, Colors::Red });
	arrowVertex.push_back({ {-0.2f, 0.8f, -0.2f}, Colors::Red });
	arrowVertex.push_back({ {0.2f, 0.8f, -0.2f}, Colors::Red });
	arrowVertex.push_back({ {0.2f, 0.8f, 0.2f}, Colors::Red });
	arrowVertex.push_back({ {-0.2f, 0.8f, 0.2f}, Colors::Red });

	arrowVertex.push_back({ {-0.1f, 0, -0.1f}, Colors::Red });
	arrowVertex.push_back({ {-0.1f, 0.8f, -0.1f}, Colors::Red });
	arrowVertex.push_back({ {0.1f, 0.8f, -0.1f}, Colors::Red });
	arrowVertex.push_back({ {0.1f, 0, -0.1f}, Colors::Red });

	arrowVertex.push_back({ {-0.1f, 0, 0.1f}, Colors::Red });
	arrowVertex.push_back({ {-0.1f, 0.8f, 0.1f}, Colors::Red });
	arrowVertex.push_back({ {0.1f, 0.8f, 0.1f}, Colors::Red });
	arrowVertex.push_back({ {0.1f, 0, 0.1f}, Colors::Red });

	arrowindices.push_back(0);
	arrowindices.push_back(2);
	arrowindices.push_back(1);

	arrowindices.push_back(0);
	arrowindices.push_back(3);
	arrowindices.push_back(2);

	arrowindices.push_back(0);
	arrowindices.push_back(4);
	arrowindices.push_back(3);

	arrowindices.push_back(0);
	arrowindices.push_back(1);
	arrowindices.push_back(4);

	//////////
	arrowindices.push_back(5);
	arrowindices.push_back(6);
	arrowindices.push_back(7);
	arrowindices.push_back(5);
	arrowindices.push_back(7);
	arrowindices.push_back(8);

	arrowindices.push_back(9);
	arrowindices.push_back(11);
	arrowindices.push_back(10);
	arrowindices.push_back(9);
	arrowindices.push_back(12);
	arrowindices.push_back(11);

	arrowindices.push_back(9);
	arrowindices.push_back(10);
	arrowindices.push_back(6);
	arrowindices.push_back(9);
	arrowindices.push_back(6);
	arrowindices.push_back(5);

	arrowindices.push_back(8);
	arrowindices.push_back(7);
	arrowindices.push_back(11);
	arrowindices.push_back(8);
	arrowindices.push_back(11);
	arrowindices.push_back(12);
#pragma endregion

	unordered_map<string, SubMeshGeometry> subMeshs;
	SubMeshGeometry subMesh;
	subMesh.baseVertexLocation = 0;
	subMesh.startIndexLocation = 0;
	subMesh.indexCount = (UINT)indices.size();
	subMeshs["sphere"] = subMesh;

	SubMeshGeometry arrowSubmesh;
	arrowSubmesh.baseVertexLocation = (UINT)vertices.size();
	arrowSubmesh.startIndexLocation = (UINT)indices.size();
	arrowSubmesh.indexCount = (UINT)arrowindices.size();
	subMeshs["arrow"] = arrowSubmesh;

	vertices.insert(vertices.end(), arrowVertex.begin(), arrowVertex.end());
	indices.insert(indices.end(), arrowindices.begin(), arrowindices.end());

	m_geo = MESHMG->AddMeshGeometry("objectCoordinator", vertices.data(), indices.data(),
		sizeof(C_Vertex), (UINT)vertices.size() * sizeof(C_Vertex),
		DXGI_FORMAT_R16_UINT, (UINT)indices.size() * sizeof(UINT16), false, &subMeshs);
}

void cObjectCoordinator::Build()
{
	m_arrowRenderItem = RENDERITEMMG->AddRenderItem("objectCoordinator");
	m_arrowRenderItem->SetGeometry(m_geo, "arrow");
	m_arrowRenderInstance[AXIS_X] = m_arrowRenderItem->GetRenderIsntance();
	m_arrowRenderInstance[AXIS_Y] = m_arrowRenderItem->GetRenderIsntance();
	m_arrowRenderInstance[AXIS_Z] = m_arrowRenderItem->GetRenderIsntance();

	const float scaleSizeXZ = 0.3f;
	const float scaleSizeY = 0.5f;
	XMMATRIX scale = XMMatrixScaling(scaleSizeXZ, scaleSizeY, scaleSizeXZ);
	XMMATRIX rotation = XMMatrixIdentity();
	XMStoreFloat4x4(&m_arrowRenderInstance[AXIS_Y]->instanceData.World, scale*rotation);
	m_arrowBoundingBox[AXIS_Y].Center = { 0,scaleSizeY/2, 0 };
	m_arrowBoundingBox[AXIS_Y].Extents = { 0.1f*scaleSizeXZ, scaleSizeY/2, 0.1f*scaleSizeXZ };

	rotation = XMMatrixRotationAxis(XMVectorSet(0, 0, 1, 0), -XM_PIDIV2);
	XMStoreFloat4x4(&m_arrowRenderInstance[AXIS_X]->instanceData.World, scale*rotation);
	m_arrowBoundingBox[AXIS_X].Center = { scaleSizeY/2, 0, 0 };
	m_arrowBoundingBox[AXIS_X].Extents = { scaleSizeY/2, 0.1f*scaleSizeXZ, 0.1f*scaleSizeXZ };

	rotation = XMMatrixRotationAxis(XMVectorSet(1, 0, 0, 0), XM_PIDIV2);
	XMStoreFloat4x4(&m_arrowRenderInstance[AXIS_Z]->instanceData.World, scale*rotation);
	m_arrowBoundingBox[AXIS_Z].Center = { 0, 0, scaleSizeY/2 };
	m_arrowBoundingBox[AXIS_Z].Extents = { 0.1f*scaleSizeXZ, 0.1f*scaleSizeXZ, scaleSizeY/2 };

	m_sphereRenderItem = RENDERITEMMG->AddRenderItem("objectCoordinator");
	m_sphereRenderItem->SetGeometry(m_geo, "sphere");
	m_sphereRenderInstance = m_sphereRenderItem->GetRenderIsntance();
	m_sphereRenderInstance->m_isRenderOK = false;

	m_sphereBounding.Center = { 0,0,0 };
	m_sphereBounding.Radius = 0.2f;
}

void cObjectCoordinator::Update()
{
	if (m_controlObject == nullptr)
	{
		m_arrowRenderInstance[AXIS_X]->m_isRenderOK = false;
		m_arrowRenderInstance[AXIS_Y]->m_isRenderOK = false;
		m_arrowRenderInstance[AXIS_Z]->m_isRenderOK = false;
		m_sphereRenderInstance->m_isRenderOK = false;
		return;
	}
	else
	{
		m_sphereRenderInstance->instanceData.World = m_controlObject->GetMatrix();
		m_sphereRenderInstance->numFramesDirty = gNumFrameResources;
		XMFLOAT3 pos = m_controlObject->GetPos();

		for (int i = 0; i < AXIS_NONE; i++)
		{
			m_arrowRenderInstance[i]->m_isRenderOK = true;
			m_arrowRenderInstance[i]->instanceData.World._41 = pos.x;
			m_arrowRenderInstance[i]->instanceData.World._42 = pos.y;
			m_arrowRenderInstance[i]->instanceData.World._43 = pos.z;
			m_arrowRenderInstance[i]->numFramesDirty = gNumFrameResources;
		}

		m_sphereRenderInstance->m_isRenderOK = true;
	}

	if (m_controlState != AXIS_NONE)
	{
		ObjectTranslation();
	}
	else
	{
		XMFLOAT4X4 objectMatF = m_controlObject->GetMatrix();
		XMMATRIX objectMat = XMLoadFloat4x4(&objectMatF);
		XMMATRIX invObjMat = XMMatrixInverse(&XMVECTOR(), objectMat);
		XMVECTOR objCenter = XMVectorSet(objectMatF._41, objectMatF._42, objectMatF._43, 1);
		XMVECTOR objRotation = XMQuaternionRotationMatrix(objectMat);

		PICKRAY mouseRay = INPUTMG->GetMousePickLay();
		PICKRAY objectLocalRay;
		objectLocalRay.ray = XMVector3Normalize(XMVector3TransformNormal(mouseRay.ray, invObjMat));
		objectLocalRay.origin = XMVector3TransformCoord(mouseRay.origin, invObjMat);

		XMVECTOR pickPos;
		float distance;

		if (INPUTMG->GetMouseOneDown(VK_LBUTTON))
		{
			XMVECTOR arrowBoundRayOrigin = mouseRay.origin - objCenter;
			for (int i = 0; i < AXIS_NONE; i++)
			{
				float dist;
				if (m_arrowBoundingBox[i].Intersects(arrowBoundRayOrigin, mouseRay.ray, dist))
				{
					m_controlState = (CLICKMOVEDIR)i;
					break;
				}

				m_controlState = AXIS_NONE;
			}

			if (m_controlState == AXIS_NONE)
			{
				if (m_sphereBounding.Intersects(objectLocalRay.origin, objectLocalRay.ray, distance))
				{
					pickPos = (mouseRay.origin + mouseRay.ray*distance);
					XMStoreFloat3(&m_prevVec, XMVector3Normalize((objectLocalRay.origin + objectLocalRay.ray*distance)));
					m_isRotationControl = true;
				}
			}
			else
			{
				XMVECTOR plane;
				float normalDot;
				float dist;
				switch (m_controlState)
				{
				case cObjectCoordinator::AXIS_X:
					plane = XMPlaneFromPointNormal(objCenter, XMVectorSet(0, 0, 1, 0));
					normalDot = XMVector3Dot(plane, mouseRay.ray).m128_f32[0];
					if (normalDot != 0)
					{
						dist = (XMVector3Dot(-plane, mouseRay.origin).m128_f32[0] - plane.m128_f32[3]) / normalDot;
						XMVECTOR pos = mouseRay.origin + mouseRay.ray*dist;
						m_prevTranslationValue = pos.m128_f32[0];
					}
					break;
				case cObjectCoordinator::AXIS_Y:
					plane = XMPlaneFromPointNormal(objCenter, XMVectorSet(0, 0, 1, 0));
					normalDot = XMVector3Dot(plane, mouseRay.ray).m128_f32[0];
					if (normalDot != 0)
					{
						dist = (XMVector3Dot(-plane, mouseRay.origin).m128_f32[0] - plane.m128_f32[3]) / normalDot;
						XMVECTOR pos = mouseRay.origin + mouseRay.ray*dist;
						m_prevTranslationValue = pos.m128_f32[1];
					}
					break;
				case cObjectCoordinator::AXIS_Z:
					plane = XMPlaneFromPointNormal(objCenter, XMVectorSet(0, 1, 0, 0));
					normalDot = XMVector3Dot(plane, mouseRay.ray).m128_f32[0];
					if (normalDot != 0)
					{
						dist = (XMVector3Dot(-plane, mouseRay.origin).m128_f32[0] - plane.m128_f32[3]) / normalDot;
						XMVECTOR pos = mouseRay.origin + mouseRay.ray*dist;
						m_prevTranslationValue = pos.m128_f32[2];
					}
					break;
				}

				if (normalDot == 0)
				{
					m_controlState = AXIS_NONE;
				}
			}
		}
		else
		{
			if (m_isRotationControl)
			{
				if (m_sphereBounding.Intersects(objectLocalRay.origin, objectLocalRay.ray, distance))
				{
					pickPos = (mouseRay.origin + mouseRay.ray*distance);

					XMVECTOR currVector = XMVector3Normalize(pickPos - objCenter);
					XMVECTOR baseVector = XMLoadFloat3(&m_prevVec);
					XMVECTOR axis = XMVector3Normalize(XMVector3Cross(XMVector3Normalize(baseVector), currVector));
					XMVECTOR rotationQuater = XMQuaternionRotationNormal(axis, XMVector3AngleBetweenNormals(currVector, baseVector).m128_f32[0]);
					XMStoreFloat4(&m_controlObject->m_quaternion, rotationQuater);
				}
			}
		}
	}

	if (!INPUTMG->GetMouseDownStay(VK_LBUTTON))
	{
		m_isRotationControl = false;
		m_controlState = AXIS_NONE;
	}
}


void cObjectCoordinator::ObjectTranslation()
{
	XMVECTOR objCenter = XMLoadFloat3(&m_controlObject->m_pos);
	PICKRAY mouseRay = INPUTMG->GetMousePickLay();
	XMVECTOR plane;
	float normalDot;
	float dist;

	switch (m_controlState)
	{
	case cObjectCoordinator::AXIS_X:
		plane = XMPlaneFromPointNormal(objCenter, XMVectorSet(0, 0, 1, 0));
		normalDot = XMVector3Dot(plane, mouseRay.ray).m128_f32[0];
		if (normalDot != 0)
		{
			dist = (XMVector3Dot(-plane, mouseRay.origin).m128_f32[0] - plane.m128_f32[3]) / normalDot;
			XMVECTOR pos = mouseRay.origin + mouseRay.ray*dist;

			float translationValue = pos.m128_f32[0]- m_prevTranslationValue;
			m_controlObject->m_pos.x += translationValue;
			m_prevTranslationValue = pos.m128_f32[0];
		}
		break;
	case cObjectCoordinator::AXIS_Y:
		plane = XMPlaneFromPointNormal(objCenter, XMVectorSet(0, 0, 1, 0));
		normalDot = XMVector3Dot(plane, mouseRay.ray).m128_f32[0];
		if (normalDot != 0)
		{
			dist = (XMVector3Dot(-plane, mouseRay.origin).m128_f32[0] - plane.m128_f32[3]) / normalDot;
			XMVECTOR pos = mouseRay.origin + mouseRay.ray*dist;
			
			float translationValue = pos.m128_f32[1] - m_prevTranslationValue;
			m_controlObject->m_pos.y += translationValue;
			m_prevTranslationValue = pos.m128_f32[1];
		}
		break;
	case cObjectCoordinator::AXIS_Z:
		plane = XMPlaneFromPointNormal(objCenter, XMVectorSet(0, 1, 0, 0));
		normalDot = XMVector3Dot(plane, mouseRay.ray).m128_f32[0];
		if (normalDot != 0)
		{
			dist = (XMVector3Dot(-plane, mouseRay.origin).m128_f32[0] - plane.m128_f32[3]) / normalDot;
			XMVECTOR pos = mouseRay.origin + mouseRay.ray*dist;
			
			float translationValue = pos.m128_f32[2] - m_prevTranslationValue;
			m_controlObject->m_pos.z += translationValue;
			m_prevTranslationValue = pos.m128_f32[2];
		}
		break;
	}
}