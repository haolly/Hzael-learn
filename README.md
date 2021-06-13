# Hzael-learn

# 总体架构
1. Application,作为容器，包含 window 跟 LayerStack
2. Window 产生 event 到 Application 的callback
3. Layer 抽象出不同的渲染逻辑

## MainLoop

1. `Application`  作为容器，包含 Window & Layer， MainLoop 在Application中
2.  使用GLFW 创建 window

## Event System

1. GLFW 的event 回调 分发到 `Application` 层
2. `Application` 分发到 `Layer` 

## 编辑器

1. 用ImGUI
2. ImGUI的panel resize 不会触发GLFW 的event

## Renderer
1. 包含一个RenderCommandQueue，管理render command 的执行

2. 对外暴露API 接口，实际实现是 RendererAPI

### RendererAPI
1. 抽象类，具体实现有 OpenGLRendererAPI/VulkanRendererAPI
2. 最后的draw call 都 submit 到了renderer 里面去

### RenderCommandBuffer
### RenderCommandQueue
1. 保存renderCommand，也就是一系列的callback
### SceneRenderer
### Pipeline
1. render state + renderpass + shader 之类的
2. OpenGL 的vertexArray 在这里创建，vertexbufferLayout 在这里设置
### RenderPass
1. 就是一个FrameBuffer
### Buffer
### Texture
1. Texture 其实就是 image + TextureProperty 
2. Image 就是image，图片，包含一个buffer用来存储数据。

## Input

1. use GLFW now

## Resource Manager

1. Shader 是由 shaderLibrary 来管理的，目前还没有lifeTime 的概念