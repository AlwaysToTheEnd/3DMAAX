#include "stdafx.h"

unique_ptr<MeshGeometry> cObjectCoordinator::m_geo = nullptr;
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

void cObjectCoordinator::MeshSetUp(ID3D12Device * device, ID3D12GraphicsCommandList * cmdList)
{
	GeometryGenerator gen;
	vector<C_Vertex> vertices;
	vector<UINT16> indices;
	m_geo = make_unique<MeshGeometry>();
	m_geo->name = "objectCoordinator";

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
	////

	SubMeshGeometry subMesh;
	subMesh.baseVertexLocation = 0;
	subMesh.startIndexLocation = 0;
	subMesh.indexCount = indices.size();
	m_geo->DrawArgs["sphere"] = subMesh;

	SubMeshGeometry arrowSubmesh;
	arrowSubmesh.baseVertexLocation = vertices.size();
	arrowSubmesh.startIndexLocation = indices.size();
	arrowSubmesh.indexCount = arrowindices.size();
	m_geo->DrawArgs["arrow"] = arrowSubmesh;

	vertices.insert(vertices.end(), arrowVertex.begin(), arrowVertex.end());
	indices.insert(indices.end(), arrowindices.begin(), arrowindices.end());

	m_geo->indexFormat = DXGI_FORMAT_R16_UINT;
	m_geo->indexBufferByteSize = indices.size() * sizeof(UINT16);
	m_geo->vertexBufferByteSize = vertices.size() * sizeof(C_Vertex);
	m_geo->vertexByteStride = sizeof(C_Vertex);
	
	m_geo->vertexBufferGPU = d3dUtil::CreateDefaultBuffer(device, cmdList,
		vertices.data(), m_geo->vertexBufferByteSize, m_geo->vertexUploadBuffer);

	m_geo->indexBufferGPU = d3dUtil::CreateDefaultBuffer(device, cmdList,
		indices.data(), m_geo->indexBufferByteSize, m_geo->indexUploadBuffer);
}

