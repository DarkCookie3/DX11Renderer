#pragma once
#include "../Utils/d3d.h"
#include "../DXObjects/VertexShader.h"
#include "../DXObjects/PixelShader.h"
#include "../Window/WindowDX.h"
#include "../CustomTypes/Material.h"

class PostProcess
{
public:

	static PostProcess& GetInstance()
	{
		static PostProcess instance;
		return instance;
	}
	PostProcess(const PostProcess&) = delete;
	PostProcess& operator=(const PostProcess&) = delete;

	void SetPostProcessShaders(std::shared_ptr<VertexShader> vertexShader, std::shared_ptr<PixelShader> pixelShader);
	void Resolve(WindowDX& window);
private:
	PostProcess() {}

	std::shared_ptr<VertexShader> m_vertexShader;
	std::shared_ptr<PixelShader> m_pixelShader;
	PipelineState m_pipelineState;
};