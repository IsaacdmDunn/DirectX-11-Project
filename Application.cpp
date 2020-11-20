#include "Application.h"
#include "DDSTextureLoader.h"
#include <time.h>

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;

    switch (message)
    {
        case WM_PAINT:
            hdc = BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

Application::Application()
{
	_hInst = nullptr;
	_hWnd = nullptr;
	_driverType = D3D_DRIVER_TYPE_NULL;
	_featureLevel = D3D_FEATURE_LEVEL_11_0;
	_pd3dDevice = nullptr;
	_pImmediateContext = nullptr;
	_pSwapChain = nullptr;
	_pRenderTargetView = nullptr;
	_pVertexShader = nullptr;
	_pPixelShader = nullptr;
	_pVertexLayout = nullptr;
    _pCubeVertexBuffer = nullptr;
    _pCubeIndexBuffer = nullptr;
    _pPyramidVertexBuffer = nullptr;
    _pPyramidIndexBuffer = nullptr;
	_pConstantBuffer = nullptr;
}

Application::~Application()
{
    delete cam;
    cam = nullptr;
	Cleanup();
}

HRESULT Application::Initialise(HINSTANCE hInstance, int nCmdShow)
{
    if (FAILED(InitWindow(hInstance, nCmdShow)))
	{
        return E_FAIL;
	}

    RECT rc;
    GetClientRect(_hWnd, &rc);
    _WindowWidth = rc.right - rc.left;
    _WindowHeight = rc.bottom - rc.top;

    if (FAILED(InitDevice()))
    {
        Cleanup();

        return E_FAIL;
    }

    objMeshData[0] = OBJLoader::Load("assets/sphere.obj", _pd3dDevice, false);
    objMeshData[1] = OBJLoader::Load("assets/donut.obj", _pd3dDevice, false);

	// Initialize the world matrix
    for (int i = 0; i < 110; i++)
    {
        XMFLOAT4X4 world;

        XMStoreFloat4x4(&world, XMMatrixIdentity());

        _worldMatrices.push_back(world);
    }
    
    currentCam = 0;
    cam = new Camera();
    cam->LookAt(
        XMFLOAT3(0.0f, 3.0f, -23.0f),
        XMFLOAT3(0.0f, 0.0f, 0.0f)
    );

    cam->UpdateViewMatrix();
   
    
    lightDirection = XMFLOAT3(0.25f, 0.5f, -1.0f);
    diffuseMaterial = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    diffuseLight = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    ambientLight = XMFLOAT4(0.2f, 0.2f, 0.2f, 0.2f);
    ambientMaterial = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
    specularLight = XMFLOAT4(0.7f, 0.7f, 0.0f, 1.0f);
    specularMaterial = XMFLOAT4(0.7f, 0.7f, 0.0f, 1.0f);
    specularPower = 10.0f;
    eye = cam->GetPositionXM();

	return S_OK;
}

HRESULT Application::InitShadersAndInputLayout()
{
	HRESULT hr;

    // Compile the vertex shader
    ID3DBlob* pVSBlob = nullptr;
    hr = CompileShaderFromFile(L"DX11 Framework.fx", "VS", "vs_4_0", &pVSBlob);

    if (FAILED(hr))
    {
        MessageBox(nullptr,
                   L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return hr;
    }

	// Create the vertex shader
	hr = _pd3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &_pVertexShader);

	if (FAILED(hr))
	{	
		pVSBlob->Release();
        return hr;
	}

	// Compile the pixel shader
	ID3DBlob* pPSBlob = nullptr;
    hr = CompileShaderFromFile(L"DX11 Framework.fx", "PS", "ps_4_0", &pPSBlob);

    if (FAILED(hr))
    {
        MessageBox(nullptr,
                   L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return hr;
    }

	// Create the pixel shader
	hr = _pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &_pPixelShader);
	pPSBlob->Release();
	pPSBlob->Release();

    if (FAILED(hr))
        return hr;

    hr = CreateDDSTextureFromFile(_pd3dDevice, L"assets/planets/earth_spec.dds", nullptr, &_pSpecularTexture[0]);
    hr = CreateDDSTextureFromFile(_pd3dDevice, L"assets/planets/earth_col.dds", nullptr, &_pDiffuseTexture[0]);
    hr = CreateDDSTextureFromFile(_pd3dDevice, L"assets/planets/gas_spec.dds", nullptr, &_pSpecularTexture[1]);
    hr = CreateDDSTextureFromFile(_pd3dDevice, L"assets/planets/gas_col.dds", nullptr, &_pDiffuseTexture[1]);
    hr = CreateDDSTextureFromFile(_pd3dDevice, L"assets/planets/sun_spec.dds", nullptr, &_pSpecularTexture[2]);
    hr = CreateDDSTextureFromFile(_pd3dDevice, L"assets/planets/sun_col.dds", nullptr, &_pDiffuseTexture[2]);
    hr = CreateDDSTextureFromFile(_pd3dDevice, L"assets/planets/mars_spec.dds", nullptr, &_pSpecularTexture[3]);
    hr = CreateDDSTextureFromFile(_pd3dDevice, L"assets/planets/mars_col.dds", nullptr, &_pDiffuseTexture[3]);
    hr = CreateDDSTextureFromFile(_pd3dDevice, L"assets/planets/moon_spec.dds", nullptr, &_pSpecularTexture[4]);
    hr = CreateDDSTextureFromFile(_pd3dDevice, L"assets/planets/moon_col.dds", nullptr, &_pDiffuseTexture[4]);

    // Define the input layout
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	UINT numElements = ARRAYSIZE(layout);

    // Create the input layout
	hr = _pd3dDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
                                        pVSBlob->GetBufferSize(), &_pVertexLayout);
	pVSBlob->Release();

	if (FAILED(hr))
        return hr;

    // Set the input layout
    _pImmediateContext->IASetInputLayout(_pVertexLayout);

	return hr;
}

