#include <D3D11_1.h>
#include <float.h>
#include "Graphics.h"
#include "Core\String.h"
#define CHECK_WIN_ERROR(x,y) if(FAILED(x)) {OutputDebugStringA(y); return;}
#define RELEASE_DX_RESOURCE(x) if(x){x->Release(); x = NULL;}

void Graphics::InitModel(Model *model, uint32 startIndex, uint32 indexCount, uint32 ID, 
						 Material *materials, uint32 matCount)
{
	for (uint32 i = 0; i < matCount; ++i)
	{
		model->materials[i] = materials[i];
	}

	model->matCount = matCount;
	model->indexCount = indexCount;
	model->startIndex = startIndex;
	model->ID = ID;
}

void Graphics::InitBatch(Renderer *renderer, ModelBatch *batch, void *space, uint32 size)
{
	uint32 initialModelCount = 30;
	batch->modelCount = initialModelCount;
	uint32 modelInfoRequired = initialModelCount * (sizeof(Model) + sizeof(uint32) * 2);
	if (size < modelInfoRequired)
	{
		OutputDebugStringA("Model Batch size provided not sufficient\n");
		return;
	}
	batch->models = (Model *)space;
	batch->modelsDataPointers = (uint32 *)((uint8 *)space + sizeof(Model) * initialModelCount);
	batch->modelsObjectCount = (uint32 *)((uint8 *)batch->modelsDataPointers + sizeof(uint32) * initialModelCount);
	batch->data = (PerModelBufferData *)((uint8 *)batch->modelsObjectCount + sizeof(uint32) * initialModelCount);
	batch->totalDataSize = (size - modelInfoRequired) / sizeof(PerModelBufferData);

	for (uint32 i = 0; i < initialModelCount; ++i)
	{
		batch->modelsDataPointers[i] = (batch->totalDataSize / initialModelCount) * i;
	}

	Graphics::InitInstanceBuffer(renderer, &batch->buffer, sizeof(PerModelBufferData) * batch->totalDataSize);
}

void Graphics::InitModelData(Renderer *renderer, ModelData *modelData, void *vertexData, uint32 vertexDataSize, uint32 stride,
							 uint16 *indices, uint32 indexCount)
{
	ID3D11Device *device = renderer->device;

	D3D11_SUBRESOURCE_DATA vertexBufferData = {};
	vertexBufferData.pSysMem = vertexData;
	modelData->vertexStride = stride;

	CD3D11_BUFFER_DESC vertexBufferDesc(vertexDataSize, D3D11_BIND_VERTEX_BUFFER);
	HRESULT hr = device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &modelData->vertexBuffer);
	CHECK_WIN_ERROR(hr, "Error creating vertex buffer\n");

	D3D11_SUBRESOURCE_DATA indexBufferData = {0};
	indexBufferData.pSysMem = indices;
	CD3D11_BUFFER_DESC indexBufferDesc(sizeof(uint16) * indexCount, D3D11_BIND_INDEX_BUFFER);

	hr = device->CreateBuffer(&indexBufferDesc, &indexBufferData, &modelData->indexBuffer);
	CHECK_WIN_ERROR(hr, "Error creating index buffer\n");
}

void Graphics::InitModelSimple(Renderer *renderer, Model3D *model, void *vertexData, uint32 vertexDataSize,
						 uint32 vertexStride,  uint32 *indices, uint32 indexCount)
{
	ID3D11Device *device = renderer->device;
	model->vertexStride = vertexStride;
	model->indexCount = indexCount;

	D3D11_SUBRESOURCE_DATA vertexBufferData = {};
	vertexBufferData.pSysMem = vertexData;
	CD3D11_BUFFER_DESC vertexBufferDesc(vertexDataSize, D3D11_BIND_VERTEX_BUFFER);
	HRESULT hr = device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &model->vertexBuffer);
	CHECK_WIN_ERROR(hr, "Error creating vertex buffer\n");

	D3D11_SUBRESOURCE_DATA indexBufferData = {};
	indexBufferData.pSysMem = indices;
	CD3D11_BUFFER_DESC indexBufferDesc(indexCount * sizeof(uint32), D3D11_BIND_INDEX_BUFFER);
	hr = device->CreateBuffer(&indexBufferDesc, &indexBufferData, &model->indexBuffer);
	CHECK_WIN_ERROR(hr, "Error creating index buffer\n");
}

