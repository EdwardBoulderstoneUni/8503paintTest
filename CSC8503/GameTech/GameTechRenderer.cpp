#include "GameTechRenderer.h"
#include "../GameTech/Mesh.h"
#include "../CSC8503Common/GameObject.h"
#include "../../Common/Camera.h"
#include "../../Common/Vector2.h"
#include "../../Common/Vector3.h"
#include "../../Common/TextureLoader.h"



using namespace NCL;
using namespace Rendering;
using namespace CSC8503;

#define SHADOWSIZE 4096
#define PAINTSIZE 1024

unsigned int loadTexture(const char* path);

Matrix4 biasMatrix = Matrix4::Translation(Vector3(0.5, 0.5, 0.5)) * Matrix4::Scale(Vector3(0.5, 0.5, 0.5));

GameTechRenderer::GameTechRenderer(GameWorld& world) : OGLRenderer(*Window::GetWindow()), gameWorld(world)	{glEnable(GL_DEPTH_TEST);

	shadowShader = new OGLShader("GameTechShadowVert.glsl", "GameTechShadowFrag.glsl");

	glGenTextures(1, &shadowTex);
	glBindTexture(GL_TEXTURE_2D, shadowTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
			     SHADOWSIZE, SHADOWSIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenFramebuffers(1, &shadowFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,GL_TEXTURE_2D, shadowTex, 0);
	glDrawBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClearColor(1, 1, 1, 1);

	//Set up the light properties
	lightColour = Vector4(0.8f, 0.8f, 0.5f, 1.0f);
	lightRadius = 1000.0f;
	lightPosition = Vector3(-200.0f, 60.0f, -200.0f);

	//Skybox!
	skyboxShader = new OGLShader("skyboxVertex.glsl", "skyboxFragment.glsl");
	skyboxMesh = new OGLMesh();
	skyboxMesh->SetVertexPositions({Vector3(-1, 1,-1), Vector3(-1,-1,-1) , Vector3(1,-1,-1) , Vector3(1,1,-1) });
	skyboxMesh->SetVertexIndices({ 0,1,2,2,3,0 });
	skyboxMesh->UploadToGPU();

	LoadSkybox();

	//PaintMethod
	//Create an FBO to paint
	paintShader = new OGLShader("PaintTextureVert.glsl","PaintTextureFrag.glsl");
	
	glGenTextures(1, &paintTex);
	glBindTexture(GL_TEXTURE_2D, paintTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 
		10, 10, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenFramebuffers(1,&paintFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, paintFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, paintTex, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClearColor(1, 1, 1, 1);

	//ExtendPaint Shader
	/*extendPaintShader = new OGLShader("ExtendPaintVert.glsl", "ExtendPaintFrag.glsl");
	if (!extendPaintShader->LoadSuccess())
	{
		return;
	}*/

	glGenTextures(1, &extendpaintTex);
	glBindTexture(GL_TEXTURE_2D, extendpaintTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
		PAINTSIZE, PAINTSIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glBindTexture(GL_TEXTURE_2D, 0);

	//USE renderbufferobject
	glGenRenderbuffers(1, &paintRenderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, paintRenderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, 50, 50);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glGenFramebuffers(1, &extendpaintFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, extendpaintFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, extendpaintTex, 0);
	glDrawBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClearColor(1, 1, 1, 1);

	glGenTextures(1, &supportpaintTex);
	glBindTexture(GL_TEXTURE_2D, supportpaintTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
		PAINTSIZE, PAINTSIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenFramebuffers(1, &supportpaintFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, supportpaintFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, supportpaintTex, 0);
	glDrawBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClearColor(1, 1, 1, 1);

	glGenTextures(1, &uvIslandsTex);
	glBindTexture(GL_TEXTURE_2D, uvIslandsTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
		PAINTSIZE, PAINTSIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenFramebuffers(1, &uvIslandsFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, uvIslandsFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, uvIslandsTex, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClearColor(0, 1, 0, 1);
}

GameTechRenderer::~GameTechRenderer()	{
	glDeleteTextures(1, &shadowTex);
	glDeleteFramebuffers(1, &shadowFBO);
	glDeleteTextures(1, &paintTex);
	glDeleteFramebuffers(1, &paintFBO);
	glDeleteTextures(1, &supportpaintTex);
	glDeleteFramebuffers(1, &supportpaintFBO);
	glDeleteTextures(1, &uvIslandsTex);
	glDeleteFramebuffers(1, &uvIslandsFBO);

}

void GameTechRenderer::LoadSkybox() {
	string filenames[6] = {
		"/Cubemap/skyrender0004.png",
		"/Cubemap/skyrender0001.png",
		"/Cubemap/skyrender0003.png",
		"/Cubemap/skyrender0006.png",
		"/Cubemap/skyrender0002.png",
		"/Cubemap/skyrender0005.png"
	};

	int width[6]	= { 0 };
	int height[6]	= { 0 };
	int channels[6] = { 0 };
	int flags[6]	= { 0 };

	vector<char*> texData(6, nullptr);

	for (int i = 0; i < 6; ++i) {
		TextureLoader::LoadTexture(filenames[i], texData[i], width[i], height[i], channels[i], flags[i]);
		if (i > 0 && (width[i] != width[0] || height[0] != height[0])) {
			std::cout << __FUNCTION__ << " cubemap input textures don't match in size?\n";
			return;
		}
	}
	glGenTextures(1, &skyboxTex);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);

	GLenum type = channels[0] == 4 ? GL_RGBA : GL_RGB;

	for (int i = 0; i < 6; ++i) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width[i], height[i], 0, type, GL_UNSIGNED_BYTE, texData[i]);
	}

	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void GameTechRenderer::RenderFrame() {
	glEnable(GL_CULL_FACE);
	glClearColor(0, 0 , 1, 1);
	BuildObjectList();
	SortObjectList();
	RenderShadowMap();
	RenderSkybox();
	RenderCamera();
	//RenderPaint();
	glDisable(GL_CULL_FACE); //Todo - text indices are going the wrong way...
}

void GameTechRenderer::BuildObjectList() {
	activeObjects.clear();

	gameWorld.OperateOnContents(
		[&](GameObject* o) {
			if (o->IsActive()) {
				const RenderObject* g = o->GetRenderObject();
				if (g) {
					activeObjects.emplace_back(g);
				}
			}
		}
	);
}

void GameTechRenderer::SortObjectList() {
	//Who cares!
}

void GameTechRenderer::RenderShadowMap() {
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glViewport(0, 0, SHADOWSIZE, SHADOWSIZE);

	glCullFace(GL_FRONT);

	BindShader(shadowShader);
	int mvpLocation = glGetUniformLocation(shadowShader->GetProgramID(), "mvpMatrix");

	Matrix4 shadowViewMatrix = Matrix4::BuildViewMatrix(lightPosition, Vector3(0, 0, 0), Vector3(0,1,0));
	Matrix4 shadowProjMatrix = Matrix4::Perspective(100.0f, 500.0f, 1, 45.0f);

	Matrix4 mvMatrix = shadowProjMatrix * shadowViewMatrix;

	shadowMatrix = biasMatrix * mvMatrix; //we'll use this one later on

	for (const auto&i : activeObjects) {
		Matrix4 modelMatrix = (*i).GetTransform()->GetMatrix();
		Matrix4 mvpMatrix	= mvMatrix * modelMatrix;
		glUniformMatrix4fv(mvpLocation, 1, false, (float*)&mvpMatrix);
		BindMesh((*i).GetMesh());
		int layerCount = (*i).GetMesh()->GetSubMeshCount();
		for (int i = 0; i < layerCount; ++i) {
			DrawBoundMesh(i);
		}
	}

	glViewport(0, 0, currentWidth, currentHeight);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glCullFace(GL_BACK);
}

void GameTechRenderer::bind_shadow_matrix(const RenderObject* i)
{
	const int shadow_location = glGetUniformLocation(boundShader->GetProgramID(), "shadowMatrix");
	Matrix4 full_shadow_mat = shadowMatrix * (*i).GetTransform()->GetMatrix();
	glUniformMatrix4fv(shadow_location, 1, false, full_shadow_mat.as_float_array());
}

//TODO: TOBE COMPLETED
//Vector3 position, float radius, float hardness, float strength,Vector4 paintcolor
void GameTechRenderer::RenderPaint()
{
	glClear(GL_COLOR_BUFFER_BIT);
	Vector3 position = Vector3(0,0,0);
	Vector4 paintColor = Vector4(1, 0, 0, 1);
	float radius = 1.0f;
	float hardness = 0.5f;
	float strength = 1.0f;
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, paintFBO);
	glClear(GL_COLOR_BUFFER_BIT);

	BindShader(paintShader);

	float number[] = { position.x,position.y,position.z};
	float color[] = { paintColor[0],paintColor[1],paintColor[2],paintColor[3]};

	glUniform3fv(glGetUniformLocation(paintShader->GetProgramID(),"paintPosition"),1,number);
	glUniform4fv(glGetUniformLocation(paintShader->GetProgramID(),"paintColor"),1,color);
	glUniform1f(glGetUniformLocation(paintShader->GetProgramID(),"radius"), radius);
	glUniform1f(glGetUniformLocation(paintShader->GetProgramID(),"hardness"), hardness);
	glUniform1f(glGetUniformLocation(paintShader->GetProgramID(),"strength"), strength);

}

void GameTechRenderer::bind_shader_defaults()
{
	const float screen_aspect = static_cast<float>(currentWidth) / static_cast<float>(currentHeight);
	Matrix4 view_matrix = gameWorld.GetMainCamera()->BuildViewMatrix();
	Matrix4 proj_matrix = gameWorld.GetMainCamera()->BuildProjectionMatrix(screen_aspect);

	const int projection_location = glGetUniformLocation(boundShader->GetProgramID(), "projMatrix");
	const int view_location = glGetUniformLocation(boundShader->GetProgramID(), "viewMatrix");

	const int light_pos_location = glGetUniformLocation(boundShader->GetProgramID(), "lightPos");
	const int light_colour_location = glGetUniformLocation(boundShader->GetProgramID(), "lightColour");
	const int light_radius_location = glGetUniformLocation(boundShader->GetProgramID(), "lightRadius");

	const int camera_location = glGetUniformLocation(boundShader->GetProgramID(), "cameraPos");
	glUniform3fv(camera_location, 1, gameWorld.GetMainCamera()->GetPosition().as_float_array());

	glUniformMatrix4fv(projection_location, 1, false, proj_matrix.as_float_array());
	glUniformMatrix4fv(view_location, 1, false, view_matrix.as_float_array());

	glUniform3fv(light_pos_location, 1, lightPosition.as_float_array());
	glUniform4fv(light_colour_location, 1, lightColour.as_float_array());
	glUniform1f(light_radius_location, lightRadius);

	const int shadow_tex_location = glGetUniformLocation(boundShader->GetProgramID(), "shadowTex");
	glUniform1i(shadow_tex_location, 1);



}

void GameTechRenderer::RenderSkybox() {
	glDisable(GL_CULL_FACE);
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);

	float screenAspect = (float)currentWidth / (float)currentHeight;
	Matrix4 viewMatrix = gameWorld.GetMainCamera()->BuildViewMatrix();
	Matrix4 projMatrix = gameWorld.GetMainCamera()->BuildProjectionMatrix(screenAspect);

	BindShader(skyboxShader);

	int projLocation = glGetUniformLocation(skyboxShader->GetProgramID(), "projMatrix");
	int viewLocation = glGetUniformLocation(skyboxShader->GetProgramID(), "viewMatrix");
	int texLocation  = glGetUniformLocation(skyboxShader->GetProgramID(), "cubeTex");

	glUniformMatrix4fv(projLocation, 1, false, (float*)&projMatrix);
	glUniformMatrix4fv(viewLocation, 1, false, (float*)&viewMatrix);

	glUniform1i(texLocation, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);

	BindMesh(skyboxMesh);
	DrawBoundMesh();

	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
}

void GameTechRenderer::RenderCamera() {
	float screenAspect = (float)currentWidth / (float)currentHeight;
	Matrix4 viewMatrix = gameWorld.GetMainCamera()->BuildViewMatrix();
	Matrix4 projMatrix = gameWorld.GetMainCamera()->BuildProjectionMatrix(screenAspect);

	OGLShader* activeShader = nullptr;
	int modelLocation	= 0;
	int colourLocation  = 0;
	int hasVColLocation = 0;
	int hasTexLocation  = 0;


	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, shadowTex);

	for (const auto&i : activeObjects) {
		OGLShader* shader = (OGLShader*)(*i).GetShader();
		BindShader(shader);

		BindTextureToShader((OGLTexture*)(*i).GetDefaultTexture(), "mainTex", 0);

		if (activeShader != shader) {
			bind_shader_defaults();
			modelLocation	= glGetUniformLocation(shader->GetProgramID(), "modelMatrix");
			colourLocation  = glGetUniformLocation(shader->GetProgramID(), "objectColour");
			hasVColLocation = glGetUniformLocation(shader->GetProgramID(), "hasVertexColours");
			hasTexLocation  = glGetUniformLocation(shader->GetProgramID(), "hasTexture");
			activeShader = shader;
		}

		Matrix4 modelMatrix = (*i).GetTransform()->GetMatrix();
		glUniformMatrix4fv(modelLocation, 1, false, (float*)&modelMatrix);			

		bind_shadow_matrix(i);

		glUniform4fv(colourLocation, 1, (float*)&i->GetColour());

		glUniform1i(hasVColLocation, !(*i).GetMesh()->GetColourData().empty());

		glUniform1i(hasTexLocation, (OGLTexture*)(*i).GetDefaultTexture() ? 1:0);

		BindMesh((*i).GetMesh());
		int layerCount = (*i).GetMesh()->GetSubMeshCount();
		for (int i = 0; i < layerCount; ++i) {
			DrawBoundMesh(i);
		}
	}
}

Matrix4 GameTechRenderer::SetupDebugLineMatrix()	const  {
	float screenAspect = (float)currentWidth / (float)currentHeight;
	Matrix4 viewMatrix = gameWorld.GetMainCamera()->BuildViewMatrix();
	Matrix4 projMatrix = gameWorld.GetMainCamera()->BuildProjectionMatrix(screenAspect);

	return projMatrix * viewMatrix;
}

Matrix4 GameTechRenderer::SetupDebugStringMatrix()	const {
	return Matrix4::Orthographic(-1, 1.0f, 100, 0, 0, 100);
}
