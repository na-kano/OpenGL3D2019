/**
* @file MainGameScene.cpp
*/
#include "MainGameScene.h"
#include "StatusScene.h"
#include "GameOverScene.h"
#include "../GLFWEW.h"
#include <glm/gtc/matrix_transform.hpp>
#include <random>
#include <sstream>
#include <iomanip>

/*
  ���b�Z�[�W���[�h�̊J�n begin ���x��
  msg �e�L�X�g
  select �I����
  set �ϐ��ԍ� ���lor�ϐ��ԍ�
  add �ϐ��ԍ� ���lor�ϐ��ԍ�
  sub �ϐ��ԍ� ���lor�ϐ��ԍ�
  if �ϐ��ԍ�(��0�łȂ����) �W�����v�惉�x��
  ���b�Z�[�W���[�h�̏I�� end
*/

/**
* �R���X�g���N�^.
*/
MainGameScene::MainGameScene() : Scene("MainGameScene")
{
}

/**
* �V�[��������������.
*
* @retval true  ����������.
* @retval false ���������s. �Q�[���i�s�s�ɂ��A�v���O�������I�����邱��.
*/
bool MainGameScene::Initialize()
{
  fontRenderer.Init(1000);
  fontRenderer.LoadFromFile("Res/font.fnt");

  meshBuffer.Init(sizeof(Mesh::Vertex) * 1000000, sizeof(GLushort) * 3000000, sizeof(Mesh::UniformDataMeshMatrix) * 100);
  meshBuffer.CreateCircle("Circle", 8);
  heightMap.Load("Res/HeightMap.tga", 100.0f, 0.5f);
  heightMap.CreateMesh(meshBuffer, "Terrain");
  texTerrain = Texture::Image2D::Create("Res/ColorMap.tga");
  texTree = Texture::Image2D::Create("Res/TestTree.tga");
  meshBuffer.LoadMesh("Res/bikuni.gltf");
  meshBuffer.LoadMesh("Res/TestTree.gltf");
  meshPlayer = meshBuffer.GetMesh("Bikuni");
  meshPlayer->SetAnimation(0);
  meshTerrain = meshBuffer.GetMesh("Terrain");
  meshCircle = meshBuffer.GetMesh("Circle");

  static const size_t treeCount = 200;
  meshTrees.reserve(treeCount);
  std::mt19937 rand;
  rand.seed(0);
  for (size_t i = 0; i < treeCount; ++i) {
    Mesh::MeshPtr p = meshBuffer.GetMesh("TestTree");
    p->translation.x = static_cast<float>(std::uniform_int_distribution<>(0, heightMap.Size().x)(rand));
    p->translation.z = static_cast<float>(std::uniform_int_distribution<>(0, heightMap.Size().y)(rand));
    p->translation.y = heightMap.Height(p->translation);
    p->rotation = glm::angleAxis(std::uniform_real_distribution<float>(0, glm::half_pi<float>())(rand), glm::vec3(0, 1, 0));
    p->scale = glm::vec3(std::normal_distribution<float>(0.7f, 0.2f)(rand));
    p->scale = glm::clamp(p->scale, 0.4f, 1.2f);
    p->SetAnimation(0);
    p->frame = std::uniform_real_distribution<float>(0, 2)(rand);
    meshTrees.push_back(p);
  }

  Shader::Cache& shaderCache = Shader::Cache::Instance();
  progMesh = shaderCache.Create("Res/Mesh.vert", "Res/Mesh.frag");
  progSkeletalMesh = shaderCache.Create("Res/SkeletalMesh.vert", "Res/SkeletalMesh.frag");
  progSkeletalMesh->BindUniformBlock("MeshMatrixUniformData", 0);

  glm::vec3 startPos(100, 0, 150);
  startPos.y = heightMap.Height(startPos);
  meshPlayer->translation = startPos;

  return true;
}

