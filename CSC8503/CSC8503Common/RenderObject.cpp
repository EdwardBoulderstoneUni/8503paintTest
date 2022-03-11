#include "RenderObject.h"

#include "Transform.h"
#include "../../Common/MeshGeometry.h"
#include "../../OpenGLRendering/OGLRenderer.h"

namespace NCL
{
	namespace Rendering
	{
		class OGLTexture;
	}
}

using namespace NCL::CSC8503;
using namespace NCL;

RenderObject::RenderObject(Transform* parentTransform, MeshGeometry* mesh, TextureBase* tex, ShaderBase* shader) {
	this->transform	= parentTransform;
	this->mesh		= mesh;
	this->texture	= tex;
	this->shader	= shader;
	this->colour	= Vector4(1.0f, 1.0f, 1.0f, 1.0f);
}

void RenderObject::render(OGLRenderer* renderer)
{
	const auto shader_changed = renderer->BindShader(shader);
	if (!shader_changed) 
		renderer->bind_shader_defaults();
	bind_shader_values(renderer);
}

void RenderObject::bind_shader_values(const OGLRenderer* renderer) const
{
	renderer->BindTextureToShader(texture, "mainTex", 0);
	renderer->bind_matrix_4_fv("modelMatrix", transform->GetMatrix().as_float_array());
	renderer->bind_4_fv("objectColour", colour.as_float_array());
	renderer->bind_1_i("hasVertexColours", mesh->GetColourData().empty());
	renderer->bind_1_i("hasTexture", static_cast<bool>(texture));
}