HRESULT Application::InitVertexBuffer()
{
	HRESULT hr;

    // Create vertex buffer
    SimpleVertex cubeVertices[] =
    {
        { XMFLOAT3(-1.0,  1.0, -1.0),		XMFLOAT3(0.0,  0.0, -1.0),		XMFLOAT2(0.0, 0.0) },	
        { XMFLOAT3(1.0,  1.0, -1.0),		XMFLOAT3(0.0,  0.0, -1.0),		XMFLOAT2(1.0, 0.0) },  	
        { XMFLOAT3(-1.0, -1.0, -1.0),		XMFLOAT3(0.0,  0.0, -1.0),		XMFLOAT2(0.0, 1.0) },  	
        { XMFLOAT3(-1.0, -1.0, -1.0),		XMFLOAT3(0.0,  0.0, -1.0),		XMFLOAT2(0.0, 1.0) },  

        { XMFLOAT3(1.0,  1.0, -1.0),		XMFLOAT3(0.0,  0.0, -1.0),		XMFLOAT2(1.0, 0.0) },  	
        { XMFLOAT3(1.0, -1.0, -1.0),		XMFLOAT3(0.0,  0.0, -1.0),		XMFLOAT2(1.0, 1.0) },  	
        { XMFLOAT3(1.0,  1.0, -1.0),		XMFLOAT3(1.0,  0.0,  0.0),		XMFLOAT2(0.0, 0.0) }, 	
        { XMFLOAT3(1.0,  1.0,  1.0),		XMFLOAT3(1.0,  0.0,  0.0),		XMFLOAT2(1.0, 0.0) },  	

        { XMFLOAT3(1.0, -1.0, -1.0),		XMFLOAT3(1.0,  0.0,  0.0),		XMFLOAT2(0.0, 1.0) },  	
        { XMFLOAT3(1.0, -1.0, -1.0),		XMFLOAT3(1.0,  0.0,  0.0),		XMFLOAT2(0.0, 1.0) },  	
        { XMFLOAT3(1.0,  1.0,  1.0),		XMFLOAT3(1.0,  0.0,  0.0),		XMFLOAT2(1.0, 0.0) },  	
        { XMFLOAT3(1.0, -1.0,  1.0),		XMFLOAT3(1.0,  0.0,  0.0),		XMFLOAT2(1.0, 1.0) },  

        { XMFLOAT3(1.0,  1.0,  1.0),		XMFLOAT3(0.0,  0.0,  1.0),		XMFLOAT2(0.0, 0.0) },  	
        { XMFLOAT3(-1.0,  1.0,  1.0),		XMFLOAT3(0.0,  0.0,  1.0),		XMFLOAT2(1.0, 0.0) },  	
        { XMFLOAT3(1.0, -1.0,  1.0),		XMFLOAT3(0.0,  0.0,  1.0),		XMFLOAT2(0.0, 1.0) },  	
        { XMFLOAT3(1.0, -1.0,  1.0),		XMFLOAT3(0.0,  0.0,  1.0),		XMFLOAT2(0.0, 1.0) },  	

        { XMFLOAT3(-1.0,  1.0,  1.0),		XMFLOAT3(0.0,  0.0,  1.0),		XMFLOAT2(1.0, 0.0) },  	
        { XMFLOAT3(-1.0, -1.0,  1.0),		XMFLOAT3(0.0,  0.0,  1.0),		XMFLOAT2(1.0, 1.0) },  	
        { XMFLOAT3(-1.0,  1.0,  1.0),		XMFLOAT3(-1.0,  0.0,  0.0),		XMFLOAT2(0.0, 0.0) }, 	
        { XMFLOAT3(-1.0,  1.0, -1.0),		XMFLOAT3(-1.0,  0.0,  0.0),		XMFLOAT2(1.0, 0.0) }, 	

        { XMFLOAT3(-1.0, -1.0,  1.0),		XMFLOAT3(-1.0,  0.0,  0.0),		XMFLOAT2(0.0, 1.0) }, 	
        { XMFLOAT3(-1.0, -1.0,  1.0),		XMFLOAT3(-1.0,  0.0,  0.0),		XMFLOAT2(0.0, 1.0) }, 	
        { XMFLOAT3(-1.0,  1.0, -1.0),		XMFLOAT3(-1.0,  0.0,  0.0),		XMFLOAT2(1.0, 0.0) }, 	
        { XMFLOAT3(-1.0, -1.0, -1.0),		XMFLOAT3(-1.0,  0.0,  0.0),		XMFLOAT2(1.0, 1.0) }, 

        { XMFLOAT3(-1.0,  1.0,  1.0),		XMFLOAT3(0.0,  1.0,  0.0),		XMFLOAT2(0.0, 0.0) }, 	
        { XMFLOAT3(1.0,  1.0,  1.0),		XMFLOAT3(0.0,  1.0,  0.0),		XMFLOAT2(1.0, 0.0) },  	
        { XMFLOAT3(-1.0,  1.0, -1.0),		XMFLOAT3(0.0,  1.0,  0.0),		XMFLOAT2(0.0, 1.0) },  	
        { XMFLOAT3(-1.0,  1.0, -1.0),		XMFLOAT3(0.0,  1.0,  0.0),		XMFLOAT2(0.0, 1.0) }, 

        { XMFLOAT3(1.0,  1.0,  1.0),		XMFLOAT3(0.0,  1.0,  0.0),		XMFLOAT2(1.0, 0.0) },  	
        { XMFLOAT3(1.0,  1.0, -1.0),		XMFLOAT3(0.0,  1.0,  0.0),		XMFLOAT2(1.0, 1.0) },  	
        { XMFLOAT3(-1.0, -1.0, -1.0),		XMFLOAT3(0.0, -1.0,  0.0),		XMFLOAT2(0.0, 0.0) },  	
        { XMFLOAT3(1.0, -1.0, -1.0),		XMFLOAT3(0.0, -1.0,  0.0),		XMFLOAT2(1.0, 0.0) }, 

        { XMFLOAT3(-1.0, -1.0,  1.0),		XMFLOAT3(0.0, -1.0,  0.0),		XMFLOAT2(0.0, 1.0) },  	
        { XMFLOAT3(-1.0, -1.0,  1.0),		XMFLOAT3(0.0, -1.0,  0.0),		XMFLOAT2(0.0, 1.0) },  	
        { XMFLOAT3(1.0, -1.0, -1.0),		XMFLOAT3(0.0, -1.0,  0.0),		XMFLOAT2(1.0, 0.0) },  	
        { XMFLOAT3(1.0, -1.0,  1.0),		XMFLOAT3(0.0, -1.0,  0.0),		XMFLOAT2(1.0, 1.0) },	
    };
    
    /*SimpleVertex1 cubeVertices[] =
    {
        { XMFLOAT3( -1.0f, -1.0f, -1.0f ),  XMFLOAT4( 1.0f, 0.0f, 0.0f, 1.0f )  },
        { XMFLOAT3( 1.0f, -1.0f, -1.0f ),   XMFLOAT4( 0.0f, 1.0f, 0.0f, 1.0f )  },
        { XMFLOAT3( -1.0f, 1.0f, -1.0f ),   XMFLOAT4( 0.0f, 0.0f, 1.0f, 1.0f )  },
        { XMFLOAT3( 1.0f, 1.0f, -1.0f ),    XMFLOAT4( 1.0f, 1.0f, 0.0f, 1.0f )  },

        { XMFLOAT3(-1.0f, -1.0f, 1.0f),     XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f)  },
        { XMFLOAT3(1.0f, -1.0f, 1.0f),      XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f)  },
        { XMFLOAT3(-1.0f, 1.0f, 1.0f),      XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f)  },
        { XMFLOAT3(1.0f, 1.0f, 1.0f),       XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)  },
    };*/

    // Create vertex buffer
    SimpleVertex1 planeVertices[] =
    {
        { XMFLOAT3(-1.0f, 0.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) }, //bl
        { XMFLOAT3(1.0f, 0.0f, 1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f) }, //br
        { XMFLOAT3(-1.0f, 0.0f, -1.0f), XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f) }, //tl
        { XMFLOAT3(1.0f, 0.0f, -1.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) }, //tr
    };

    // Create vertex buffer
    SimpleVertex1 pyramidVertices[] =
    {
        { XMFLOAT3(-1.0f, 0.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) }, //bl
        { XMFLOAT3(1.0f, 0.0f, 1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f) }, //br
        { XMFLOAT3(-1.0f, 0.0f, -1.0f), XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f) }, //tl
        { XMFLOAT3(1.0f, 0.0f, -1.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) }, //tr
        { XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) }, //top
    };

    D3D11_BUFFER_DESC bdCube;
	ZeroMemory(&bdCube, sizeof(bdCube));
    bdCube.Usage = D3D11_USAGE_DEFAULT;
    bdCube.ByteWidth = sizeof(cubeVertices);
    bdCube.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bdCube.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA InitCubeData;
	ZeroMemory(&InitCubeData, sizeof(InitCubeData));
    InitCubeData.pSysMem = cubeVertices;

    hr = _pd3dDevice->CreateBuffer(&bdCube, &InitCubeData, &_pCubeVertexBuffer);


    D3D11_BUFFER_DESC bdPyramid;
    ZeroMemory(&bdPyramid, sizeof(bdPyramid));
    bdPyramid.Usage = D3D11_USAGE_DEFAULT;
    bdPyramid.ByteWidth = sizeof(pyramidVertices);
    bdPyramid.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bdPyramid.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA InitPyramidData;
    ZeroMemory(&InitPyramidData, sizeof(InitPyramidData));
    InitPyramidData.pSysMem = pyramidVertices;

    hr = _pd3dDevice->CreateBuffer(&bdPyramid, &InitPyramidData, &_pPyramidVertexBuffer);

    D3D11_BUFFER_DESC bdPlane;
    ZeroMemory(&bdPlane, sizeof(bdPlane));
    bdPlane.Usage = D3D11_USAGE_DEFAULT;
    bdPlane.ByteWidth = sizeof(planeVertices);
    bdPlane.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bdPlane.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA InitPlaneData;
    ZeroMemory(&InitPlaneData, sizeof(InitPlaneData));
    InitPlaneData.pSysMem = planeVertices;

    hr = _pd3dDevice->CreateBuffer(&bdPlane, &InitPlaneData, &_pPlaneVertexBuffer);

    if (FAILED(hr))
        return hr;

	return S_OK;
}