void Graphics::Init(Renderer *renderer)
{
	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.MinLOD = -FLT_MAX;
	samplerDesc.MaxLOD = FLT_MAX;
	HRESULT hr = renderer->device->CreateSamplerState(&samplerDesc, &renderer->normalSampler);
	CHECK_WIN_ERROR(hr, "Error creating Sampler \n");

	D3D11_SAMPLER_DESC cmpSamplerDesc = {};
	cmpSamplerDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
	cmpSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	cmpSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	cmpSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	cmpSamplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
	cmpSamplerDesc.MinLOD = -FLT_MAX;
	cmpSamplerDesc.MaxLOD = FLT_MAX;
	hr = renderer->device->CreateSamplerState(&cmpSamplerDesc, &renderer->comparisonSampler);
	CHECK_WIN_ERROR(hr, "Error creating Sampler \n");

	renderer->fontEngine = NULL;

	D3D11_RENDER_TARGET_BLEND_DESC renderBlendDesc = {0};
	renderBlendDesc.BlendEnable = true;
	renderBlendDesc.SrcBlend = D3D11_BLEND_SRC_ALPHA;
	renderBlendDesc.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	renderBlendDesc.BlendOp = D3D11_BLEND_OP_ADD;
	renderBlendDesc.SrcBlendAlpha = D3D11_BLEND_INV_DEST_ALPHA;
	renderBlendDesc.DestBlendAlpha = D3D11_BLEND_ONE;
	renderBlendDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	renderBlendDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	D3D11_BLEND_DESC blendDesc = {0};
	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.RenderTarget[0] = renderBlendDesc;

	hr = renderer->device->CreateBlendState(&blendDesc, &renderer->blendStates[TEXTURE].blendState);
	CHECK_WIN_ERROR(hr, "Error creating blend state\n");
	Vector4 *blendFactor = (Vector4 *)&renderer->blendStates[TEXTURE].blendFactor[0];
	*blendFactor = Vector4(1,1,1,1);
	renderer->blendStates[TEXTURE].mask = 0xffffffff;

	renderer->context->OMGetBlendState(&renderer->blendStates[NORMAL].blendState, 
									   renderer->blendStates[NORMAL].blendFactor, 
									   &renderer->blendStates[NORMAL].mask);


	uint32 whiteTex[] = {
		0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
		0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
		0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
		0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
	};

	TextureDesc texDesc;
	texDesc.mipCount = 1;
	texDesc.width = 4;
	texDesc.height = 4;
	texDesc.data = whiteTex;
	Graphics::InitTexture(renderer, &renderer->quadTex, texDesc);

	Graphics::InitConstantBuffer(renderer, &renderer->textureBuffer, sizeof(PerTextureBufferData));
	Graphics::InitConstantBuffer(renderer, &renderer->perModelBuffer, sizeof(PerModelBufferData));
	Graphics::InitConstantBuffer(renderer, &renderer->materialBuffer, sizeof(Material) * 13);
}

void Graphics::InitConstantBuffer(Renderer *renderer, ConstantBuffer *buffer, uint32 size)
{
	uint32 alignedSize = (((size - 1) / 16) + 1) * 16;
	CD3D11_BUFFER_DESC perSceneBufferDesc(alignedSize, D3D11_BIND_CONSTANT_BUFFER);
	HRESULT hr = renderer->device->CreateBuffer(&perSceneBufferDesc, NULL, &buffer->d3dBuffer);
	CHECK_WIN_ERROR(hr, "Error creating Per Scene buffer\n");
}

