# Hzael-learn

# 总体架构
1. Application,作为容器，包含 window 跟 LayerStack
2. Window 产生 event 到 Application 的callback
3. Layer 抽象出不同的渲染逻辑

## MainLoop

1. `Application`  作为容器，包含 Window & Layer， MainLoop 在Application中

## Renderer

1. Renderer & RenderCommand 作为外层API 接口
2. Renderer & RenderCommand 最后都将调用转到RenderAPI 里面去
3. `RenderAPI` 是一个Interface，具体实现由`OpenGLRendererAPI` 等
4. `Shader` , `VertexBuffer` ,`IndexBuffer` , `VertexArray` 等也是interface，具体由不同的Graphics API 去实现，类似于OGRE 中

## Input

1. use GLFW now