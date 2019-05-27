/**
* @file Font.cpp
*/
#define  _CRT_SECURE_NO_WARNINGS
#include "Font.h"
#include <memory>
#include <iostream>
#include <stdio.h>

/**
* �t�H���g�`��@�\���i�[���閼�O���.
*/
namespace Font {

/**
* �t�H���g�`��I�u�W�F�N�g������������.
*
* @param maxChar   �ő�`�敶����.
*
* @retval true  ����������.
* @retval false ���������s.
*/
bool Renderer::Init(size_t maxChar)
{
  return spriteRenderer.Init(maxChar, "Res/Font.vert", "Res/Font.frag");
}

/**
* �t�H���g�t�@�C����ǂݍ���.
*
* @param filename �t�H���g�t�@�C����.
*
* @retval true  �ǂݍ��ݐ���.
* @retval false �ǂݍ��ݎ��s.
*/
bool Renderer::LoadFromFile(const char* filename)
{
  const std::unique_ptr<FILE, decltype(&fclose)> fp(fopen(filename, "r"), &fclose);
  if (!fp) {
    return false;
  }

  int line = 1;
  int ret = fscanf(fp.get(), "info face=\"%*[^\"]\" size=%f bold=%*d italic=%*d charset=%*s"
    " unicode=%*d stretchH=%*d smooth=%*d aa=%*d padding=%f,%f,%f,%f spacing=%f,%f%*[^\n]",
    &baseFontSize, &padding[0], &padding[1], &padding[2], &padding[3],
    &spacing[0], &spacing[1]);
  if (ret < 7) {
    std::cerr << "ERROR: " << filename << "�̓ǂݍ��݂Ɏ��s(line=" << line << ")\n";
    return false;
  }
  ++line;

  glm::vec2 scale;
  ret = fscanf(fp.get(), " common lineHeight=%f base=%f scaleW=%f scaleH=%f pages=%*d packed=%*d%*[^\n]",
    &lineHeight, &base, &scale.x, &scale.y);
  if (ret < 4) {
    std::cerr << "ERROR: " << filename << "�̓ǂݍ��݂Ɏ��s(line=" << line << ")\n";
    return false;
  }
  ++line;

  // �ʒu���t�@�C���̃p�X���������o��.
  std::string basePath = filename;
  const size_t lastSlashIndex = basePath.find_last_of('/', std::string::npos);
  if (lastSlashIndex == std::string::npos) {
    basePath.clear();
  } else {
    basePath.resize(lastSlashIndex + 1);
  }

  std::vector<std::string> texNameList;
  for (;;) {
    char tex[256];
    ret = fscanf(fp.get(), " page id=%*d file=\"%255[^\"]\"", tex);
    if (ret < 1) {
      break;
    }
    tex[255] = '\0';
    texNameList.push_back(basePath + tex);
    ++line;
  }
  if (texNameList.empty()) {
    std::cerr << "ERROR: " << filename << "�̓ǂݍ��݂Ɏ��s(line=" << line << ")\n";
    return false;
  }

  int charCount;
  ret = fscanf(fp.get(), " chars count=%d", &charCount);
  if (ret < 1) {
    std::cerr << "ERROR: " << filename << "�̓ǂݍ��݂Ɏ��s(line=" << line << ")\n";
    return false;
  }
  ++line;

  fixedAdvance = 0;
  fontList.clear();
  fontList.resize(65536);
  for (int i = 0; i < charCount; ++i) {
    FontInfo font;
    ret = fscanf(fp.get(), " char id=%d x=%f y=%f width=%f height=%f xoffset=%f yoffset=%f xadvance=%f page=%d chnl=%*d",
      &font.id, &font.uv.x, &font.uv.y, &font.size.x, &font.size.y, &font.offset.x, &font.offset.y, &font.xadvance, &font.page);
    if (ret < 9) {
      std::cerr << "ERROR: " << filename << "�̓ǂݍ��݂Ɏ��s(line=" << line << ")\n";
      return false;
    }
    // �t�H���g�t�@�C���͍��オ���_�Ȃ̂ŁAOpenGL�̍��W�n(���������_)�ɕϊ�.
    font.uv.y = scale.y - font.uv.y - font.size.y;
    if (font.id >= 0 && font.id < static_cast<int>(fontList.size())) {
      fontList[font.id] = font;
      if (font.xadvance > fixedAdvance) {
        fixedAdvance = font.xadvance;
      }
    }
    ++line;
  }

  // �e�N�X�`����ǂݍ���.
  texList.clear();
  texList.reserve(texNameList.size());
  for (const auto& e : texNameList) {
    Texture::Image2DPtr tex = Texture::Image2D::Create(e.c_str());
    if (!tex) {
      return false;
    }
    texList.push_back(tex);
  }
  return true;
}

/**
* �����F��ݒ肷��.
*
* @param c�����F.
*/
void Renderer::Color(const glm::vec4& c)
{
  color = c;
}

/**
*�����F���擾����.
*
* @return�����F.
*/
const glm::vec4& Renderer::Color() const
{
  return color;
}

/**
* �������ǉ�����.
*
* @param position �\���J�n���W.
* @param str      �ǉ����镶����.
*
* @retval true  �ǉ�����.
* @retval false �ǉ����s.
*/
bool Renderer::AddString(const glm::vec2& position, const wchar_t* str)
{
  glm::vec2 pos = position;
  for (const wchar_t* itr = str; *itr; ++itr) {
    const FontInfo& font = fontList[*itr];
    if (font.id >= 0 && font.size.x && font.size.y) {
      // �X�v���C�g�̍��W�͉摜�̒��S���w�肷�邪�A�t�H���g�͍�����w�肷��̂ŁA���̍���ł��������߂̕␳�l���v�Z����.
      const float baseX = font.size.x * 0.5f + font.offset.x;
      const float baseY = base - font.size.y * 0.5f - font.offset.y;
      glm::vec3 offsetedPos = glm::vec3(pos + glm::vec2(baseX, baseY) * scale, 0);
      if (!propotional) {
        offsetedPos.x = pos.x;
      }
      Sprite sprite(texList[font.page]);
      sprite.Position(offsetedPos);
      sprite.Rectangle({ font.uv, font.size });
      sprite.Scale(scale);
      sprite.Color(color);
      if (!spriteRenderer.AddVertices(sprite)) {
        return false;
      }
    }
    pos.x += (propotional ? font.xadvance : fixedAdvance) * scale.x;
  }
  return true;
}

/**
* VBO���V�X�e���������Ƀ}�b�s���O����.
*/
void Renderer::BeginUpdate()
{
  spriteRenderer.BeginUpdate();
}

/**
* VBO�̃}�b�s���O����������.
*/
void Renderer::EndUpdate()
{
  spriteRenderer.EndUpdate();
}

/**
* �t�H���g��`�悷��.
*/
void Renderer::Draw(const glm::vec2& ss) const
{
  spriteRenderer.Draw(ss);
}

} // namespace Font
