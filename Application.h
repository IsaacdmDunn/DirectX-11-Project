#pragma once

#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
#include "resource.h"
#include <vector>
#include "Vector3D.h"
#include "Shapes.h"

using namespace DirectX;

struct Material
{
	Material() { ZeroMemory(this, sizeof(this)); };
	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular;
	XMFLOAT4 Reflect;
};

//
//typedef struct //D3D11_RASTERIZER_DESC {
//	D3D11_FILL_MODE FillMode;
//	D3D11_CULL_MODE CullMode;
//	BOOL            FrontCounterClockwise;
//	INT             DepthBias;
//	FLOAT           DepthBiasClamp;
//	FLOAT           SlopeScaledDepthBias;
//	BOOL            DepthClipEnable;
//	BOOL            ScissorEnable;
//	BOOL            MultisampleEnable;
//	BOOL            AntialiasedLineEnable;
//} D3D11_RASTERIZER_DESC;

class Application
{
private:
	HINSTANCE               _hInst;
	HWND                    _hWnd;
	D3D_DRIVER_TYPE         _driverType;
	D3D_FEATURE_LEVEL       _featureLevel;
	ID3D11Device*           _pd3dDevice;
	ID3D11DeviceContext*    _pImmediateContext;
	IDXGISwapChain*         _pSwapChain;
	ID3D11RenderTargetView* _pRenderTargetView;
	ID3D11VertexShader*     _pVertexShader;
	ID3D11PixelShader*      _pPixelShader;
	ID3D11InputLayout*      _pVertexLayout;
	ID3D11Buffer*			_pCubeIndexBuffer;
	ID3D11Buffer*			_pPlaneIndexBuffer;
	ID3D11Buffer*           _pPyramidIndexBuffer;
	ID3D11Buffer*           _pConstantBuffer;
	XMFLOAT4X4              _world;
	std::vector<XMFLOAT4X4> _worldMatrices;
	XMFLOAT4X4              _view;
	XMFLOAT4X4              _projection;
	ID3D11DepthStencilView* _depthStencilView;
	ID3D11Texture2D* _depthStencilBuffer;
	ID3D11RasterizerState* _wireFrame;
	ID3D11RasterizerState* _solid;
	bool wireFrameActive = false;
	float gTime;



private:
	HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
	HRESULT InitDevice();
	void Cleanup();
	HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
	HRESULT InitShadersAndInputLayout();
	HRESULT InitVertexBuffer();
	HRESULT InitIndexBuffer();

	UINT _WindowHeight;
	UINT _WindowWidth;

public:
	Application();
	~Application();

	HRESULT Initialise(HINSTANCE hInstance, int nCmdShow);

	void Update();
	void Draw();
	HRESULT InitWireframeView();
	HRESULT InitSolidView();

	Shapes* shapes;
};

