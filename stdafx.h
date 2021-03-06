// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 또는 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
// Windows 헤더 파일:
#include <windows.h>

// C 런타임 헤더 파일입니다.
#include <wrl.h>
#include <dxgi1_4.h>
#include <d3d12.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>
#include <d2d1.h>
#include <dwrite.h>
#include <string>
#include <memory>
#include <algorithm>
#include <vector>
#include <array>
#include <unordered_map>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <cassert>
#include <comdef.h>
#include <ppl.h>
#include <concurrent_unordered_map.h>
#include <concurrent_vector.h>
#include <list>
#include <map>
#include <numeric>
#include <functional>
#include <random>

#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "d2d1.lib")

const int gNumFrameResources = 3;

using namespace std;
using Microsoft::WRL::ComPtr;
using namespace DirectX;

#include "d3dx12.h"
#include "GeometryGenerator.h"
#include "GameTimer.h"
#include "DDSTextureLoader.h"
#include "MathHelper.h"
#include "CommonStates.h"
#include "DescriptorHeap.h"
#include "DirectXHelpers.h"
#include "EffectPipelineStateDescription.h"
#include "Effects.h"
#include "GamePad.h"
#include "GeometricPrimitive.h"
#include "GraphicsMemory.h"
#include "Keyboard.h"
#include "Model.h"
#include "Mouse.h"
#include "PostProcess.h"
#include "PrimitiveBatch.h"
#include "RenderTargetState.h"
#include "ResourceUploadBatch.h"
#include "ScreenGrab.h"
#include "SimpleMath.h"
#include "SpriteBatch.h"
#include "SpriteFont.h"
#include "VertexTypes.h"
#include "WICTextureLoader.h"
		
#include "D12Util.h"
#include "cPercentageValueSet.h"
#include "cOcTree.h"
#include "UploadBuffer.h"
#include "cTextureHeap.h"
#include "FrameResource.h"
#include "cRenderItemMG.h"
#include "cMeshgeometryMG.h"
#include "cFontManager.h"
#include "cInputDeviceMG.h"
#include "cObejct.h"
#include "cObjectCoordinator.h"
#include "cDrawElement.h"
#include "cCSGObject.h"
#include "cDrawDot.h"
#include "cDrawLines.h"
#include "cDrawPlane.h"
#include "cMesh.h"

#include "cOperation.h"
#include "cOper_Add_Plane.h"
#include "cOper_Add_Draws.h"
#include "cOper_Add_Line.h"
#include "cOper_Add_Dot.h"
#include "cOper_Add_Mesh.h"
#include "cOper_Push_Mesh.h"

#include "cUIObject.h"
#include "cUIButton.h"
#include "cButtonCollector.h"
#include "cUIOperWindow.h"
#include "cUIObjectMG.h"
#include "cOperator.h"
#include "cCamera.h"

#include "D12App.h"
#include "D12D3Maaax.h"