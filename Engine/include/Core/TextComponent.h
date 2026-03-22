#pragma once
#include "Core/Font.h"
#include <vector>

#include "Vertex.h"

struct Text
{
	AtlasFont* font;
	std::string message;

	void CreateVertexBuffer()
	{
		std::vector<Vertex2D> vertices;
		for (auto& letter : message)
		{
			// 4 vertices et 2 triangles 
		}
		char c = '\n';

	}
};