HRESULT Application::InitIndexBuffer()
{
	HRESULT hr;

    // Create cube index buffer
    WORD cubeIndices[] =
    {
        0,1,2 ,3,4,5,
        6,7,8 ,9,10,11,
        12,13,14 ,15,16,17,
        18,19,20 ,21,22,23,
        24,25,26 ,27,28,29,
        30,31,32 ,33,34,35
    };

    // Create pyramid index buffer
    WORD pyramidIndices[] =
    {
        0, 1, 2,    // side 1
        1, 3, 2,    
        2, 0, 4,    // side 2
        3, 2, 4,    // side 3
        1, 3, 4,    // side 4
        0, 1, 4,    // side 5
    };

    // Create plane index buffer
    WORD planeIndices[] =
    {
        0, 1, 2,    // side 1
        1, 3, 2,
    };

	D3D11_BUFFER_DESC bdCube;
	ZeroMemory(&bdCube, sizeof(bdCube));

    bdCube.Usage = D3D11_USAGE_DEFAULT;
    bdCube.ByteWidth = sizeof(cubeIndices);
    bdCube.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bdCube.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitCubeData;
	ZeroMemory(&InitCubeData, sizeof(InitCubeData));
    InitCubeData.pSysMem = cubeIndices;
    hr = _pd3dDevice->CreateBuffer(&bdCube, &InitCubeData, &_pCubeIndexBuffer);


    D3D11_BUFFER_DESC bdPyramid;
    ZeroMemory(&bdPyramid, sizeof(bdPyramid));

    bdPyramid.Usage = D3D11_USAGE_DEFAULT;
    bdPyramid.ByteWidth = sizeof(pyramidIndices);
    bdPyramid.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bdPyramid.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA InitData;
    ZeroMemory(&InitData, sizeof(InitData));
    InitData.pSysMem = pyramidIndices;
    hr = _pd3dDevice->CreateBuffer(&bdPyramid, &InitData, &_pPyramidIndexBuffer);


    D3D11_BUFFER_DESC bdPlane;
    ZeroMemory(&bdPlane, sizeof(bdPlane));

    bdPlane.Usage = D3D11_USAGE_DEFAULT;
    bdPlane.ByteWidth = sizeof(planeIndices);
    bdPlane.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bdPlane.CPUAccessFlags = 0;

    //D3D11_SUBRESOURCE_DATA InitData;
    ZeroMemory(&InitData, sizeof(InitData));
    InitData.pSysMem = planeIndices;
    hr = _pd3dDevice->CreateBuffer(&bdPlane, &InitData, &_pPlaneIndexBuffer);

    if (FAILED(hr))
        return hr;

	return S_OK;
}