void Graphics::InitInstanceBuffer(Renderer *renderer, InstanceBuffer *buffer, uint32 size)
{
	D3D11_BUFFER_DESC instanceBufferDesc;
	instanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	instanceBufferDesc.ByteWidth = size;
	instanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	instanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	instanceBufferDesc.MiscFlags = 0;
	HRESULT hr = renderer->device->CreateBuffer(&instanceBufferDesc, NULL, &buffer->d3dBuffer);
	CHECK_WIN_ERROR(hr, "Error creating instance buffer\n");
}

void Graphics::InitPipeline(Renderer *renderer, GraphicsPipeline *pipeline, VertexInputLayout inputLayout,
							FileData *vertexShaderFile, FileData *pixelShaderFile)
{
	ID3D11Device *device = renderer->device;
	HRESULT hr = device->CreateVertexShader(vertexShaderFile->data, vertexShaderFile->size, 
											NULL, &pipeline->vertexShader);
	CHECK_WIN_ERROR(hr, "Error creating vertex shader\n");

	DXGI_FORMAT formatFromType[5] = {DXGI_FORMAT_R32_FLOAT, DXGI_FORMAT_R32G32_FLOAT, DXGI_FORMAT_R32G32B32_FLOAT, DXGI_FORMAT_R32G32B32A32_FLOAT, 
		DXGI_FORMAT_R32_FLOAT};

	D3D11_INPUT_ELEMENT_DESC *vertexDesc = new D3D11_INPUT_ELEMENT_DESC[inputLayout.inputCount + inputLayout.instanceCount];
	for (uint32 i = 0; i < inputLayout.inputCount; i++)
	{
		D3D11_INPUT_ELEMENT_DESC *desc = &vertexDesc[i];
		desc->SemanticName = inputLayout.names[i];
		desc->SemanticIndex = 0;
		desc->Format = formatFromType[inputLayout.components[i]];
		desc->InputSlot = 0;
		desc->AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		desc->InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		desc->InstanceDataStepRate = 0;
	}

	uint32 semanticIndex = 0;
	for (uint32 i = 0; i < inputLayout.instanceCount; i++)
	{
		D3D11_INPUT_ELEMENT_DESC *desc = &vertexDesc[inputLayout.inputCount + i];
		if (i > 0 && String::Compare(inputLayout.instanceNames[i], inputLayout.instanceNames[i - 1]))
		{
			++semanticIndex;
		}
		else
		{
			semanticIndex = 0;
		}
		desc->SemanticName = inputLayout.instanceNames[i];
		desc->SemanticIndex = semanticIndex;
		desc->Format = formatFromType[inputLayout.instanceComponents[i]];
		desc->InputSlot = 1;
		desc->AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		desc->InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
		desc->InstanceDataStepRate = 1;
	}

	hr = device->CreateInputLayout(vertexDesc, inputLayout.inputCount + inputLayout.instanceCount, 
								   vertexShaderFile->data, vertexShaderFile->size,
								   &pipeline->inputLayout);

	delete []vertexDesc;

	CHECK_WIN_ERROR(hr, "Error creating input layout\n");

	if (pixelShaderFile)
	{
		hr = device->CreatePixelShader(pixelShaderFile->data, pixelShaderFile->size, 
									   NULL, &pipeline->pixelShader);
		CHECK_WIN_ERROR(hr, "Error creating pixel shader\n");
	}
}

void Graphics::InitScreenRenderTarget(Renderer *renderer, ScreenRenderTarget *renderTarget)
{
	ID3D11Device *device = renderer->device;

	renderTarget->width = renderer->screenSize.x;
	renderTarget->height = renderer->screenSize.y;

	D3D11_TEXTURE2D_DESC textureDesc = {0};
	textureDesc.Width = (UINT)renderTarget->width;
	textureDesc.Height = (UINT)renderTarget->height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	ID3D11Texture2D *texture;
	HRESULT hr = renderer->device->CreateTexture2D(&textureDesc, NULL, &texture);
	CHECK_WIN_ERROR(hr, "Error creating render target texture\n");

	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;
	hr = renderer->device->CreateRenderTargetView(texture, &renderTargetViewDesc, &renderTarget->renderTarget);
	if (SUCCEEDED(hr))
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
		shaderResourceViewDesc.Format = textureDesc.Format;
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		shaderResourceViewDesc.Texture2D.MipLevels = 1;

		hr = renderer->device->CreateShaderResourceView(texture, &shaderResourceViewDesc, &renderTarget->renderTexture);
		if (FAILED(hr))
		{
			OutputDebugStringA("Error creating render target texture\n");
		}
	}
	else
	{
		OutputDebugStringA("Error creating render target\n");
	}
	texture->Release();
}

