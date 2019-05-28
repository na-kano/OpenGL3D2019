/**
* @file Scene.cpp
*/
#include "Scene.h"
#include <iostream>

/**
* �R���X�g���N�^.
*
* @param name �V�[����.
*/
Scene::Scene(const char* name) : name(name)
{
  std::cout << "Scene Constructor: " << name << "\n";
}

/**
* �f�X�g���N�^.
*/
Scene::~Scene()
{
  Finalize();
  std::cout << "Scene Destructor: " << name << "\n";
}

/**
* �V�[����������Ԃɂ���.
*/
void Scene::Play()
{
  isActive = true;
  std::cout << "Scene Play: " << name << "\n";
}

/**
* �V�[�����~��Ԃɂ���.
*/
void Scene::Stop()
{
  isActive = false;
  std::cout << "Scene Stop: " << name << "\n";
}

/**
* �V�[����\������.
*/
void Scene::Show()
{
  isVisible = true;
  std::cout << "Scene Show: " << name << "\n";
}

/**
* �V�[�����\���ɂ���.
*/
void Scene::Hide()
{
  isVisible = false;
  std::cout << "Scene Hide: " << name << "\n";
}

/**
* �V�[�������擾����.
*
* @return �V�[����.
*/
const std::string& Scene::Name() const
{
  return name;
}

/**
* �V�[���̊�����Ԃ𒲂ׂ�.
*
* @retval true  �������Ă���.
* @retval false ��~���Ă���.
*/
bool Scene::IsActive() const
{
  return isActive;
}

/**
* �V�[���̕\����Ԃ𒲂ׂ�.
*
* @retval true  �\�����.
* @retval false ��\�����.
*/
bool Scene::IsVisible() const
{
  return isVisible;
}

/**
* �V�[���X�^�b�N�̃V���O���g���C���X�^���X���擾����.
*
* @return �V�[���X�^�b�N�̃V���O���g���C���X�^���X.
*/
SceneStack& SceneStack::Instance()
{
  static SceneStack instance;
  return instance;
}

/**
* �R���X�g���N�^.
*/
SceneStack::SceneStack()
{
  stack.reserve(16);
}

/**
* �V�[�����v�b�V������.
*
* @param p �V�����V�[��.
*/
void SceneStack::Push(ScenePtr p)
{
  if (!stack.empty()) {
    Current().Stop();
  }
  stack.push_back(p);
  std::cout << "SceneStack Push: " << p->Name() << "\n";
  Current().Initialize();
  Current().Play();
}

/**
* �V�[�����|�b�v����.
*/
void SceneStack::Pop()
{
  if (stack.empty()) {
    std::cout << "SceneStack Pop: [�x��]�V�[���X�^�b�N����ł�.\n";
    return;
  }
  Current().Stop();
  Current().Finalize();
  const std::string sceneName = Current().Name();
  stack.pop_back();
  std::cout << "SceneStack Pop: " << sceneName << "\n";
  if (!stack.empty()) {
    Current().Play();
  }
}

/**
* �V�[����u��������.
*
* @param p �V�����V�[��.
*/
void SceneStack::Replace(ScenePtr p)
{
  std::string sceneName = "(Empty)";
  if (stack.empty()) {
    std::cout << "SceneStack Replace: [�x��]�V�[���X�^�b�N����ł�.\n";
  } else {
    sceneName = Current().Name();
    Current().Stop();
    Current().Finalize();
    stack.pop_back();
  }
  stack.push_back(p);
  std::cout << "SceneStack Replace: " << sceneName << " -> " << p->Name() << "\n";
  Current().Initialize();
  Current().Play();
}

/**
* ���݂̃V�[�����擾����.
*
* @return ���݂̃V�[��.
*/
Scene& SceneStack::Current()
{
  return *stack.back();
}

/**
* ���݂̃V�[�����擾����.
*
* @return ���݂̃V�[��.
*/
const Scene& SceneStack::Current() const
{
  return *stack.back();
}

/**
* �V�[���̐����擾����.
*
* @return �X�^�b�N�ɐς܂�Ă���V�[���̐�.
*/
size_t SceneStack::Size() const
{
  return stack.size();
}

/**
* �X�^�b�N���󂩂ǂ����𒲂ׂ�.
*
* @retval true  �X�^�b�N�͋�.
* @retval false �X�^�b�N��1�ȏ�̃V�[�����ς܂�Ă���.
*/
bool SceneStack::Empty() const
{
  return stack.empty();
}

/**
* �V�[�����X�V����.
*
* @param deltaTime �O��̍X�V����̌o�ߎ���(�b).
*/
void SceneStack::Update(float deltaTime)
{
  if (!Empty()) {
    Current().ProcessInput();
  }
  for (auto& e : stack) {
    e->Update(deltaTime);
  }
}

/**
* �V�[����`�悷��.
*/
void SceneStack::Render()
{
  for (auto& e : stack) {
    if (e->IsVisible()) {
      e->Render();
    }
  }
}