HRESULT Application::InitWindow(HINSTANCE hInstance, int nCmdShow)
{
    // Register class
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_TUTORIAL1);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW );
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = L"TutorialWindowClass";
    wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_TUTORIAL1);
    if (!RegisterClassEx(&wcex))
        return E_FAIL;

    // Create window
    _hInst = hInstance;
    RECT rc = {0, 0, 640, 480};
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
    _hWnd = CreateWindow(L"TutorialWindowClass", L"DX11 Framework", WS_OVERLAPPEDWINDOW,
                         CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance,
                         nullptr);
    if (!_hWnd)
		return E_FAIL;

    ShowWindow(_hWnd, nCmdShow);

    return S_OK;
}

HRESULT Application::CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
    HRESULT hr = S_OK;

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
    // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows 
    // the shaders to be optimized and to run exactly the way they will run in 
    // the release configuration of this program.
    dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

    ID3DBlob* pErrorBlob;
    hr = D3DCompileFromFile(szFileName, nullptr, nullptr, szEntryPoint, szShaderModel, 
        dwShaderFlags, 0, ppBlobOut, &pErrorBlob);

    if (FAILED(hr))
    {
        if (pErrorBlob != nullptr)
            OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());

        if (pErrorBlob) pErrorBlob->Release();

        return hr;
    }

    if (pErrorBlob) pErrorBlob->Release();

    return S_OK;
}

