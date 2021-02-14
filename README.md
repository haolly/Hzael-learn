# Hzael-learn

# 总体架构
1. Application,作为容器，包含 window 跟 LayerStack
2. Window 产生 event 到 Application 的callback
3. Layer 抽象出不同的渲染逻辑