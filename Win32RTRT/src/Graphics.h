#pragma once
#include "Core/Math.h"
#include "PlatformFunctions.h"



struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11VertexShader;
struct ID3D11InputLayout;
struct ID3D11PixelShader;
struct ID3D11Buffer;
struct ID3D11RenderTargetView;
struct ID3D11DepthStencilView;
struct ID3D11ShaderResourceView;
struct ID3D11SamplerState;
struct ID3D11BlendState;
struct ID3D11DepthStencilState;
struct ID3D11Texture2D;
struct Camera;

struct Material
{
	Color ambientColor;
	Color diffuseColor;
	Material()
	{
		ambientColor = Color(0,0,0,0);
		diffuseColor = Color(0,0,0,0);
	}

	Material(Color c, Color d)
	{
		ambientColor = c;
		diffuseColor = d;
	}
};

struct Model
{
	uint32 ID;
	uint32 startIndex;
	uint32 indexCount;
	Material materials[10];
	uint32 matCount;
};

struct ModelData
{
	uint32 vertexStride;
	ID3D11Buffer *vertexBuffer;
	ID3D11Buffer *indexBuffer;
};

struct PerTextureBufferData
{
	Rectangle2 sourceRect;
	Material mat;
};

struct PerModelBufferData
{
	Matrix4x4 model;
};

struct PerSceneBufferData
{
	Matrix4x4 projection;
	Matrix4x4 view;
};

struct GraphicsPipeline
{
	ID3D11VertexShader *vertexShader;
	ID3D11PixelShader *pixelShader;
	ID3D11InputLayout *inputLayout;
};

struct SceneRenderTarget
{
	ID3D11RenderTargetView *renderTarget;
	ID3D11ShaderResourceView *renderTexture;
	ID3D11DepthStencilView *depthStencilView;
	float width;
	float height;
};

struct ShadowRenderTarget
{
	ID3D11ShaderResourceView *renderTexture;
	ID3D11DepthStencilView *depthStencilView;
	float width;
	float height;
};

struct ScreenRenderTarget
{
	ID3D11RenderTargetView *renderTarget;
	ID3D11ShaderResourceView *renderTexture;
	float width;
	float height;
};

struct ConstantBuffer
{
	ID3D11Buffer *d3dBuffer;
};

struct InstanceBuffer
{
	ID3D11Buffer *d3dBuffer;
};

enum InputType
{
	FLOAT1 = 0,
	FLOAT2 = 1,
	FLOAT3 = 2,
	FLOAT4 = 3,
	UINT1 = 4
};

struct VertexInputLayout
{
	char **names;
	InputType *components;
	uint32 inputCount;
	char **instanceNames;
	InputType *instanceComponents;
	uint32 instanceCount;
};

struct Texture
{
	ID3D11ShaderResourceView *tex;
	ID3D11Texture2D *texResource;
	uint32 width;
	uint32 height;
};

struct TextureDesc
{
	uint32 width;
	uint32 height;
	uint32 mipCount;
	void *data;
};

struct Model3D
{
	ID3D11Buffer *vertexBuffer;
	ID3D11Buffer *indexBuffer;
	uint32 vertexStride;
	uint32 indexCount;
};

enum RenderType
{
	TRIANGLES,
	LINES
};

struct FontEngine
{
	Texture fontTexture;
};

enum BlendStateType
{
	NORMAL = 0,
	TEXTURE = 1
};

struct BlendState
{
	ID3D11BlendState *blendState;
	float blendFactor[4];
	uint32 mask;
};

struct ModelBatch
{
	PerModelBufferData *data;
	InstanceBuffer buffer;
	uint32 totalDataSize;
	Model *models;
	uint32 *modelsDataPointers;
	uint32 *modelsObjectCount;
	uint32 modelCount;
};

struct Renderer
{
	Vector2 screenSize;
	ID3D11Device *device;

	ID3D11DeviceContext *context;
	ID3D11SamplerState *normalSampler;
	ID3D11SamplerState *comparisonSampler;
	BlendState blendStates[2];

	ConstantBuffer perModelBuffer;
	ConstantBuffer materialBuffer;
	ConstantBuffer textureBuffer;

	FontEngine *fontEngine;
	Texture quadTex;
	Model quad;