HRESULT Application::InitDevice()
{
    D3D11_TEXTURE2D_DESC depthStencilDesc;

    depthStencilDesc.Width = _WindowWidth;
    depthStencilDesc.Height = _WindowHeight;
    depthStencilDesc.MipLevels = 1;
    depthStencilDesc.ArraySize = 1;
    depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilDesc.SampleDesc.Count = 1;
    depthStencilDesc.SampleDesc.Quality = 0;
    depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
    depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthStencilDesc.CPUAccessFlags = 0;
    depthStencilDesc.MiscFlags = 0;


    HRESULT hr = S_OK;

    UINT createDeviceFlags = 0;

#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_DRIVER_TYPE driverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
    };

    UINT numDriverTypes = ARRAYSIZE(driverTypes);

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };

	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 1;
    sd.BufferDesc.Width = _WindowWidth;
    sd.BufferDesc.Height = _WindowHeight;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = _hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;

    for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
    {
        _driverType = driverTypes[driverTypeIndex];
        hr = D3D11CreateDeviceAndSwapChain(nullptr, _driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
                                           D3D11_SDK_VERSION, &sd, &_pSwapChain, &_pd3dDevice, &_featureLevel, &_pImmediateContext);
        if (SUCCEEDED(hr))
            break;
    }
    
    if (FAILED(hr))
        return hr;

    // Create a render target view
    ID3D11Texture2D* pBackBuffer = nullptr;
    hr = _pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

    if (FAILED(hr))
        return hr;

    hr = _pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &_pRenderTargetView);
    pBackBuffer->Release();

    if (FAILED(hr))
        return hr;

    _pd3dDevice->CreateTexture2D(&depthStencilDesc, nullptr, &_depthStencilBuffer);
    _pd3dDevice->CreateDepthStencilView(_depthStencilBuffer, nullptr, &_depthStencilView);


    _pImmediateContext->OMSetRenderTargets(1, &_pRenderTargetView, _depthStencilView);

    // Setup the viewport
    D3D11_VIEWPORT vp;
    vp.Width = (FLOAT)_WindowWidth;
    vp.Height = (FLOAT)_WindowHeight;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    _pImmediateContext->RSSetViewports(1, &vp);

	InitShadersAndInputLayout();

	InitVertexBuffer();
	InitIndexBuffer();

    // Set primitive topology
    _pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Create the constant buffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
    hr = _pd3dDevice->CreateBuffer(&bd, nullptr, &_pConstantBuffer);

    ////sets render state
    //
    //D3D11_RASTERIZER_DESC wfdesc;
    //ZeroMemory(&wfdesc, sizeof(D3D11_RASTERIZER_DESC));

    //wfdesc.FillMode = D3D11_FILL_WIREFRAME;
    //wfdesc.CullMode = D3D11_CULL_NONE;
    //hr = _pd3dDevice->CreateRasterizerState(&wfdesc, &_wireFrame);

    

    if (FAILED(hr))
        return hr;

    return S_OK;
}

void Application::Cleanup()
{
    if (_pImmediateContext) _pImmediateContext->ClearState();

    if (_pConstantBuffer) _pConstantBuffer->Release();
    if (_pCubeVertexBuffer) _pCubeVertexBuffer->Release();
    if (_pCubeIndexBuffer) _pCubeIndexBuffer->Release();
    if (_pPyramidVertexBuffer) _pPyramidVertexBuffer->Release();
    if (_pPyramidIndexBuffer) _pPyramidIndexBuffer->Release();
    if (_pPlaneVertexBuffer) _pPlaneVertexBuffer->Release();
    if (_pPlaneIndexBuffer) _pPlaneIndexBuffer->Release();
    if (_pVertexLayout) _pVertexLayout->Release();
    if (_pVertexShader) _pVertexShader->Release();
    if (_pPixelShader) _pPixelShader->Release();
    if (_pRenderTargetView) _pRenderTargetView->Release();
    if (_pSwapChain) _pSwapChain->Release();
    if (_pImmediateContext) _pImmediateContext->Release();
    if (_pd3dDevice) _pd3dDevice->Release();

    if (_depthStencilView) _depthStencilView->Release();
    if (_depthStencilBuffer) _depthStencilBuffer->Release();
    if (_wireFrame) _wireFrame->Release();
    if (_solid) _solid->Release();

}

