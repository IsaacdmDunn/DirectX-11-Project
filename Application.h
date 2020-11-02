#pragma once

#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
#include "resource.h"
#include <vector>
#include "Vector3D.h"

using namespace DirectX;

struct SimpleVertex1
{
    XMFLOAT3 Pos;
    XMFLOAT4 Color;
	
};

struct SimpleVertex
{
    XMFLOAT3 Pos;
	XMFLOAT3 Normal;
	XMFLOAT2 TexCoords;
};

struct ConstantBuffer
{
	XMMATRIX mWorld;
	XMMATRIX mView;
	XMMATRIX mProjection;
	XMFLOAT4 DiffuseMtrl;
	XMFLOAT4 DiffuseLight;
	XMFLOAT4 AmbientMtrl;
	XMFLOAT4 AmbientLight;
	XMFLOAT4 SpecularMtrl;
	XMFLOAT4 SpecularLight;
	float specularPower;
	XMFLOAT3 EyePos;
	XMFLOAT3 LightVec3;

};
//
//typedef struct D3D11_RASTERIZER_DESC {
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
	ID3D11Buffer*           _pCubeVertexBuffer;
	ID3D11Buffer*			_pCubeIndexBuffer;
	ID3D11Buffer*			_pPlaneVertexBuffer;
	ID3D11Buffer*			_pPlaneIndexBuffer;
	ID3D11Buffer*           _pPyramidVertexBuffer;
	ID3D11Buffer*           _pPyramidIndexBuffer;
	ID3D11Buffer*           _pConstantBuffer;
	XMFLOAT4X4              _world;
	std::vector<XMFLOAT4X4> _worldMatrices;
	std::vector<XMFLOAT4X4> _worldMatrices2;
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

	XMFLOAT3 lightDirection;
	XMFLOAT4 diffuseMaterial;
	XMFLOAT4 diffuseLight;
	XMFLOAT4 ambientMaterial;
	XMFLOAT4 ambientLight;

	XMFLOAT4 specularLight;
	XMFLOAT4 specularMaterial;
	float specularPower;
	XMVECTOR eye;

	int x;
	int y;
	int z;
};

