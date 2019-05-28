/**
* @file StatusScene.cpp
*/
#include "StatusScene.h"
#include "../GLFWEW.h"

/**
* �R���X�g���N�^.
*/
StatusScene::StatusScene() : Scene("StatusScene")
{
}

/**
* �V�[��������������.
*
* @retval true  ����������.
* @retval false ���������s. �Q�[���i�s�s�ɂ��A�v���O�������I�����邱��.
*/
bool StatusScene::Initialize()
{
  fontRenderer.Init(1000);
  fontRenderer.LoadFromFile("Res/font.fnt");
  return true;
}

/**
* �V�[���̓��͂���������.
*/
void StatusScene::ProcessInput()
{
  GLFWEW::Window& window = GLFWEW::Window::Instance();
  const GamePad gamepad = window.GetGamePad();
  if (gamepad.buttonDown & GamePad::B) {
    SceneStack::Instance().Pop();
  }
}

/**
* �V�[�����X�V����.
*
* @param deltaTime  �O��̍X�V����̌o�ߎ���(�b).
*/
void StatusScene::Update(float deltaTime)
{
  fontRenderer.BeginUpdate();
  fontRenderer.AddString(glm::vec2(-600, 320), L"�X�e�[�^�X���");
  fontRenderer.AddString(glm::vec2(-400, 360 - 32 * 4), L"�̗́F�@�Q�O�^�Q�Q");
  fontRenderer.AddString(glm::vec2(-400, 360 - 32 * 6), L"�@�́F�@�P�T�^�P�T");
  fontRenderer.AddString(glm::vec2(-400, 360 - 32 * 8), L"����F�@���̎���");
  fontRenderer.AddString(glm::vec2(-400, 360 - 32 * 10), L"�h��F�@�Âт��֕t����");
  fontRenderer.EndUpdate();
}

/**
* �V�[����`�悷��.
*/
void StatusScene::Render()
{
  GLFWEW::Window& window = GLFWEW::Window::Instance();
  const glm::vec2 screenSize(window.Width(), window.Height());
  fontRenderer.Draw(screenSize);
}

/**
* �V�[����j������.
*/
void StatusScene::Finalize()
{
}

/**
* �V�[����������Ԃɂ���.
*/
void StatusScene::Play()
{
  GLFWEW::Window::Instance().EnableMouseCursor();
  Scene::Play();
}