void Graphics::InitSceneRenderTarget(Renderer *renderer, SceneRenderTarget *renderTarget)
{
	ID3D11Device *device = renderer->device;

#ifdef WP
	float ratio = 1.0f;
#else
	float ratio = 1.0f;
#endif
	renderTarget->width = renderer->screenSize.x * ratio;
	renderTarget->height = renderer->screenSize.y * ratio;

	D3D11_TEXTURE2D_DESC textureDesc = {0};
	textureDesc.Width = (UINT)renderTarget->width;
	textureDesc.Height = (UINT)renderTarget->height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	ID3D11Texture2D *texture;
	HRESULT hr = renderer->device->CreateTexture2D(&textureDesc, NULL, &texture);
	CHECK_WIN_ERROR(hr, "Error creating render target texture\n");

	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;
	hr = renderer->device->CreateRenderTargetView(texture, &renderTargetViewDesc, &renderTarget->renderTarget);
	if (SUCCEEDED(hr))
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
		shaderResourceViewDesc.Format = textureDesc.Format;
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		shaderResourceViewDesc.Texture2D.MipLevels = 1;

		hr = renderer->device->CreateShaderResourceView(texture, &shaderResourceViewDesc, &renderTarget->renderTexture);
		if (FAILED(hr))
		{
			OutputDebugStringA("Error creating render target texture\n");
		}
	}
	else
	{
		OutputDebugStringA("Error creating render target\n");
	}
	texture->Release();

	CD3D11_TEXTURE2D_DESC depthStencilDesc(DXGI_FORMAT_D24_UNORM_S8_UINT, (UINT)renderTarget->width, (UINT)renderTarget->height, 
										   1, 1, D3D11_BIND_DEPTH_STENCIL);
	ID3D11Texture2D *depthStencil;
	hr = renderer->device->CreateTexture2D(&depthStencilDesc, NULL, &depthStencil);
	CHECK_WIN_ERROR(hr, "Error creating depth stencil");

	CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
	hr = renderer->device->CreateDepthStencilView(depthStencil, &depthStencilViewDesc, &renderTarget->depthStencilView);
	depthStencil->Release();
	CHECK_WIN_ERROR(hr, "Error creating depth stencil view\n");
}

void Graphics::InitShadowRenderTarget(Renderer *renderer, ShadowRenderTarget *renderTarget)
{
	ID3D11Device *device = renderer->device;

#ifdef WP
	float ratio = 0.5f;
#else
	float ratio = 2.0f;
#endif
	renderTarget->width = renderer->screenSize.x * ratio;
	renderTarget->height = renderer->screenSize.y * ratio;
	D3D11_TEXTURE2D_DESC textureDesc = {0};
	textureDesc.Width = (UINT)renderTarget->width;
	textureDesc.Height = (UINT)renderTarget->height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
#ifdef WP
	textureDesc.Format = DXGI_FORMAT_R16_TYPELESS;
#else
	textureDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
#endif
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	ID3D11Texture2D *depthStencil;
	HRESULT hr = renderer->device->CreateTexture2D(&textureDesc, NULL, &depthStencil);
	CHECK_WIN_ERROR(hr, "Error creating depth stencil\n");

#ifdef WP
	CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D, DXGI_FORMAT_D16_UNORM);
#else
	CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D, DXGI_FORMAT_D24_UNORM_S8_UINT);
