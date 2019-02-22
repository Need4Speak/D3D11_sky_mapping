//Include and link appropriate libraries and headers//
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3dx10.lib")
#pragma comment (lib, "D3D10_1.lib")
#pragma comment (lib, "DXGI.lib")
#pragma comment (lib, "D2D1.lib")
#pragma comment (lib, "dwrite.lib")
#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")

#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>
#include <xnamath.h>
#include <D3D10_1.h>
#include <DXGI.h>
#include <D2D1.h>
#include <sstream>
#include <dwrite.h>
#include <dinput.h>
#include <vector>

//Global Declarations - Interfaces//
IDXGISwapChain* SwapChain;
ID3D11Device* d3d11Device;
ID3D11DeviceContext* d3d11DevCon;
ID3D11RenderTargetView* renderTargetView;
ID3D11Buffer* squareIndexBuffer;
ID3D11DepthStencilView* depthStencilView;
ID3D11Texture2D* depthStencilBuffer;
ID3D11Buffer* squareVertBuffer;
ID3D11VertexShader* VS;
ID3D11PixelShader* PS;
ID3D11PixelShader* D2D_PS;
ID3D10Blob* D2D_PS_Buffer;
ID3D10Blob* VS_Buffer;
ID3D10Blob* PS_Buffer;
ID3D11InputLayout* vertLayout;
ID3D11Buffer* cbPerObjectBuffer;
ID3D11BlendState* Transparency;
ID3D11RasterizerState* CCWcullMode;
ID3D11RasterizerState* CWcullMode;
ID3D11RasterizerState* CarcullMode;
ID3D11ShaderResourceView* GroundTexture;
ID3D11ShaderResourceView* BoxTexture;
ID3D11SamplerState* CubesTexSamplerState;
ID3D11Buffer* cbPerFrameBuffer;

ID3D10Device1 *d3d101Device;
IDXGIKeyedMutex *keyedMutex11;
IDXGIKeyedMutex *keyedMutex10;
ID2D1RenderTarget *D2DRenderTarget;
ID2D1SolidColorBrush *Brush;
ID3D11Texture2D *BackBuffer11;
ID3D11Texture2D *sharedTex11;
ID3D11Buffer *d2dVertBuffer;
ID3D11Buffer *d2dIndexBuffer;
ID3D11ShaderResourceView *d2dTexture;
IDWriteFactory *DWriteFactory;
IDWriteTextFormat *TextFormat;

IDirectInputDevice8* DIKeyboard;
IDirectInputDevice8* DIMouse;

ID3D11Buffer* sphereIndexBuffer;
ID3D11Buffer* sphereVertBuffer;
// 车体模型
ID3D11Buffer* carIndexBuffer;
ID3D11Buffer* carVertBuffer;

// 轮子模型
ID3D11Buffer* wheelIndexBuffer;
ID3D11Buffer* wheelVertBuffer;

ID3D11VertexShader* SKYMAP_VS;
ID3D11PixelShader* SKYMAP_PS;
ID3D10Blob* SKYMAP_VS_Buffer;
ID3D10Blob* SKYMAP_PS_Buffer;

ID3D11ShaderResourceView* smrv;

ID3D11DepthStencilState* DSLessEqual;
ID3D11RasterizerState* RSCullNone;

std::wstring printText;

//Global Declarations - Others//
LPCTSTR WndClassName = L"firstwindow";
HWND hwnd = NULL;
HRESULT hr;

int Width = 800;
int Height = 600;

DIMOUSESTATE mouseLastState;
LPDIRECTINPUT8 DirectInput;

float rotx = 0;
float rotz = 0;
float scaleX = 1.0f;
float scaleY = 1.0f;

XMMATRIX Rotationx;
XMMATRIX Rotationy;
XMMATRIX Rotationz;

XMMATRIX WVP;
// 小车
XMMATRIX boxWorld;
XMMATRIX wheelWorld[4];
XMMATRIX wheel1World;
XMMATRIX wheel2World;
XMMATRIX wheel3World;
XMMATRIX wheel4World;
XMMATRIX houseWorld;
// 地表
XMMATRIX groundWorld;
// 天空盒
XMMATRIX sphereWorld;

