#pragma once

#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
#include "resource.h"
#include <vector>
#include "Vector3D.h"
#include "Structures.h"
#include "OBJLoader.h" 
#include "Camera.h" 
#include "GameObject.h" 

using namespace DirectX;

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

	ID3D11ShaderResourceView* CubesTexture;
	ID3D11SamplerState* CubesTexSamplerState;

	ID3D11ShaderResourceView* _pDiffuseTexture[6];
	ID3D11ShaderResourceView* _pSpecularTexture[6];
	ID3D11SamplerState* _pSamplerLinear = nullptr;

	MeshData objMeshData[3];
	std::vector<ID3D11ShaderResourceView*> mTextures;

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

	Camera* cam;
	int currentCam;

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

	GameObject sphere[109];
};