#endif
	hr = renderer->device->CreateDepthStencilView(depthStencil, &depthStencilViewDesc, &renderTarget->depthStencilView);
	CHECK_WIN_ERROR(hr, "Error creating depth stencil view\n");

	if (SUCCEEDED(hr))
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
#ifdef WP
		shaderResourceViewDesc.Format = DXGI_FORMAT_R16_UNORM;
#else
		shaderResourceViewDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
#endif
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		shaderResourceViewDesc.Texture2D.MipLevels = 1;

		hr = renderer->device->CreateShaderResourceView(depthStencil, &shaderResourceViewDesc, &renderTarget->renderTexture);
		if (FAILED(hr))
		{
			OutputDebugStringA("Error creating render target texture\n");
		}
	}
	else
	{
		OutputDebugStringA("Error creating render target\n");
	}
	depthStencil->Release();
}

void Graphics::InitFontEngine(Renderer *renderer, FontEngine *fontEngine, TextureDesc fontTexDesc)
{
	Graphics::InitTexture(renderer, &fontEngine->fontTexture, fontTexDesc);
}

void Graphics::InitTexture(Renderer *renderer, Texture *texture, TextureDesc textureDesc)
{
	D3D11_TEXTURE2D_DESC texDesc = {};
	texDesc.Width = textureDesc.width;
	texDesc.Height = textureDesc.height;
	texDesc.MipLevels = textureDesc.mipCount;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DYNAMIC;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA texData[15] = {};
	uint8 *mipData = (uint8 *)textureDesc.data;
	uint32 mipWidth = textureDesc.width;
	uint32 mipHeight = textureDesc.height;
	for (uint32 i = 0; i < ARRAYSIZE(texData) && i < textureDesc.mipCount; i++)
	{
		texData[i].pSysMem = mipData;
		texData[i].SysMemPitch = mipWidth * 4;
		mipData += mipWidth * mipHeight * 4;
		mipWidth /= 2;
		mipHeight /= 2;
	}

	HRESULT hr = renderer->device->CreateTexture2D(&texDesc, texData, &texture->texResource);
	CHECK_WIN_ERROR(hr, "Error creating texture2D\n");

	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc = {};
	viewDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	viewDesc.Texture2D.MostDetailedMip = 0;
	viewDesc.Texture2D.MipLevels = -1;

	hr = renderer->device->CreateShaderResourceView(texture->texResource, &viewDesc, &texture->tex);
	CHECK_WIN_ERROR(hr, "Error creating texture view\n");
}

void Graphics::SetFontEngine(Renderer *renderer, FontEngine *engine)
{
	renderer->fontEngine = engine;
}

static const D3D11_PRIMITIVE_TOPOLOGY topologyFromRenderType[2] = 
{
	D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
	D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP
};

void Graphics::BindModelData(Renderer *renderer, ModelData *modelData, ModelBatch *modelBatch)
{
	ID3D11DeviceContext *context = renderer->context;
	UINT vertexStrides[] = { modelData->vertexStride, sizeof(PerModelBufferData) };
	UINT offset[] = { 0, 0 };
	ID3D11Buffer *buffers[] = {modelData->vertexBuffer, modelBatch->buffer.d3dBuffer};
	context->IASetVertexBuffers(0, 2, buffers, vertexStrides, offset);
	context->IASetIndexBuffer(modelData->indexBuffer, DXGI_FORMAT_R16_UINT, 0);
	context->IASetPrimitiveTopology(topologyFromRenderType[TRIANGLES]);
}

void Graphics::RenderModel(Renderer *renderer, Model *model, RenderType renderType)
{
	ID3D11DeviceContext *context = renderer->context;
	context->DrawIndexed(model->indexCount, model->startIndex, 0); 
}