void Application::Update()
{
    // Update our time
    static float t = 0.0f;

    if (_driverType == D3D_DRIVER_TYPE_REFERENCE)
    {
        t += (float) XM_PI * 0.0125f;
    }
    else
    {
        static DWORD dwTimeStart = 0;
        DWORD dwTimeCur = GetTickCount();

        if (dwTimeStart == 0)
            dwTimeStart = dwTimeCur;

        t = (dwTimeCur - dwTimeStart) / 1000.0f;
    }
    if (GetKeyState('1') & 0x8000)
    {
        currentCam = 0;
    }
    else if (GetKeyState('2') & 0x8000)
    {
        currentCam = 1;
    }
    if (GetKeyState('W') & 0x8000)
    {
        cam->Walk(0.3f);
    }
    else if (GetKeyState('S') & 0x8000)
    {
        cam->Walk(-0.3f);
    }
    if (GetKeyState('A') & 0x8000)
    {
        cam->Strafe(-0.3f);
    }
    else if (GetKeyState('D') & 0x8000)
    {
        cam->Strafe(0.3f);
    }
    else if (GetKeyState('Q') & 0x8000)
    {
        cam->RotateY(-.02f);
    }
    else if (GetKeyState('E') & 0x8000)
    {
        cam->RotateY(.02f);
    }
    //cam[0]->SetLens(0.25f * 3.1452, _WindowWidth / _WindowHeight, 1.0f, 1000.0f);

    //
    
    //cam->LookAt( XMFLOAT3(0.0f, 8.0f, -15.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f));
    cam->UpdateViewMatrix();

    gTime = t / 3;

    //
    // Animate the cube
    //
    InitWireframeView();

    //sun
    XMMATRIX rotated = XMMatrixIdentity();

    rotated = XMMatrixMultiply(rotated, XMMatrixScaling(2, 2, 2));
    rotated = XMMatrixMultiply(rotated, XMMatrixRotationY(gTime));
    rotated = XMMatrixMultiply(rotated, XMMatrixTranslation(0, 0, 0));
    XMStoreFloat4x4(&_worldMatrices[0], rotated);

    //planets
    rotated = XMMatrixIdentity();
    rotated = XMMatrixMultiply(rotated, XMMatrixScaling(.65, .65, .65));
    rotated = XMMatrixMultiply(rotated, XMMatrixRotationY(gTime));
    rotated = XMMatrixMultiply(rotated, XMMatrixTranslation(0, 0, 9));
    rotated = XMMatrixMultiply(rotated, XMMatrixRotationY(gTime));
    XMStoreFloat4x4(&_worldMatrices[1], rotated);

    rotated = XMMatrixIdentity();
    rotated = XMMatrixMultiply(rotated, XMMatrixScaling(.4, .4, .4));
    rotated = XMMatrixMultiply(rotated, XMMatrixRotationY(gTime *2));
    rotated = XMMatrixMultiply(rotated, XMMatrixTranslation(0, 0, 6));
    rotated = XMMatrixMultiply(rotated, XMMatrixRotationY(gTime *1.5));
    XMStoreFloat4x4(&_worldMatrices[2], rotated);

    rotated = XMMatrixIdentity();
    rotated = XMMatrixMultiply(rotated, XMMatrixScaling(1, 1, 1));
    rotated = XMMatrixMultiply(rotated, XMMatrixRotationY(gTime *.5));
    rotated = XMMatrixMultiply(rotated, XMMatrixTranslation(0, 0, 16));
    rotated = XMMatrixMultiply(rotated, XMMatrixRotationY(gTime *0.5));
    XMStoreFloat4x4(&_worldMatrices[3], rotated);

    rotated = XMMatrixIdentity();
    rotated = XMMatrixMultiply(rotated, XMMatrixScaling(.8, .8, .8));
    rotated = XMMatrixMultiply(rotated, XMMatrixRotationY(gTime * .3));
    rotated = XMMatrixMultiply(rotated, XMMatrixTranslation(0, 0, 22));
    rotated = XMMatrixMultiply(rotated, XMMatrixRotationY(gTime * .3));
    XMStoreFloat4x4(&_worldMatrices[8], rotated);

    //moon
    rotated = XMMatrixIdentity();
    rotated = XMMatrixMultiply(rotated, XMMatrixScaling(.2, .2, .2));
    rotated = XMMatrixMultiply(rotated, XMMatrixTranslation(0, 0, 2));
    rotated = XMMatrixMultiply(rotated, XMMatrixRotationY(gTime *3));
    rotated = XMMatrixMultiply(rotated, XMMatrixTranslation(0, 0, 6));
    rotated = XMMatrixMultiply(rotated, XMMatrixRotationY(gTime *1.5));
    XMStoreFloat4x4(&_worldMatrices[4], rotated);

    rotated = XMMatrixIdentity();
    rotated = XMMatrixMultiply(rotated, XMMatrixScaling(.25, .25, .25));
    rotated = XMMatrixMultiply(rotated, XMMatrixTranslation(0, 0, 2));
    rotated = XMMatrixMultiply(rotated, XMMatrixRotationY(gTime * 3));
    rotated = XMMatrixMultiply(rotated, XMMatrixTranslation(0, 0, 9));
    rotated = XMMatrixMultiply(rotated, XMMatrixRotationY(gTime));
    XMStoreFloat4x4(&_worldMatrices[5], rotated);

    rotated = XMMatrixIdentity();
    rotated = XMMatrixMultiply(rotated, XMMatrixScaling(.4, .4, .4));
    rotated = XMMatrixMultiply(rotated, XMMatrixTranslation(0, 0, 4));
    rotated = XMMatrixMultiply(rotated, XMMatrixRotationY(gTime * 1.5));
    rotated = XMMatrixMultiply(rotated, XMMatrixTranslation(0, 0, 16));
    rotated = XMMatrixMultiply(rotated, XMMatrixRotationY(gTime * 0.5));
    XMStoreFloat4x4(&_worldMatrices[6], rotated);

    rotated = XMMatrixIdentity();
    rotated = XMMatrixMultiply(rotated, XMMatrixScaling(.1, .1, .1));
    rotated = XMMatrixMultiply(rotated, XMMatrixTranslation(0, 0, 2.5));
    rotated = XMMatrixMultiply(rotated, XMMatrixRotationY(gTime * 5));
    rotated = XMMatrixMultiply(rotated, XMMatrixTranslation(0, 0, 16));
    rotated = XMMatrixMultiply(rotated, XMMatrixRotationY(gTime * 0.5));
    XMStoreFloat4x4(&_worldMatrices[7], rotated);

    for (int i = 0; i < 100; i++)
    {
        InitSolidView();
        float rotationOffset = rand() % 100;
        float localTranformOffet = (rand() % 30) / 10;
        int v1 = rand() % 100;
        rotated = XMMatrixIdentity();
        rotated = XMMatrixMultiply(rotated, XMMatrixScaling(.05, .05, .05));
        rotated = XMMatrixMultiply(rotated, XMMatrixTranslation(0, 0, 3 + localTranformOffet));
        rotated = XMMatrixMultiply(rotated, XMMatrixRotationY(gTime + rotationOffset));
        rotated = XMMatrixMultiply(rotated, XMMatrixTranslation(0, 0, 22));
        rotated = XMMatrixMultiply(rotated, XMMatrixRotationY(gTime * 0.3));
        XMStoreFloat4x4(&_worldMatrices[i+9], rotated);
    }

    rotated = XMMatrixIdentity();
    rotated = XMMatrixMultiply(rotated, XMMatrixScaling(3, 3, 3));
    rotated = XMMatrixMultiply(rotated, XMMatrixTranslation(0, -5, 0));
    XMStoreFloat4x4(&_worldMatrices[109], rotated);
}