void cObjectCoordinator::SetUp()
{
	m_arrowRenderItem = RENDERITEMMG->AddRenderItem("objectCoordinator");
	m_arrowRenderItem->SetGeometry(m_geo.get(), "arrow");
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
	m_sphereRenderItem->SetGeometry(m_geo.get(), "sphere");
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
				float t;
				switch (m_controlState)
				{
				case cObjectCoordinator::AXIS_X:
					plane = XMPlaneFromPointNormal(objCenter, XMVectorSet(0, 0, 1, 0));
					normalDot = XMVector3Dot(plane, mouseRay.ray).m128_f32[0];
					if (normalDot != 0)
					{
						t = (XMVector3Dot(-plane, mouseRay.origin).m128_f32[0] - plane.m128_f32[3]) / normalDot;
						XMVECTOR pos = mouseRay.origin + mouseRay.ray*t;
						m_prevTranslationValue = pos.m128_f32[0];
					}
					break;
				case cObjectCoordinator::AXIS_Y:
					plane = XMPlaneFromPointNormal(objCenter, XMVectorSet(0, 0, 1, 0));
					normalDot = XMVector3Dot(plane, mouseRay.ray).m128_f32[0];
					if (normalDot != 0)
					{
						t = (XMVector3Dot(-plane, mouseRay.origin).m128_f32[0] - plane.m128_f32[3]) / normalDot;
						XMVECTOR pos = mouseRay.origin + mouseRay.ray*t;
						m_prevTranslationValue = pos.m128_f32[1];
					}
					break;
				case cObjectCoordinator::AXIS_Z:
					plane = XMPlaneFromPointNormal(objCenter, XMVectorSet(0, 1, 0, 0));
					normalDot = XMVector3Dot(plane, mouseRay.ray).m128_f32[0];
					if (normalDot != 0)
					{
						t = (XMVector3Dot(-plane, mouseRay.origin).m128_f32[0] - plane.m128_f32[3]) / normalDot;
						XMVECTOR pos = mouseRay.origin + mouseRay.ray*t;
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
	float t;

	switch (m_controlState)
	{
	case cObjectCoordinator::AXIS_X:
		plane = XMPlaneFromPointNormal(objCenter, XMVectorSet(0, 0, 1, 0));
		normalDot = XMVector3Dot(plane, mouseRay.ray).m128_f32[0];
		if (normalDot != 0)
		{
			t = (XMVector3Dot(-plane, mouseRay.origin).m128_f32[0] - plane.m128_f32[3]) / normalDot;
			XMVECTOR pos = mouseRay.origin + mouseRay.ray*t;

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
			t = (XMVector3Dot(-plane, mouseRay.origin).m128_f32[0] - plane.m128_f32[3]) / normalDot;
			XMVECTOR pos = mouseRay.origin + mouseRay.ray*t;
			
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
			t = (XMVector3Dot(-plane, mouseRay.origin).m128_f32[0] - plane.m128_f32[3]) / normalDot;
			XMVECTOR pos = mouseRay.origin + mouseRay.ray*t;
			
			float translationValue = pos.m128_f32[2] - m_prevTranslationValue;
			m_controlObject->m_pos.z += translationValue;
			m_prevTranslationValue = pos.m128_f32[2];
		}
		break;
	}
}

//if (m_controlObject == nullptr)
//{
//	m_sphereRenderInstance->m_isRenderOK = false;
//	return;
//}
//else
//{
//	m_sphereRenderInstance->instanceData.World = m_controlObject->GetMatrix();
//	m_sphereRenderInstance->numFramesDirty = gNumFrameResources;
//	m_sphereRenderInstance->m_isRenderOK = true;
//	m_arrowRenderInstance->m_isRenderOK = true;
//	m_arrowRenderInstance->numFramesDirty = gNumFrameResources;
//}
//
//XMVECTOR objCenter = XMLoadFloat3(&m_controlObject->m_pos);
//XMVECTOR objRotation = XMLoadFloat3(&m_controlObject->m_rotation);
//XMMATRIX objectMat = XMLoadFloat4x4(&m_controlObject->GetMatrix());
//XMMATRIX invObjMat = XMMatrixInverse(&XMVECTOR(), objectMat);
////XMMATRIX invTransPos = MathHelper::InverseTranspose(objectMat);
////XMMATRIX rotationMat = XMMatrixRotationRollPitchYawFromVector(objRotation);
//
//PICKRAY mouseRay = INPUTMG->GetMousePickLay();
//PICKRAY objectLocalRay;
//XMVECTOR pickPos;
//objectLocalRay.ray = XMVector3Normalize(XMVector3TransformNormal(mouseRay.ray, invObjMat));
//objectLocalRay.origin = XMVector3TransformCoord(mouseRay.origin, invObjMat);
//
//float distance;
//
//if (INPUTMG->GetMouseOneDown(VK_LBUTTON))
//{
//	if (m_geo->octree->Picking(objectLocalRay, distance))
//	{
//		pickPos = (mouseRay.origin + mouseRay.ray*distance);
//		m_arrowRenderInstance->instanceData.World._41 = pickPos.m128_f32[0];
//		m_arrowRenderInstance->instanceData.World._42 = pickPos.m128_f32[1];
//		m_arrowRenderInstance->instanceData.World._43 = pickPos.m128_f32[2];
//
//		XMStoreFloat3(&m_prevVec, XMVector3Normalize(pickPos - objCenter));
//		m_isRotationControl = true;
//	}
//}

//if (!INPUTMG->GetMouseDownStay(VK_LBUTTON))
//{
//	m_isRotationControl = false;
//}
//
//if (m_isRotationControl)
//{
//	if (m_geo->octree->Picking(objectLocalRay, distance))
//	{
//		pickPos = (mouseRay.origin + mouseRay.ray*distance);
//
//		XMVECTOR currVector = XMVector3Normalize(pickPos - objCenter);
//		XMVECTOR prevVector = XMLoadFloat3(&m_prevVec);
//		XMVECTOR rotationQuater = XMQuaternionRotationAxis(prevVector, XMVector3AngleBetweenNormals(prevVector, currVector).m128_f32[0]);
//		XMVECTOR rotationValue;
//
//		double q2sqr = rotationQuater.m128_f32[1] * rotationQuater.m128_f32[1];
//		double t0 = -2.0 * (q2sqr + rotationQuater.m128_f32[2] * rotationQuater.m128_f32[2]) + 1.0;
//		double t1 = +2.0 * (rotationQuater.m128_f32[0] * rotationQuater.m128_f32[1] + rotationQuater.m128_f32[3] * rotationQuater.m128_f32[2]);
//		double t2 = -2.0 * (rotationQuater.m128_f32[0] * rotationQuater.m128_f32[2] - rotationQuater.m128_f32[3] * rotationQuater.m128_f32[1]);
//		double t3 = +2.0 * (rotationQuater.m128_f32[2] * rotationQuater.m128_f32[2] + rotationQuater.m128_f32[3] * rotationQuater.m128_f32[0]);
//		double t4 = -2.0 * (rotationQuater.m128_f32[0] * rotationQuater.m128_f32[0] + q2sqr) + 1.0;
//
//		t2 = t2 > 1.0 ? 1.0 : t2;
//		t2 = t2 < -1.0 ? -1.0 : t2;
//
//		rotationValue.m128_f32[0] = asin(t2);
//		rotationValue.m128_f32[1] = atan2(t3, t4);
//		rotationValue.m128_f32[2] = atan2(t1, t0);
//		rotationValue.m128_f32[3] = 0;
//
//		XMStoreFloat3(&m_controlObject->m_rotation, objRotation + rotationValue);
//		XMStoreFloat3(&m_prevVec, currVector);
//	}
//	else
//	{
//		m_isRotationControl = false;
//	}
//}


/*if (GetAsyncKeyState(VK_F1))
{
	m_controlObject->m_quaternion.y = XM_PIDIV4;
	m_controlObject->m_quaternion.x = 0;
	m_controlObject->m_quaternion.z = 0;
	m_controlObject->m_pos.x = 2;
}*/

//if (GetAsyncKeyState(VK_UP))
//{
//	objRotation.m128_f32[0]+= 0.5f*DELTATIME;
//	XMStoreFloat4(&m_controlObject->m_rotation,
//		XMQuaternionNormalize(objRotation));
//}

//if (GetAsyncKeyState(VK_RIGHT))
//{
//	objRotation.m128_f32[1] += 0.5f*DELTATIME;
//	XMStoreFloat4(&m_controlObject->m_rotation,
//		XMQuaternionNormalize(objRotation));
//}