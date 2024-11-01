#ifndef GAME_H
#define GAME_H

// ��������Ϸ�ĵ�ǰ״̬
enum GameState {
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN
};

class Game
{
public:
    Game(unsigned int width, unsigned int height);
    ~Game();

    // ��Ϸ״̬
    GameState  State;
    bool  Keys[1024];
    bool  KeysProcessed[1024];
    unsigned int     Width, Height;

    // ��ʼ����Ϸ״̬���������е���ɫ��/����/�ؿ���
    void Init();

    // ��Ϸѭ��
    void ProcessInput(float dt);
    void Update(float dt);
    void Render();
};

#endif

/*
ͨ������£�`glad` ��ͷ�ļ�Ӧ�÷���Դ�ļ���.c �� .cpp �ļ����У���������ͷ�ļ���.h �ļ����С�������һЩԭ������ʵ����

1. **�����ظ�����**�������� `glad` �İ�������ͷ�ļ��У������ͷ�ļ������Դ�ļ����������ܻᵼ���ظ��������⣬���ӱ���ʱ�䣬����������Ǳ�ڵķ��ų�ͻ��

2. **���ư�����������**����Դ�ļ��а��� `glad` ��������õؿ���ÿ��Դ�ļ���������ϵ��������ÿ��Դ�ļ���ֻ��Ҫ֪�����Լ���Ҫ��������������ȫ����

3. **��������ļ���**��ͨ��ͼ�ο⣬�� OpenGL��GLFW �� SDL���Ƽ���Դ�ļ������롣�������Ա��ִ����ģ�黯�����������ⲻ��Ҫ�İ�����

### ʹ��ʾ��

�����Դ�ļ��У�Ӧ���������� `glad`��

```c
#include <glad/glad.h> // glad must come before include OpenGL headers
#include <GLFW/glfw3.h> // GLFW (��������ͼ�ο�)
```

ȷ������֮ǰû����ʽ�ذ����κ� OpenGL ��ͷ�ļ�����Ϊ `glad` ��Ҫ������� OpenGL �������Դ��˱�Ҫ�� OpenGL ���塣

### �ܽ�

�� `glad` �İ�������Դ�ļ�����һ�����õ�ʵ���������������Ǳ�ڵ����⣬�������Ŀ��ά���ԺͿɶ��ԡ�

*/