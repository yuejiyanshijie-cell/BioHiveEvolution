# BioHive Evolution - 第一人称简陋可验证初版

## 已实现的核心功能

### 1. 第一人称角色控制 (BioHiveCharacter)
- WASD移动
- 鼠标视角控制
- 第一人称相机

### 2. 吞噬系统
- E键吞噬前方物体
- 射线检测目标
- 根据目标类型获取不同生物质
  - 普通目标: 10生物质
  - 小型目标: 5生物质
  - 大型目标: 25生物质
- 吞噬冷却时间: 1秒

### 3. 进化系统
- 5个器官可解锁
- 器官成本: 10, 25, 50, 100, 200生物质
- 按数字键1-5进化对应器官
- 位掩码记录已解锁器官

### 4. UI显示 (BioHiveHUD)
- 左上角显示当前生物质数量
- 显示已解锁器官数量
- 底部显示操作提示

## 文件结构

```
Source/BioHive/
├── Characters/
│   ├── BioHiveCharacter.h      # 第一人称角色头文件
│   └── BioHiveCharacter.cpp    # 第一人称角色实现
├── UI/
│   ├── BioHiveHUD.h            # HUD头文件
│   └── BioHiveHUD.cpp          # HUD实现
├── BioHiveFPGameMode.h         # 测试GameMode头文件
└── BioHiveFPGameMode.cpp       # 测试GameMode实现
```

## 如何运行

### 前置要求
1. 安装 Unreal Engine 5.4
2. 安装 Visual Studio 2022 (带C++游戏开发组件)

### 编译步骤

#### 方法1: 使用批处理脚本(推荐)
1. 双击 `LaunchProject.bat`
2. 选择选项 `2` (Build project)
3. 等待编译完成
4. 选择选项 `3` (Launch editor)

#### 方法2: 手动编译
```batch
cd "c:\Users\86151\新建文件夹\BioHiveEvolution"
"C:\Program Files\Epic Games\UE_5.4\Engine\Build\BatchFiles\GenerateProjectFiles.bat" BioHiveEvolution.uproject
"C:\Program Files\Epic Games\UE_5.4\Engine\Build\BatchFiles\Build.bat" BioHiveEvolutionEditor Win64 Development BioHiveEvolution.uproject
```

### 在编辑器中设置

1. 打开项目后,创建一个新的空白关卡
2. 在World Settings中设置:
   - GameMode Override: `BioHiveFPGameMode`
3. 添加Player Start
4. 点击Play测试

### 输入映射设置

需要在项目设置中添加以下输入绑定:

**Axis Mappings:**
- MoveForward: W(+1), S(-1)
- MoveRight: A(-1), D(+1)
- LookUp: Mouse Y
- Turn: Mouse X

**Action Mappings:**
- Consume: E键
- EvolveOrgan1: 1键
- EvolveOrgan2: 2键
- EvolveOrgan3: 3键
- EvolveOrgan4: 4键
- EvolveOrgan5: 5键

## 测试建议

1. **基础移动测试**: WASD移动,鼠标看四周
2. **吞噬测试**: 放置一些Static Mesh Actor,按E吞噬
3. **进化测试**: 积累足够生物质后,按1-5解锁器官
4. **UI测试**: 观察左上角数值变化

## 下一步扩展方向

1. 添加更多生物类型和对应的生物质值
2. 实现器官解锁后的实际效果(如移动速度提升、吞噬范围扩大等)
3. 添加更丰富的UI(进度条、图标等)
4. 实现母巢核心建筑
5. 添加敌人AI
6. 实现昼夜循环和天气系统
7. 添加音效和粒子效果

## 已知限制

- 这是一个简陋的验证原型,画面和功能都很基础
- 需要手动在编辑器中放置可吞噬物体
- 没有保存/加载功能
- UI非常简陋,仅显示文本

## 技术要点

- 使用射线检测进行吞噬交互
- 位掩码存储器官解锁状态
- Canvas绘制简单HUD
- 第一人称相机附着到角色头部
