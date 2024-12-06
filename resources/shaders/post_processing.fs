#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D scene;
uniform vec2  offsets[9];
uniform int     edge_kernel[9];
uniform float  blur_kernel[9];

uniform bool chaos;
uniform bool confuse;
uniform bool shake;

void main()
{
    //�ڿ�ʼ����֮ǰ��ȷ����ɫ��ʼ��Ϊ��ɫ��͸������
    color = vec4(0.0f);

    vec3 sample[9];
    // ֻ�������û��һ���Ч��ʱ���Ŵ������н��ж�β�����ÿ������ʹ��ƫ��������ȡ�������ص���ɫ��
    if(chaos || shake)
        for(int i = 0; i < 9; i++)
            sample[i] = vec3(texture(scene, TexCoords.st + offsets[i]));

    // ����Ч��:ʹ�ñ�Ե����˶Բ�������ɫ���мӺʹ��������ԵЧ����
    if(chaos)
    {
        for(int i = 0; i < 9; i++)
            color += vec4(sample[i] * edge_kernel[i], 0.0f);
        color.a = 1.0f;
    }
    //ֱ�ӻ�ȡ��ǰ�����ϵ���ɫ�����ҽ�����з�ת����
    else if(confuse)
    {
        color = vec4(1.0 - texture(scene, TexCoords).rgb, 1.0);
    }
    //ʹ��ģ������˶Բ�������ɫ���мӺͣ��������յ�ģ��Ч����
    else if(shake)
    {
        for(int i = 0; i < 9; i++)
            color += vec4(sample[i] * blur_kernel[i], 0.0f);
        color.a = 1.0f;
    }
    else
    {
        color =  texture(scene, TexCoords);
    }
}

