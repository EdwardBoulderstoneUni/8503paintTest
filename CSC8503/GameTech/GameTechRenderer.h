#pragma once
#include "../../Plugins/OpenGLRendering/OGLRenderer.h"
#include "../../Plugins/OpenGLRendering/OGLShader.h"
#include "../../Plugins/OpenGLRendering/OGLTexture.h"
#include "../../Plugins/OpenGLRendering/OGLMesh.h"

#include "../CSC8503Common/GameWorld.h"

namespace NCL {
	class Maths::Vector3;
	class Maths::Vector4;
	namespace CSC8503 {
		class RenderObject;

		class GameTechRenderer : public OGLRenderer	{
		public:
			GameTechRenderer(GameWorld& world);
			~GameTechRenderer();

			//Vector3 position,float radius = 1.0f,float hardness = 0.5f,float strength = 0.5f,Vector4 paintcolor = Vector4(1, 0, 0, 1)
			void RenderPaint();
			void bind_shader_defaults() override;
		protected:
			void RenderFrame()	override;

			Matrix4 SetupDebugLineMatrix()	const override;
			Matrix4 SetupDebugStringMatrix()const override;

			OGLShader*		defaultShader;

			GameWorld&	gameWorld;

			void BuildObjectList();
			void SortObjectList();
			void RenderShadowMap();
			void bind_shadow_matrix(const RenderObject* i);
			void RenderCamera(); 
			void RenderSkybox();
			
			void LoadSkybox();

			vector<const RenderObject*> activeObjects;

			OGLShader*  skyboxShader;
			OGLMesh*	skyboxMesh;
			GLuint		skyboxTex;

			//shadow mapping things
			OGLShader*	shadowShader;
			GLuint		shadowTex;
			GLuint		shadowFBO;
			Matrix4     shadowMatrix;

			OGLShader* paintShader;
			GLuint paintTex;
			GLuint paintFBO;

			GLuint paintRenderbuffer;

			OGLShader* extendPaintShader;
			GLuint extendpaintTex;
			GLuint extendpaintFBO;

			GLuint supportpaintTex;
			GLuint supportpaintFBO;

			GLuint uvIslandsTex;
			GLuint uvIslandsFBO;

			Vector4		lightColour;
			float		lightRadius;
			Vector3		lightPosition;

			//TEST
			OGLMesh* cubeMesh = nullptr;
			OGLTexture* dogTex = nullptr;
			
		};
	}
}

