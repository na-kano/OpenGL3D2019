/**
* @file StatusScene.h
*/
#ifndef STATUSSCENE_H_INCLUDED
#define STATUSSCENE_H_INCLUDED
#include "../Scene.h"
#include "../Font.h"

/**
* �X�e�[�^�X���.
*/
class StatusScene : public Scene
{
public:
  StatusScene();
  virtual bool Initialize() override;
  virtual void ProcessInput() override;
  virtual void Update(float) override;
  virtual void Render() override;
  virtual void Finalize() override;
  virtual void Play() override;
public:
  Font::Renderer fontRenderer;
};

#endif // STATUSSCENE_H_INCLUDED
