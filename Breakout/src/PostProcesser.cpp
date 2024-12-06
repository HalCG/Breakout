#include "PostProcesser.h"

#include <iostream>


/*
    ��Ⱦ����Ϊԭʼ����Ⱦ֡��������ز����󣬴�����ݣ�Ȼ���ٷ���֡�����У�Ȼ�������TEXTURE0 �У���render�н�����Ⱦ
*/
PostProcessor::PostProcessor(Shader shader, unsigned int width, unsigned int height)
    : PostProcessingShader(shader), Texture(), Width(width), Height(height), Confuse(false), Chaos(false), Shake(false)
{
    //==============================================================================================================
    // 1. ��ʼ��֡������� 
    // ����֡���壺����һ�����ز���֡������� (MSFBO)��һ������֡������� (FBO)���Լ�һ����Ⱦ������� (RBO)��
    glGenFramebuffers(1, &this->MSFBO);
    glGenFramebuffers(1, &this->FBO);
    glGenRenderbuffers(1, &this->RBO);

    // ��ʼ�����ز�����ɫ����  
    // �󶨺����� RBO������Ⱦ����������洢�ռ�,���󶨵����ز���֡���塣����ʹ�� 4 �����ز���������ݣ���
    glBindFramebuffer(GL_FRAMEBUFFER, this->MSFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, this->RBO);

    //Ϊһ�����ز�����Ⱦ�������Renderbuffer Object, RBO�������ڴ棬��������洢��ʽ��
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_RGB, width, height); 
    // �� RBO ���ӵ� MSFBO������Ⱦ������󸽼ӵ����ز���֡�������ɫ������
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, this->RBO); 

    // ���֡����״̬�������ز���֡�����Ƿ��ʼ���ɹ���
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::POSTPROCESSOR: Failed to initialize MSFBO" << std::endl;

    //==============================================================================================================
    // 2. ��ʼ������ FBO ������
    // ����ʼ�� FBO / �����Խ����ز�����ɫ��������������������ɫ�����������ں���Ч����
    glBindFramebuffer(GL_FRAMEBUFFER, this->FBO);
    this->Texture.Generate(width, height, NULL);

    //��һ������texture�����ӵ�֡��������framebuffer����ָ�����ӵ㣨attachment point���������ִ� OpenGL ��Ⱦ�����еĹؼ����裬�����㽫��Ⱦ���ֱ������������У�������Ĭ�ϵĴ���ϵͳ�����磬��ʾ����
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->Texture.ID, 0); 
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::POSTPROCESSOR: Failed to initialize FBO" << std::endl;

    //���֡���壺����ǰ֡�����󣬻ָ���Ĭ��֡���塣
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //==============================================================================================================
    // 3. ��ʼ����Ⱦ���ݺ���ɫ�� uniform
    //��ʼ����Ⱦ���ݣ����� initRenderData() ���������ö����������VAO���Ͷ��㻺�����VBO����
    this->initRenderData();
    this->PostProcessingShader.SetInteger("scene", 0, true);

    //����ƫ����������һ��ƫ�������飬���ں��ڴ���Ч������ģ������Ȼ���䴫�ݸ���ɫ��
    float offset = 1.0f / 300.0f;
    float offsets[9][2] = {
        { -offset,  offset  },  // top-left
        {  0.0f,    offset  },  // top-center
        {  offset,  offset  },  // top-right
        { -offset,  0.0f    },  // center-left
        {  0.0f,    0.0f    },  // center-center
        {  offset,  0.0f    },  // center - right
        { -offset, -offset  },  // bottom-left
        {  0.0f,   -offset  },  // bottom-center
        {  offset, -offset  }   // bottom-right    
    };
    glUniform2fv(glGetUniformLocation(this->PostProcessingShader.ID, "offsets"), 9, (float*)offsets);

    //���ñ�Ե����ںˣ�����һ����Ե����ںˣ������䴫�ݸ���ɫ����
    int edge_kernel[9] = {
        -1, -1, -1,
        -1,  8, -1,
        -1, -1, -1
    };
    glUniform1iv(glGetUniformLocation(this->PostProcessingShader.ID, "edge_kernel"), 9, edge_kernel);

    //����ģ���ںˣ�����һ��ģ���ںˣ������䴫�ݸ���ɫ����
    float blur_kernel[9] = {
        1.0f / 16.0f, 2.0f / 16.0f, 1.0f / 16.0f,
        2.0f / 16.0f, 4.0f / 16.0f, 2.0f / 16.0f,
        1.0f / 16.0f, 2.0f / 16.0f, 1.0f / 16.0f
    };
    glUniform1fv(glGetUniformLocation(this->PostProcessingShader.ID, "blur_kernel"), 9, blur_kernel);
}