/*
���Ƭ����ɫ���������� GLSL (OpenGL Shading Language) ��д�ģ������� OpenGL �汾 3.3 �����ϡ������������������Ч����Ҫ���ڴ���һЩ�Ӿ�Ч��������ͼ��ģ������ת��ɫ�Լ����������ҡ��͡��𶯡���Ч���������Ƕ�ÿ�����ֵ���ϸ���ͣ�

### �������

1. **��������**:
   ```glsl
   in vec2 TexCoords;  // �������������
   out vec4 color;     // �������ɫֵ
   ```
   - `TexCoords`: ������������꣬ͨ���ɶ�����ɫ�����ɣ�ָ�����������и�Ƭ�εĲ���λ�á�
   - `color`: ��Ƭ�ε������ɫֵ��

2. **ͳһ���� (Uniform Variables)**:
   ```glsl
   uniform sampler2D scene;       // ���ڲ���������
   uniform vec2  offsets[9];      // �洢�������������ƫ����
   uniform int     edge_kernel[9]; // ��Ե�������
   uniform float  blur_kernel[9];  // ģ�������
   ```
   - `scene`: ����һ��2D����ͨ��������������ͨ�� OpenGL ID ����ġ�����������Ⱦ�Ĳ��ʻ�ͼ�񣬱��糡������ɫ���塣
   - `offsets`: �����ڴ���Ч��ʱָ����������������ƫ�������飨Ĭ��Ϊ 3x3 ����ľŸ�ƫ�ƣ���
   - `edge_kernel` �� `blur_kernel`: ���ڱ�Ե����ģ������ľ�������顣

3. **Ч������ (Effect Switches)**:
   ```glsl
   uniform bool chaos;   // ����Ч������
   uniform bool confuse;  // ��ɫ��������
   uniform bool shake;    // ��Ч������
   ```
   - �����������������ڿ��Ʋ�ͬ��Ч����ֻ���ڶ�Ӧ��Ч������ʱ����صĴ������Ż�ִ�С�

4. **������ (main function)**:
   - **��ʼ�� `color`**:
     ```glsl
     color = vec4(0.0f);
     ```
     - �ڿ�ʼ����֮ǰ��ȷ����ɫ��ʼ��Ϊ��ɫ��͸������

   - **��������**:
     ```glsl
     vec3 sample[9];
     if(chaos || shake)
         for(int i = 0; i < 9; i++)
             sample[i] = vec3(texture(scene, TexCoords.st + offsets[i]));
     ```
     - ֻ�������û��һ���Ч��ʱ���Ŵ������н��ж�β�����ÿ������ʹ��ƫ��������ȡ�������ص���ɫ��

   - **����Ч������**:
     ```glsl
     if(chaos)
     {           
         for(int i = 0; i < 9; i++)
             color += vec4(sample[i] * edge_kernel[i], 0.0f);
         color.a = 1.0f;
     }
     ```
     - ��������˻���Ч����ʹ�ñ�Ե����˶Բ�������ɫ���мӺʹ��������ԵЧ����

   - **��ɫ��תЧ��**:
     ```glsl
     else if(confuse)
     {
         color = vec4(1.0 - texture(scene, TexCoords).rgb, 1.0);
     }
     ```
     - ���÷�ת��������Ч��ʱ��ֱ�ӻ�ȡ��ǰ�����ϵ���ɫ�����ҽ�����з�ת����

   - **��Ч������**:
     ```glsl
     else if(shake)
     {
         for(int i = 0; i < 9; i++)
             color += vec4(sample[i] * blur_kernel[i], 0.0f);
         color.a = 1.0f;
     }
     ```
     - �����������Ч����ʹ��ģ������˶Բ�������ɫ���мӺͣ��������յ�ģ��Ч����

   - **Ĭ����Ⱦ**:
     ```glsl
     else
     {
         color =  texture(scene, TexCoords);
     }
     ```
     - ���û�������κ���Ч���������������в�����������ɫ��

### `scene` ����Դ��
- **�����**: `scene` ����ͨ������ OpenGL �����򲿷ִ��������õģ���������ʹ�� `glGenTextures()`��`glBindTexture()` �� `glTexImage2D()` �Ⱥ���ʱ����ġ�
- **���ݵ���ɫ��**: Ϊ����Ƭ����ɫ����ʹ�� `scene`����Ҫͨ�� `glUniform1i()` ����������Ԫ���������ݸ���ɫ����������˵��ͨ�����ڻ���֮ǰ���������Ĳ�����
   
  ```cpp
  glActiveTexture(GL_TEXTURE0);  // ��������Ԫ 0
  glBindTexture(GL_TEXTURE_2D, sceneTextureID);  // ������
  glUniform1i(glGetUniformLocation(shaderProgram, "scene"), 0); // ������Ԫ�������ݵ���ɫ��
  ```

### �ܽ�
��δ���չʾ��һ��ͨ���������ƶ����Ӿ�Ч����Ƭ����ɫ���������ڴ���������������Ч�����ء�ʹ�þ���˶��������ؽ��д����Ӷ�ʵ���˱�Ե��⡢��ɫ��ת��ģ����Ч�������ּ��������ͼ��Ӧ���кܳ�����������Ϸ��Ⱦ��ͼ���� ������и�������������Ҫ��һ����˵����������ң�

*/

/*
`TexCoords.st` ��δ���ͨ��������ͼ�α�̻���ɫ������У�����ʾһ����������Ľṹ��������˵��`TexCoords` ��һ��������������ı������� `st` ͨ����������������

1. **s**����Ӧ�������ˮƽ���꣨X���򣩣�ͨ����ʾ�������е��С�
2. **t**����Ӧ������Ĵ�ֱ���꣨Y���򣩣�ͨ����ʾ�������е��С�

### ��ϸ����

- **��������**����������������ӳ������������������ϵ��ÿ�����أ�����Ԫ��texel��������ֵͨ���� [0, 1] ��Χ�ڡ����磬���� `(0, 0)` ��ʾ��������½ǣ��� `(1, 1)` ��ʾ���Ͻǡ�

- **����ϵͳ**���ںܶ�ͼ��ϵͳ�У�`s` �� `t` ���������������ͼ��ģ���ͬ�ڶ�άƽ���ϵĶ�ά����ϵ��

### ʾ���÷�
�� OpenGL �� DirectX ��ͼ�� API �У������ܻῴ���������µĴ��룺
```cpp
// ����������һ���ṹ�����洢��������
struct TexCoord {
    float s;
    float t;
};

// ����һ����������ʵ��
TexCoord TexCoords;
TexCoords.s = 0.5f; // ˮƽ����
TexCoords.t = 1.0f; // �ϲ�
```

### С��
`TexCoords.st` �Ƕ��������ʱʹ����������á�����������������ȷ��Ⱦ�ʹ�������������Ҫ��������и�����������Ļ����ӣ����Է����ҿ����ṩ����ϸ��˵����

*/