XMMATRIX d2dWorld;
bool FirstPerson = false;
// 摄像机
XMMATRIX camView;
XMMATRIX camProjection;
XMVECTOR camPosition;
XMVECTOR camTarget;
XMVECTOR camUp;
XMVECTOR DefaultForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
XMVECTOR DefaultRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
XMVECTOR camForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
XMVECTOR camRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
XMMATRIX camRotationMatrix;
bool CamUpdateFlag = false;


float moveLeftRight = 0.0f;
float moveBackForward = 0.0f;

float carForward = 0.0f;
float carLeft = 0.0f;
float carLastForward = 0.0f;
float carLastLeft = 0.0f;

float camYaw = 0.0f;
float camPitch = 0.0f;
int position = 0;

// 小车左右转向角度
float camCarTurnRigth = 0.0f;

int cylinderVerticesCount;

int NumSphereVertices;
int NumSphereFaces;

XMMATRIX Rotation;
XMMATRIX Scale;
XMMATRIX Translation;
float rot = 0.01f;

double countsPerSecond = 0.0;
__int64 CounterStart = 0;

int frameCount = 0;
int fps = 0;

__int64 frameTimeOld = 0;
double frameTime;

// 结构体
struct Vertex	//Overloaded Vertex Structure
{
	Vertex() {}
	Vertex(float x, float y, float z,
		float u, float v,
		float nx, float ny, float nz)
		: pos(x, y, z), texCoord(u, v), normal(nx, ny, nz) {}

	XMFLOAT3 pos;
	XMFLOAT2 texCoord;
	XMFLOAT3 normal;
};

struct MeshData
{
	std::vector<Vertex> Vertices;
	std::vector<UINT> Indices;
};

struct cbPerObject
{
	XMMATRIX  WVP;
	XMMATRIX World;
};
cbPerObject cbPerObj;

struct Light
{
	Light()
	{
		ZeroMemory(this, sizeof(Light));
	}
	XMFLOAT3 dir;
	float pad;
	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;
};
Light light;

struct cbPerFrame
{
	Light  light;
};
cbPerFrame constbuffPerFrame;

D3D11_INPUT_ELEMENT_DESC layout[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL",	 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0}
};
UINT numElements = ARRAYSIZE(layout);

// 函数声明
bool InitializeDirect3d11App(HINSTANCE hInstance);
void CleanUp();
bool InitScene();
void DrawScene();
void UpdateScene(double time);
bool InitializeWindow(HINSTANCE hInstance,
	int ShowWnd,
	int width, int height,
	bool windowed);
int messageloop();
//2d文字
bool InitD2D_D3D101_DWrite(IDXGIAdapter1 *Adapter);
void InitD2DScreenTexture();
void RenderText(std::wstring text, int inInt);

void UpdateCamera();
bool InitCarBuffer();
void InitBoxBuffer();
void InitWheelBuffer();
void DrawCar();
void DrawBox();
void DrawWheel();
void UpdateCar();
void UpdateBox(int position);
void UpdateWheel(int position);
// 创建几何体
void CreateSphere(int LatLines, int LongLines);
void CreateCylinder(float bottomRadius, float topRadius, float height, UINT sliceCount, UINT stackCount, MeshData& meshData);
void BuildCylinderTopCap(float bottomRadius, float topRadius, float height, UINT sliceCount, UINT stackCount, MeshData& meshData);
void BuildCylinderBottomCap(float bottomRadius, float topRadius, float height, UINT sliceCount, UINT stackCount, MeshData& meshData);
// timer
void StartTimer();
double GetTime();
double GetFrameTime();
// 输入
bool InitDirectInput(HINSTANCE hInstance);
void DetectInput(double time);

LRESULT CALLBACK WndProc(HWND hWnd,
	UINT msg,
	WPARAM wParam,
	LPARAM lParam);