void PostProcessor::BeginRender()
{
    //�󶨶��ز���֡���壺����Ⱦ֮ǰ�� MSFBO��
    glBindFramebuffer(GL_FRAMEBUFFER, this->MSFBO);

    //�����ɫ���壺���������ɫΪ��ɫ���������ɫ����������׼����Ⱦ�µĳ�����
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void PostProcessor::EndRender()
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, this->MSFBO);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, this->FBO);

    //�� MSFBO �е���ɫ������������ FBO �У��Ա�����ĺ��ڴ���
    glBlitFramebuffer(0, 0, this->Width, this->Height, 0, 0, this->Width, this->Height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

    // ʹ�󶨵Ķ�д��������Ĭ��״̬
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PostProcessor::Render(float time)
{
    // ʹ�ú��ڴ�����ɫ��������ʱ���Ч��ѡ��� uniform��
    this->PostProcessingShader.Use();
    this->PostProcessingShader.SetFloat("time", time);
    this->PostProcessingShader.SetInteger("confuse", this->Confuse);
    this->PostProcessingShader.SetInteger("chaos", this->Chaos);
    this->PostProcessingShader.SetInteger("shake", this->Shake);

    // ������� VAO��Ȼ�����һ������������Ļ���ı��Σ�����ʾ���ڴ���Ч����
    glActiveTexture(GL_TEXTURE0);// ��������Ԫ 0
    this->Texture.Bind();
    glBindVertexArray(this->VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void PostProcessor::initRenderData()
{
    // configure VAO/VBO
    unsigned int VBO;
    float vertices[] = {
        // pos        // tex
        -1.0f, -1.0f, 0.0f, 0.0f,
         1.0f,  1.0f, 1.0f, 1.0f,
        -1.0f,  1.0f, 0.0f, 1.0f,

        -1.0f, -1.0f, 0.0f, 0.0f,
         1.0f, -1.0f, 1.0f, 0.0f,
         1.0f,  1.0f, 1.0f, 1.0f
    };
    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(this->VAO);
    glEnableVertexAttribArray(0);// ����λ���������飬���shader�ļ���
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


/*
`glBlitFramebuffer` ���������� OpenGL �н�һ��֡�����������ݸ��Ƶ���һ��֡������������ʵ��ͼ������ߺ���Ч��ʱ�ǳ����ã����罫���ز����Ļ�������������һ�������С��������������������������ľ��庬�壺

### �﷨

```cpp
glBlitFramebuffer(srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, filter);
```

### ����

1. **Դ���β�����**
   - `srcX0` �� `srcY0`��Դ���ε����½����� (0, 0)��
   - `srcX1` �� `srcY1`��Դ���ε����Ͻ����� (`this->Width`, `this->Height`)��

   �ⲿ�ֶ����˴��ĸ������֡�������������ݡ�����������У�����ʾ��ԭ�� (0, 0) �� (Width, Height)��������֡����Ĵ�С��

2. **Ŀ����β�����**
   - `dstX0` �� `dstY0`��Ŀ����ε����½����� (0, 0)��
   - `dstX1` �� `dstY1`��Ŀ����ε����Ͻ����� (`this->Width`, `this->Height`)��

   �ⲿ�ֶ���������Ӧ�ñ����Ƶ��ĸ������������ͬ����ʾ�������ݵ���һ��֡�������������

3. **`GL_COLOR_BUFFER_BIT`**��ָ��Ҫ���ƵĻ��������͡���������ʾ����ֻ�븴����ɫ���岿�֡�`GL_COLOR_BUFFER_BIT` ��һ����������ʾ��Դ֡�������ɫ��Ϣ���Ƶ�Ŀ��֡���塣

4. **`GL_NEAREST`**��ָ������ģʽ������������У�ʹ�����������ˣ�����򵥵ķ�ʽ��������ζ���ڽ��и���ʱ���������κβ�ֵ���㣬ֱ��ȡ���������ֵ��

### ���庬��

��ˣ����´����У�

```cpp
glBlitFramebuffer(0, 0, this->Width, this->Height, 0, 0, this->Width, this->Height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
```

�ĺ����ǣ�

- �ӵ�ǰ�󶨵Ķ�ȡ֡��������ͨ���Ƕ��ز���֡���壩�е� `(0, 0)` �� `(this->Width, this->Height)` ��������ɫ���ݡ�
- ����Щ����д�뵽��ǰ�󶨵Ļ���֡������������ͨ��֡���壩�е���Ӧ���򣬴�СҲΪ `(0, 0)` �� `(this->Width, this->Height)`��
- �����������ɫ��ֵ��ʽ���и��ơ�

### Ӧ�ó���

���ֲ���ͨ���ڽ��к��ڴ���Ч��ʱʹ�ã�������ʹ�ö��ز�������ݺ󣬽�����������ɫ������������blit����һ����һ�������У��Ա��������ɫ��Ч������ģ������Ե���ȣ���ͨ�����ַ�ʽ��������ڼ��ٿ���ݵ�ͬʱ�Ż���ȾЧ����

*/

/*
`glRenderbufferStorageMultisample` ���������� OpenGL ��Ϊ���ز�����Ⱦ�������renderbuffer������洢����ר�����ڴ������ж��ز�����anti-aliasing����������Ⱦ���壬�������Ⱦ���������پ������

### ����ԭ��

```cpp
void glRenderbufferStorageMultisample(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height);
```

### �������

1. **`target`**��
   - ���ͣ�`GLenum`
   - ˵����ָ����Ⱦ��������͡����������ʹ�� `GL_RENDERBUFFER` ��ʾ�������ڲ���һ����Ⱦ�������

2. **`samples`**��
   - ���ͣ�`GLsizei`
   - ˵����ָ��Ҫʹ�õĶ��ز�����������һ�����͵�ֵ�� `4`����ʾÿ������ʹ�� 4 �β�����������������ṩ���õĿ����Ч������Ҳ�������ڴ�ʹ�úͼ���ɱ���

3. **`internalformat`**��
   - ���ͣ�`GLenum`
   - ˵����ָ����Ⱦ������ڲ���ʽ������������У�ʹ�� `GL_RGB`����ζ�Ŵ洢����ɫ��ʽΪ RGB���������������������ɫ�Ĵ洢��ʽ��ռ�õ��ڴ档

4. **`width`** �� **`height`**��
   - ���ͣ�`GLsizei`
   - ˵����ָ����Ⱦ����Ŀ�Ⱥ͸߶ȣ�������Ϊ��λ���ⶨ������Ⱦ����ĳߴ硣

### ���庬��

`glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_RGB, width, height);` ���д���������ǣ�

- **����һ���µĶ��ز�����Ⱦ�������**����Ϊ������ڴ档
- **���ø���Ⱦ����Ĳ���**��������������Ϊ `4`����ʾ���� 4x ���ز��������ڲ���ʽΪ `GL_RGB`����Ⱥ͸߶ȷֱ�Ϊ `width` �� `height`��

�������ͨ���ڴ������ز���֡�������ʱʹ�ã���� `glFramebufferRenderbuffer` ���丽�ӵ�֡�����С�

### Ӧ�ó���

- **�����**��ͨ��ʹ�ö��ز�������Ⱦ����ͼ���ڱ�Ե�����پ�ݻ������󣬴Ӷ����ͼ��������
- **���Ի���**����Ⱦ���������������Ⱦ��ʹ�ã�����Ⱦ�ɹ����������Ȼ����к��ڴ���Ч����

ʹ�� `glRenderbufferStorageMultisample` �ǽ��и�����ͼ����Ⱦ����Ҫ���裬��������Ҫ����ݵ�Ӧ�ó���������Ϸ��ͼ��Ӧ�õȣ��зǳ�Ƶ����


`glRenderbufferStorageMultisample` �������ڴ������ز�����multisampling����Ⱦ�����������ּ����������ͼ����Ⱦ���������ر����ڿ���ݷ��档���ز�������Ⱦ�����п��Լ��پ�ݱ�Ե��ʹ��ͼ�εı�Ե����ƽ����

### ����ԭ��:
1. **���ز����ĸ���**��
   - ����Ⱦʱ��ÿ�����ص���ɫֵ������ͨ��һ����һ�Ĳ�������ȷ��������ͨ����������㣨���Ӳ����㣩�����в�������Щ������ֱ���һ�������ڣ�����ÿ�������㽫�õ�һ����������ɫֵ��

2. **�������洢**��
   - `glRenderbufferStorageMultisample` ��������Ⱦ��������Ϊÿ�����ط����������������ʾ������ 4 ���������� 4x MSAA Ч��������Щ�����ֱ�洢����Ⱦ�������У��������պϳ���ɫֵʱ��������ǵ�ƽ��ֵ���߲��������ϳ��㷨��

3. **���͵Ĳ���**:
   - �����ʾ����ʹ���� `GL_RGB`������ζ��ÿ�������洢 RGB ��ɫ��Ϣ������洢�ĸ�ʽȡ���� OpenGL �ľ���ʵ�ֺ����ã�������������Ϊÿ�������洢������ɫͨ������Ϣ��

### �ܽ�:
�����֮������ `glRenderbufferStorageMultisample` �ᴴ��һ�����ж��ز�����������Ⱦ��������ÿ�����ؾ��ж�������㣬������ִ�п����ʱ���ܸ��õ�ƽ����Ե������ͼ���������������������˽���ز����ľ����㷨����ʵ��ϸ�ڣ����Խ�һ�����ۣ�


*/

/*
���������н��������� OpenGL �������ã�������ǵ����ú����һ������

### 1. `glRenderbufferStorageMultisample`

```cpp
glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_RGB, width, height);
```

- **����Ŀ��**��Ϊһ�����ز�����Ⱦ�������Renderbuffer Object, RBO�������ڴ棬��������洢��ʽ��

- **��������**��
  - **`GL_RENDERBUFFER`**������Ŀ�����ͣ���ʾ�������ڴ���һ����Ⱦ�������
  - **`4`**����ʾ����Ⱦ����ʹ�� 4x ���ز�����ÿ�����ز��� 4 �Σ����������ڼ��پ������������Ⱦ������
  - **`GL_RGB`**��ָ���洢���ڲ���ʽΪ RGB����ÿ�������ɺ졢�̡���������ɫ��ɣ���˵�������ݵ���ɫ��ʽ��
  - **`width` �� `height`**��ָ����Ⱦ����Ŀ�Ⱥ͸߶ȣ�������Ϊ��λ������������Ⱦ����ĳߴ硣

### 2. `glFramebufferRenderbuffer`

```cpp
glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, this->RBO);
```

- **����Ŀ��**����֮ǰ�����Ķ��ز�����Ⱦ������󸽼ӵ���ǰ��֡�������Framebuffer Object, FBO���ϣ��Ա�����Ⱦ������ʹ�á�

- **��������**��
  - **`GL_FRAMEBUFFER`**������Ŀ�����ͣ���ʾ�������ڲ���һ��֡�������
  - **`GL_COLOR_ATTACHMENT0`**�����Ǹ��ӵ�Ŀ�ָ꣬����ɫ���� 0��OpenGL �����û����Ӷ����������ͨ����һ����ɫ����ʹ�� 0 ��Ϊ������
  - **`GL_RENDERBUFFER`**���ٴ�ָ�����Ǹ��ӵ���һ����Ⱦ�������������
  - **`this->RBO`**������֮ǰ��������Ⱦ�������� ID����ʾ����ϣ��������ض�����Ⱦ���帽�ӵ�֡�����С�

### ���幦�ܺ�����

�����д�������幦�ܿ����ܽ����£�

1. **�������ز�������Ⱦ����**����һ�д���ʹ�� `glRenderbufferStorageMultisample` Ϊһ�����ز�������Ⱦ�����������ڴ棬��ָ���˶��ز����͸�ʽ������ζ���ں�������Ⱦ�����У�ÿ�����ض������ж�����������̶ȵؼ��پ������

2. **����Ⱦ���帽�ӵ�֡����**���ڶ��д��뽫�����Ⱦ������󸽼ӵ���ǰ��֡�������ɫ�����У��Ա��������Ⱦ�������Խ�����������������С�

### Ӧ�ó���

��������ͨ��������Ҫ��������Ⱦ�ĳ�����������Ϸ��ͼ��Ӧ�á������ö��ز�����ͼ����Ⱦ��������������ƣ��Ӷ����Ӿ��ϸ���ƽ����û�����Եľ�ݱ�Ե��

### ע������

- ��ʹ�ö��ز���ʱ��Ҫȷ��֧�ָù��ܵ� OpenGL �����ģ�ͨ�����ṩ���°汾�� OpenGL��֧�ֶ��ز�������
- �������Ⱦ�󣬿��ܻ���Ҫʹ�� `glBlitFramebuffer` �Ⱥ��������ز�������Ⱦ������������������֡�����У��Ա���к��ڴ���������Ⱦ��

*/

/*
`glFramebufferTexture2D` �� OpenGL ��һ�����������ڽ�һ������texture�����ӵ�֡��������framebuffer����ָ�����ӵ㣨attachment point��������������У���������Ϊ��ɫ���ӵ㣨color attachment���������ִ� OpenGL ��Ⱦ�����еĹؼ����裬�����㽫��Ⱦ���ֱ������������У�������Ĭ�ϵĴ���ϵͳ�����磬��ʾ�����������������������ϸ˵������;��

### ���ã�
1. **��������֡������**��
   - `glFramebufferTexture2D` ��ָ��������������������� `this->Texture.ID`�����ӵ���ǰ��֡�������ϣ�ʹ���Ϊ�û�������һ����ɫ���ӵ㡣`GL_COLOR_ATTACHMENT0` ��ʾ���ǵ�һ����ɫ���ӵ㡣

2. **��Ⱦ������**��
   - ͨ���������ӵ�֡�����������л��Ƶ����֡����������Ⱦ�������������������������У���������Ļ����ʹ����������ں�������Ⱦ���������紦���Ч��post-processing��������ӳ�䣨texture mapping������Ϊ�����������ͼ��texture����

3. **������ȾЧ��**��
   - ʹ��֡��������һ����ҪĿ����ʵ�ָ��ӵ���ȾЧ�������磬���Խ�������Ⱦ��һ�������ϣ�Ȼ�󽫸�����Ӧ������Ļ�ϵ��ı��Σ�quad���Խ��к�Ч�������⣬���Ӷ����ɫ���ӵ����ʵ�ֶ�����Ⱦ������Ա��ڶ����������Ⱦ��ͬ����Ϣ������ɫ����ȡ����ߵȣ���

### ������⣺
- `GL_FRAMEBUFFER`����ǰ�󶨵�֡����������
- `GL_COLOR_ATTACHMENT0`��Ҫ���ӵ���ɫ���ӵ㣬OpenGL ֧�ֶ����ɫ���ӵ㣨���� `GL_COLOR_ATTACHMENT1`��`GL_COLOR_ATTACHMENT2` �ȣ���
- `GL_TEXTURE_2D`��Ҫ���ӵ������Ŀ�����ͣ���������һ�ֶ�ά����
- `this->Texture.ID`��Ҫ���ӵ�����ľ���� ID����ͨ�������������ɺ�����ʱ��õġ�
- `0`�����������ʾ����� mipmap �㼶��ͨ�����ڴ����������ʹ�� 0 ��ʾʹ�û�������base level����

### ���̣�
1. **��ʼ������**����ʹ�� `glFramebufferTexture2D` ֮ǰ����Ҫȷ��������ȷ���������ã�������������ĸ�ʽ�ͳߴ�ȡ�
2. **��֡������**�����ô˺���֮ǰ����ȷ��֡�����������Ѿ����󶨣�ʹ�� `glBindFramebuffer`����
3. **���״̬**������������󣬿���ͨ�� `glCheckFramebufferStatus` ���֡��������״̬��ȷ������Ч��

### ʵ��Ӧ��ʾ����
���������ڿ���һ����Ϸ����ϣ��ʵ����Ļģ��Ч������������Ƚ���ǰ֡��Ⱦ��һ������Ȼ��ʹ�ø��������ģ��������󽫴�����������ʾ����Ļ�ϡ�

��֮��`glFramebufferTexture2D` ���ִ� OpenGL ��Ⱦ�����е�һ�����������飬ʹ�ø�Ч����Ⱦ�ͺ����Ϊ���ܡ�������и���������������Ҫ����̽��ĳ�����棬������ң�

*/

/*
����δ����У�`offset` ��ͨ���� 1.0f ���� 300.0f ������ģ������ÿ��ƫ�����ķ��ȡ�ѡ�� 1.0f ���� 300.0f ��ԭ��ͨ����ͼ��Ӧ���еľ������󡢳�����С���Ӿ�Ч���йء�

**���ܵ�ԭ�������**

1. **�ֱ��������**����һЩͼ�γ����У���������2D��Ⱦ�У���������������Ļ�ֱ��ʽ��м��㡣�� 1.0f ���� 300.0f��������ƫ������Ӧ�ض��ķֱ��ʣ��Ա��ڴ���Ļ��С��Ļ�ϱ���һ�µ��Ӿ�Ч����

2. **���ƾ���**��300��һ����Խϴ����ֵ���ᵼ��offset��ֵ��Խ�С��ԼΪ0.0033������ʹ��ƫ��������Ⱦ���˶��򶯻�ʱ���Ӿ�ϸ���������Ӿ��ϵ�ͻأ�С�

3. **������С**�����Ӧ�ó��������ض����߼��򳡾��ı�����������ĳЩ��Ϸ��Ӧ���У���Ҫȷ��ƫ�����뱳��������ʵ�ʳߴ�ƥ�䣬���ܾͻ�ʹ�������ĳ�����

4. **�Ӿ�Ч��**�������ֵѡ����ܳ����Ӿ�Ч���Ŀ��ǡ�������ͨ��ʵ��ó����Ч��ʱʹ�õľ���ֵ��

��Ȼ��ѡ������ض�ֵ������ԭ����������Ӧ�õ������ĺͿ����ߵ���ƾ�����ء�����и������������Ϣ����빦��˵���������ṩ����ϸ�ķ�����

*/