	ModelBatch batch;
};

namespace Graphics
{
	void Init(Renderer *renderer);
	void InitModelSimple(Renderer * renderer, Model3D *model, void *vertexData, uint32 vertexDataSize,
				   uint32 vertexStride, uint32 *indices, uint32 indexCount);
	void InitModel(Model *model, uint32 startIndex, uint32 indexCount, uint32 ID, 
				   Material *materials, uint32 matCount);
	void InitModelData(Renderer *renderer, ModelData *modelData, void *vertexData, uint32 vertexDataSize, uint32 stride,
					   uint16 *indices, uint32 indexCount);
	void InitPipeline(Renderer *renderer, GraphicsPipeline *pipeline, VertexInputLayout inputLayout,
					  FileData *vertexShaderFile, FileData *pixelShaderFile);
	void InitConstantBuffer(Renderer *renderer, ConstantBuffer *constantBuffer, uint32 size);
	void InitInstanceBuffer(Renderer *renderer, InstanceBuffer *instanceBuffer, uint32 size);
	void InitSceneRenderTarget(Renderer *renderer, SceneRenderTarget *renderTarget);
	void InitScreenRenderTarget(Renderer *renderer, ScreenRenderTarget *renderTarget);
	void InitShadowRenderTarget(Renderer *renderer, ShadowRenderTarget *renderTarget);
	void InitFontEngine(Renderer *renderer, FontEngine *fontEngine, TextureDesc fontTextureDesc);
	void InitTexture(Renderer *renderer, Texture *texture, TextureDesc textureDesc); 
	void InitBatch(Renderer *renderer, ModelBatch *batch, void *space, uint32 size);

	void ClearRenderTarget(Renderer *renderer, SceneRenderTarget *renderTarget, Color color = Color(1,1,1));
	void ClearRenderTarget(Renderer *renderer, ScreenRenderTarget *renderTarget, Color color = Color(1,1,1));
	void ClearRenderTarget(Renderer *renderer, ShadowRenderTarget *renderTarget, Color color = Color(1,1,1));

	void BindRenderTarget(Renderer *renderer, SceneRenderTarget *renderTarget);
	void BindRenderTarget(Renderer *renderer, ScreenRenderTarget *renderTarget);
	void BindRenderTarget(Renderer *renderer, ShadowRenderTarget *renderTarget);
	void SetBlendState(Renderer *renderer, BlendStateType blendType);
	void BindTexture(Renderer *renderer, SceneRenderTarget *renderTarget);
	void BindTexture(Renderer *renderer, ScreenRenderTarget *renderTarget);
	void BindTexture(Renderer *renderer, ShadowRenderTarget *renderTarget);
	void BindTexture(Renderer *renderer, Texture *texture);
	void SetFontEngine(Renderer *renderer, FontEngine *fontEngine);

	void UnbindTexture(Renderer *renderer);

	void BindConstantBuffer(Renderer *renderer, ConstantBuffer *buffer, void *data, uint32 slot);
	void BindPipeline(Renderer *renderer, GraphicsPipeline *pipeline);
	void BindModelData(Renderer *renderer, ModelData *modelData, ModelBatch *modelBatch);

	void ResetBatch(ModelBatch *batch);
	void BatchModel(ModelBatch *batch, Model *model, PerModelBufferData data);
	void RenderBatch(Renderer *renderer, ModelBatch *batch);
	void RenderModel(Renderer *renderer, Model *model, RenderType renderType = TRIANGLES);
	void RenderModel(Renderer *renderer, Model3D *model);
	void RenderText(Renderer *renderer, char *text, Vector2 position, float size);
	void RenderQuad(Renderer *renderer, Vector2 position, float width, float height, Color color);

	float FontWidthFromHeight(float height);

	void Release(Renderer *renderer);
	void Release(ModelData *model);
	void Release(SceneRenderTarget *renderTarget);
	void Release(ShadowRenderTarget *renderTarget);
	void Release(ScreenRenderTarget *renderTarget);
	void Release(GraphicsPipeline *pipeline);
	void Release(ConstantBuffer *buffer);
	void Release(Texture *tex);
	void Release(FontEngine *fontEngine);
	void Release(ModelBatch *batch);
	void Release(InstanceBuffer *buffer);
	void Release(Model3D *model);
}