void Graphics::RenderModel(Renderer *renderer, Model3D *model)
{
	ID3D11DeviceContext *context = renderer->context;
	UINT vertexStride = model->vertexStride;
	UINT offset = 0;

	context->IASetVertexBuffers(0, 1, &model->vertexBuffer, &vertexStride,
		&offset);
	context->IASetIndexBuffer(model->indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->DrawIndexed(model->indexCount, 0, 0);
}

void Graphics::RenderBatch(Renderer *renderer, ModelBatch *batch)
{
	ID3D11DeviceContext *context = renderer->context;
	for (uint32 modelID = 0; modelID < batch->modelCount; ++modelID)
	{
		uint32 objectCount = batch->modelsObjectCount[modelID];
		if (objectCount > 0)
		{
			Model *model = &batch->models[modelID];
			Graphics::BindConstantBuffer(renderer, &renderer->materialBuffer, model->materials, 4);
			D3D11_MAPPED_SUBRESOURCE mappedInstancedData = {};
			ZeroMemory(&mappedInstancedData, sizeof(D3D11_MAPPED_SUBRESOURCE));
			HRESULT hr = context->Map(batch->buffer.d3dBuffer, 0, D3D11_MAP_WRITE_DISCARD,
										0, &mappedInstancedData);
			if (SUCCEEDED(hr))
			{
				void *dataPtr = (void *)&batch->data[batch->modelsDataPointers[modelID]];
				memcpy(mappedInstancedData.pData, dataPtr, sizeof(PerModelBufferData) * objectCount);
			}
			context->Unmap(batch->buffer.d3dBuffer, 0);
			uint32 instanceOffset = batch->modelsDataPointers[modelID];
			context->DrawIndexedInstanced(model->indexCount, objectCount, model->startIndex, 0, 0);
		}
	}
}

void Graphics::BatchModel(ModelBatch *batch, Model *model, PerModelBufferData data)
{
	Assert(model->ID < batch->modelCount);
	Assert(batch->modelsDataPointers[model->ID] + batch->modelsObjectCount[model->ID] < batch->totalDataSize);
	batch->models[model->ID] = *model;
	batch->data[batch->modelsDataPointers[model->ID] + batch->modelsObjectCount[model->ID]] = data;
	++batch->modelsObjectCount[model->ID];
}

float Graphics::FontWidthFromHeight(float height)
{
	return height * 0.4f;
}

void Graphics::RenderText(Renderer *renderer, char *text, Vector2 position, float size)
{
	FontEngine *engine = renderer->fontEngine;
	Graphics::BindTexture(renderer, &engine->fontTexture);

	Material textMat(Color(1,1,1,1), Color(0,0,0,0));

	float fontHeight = size;
	float normalizedWidth = Graphics::FontWidthFromHeight(1.0f);
	float fontWidth = Graphics::FontWidthFromHeight(fontHeight);
	char *c = text;
	float currentXOffset = 0.0f;
	float paddingX = (0.1f - 0.1f * normalizedWidth) / 2.0f;
	while (*c != '\0')
	{
		Matrix4x4 translation = Math::GetTranslation(position.x + currentXOffset, position.y, 0);
		Matrix4x4 modelMatrix = Math::Transpose(translation * Math::GetScale(fontWidth, fontHeight, 0));
		PerModelBufferData perModelBufferData = { modelMatrix };
		Graphics::BindConstantBuffer(renderer, &renderer->perModelBuffer, &perModelBufferData, 1);

		uint32 offsetASCII = *(c++) - 32;
		uint32 x = offsetASCII % 10;
		uint32 y = offsetASCII / 10;
		Rectangle2 sourceRect = Rectangle2(0.1f * x + paddingX,0.1f * y,0.1f * normalizedWidth, 0.1f);
		PerTextureBufferData perTextureData = {sourceRect, textMat};
		Graphics::BindConstantBuffer(renderer, &renderer->textureBuffer, &perTextureData, 2);
		Graphics::RenderModel(renderer, &renderer->quad);

		currentXOffset += fontWidth;
	}
	Graphics::UnbindTexture(renderer);
}

void Graphics::RenderQuad(Renderer *renderer, Vector2 position, float width, float height, Color color)
{
	Graphics::BindTexture(renderer, &renderer->quadTex);

	Material textMat(color, Color(0,0,0,0));

	Matrix4x4 translation = Math::GetTranslation(position.x, position.y, 0);
	Matrix4x4 modelMatrix = Math::Transpose(translation * Math::GetScale(width, height, 0));
	PerModelBufferData perModelBufferData = { modelMatrix};
	Graphics::BindConstantBuffer(renderer, &renderer->perModelBuffer, &perModelBufferData, 1);

	Rectangle2 sourceRect = Rectangle2(0,0,1,1);
	PerTextureBufferData perTextureData = {sourceRect, textMat};
	Graphics::BindConstantBuffer(renderer, &renderer->textureBuffer, &perTextureData, 2);
	Graphics::RenderModel(renderer, &renderer->quad);
	Graphics::UnbindTexture(renderer);
}

void Graphics::ResetBatch(ModelBatch *batch)
{
	for (uint32 i = 0; i < batch->modelCount; ++i)
	{
		batch->modelsObjectCount[i] = 0;
	}
}

void Graphics::BindPipeline(Renderer *renderer, GraphicsPipeline *pipeline)
{
	renderer->context->VSSetShader(pipeline->vertexShader, NULL, 0);
	renderer->context->IASetInputLayout(pipeline->inputLayout);
	renderer->context->PSSetShader(pipeline->pixelShader, 0, 0);
}

void Graphics::BindConstantBuffer(Renderer *renderer, ConstantBuffer *buffer, void *data, uint32 slot)
{
	renderer->context->UpdateSubresource(buffer->d3dBuffer, 0, NULL, data, 0, 0);
	renderer->context->VSSetConstantBuffers(slot, 1, &buffer->d3dBuffer);
	renderer->context->PSSetConstantBuffers(slot, 1, &buffer->d3dBuffer);
}

void Graphics::ClearRenderTarget(Renderer *renderer, SceneRenderTarget *renderTarget, Color color)
{
	const float clearColor[] = { color.r, color.g, color.b, color.a};
	renderer->context->ClearRenderTargetView(renderTarget->renderTarget, clearColor);
	renderer->context->ClearDepthStencilView(renderTarget->depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void Graphics::ClearRenderTarget(Renderer *renderer, ScreenRenderTarget *renderTarget, Color color)
{
	const float clearColor[] = { color.r, color.g, color.b, color.a};
	renderer->context->ClearRenderTargetView(renderTarget->renderTarget, clearColor);
}

void Graphics::ClearRenderTarget(Renderer *renderer, ShadowRenderTarget *renderTarget, Color color)
{
	renderer->context->ClearDepthStencilView(renderTarget->depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void Graphics::BindRenderTarget(Renderer *renderer, SceneRenderTarget *renderTarget)
{
	CD3D11_VIEWPORT viewport(0.0f,0.0f, renderTarget->width, renderTarget->height);
	renderer->context->RSSetViewports(1, &viewport);
	renderer->context->OMSetRenderTargets(1, &renderTarget->renderTarget, renderTarget->depthStencilView);
}

void Graphics::BindRenderTarget(Renderer *renderer, ScreenRenderTarget *renderTarget)
{
	CD3D11_VIEWPORT viewport(0.0f,0.0f, renderTarget->width, renderTarget->height);
	renderer->context->RSSetViewports(1, &viewport);
	renderer->context->OMSetRenderTargets(1, &renderTarget->renderTarget, NULL);
}

void Graphics::BindRenderTarget(Renderer *renderer, ShadowRenderTarget *renderTarget)
{
	CD3D11_VIEWPORT viewport(0.0f,0.0f, renderTarget->width, renderTarget->height);
	renderer->context->RSSetViewports(1, &viewport);
	ID3D11RenderTargetView *renderTargets[] = {NULL};
	renderer->context->OMSetRenderTargets(1, renderTargets, renderTarget->depthStencilView);
}

void Graphics::SetBlendState(Renderer *renderer, BlendStateType blendType)
{
	BlendState *blendState = &renderer->blendStates[blendType];
	renderer->context->OMSetBlendState(blendState->blendState, blendState->blendFactor, blendState->mask);
}

void Graphics::BindTexture(Renderer *renderer, SceneRenderTarget *renderTarget)
{
	renderer->context->PSSetShaderResources(0, 1, &renderTarget->renderTexture);
	renderer->context->PSSetSamplers(0, 1, &renderer->normalSampler);
}

void Graphics::BindTexture(Renderer *renderer, Texture *texture)
{
	renderer->context->PSSetShaderResources(0, 1, &texture->tex);
	renderer->context->PSSetSamplers(0, 1, &renderer->normalSampler);
}

void Graphics::BindTexture(Renderer *renderer, ShadowRenderTarget *renderTarget)
{
	renderer->context->PSSetShaderResources(0, 1, &renderTarget->renderTexture);
	renderer->context->PSSetSamplers(0, 1, &renderer->comparisonSampler);
}

void Graphics::BindTexture(Renderer *renderer, ScreenRenderTarget *renderTarget)
{
	renderer->context->PSSetShaderResources(0, 1, &renderTarget->renderTexture);
	renderer->context->PSSetSamplers(0, 1, &renderer->normalSampler);
}


void Graphics::UnbindTexture(Renderer *renderer)
{
	ID3D11ShaderResourceView *currentTexture[] = { NULL };
	renderer->context->PSSetShaderResources(0, 1, currentTexture);
}

void Graphics::Release(Renderer *renderer)
{
	RELEASE_DX_RESOURCE(renderer->normalSampler);
	RELEASE_DX_RESOURCE(renderer->comparisonSampler);
	RELEASE_DX_RESOURCE(renderer->blendStates[NORMAL].blendState);
	RELEASE_DX_RESOURCE(renderer->blendStates[TEXTURE].blendState);
	Graphics::Release(&renderer->quadTex);
	Graphics::Release(&renderer->perModelBuffer);
	Graphics::Release(&renderer->materialBuffer);
	Graphics::Release(&renderer->textureBuffer);
}

void Graphics::Release(SceneRenderTarget *renderTarget)
{
	RELEASE_DX_RESOURCE(renderTarget->depthStencilView);
	RELEASE_DX_RESOURCE(renderTarget->renderTexture);
	RELEASE_DX_RESOURCE(renderTarget->renderTarget);
}

void Graphics::Release(GraphicsPipeline *pipeline)
{
	RELEASE_DX_RESOURCE(pipeline->inputLayout);
	RELEASE_DX_RESOURCE(pipeline->vertexShader);
	RELEASE_DX_RESOURCE(pipeline->pixelShader);
}

void Graphics::Release(ShadowRenderTarget *renderTarget)
{
	RELEASE_DX_RESOURCE(renderTarget->depthStencilView);
	RELEASE_DX_RESOURCE(renderTarget->renderTexture);
}

void Graphics::Release(ScreenRenderTarget *renderTarget)
{
	RELEASE_DX_RESOURCE(renderTarget->renderTarget);
	RELEASE_DX_RESOURCE(renderTarget->renderTexture);
}

void Graphics::Release(ModelData *model)
{
	RELEASE_DX_RESOURCE(model->indexBuffer);
	RELEASE_DX_RESOURCE(model->vertexBuffer);
}

void Graphics::Release(ConstantBuffer *buffer)
{
	RELEASE_DX_RESOURCE(buffer->d3dBuffer);
}

void Graphics::Release(Texture *texture)
{
	RELEASE_DX_RESOURCE(texture->tex);
	RELEASE_DX_RESOURCE(texture->texResource);
}

void Graphics::Release(ModelBatch *batch)
{
	Graphics::Release(&batch->buffer);
}

void Graphics::Release(InstanceBuffer *buffer)
{
	RELEASE_DX_RESOURCE(buffer->d3dBuffer);
}

void Graphics::Release(FontEngine *fontEngine)
{
	Graphics::Release(&fontEngine->fontTexture);
}

void Graphics::Release(Model3D *model)
{
	RELEASE_DX_RESOURCE(model->vertexBuffer);
	RELEASE_DX_RESOURCE(model->indexBuffer);
}