void Application::Draw()
{
    //
    // Clear the back buffer
    //
    float ClearColor[4] = {0.0f, 0.125f, 0.3f, 1.0f}; // red,green,blue,alpha
    _pImmediateContext->ClearRenderTargetView(_pRenderTargetView, ClearColor);

    for (int i = 0; i < 110; i++)
    {
        // Set index and vertex buffer
        // Set vertex buffer
        UINT stride = sizeof(SimpleVertex);
        UINT offset = 0;
        if (i < 9)
        {
            _pImmediateContext->IASetVertexBuffers(0, 1, &objMeshData[0].VertexBuffer, &stride, &offset);
            _pImmediateContext->IASetIndexBuffer(objMeshData[0].IndexBuffer, DXGI_FORMAT_R16_UINT, 0);
        }
        else 
        { 
            _pImmediateContext->IASetVertexBuffers(0, 1, &objMeshData[1].VertexBuffer, &stride, &offset);
            _pImmediateContext->IASetIndexBuffer(objMeshData[1].IndexBuffer, DXGI_FORMAT_R16_UINT, 0);
        }

        XMMATRIX world = XMLoadFloat4x4(&_worldMatrices[i]);
        XMMATRIX view = XMLoadFloat4x4(&_view);
        XMMATRIX projection = XMLoadFloat4x4(&_projection);

        ConstantBuffer cb;
        cb.mView = XMMatrixTranspose(cam->View());
        cb.mProjection = XMMatrixTranspose(cam->Proj());
        cb.mWorld = XMMatrixTranspose(world);

        cb.DiffuseMtrl = diffuseMaterial;
        cb.DiffuseLight = diffuseLight;
        cb.LightVec3 = lightDirection;

        cb.AmbientLight = ambientLight;
        cb.AmbientMtrl = ambientMaterial;

        cb.SpecularLight = specularLight;
        cb.SpecularMtrl = specularMaterial;
        cb.specularPower = specularPower;
        cb.EyePos = { eye.m128_f32[0], eye.m128_f32[1], eye.m128_f32[2] };


        _pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);

        //
        // Renders a triangle
        //
        _pImmediateContext->VSSetShader(_pVertexShader, nullptr, 0);
        _pImmediateContext->VSSetConstantBuffers(0, 1, &_pConstantBuffer);
        _pImmediateContext->PSSetConstantBuffers(0, 1, &_pConstantBuffer);
        _pImmediateContext->PSSetShader(_pPixelShader, nullptr, 0);
        _pImmediateContext->PSSetShaderResources(0, 1, &_pDiffuseTexture[1]);
        _pImmediateContext->PSSetShaderResources(1, 1, &_pSpecularTexture[1]);
        _pImmediateContext->PSSetSamplers(0, 1, &_pSamplerLinear);
        if (i == 0)
        {
            _pImmediateContext->PSSetShaderResources(0, 1, &_pDiffuseTexture[2]);
            _pImmediateContext->PSSetShaderResources(1, 1, &_pSpecularTexture[2]);
            _pImmediateContext->DrawIndexed(objMeshData[0].IndexCount, 0, 0);
        }
        else if (i == 1) 
        {
            _pImmediateContext->PSSetShaderResources(0, 1, &_pDiffuseTexture[0]);
            _pImmediateContext->PSSetShaderResources(1, 1, &_pSpecularTexture[0]);
            _pImmediateContext->DrawIndexed(objMeshData[0].IndexCount, 0, 0);
        }
        else if (i == 2)
        {
            _pImmediateContext->PSSetShaderResources(0, 1, &_pDiffuseTexture[3]);
            _pImmediateContext->PSSetShaderResources(1, 1, &_pSpecularTexture[3]);
            _pImmediateContext->DrawIndexed(objMeshData[0].IndexCount, 0, 0);
        }
        else if (i == 3)
        {
            _pImmediateContext->PSSetShaderResources(0, 1, &_pDiffuseTexture[1]);
            _pImmediateContext->PSSetShaderResources(1, 1, &_pSpecularTexture[1]);
            _pImmediateContext->DrawIndexed(objMeshData[0].IndexCount, 0, 0);
        }
        else if (i == 8)
        {
            _pImmediateContext->PSSetShaderResources(0, 1, &_pDiffuseTexture[1]);
            _pImmediateContext->PSSetShaderResources(1, 1, &_pSpecularTexture[1]);
            _pImmediateContext->DrawIndexed(objMeshData[0].IndexCount, 0, 0);
        }
        else if (i < 9)
        {

            _pImmediateContext->PSSetShaderResources(0, 1, &_pDiffuseTexture[4]);
            _pImmediateContext->PSSetShaderResources(1, 1, &_pSpecularTexture[4]);
            _pImmediateContext->DrawIndexed(objMeshData[0].IndexCount, 0, 0);
        }
        else if (i > 9 && i < 109)
        {
            _pImmediateContext->PSSetShaderResources(0, 1, &_pDiffuseTexture[4]);
            _pImmediateContext->PSSetShaderResources(1, 1, &_pSpecularTexture[4]);
            _pImmediateContext->DrawIndexed(objMeshData[0].IndexCount, 0, 0);
        }
        else
        {

            _pImmediateContext->PSSetShaderResources(0, 1, &_pDiffuseTexture[1]);
            _pImmediateContext->PSSetShaderResources(1, 1, &_pSpecularTexture[1]);
            _pImmediateContext->DrawIndexed(objMeshData[1].IndexCount, 0, 0);
        }
    }

    _pImmediateContext->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    //
    // Present our back buffer to our front buffer
    //
    _pSwapChain->Present(0, 0);

}

HRESULT Application::InitWireframeView()
{
    D3D11_RASTERIZER_DESC wfdesc;
    ZeroMemory(&wfdesc, sizeof(D3D11_RASTERIZER_DESC));
    wfdesc.FillMode = D3D11_FILL_WIREFRAME;
    wfdesc.CullMode = D3D11_CULL_NONE;
    return _pd3dDevice->CreateRasterizerState(&wfdesc, &_wireFrame);
}

HRESULT Application::InitSolidView()
{
    D3D11_RASTERIZER_DESC sodesc;
    ZeroMemory(&sodesc, sizeof(D3D11_RASTERIZER_DESC));
    sodesc.FillMode = D3D11_FILL_SOLID;
    sodesc.CullMode = D3D11_CULL_BACK;
    return _pd3dDevice->CreateRasterizerState(&sodesc, &_solid);
}