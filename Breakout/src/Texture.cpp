#include <iostream>

#include "texture.h"


Texture2D::Texture2D()
    : Width(0), Height(0), Internal_Format(GL_RGB), Image_Format(GL_RGB), Wrap_S(GL_REPEAT), Wrap_T(GL_REPEAT), Filter_Min(GL_LINEAR), Filter_Max(GL_LINEAR)
{
    glGenTextures(1, &this->ID);
}

void Texture2D::Generate(GLuint width, GLuint height, unsigned char* data)
{
    this->Width = width;
    this->Height = height;
    // Create Texture
    glBindTexture(GL_TEXTURE_2D, this->ID);
    glTexImage2D(GL_TEXTURE_2D, 0, this->Internal_Format, width, height, 0, this->Image_Format, GL_UNSIGNED_BYTE, data);
    // Set Texture wrap and filter modes
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, this->Wrap_S);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, this->Wrap_T);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, this->Filter_Min);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, this->Filter_Max);
    // Unbind texture
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2D::Bind() const
{
    glBindTexture(GL_TEXTURE_2D, this->ID);
}


/*
�� OpenGL �У���Щ����������������Ĳ�����������ÿ����������ϸ˵����

### 1. `GL_TEXTURE_WRAP_S`

- **����**: �����������ָ�������� S ����ͨ����Ӧ����������� X �ᣩ�ϵİ�װ��ʽ��
- **����ֵ**:
  - `GL_REPEAT`: �ظ�����������Χ�Ĳ��ֽ����ظ�����
  - `GL_CLAMP_TO_EDGE`: ���������������� [0, 1] ��Χ�ڣ�������Χ�����꽫����������Ե����ɫ��
  - `GL_MIRRORED_REPEAT`: �ظ���������ÿ�������з�ת����

### 2. `GL_TEXTURE_WRAP_T`

- **����**: �����������ָ�������� T ����ͨ����Ӧ����������� Y �ᣩ�ϵİ�װ��ʽ�������÷��� `GL_TEXTURE_WRAP_S` ���ơ�
- **����ֵ**: ͬ `GL_TEXTURE_WRAP_S` ����ֵ��

### 3. `GL_TEXTURE_MIN_FILTER`

- **����**: �������ָ�����������ʵ���سߴ�С����Ļ���سߴ�ʱ�����ѡ������Ĺ��˷�����Ӱ��������Сʱ����ȾЧ����
- **����ֵ**:
  - `GL_NEAREST`: ʹ����������أ�����ˣ���
  - `GL_LINEAR`: ʹ�����Բ�ֵ�����Թ��ˣ���
  - `GL_NEAREST_MIPMAP_NEAREST`: ʹ������� mipmap �������������ء�
  - `GL_LINEAR_MIPMAP_NEAREST`: ʹ������� mipmap ��������Թ��ˡ�
  - `GL_NEAREST_MIPMAP_LINEAR`: ʹ�����Բ�ֵѡ�� mipmap ����ʹ��������صĲ�ֵ��
  - `GL_LINEAR_MIPMAP_LINEAR`: ʹ�����Բ�ֵѡ�� mipmap ����ʹ�����Թ��ˡ�

### 4. `GL_TEXTURE_MAG_FILTER`

- **����**: �������ָ�����������ʵ���سߴ������Ļ���سߴ�ʱ�����ѡ������Ĺ��˷�ʽ��Ӱ������Ŵ�ʱ����ȾЧ����
- **����ֵ**:
  - `GL_NEAREST`: ʹ����������أ�����ˣ���
  - `GL_LINEAR`: ʹ�����Բ�ֵ�����Թ��ˣ���

��Щ����ͨ��������Ϊ�������Ĳ������Կ��������ڲ�ͬ����µ���ʾ��ʹ�÷�ʽ��ͨ����Щ���ã������߿��Ի�ø����������Ӿ�Ч������������ʧ��;������

1. **�����������**:
   ```cpp
   glGenTextures(1, &this->ID);
   ```
   - `glGenTextures` ������������һ�������������`1` ��ʾ����һ������`&this->ID` ��һ������ ID ��ָ�룬���������ɵ��������ı�ʶ����

2. **��������Ŀ�Ⱥ͸߶�**:
   ```cpp
   this->Width = width;
   this->Height = height;
   ```
   - ������������Ⱥ͸߶ȱ��浽���Ա���� `Width` �� `Height` �У��Ա��ں����Ĳ�����ʹ�á�

3. **������**:
   ```cpp
   glBindTexture(GL_TEXTURE_2D, this->ID);
   ```
   - `glBindTexture` ���������ɵ��������󶨵���ǰ������Ԫ��ʹ����������������������������`GL_TEXTURE_2D` ָ������������͡�

4. **��������ͼ��**:
   ```cpp
   glTexImage2D(GL_TEXTURE_2D, 0, this->Internal_Format, width, height, 0, this->Image_Format, GL_UNSIGNED_BYTE, data);
   ```
   - `glTexImage2D` �������ڴ����Ͷ��� 2D ��������˵�����£�
     - `GL_TEXTURE_2D`: ָ����������͡�
     - `0`: mipmap ����0 ��ʾ��ɫ����
     - `this->Internal_Format`: �����ڲ���ʽ���������������ɫ������� GL_RGB��GL_RGBA �ȣ���
     - `width`, `height`: ����Ŀ�Ⱥ͸߶ȡ�
     - `0`: �߾���ȣ����� 2D ����ͨ��Ϊ 0����
     - `this->Image_Format`: ����ָ���ṩ��ͼ�����ݸ�ʽ���� GL_RGB��GL_RGBA����
     - `GL_UNSIGNED_BYTE`: ��������ָʾ����ͨ����Ӧ��ͼ�����ݵ����͡�
     - `data`: ָ��ͼ�����ݵ�ָ�롣

5. **�����������**:
   ```cpp
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, this->Wrap_S);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, this->Wrap_T);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, this->Filter_Min);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, this->Filter_Max);
   ```
   - ����� `glTexParameteri` ������������Ĳ������Կ�������İ�װ�͹�����Ϊ��
     - `GL_TEXTURE_WRAP_S` �� `GL_TEXTURE_WRAP_T`�������������� S �� T ���򣨼� X �� Y ���򣩵İ�װ��ʽ���ظ�����Ե�н��ȣ���
     - `GL_TEXTURE_MIN_FILTER` �� `GL_TEXTURE_MAG_FILTER`������������С�ͷŴ�ʱ�Ĺ��˷�����������ڹ��ˡ����Թ��˵ȣ���

6. **�������**:
   ```cpp
   glBindTexture(GL_TEXTURE_2D, 0);
   ```
   - ͨ�������� ID ��Ϊ 0�����Խ��ǰ�󶨵���������һ���õ��������Ա��ⲻС���޸ĵ�ǰ����

### �ܽ�

��δ���չʾ������� OpenGL �д���һ�� 2D �������������ԡ�ͨ���������� ID���ṩ�������ݡ����ð�װ�͹��˲������Լ����������Ϊ�����ʹ������׼�������� OpenGL �������еĳ������̡�

*/