/**
* �V�[�����X�V����.
*
* @param sceneStack �V�[������I�u�W�F�N�g.
* @param deltaTime  �O��̍X�V����̌o�ߎ���(�b).
*
* TODO: �n�ʂ̏���ړ�.
* TODO: �W�����v����.
* TODO: �U������ƍU������.
* TODO: �G�̏o��.
* TODO: �G�̎v�l.
* TODO: �؂�A����.
* TODO: ���l�Ɖ�b.
* TODO: �{�X.
* TODO: �@��.
*/
void MainGameScene::Update(SceneStack& sceneStack, float deltaTime)
{
  GLFWEW::Window& window = GLFWEW::Window::Instance();

  fontRenderer.BeginUpdate();
  if (IsActive()) {
    std::wstringstream wss;
    wss << L"FPS:" << std::fixed << std::setprecision(2) << window.Fps();
    fontRenderer.AddString(glm::vec2(-600, 300), wss.str().c_str());
    fontRenderer.AddString(glm::vec2(-600, 260), L"���C���Q�[�����");
  }
  fontRenderer.EndUpdate();

  if (IsActive()) {
    const glm::aligned_vec3 dir(0, 0, -1);
    const glm::aligned_vec3 left = glm::normalize(glm::cross(glm::aligned_vec3(0, 1, 0), dir));
    const float dt = static_cast<float>(window.DeltaTime());
    glm::aligned_vec3 move(0);
    const float speed = 5.0f;
    if (window.KeyPressed(GLFW_KEY_W)) {
      move += dir * dt * speed;
    } else if (window.KeyPressed(GLFW_KEY_S)) {
      move -= dir * dt * speed;
    }
    if (window.KeyPressed(GLFW_KEY_A)) {
      move += left * dt * speed;
    } else if (window.KeyPressed(GLFW_KEY_D)) {
      move -= left * dt * speed;
    }
    if (glm::dot(move, move)) {
      meshPlayer->translation += move;
      meshPlayer->translation.y = heightMap.Height(meshPlayer->translation);
      move = glm::normalize(move);
      meshPlayer->rotation = glm::aligned_quat(glm::vec3(0, std::atan2(-move.z, move.x) + glm::radians(90.0f), 0));
      if (meshPlayer->GetAnimation() != 0) {
        meshPlayer->SetAnimation(0);
      }
    } else {
      if (meshPlayer->GetAnimation() != 1) {
        meshPlayer->SetAnimation(1);
      }
    }

    const glm::vec2 currentMousePos = GLFWEW::Window::Instance().MousePosition();
    const glm::vec2 mouseMove = currentMousePos - prevMousePos;
    prevMousePos = currentMousePos;
    glm::mat4 matRX(1);
    if (mouseMove.x) {
      matRX = glm::rotate(glm::aligned_mat4(1), -mouseMove.x / 100.0f, glm::aligned_vec3(0, 1, 0));
    }
    glm::mat4 matRY(1);
    if (mouseMove.y) {
      matRY = glm::rotate(glm::aligned_mat4(1), mouseMove.y / 100.0f, left);
    }
    //dir = matRX * matRY * glm::vec4(dir, 1);
    //dir = normalize(dir);

    meshBuffer.ResetUniformData();
    meshPlayer->Update(deltaTime);
    meshTerrain->Update(deltaTime);
    meshCircle->Update(deltaTime);
    for (auto& e : meshTrees) {
      e->Update(deltaTime * 0.25f);
    }
    meshBuffer.UploadUniformData();

    // �V�[���؂�ւ�.
    const GamePad gamepad = window.GetGamePad();
    if (gamepad.buttonDown & GamePad::X) {
      sceneStack.Push(std::make_shared<StatusScene>());
    } else if (gamepad.buttonDown & GamePad::START) {
      sceneStack.Replace(std::make_shared<GameOverScene>());
    }
  }
}

/**
* �V�[����`�悷��.
*/
void MainGameScene::Render()
{
  const GLFWEW::Window& window = GLFWEW::Window::Instance();

  const bool pushSpaceBar = window.KeyPressed(GLFW_KEY_SPACE);
  const glm::aligned_vec3 cameraPos = meshPlayer->translation + glm::aligned_vec3(0, pushSpaceBar ? 100 : 15, 7.5f);
  const glm::mat4 matView = glm::lookAt(cameraPos, meshPlayer->translation + glm::aligned_vec3(0, 1.25f, 0), glm::aligned_vec3(0, 1, 0));
  const float aspectRatio = static_cast<float>(window.Width()) / static_cast<float>(window.Height());
  const glm::mat4 matProj = glm::perspective(glm::radians(30.0f), aspectRatio, 1.0f, 1000.0f);
  const glm::mat4 matModel = glm::scale(glm::mat4(1), glm::vec3(1));

  meshBuffer.Bind();

  {
    progMesh->Use();

    progMesh->SetViewProjectionMatrix(matProj * matView * matModel);
    texTerrain->Bind(0);
    const GLint locMeshIndex = progMesh->GetUniformLocation("meshIndex");
    if (locMeshIndex > 0) {
      progMesh->SetUniformInt(locMeshIndex, 0);
    }
    meshTerrain->Draw();
    meshCircle->Draw();

    texTree->Bind(0);
    for (const auto& e : meshTrees) {
      e->Draw();
    }

    progMesh->Unuse();
  }

  {
    progSkeletalMesh->Use();

    progSkeletalMesh->SetViewProjectionMatrix(matProj * matView);
    const GLint locMeshIndex = progSkeletalMesh->GetUniformLocation("meshIndex");
    if (locMeshIndex > 0) {
      progSkeletalMesh->SetUniformInt(locMeshIndex, 0);
    }
    meshPlayer->Draw();

    progSkeletalMesh->Unuse();
  }

  texTerrain->Unbind(0);
  meshBuffer.Unbind();

  const glm::vec2 screenSize(window.Width(), window.Height());
  fontRenderer.Draw(screenSize);
}

/**
* �V�[����j������.
*/
void MainGameScene::Finalize()
{
  GLFWEW::Window::Instance().EnableMouseCursor();
}

/**
* �V�[����������Ԃɂ���.
*/
void MainGameScene::Play()
{
  GLFWEW::Window::Instance().DisableMouseCursor();
  prevMousePos = GLFWEW::Window::Instance().MousePosition();
  Scene::Play();
}
