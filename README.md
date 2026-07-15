# BioHive Evolution - Hive Genesis: Forest Dominion

![UE5](https://img.shields.io/badge/Unreal%20Engine-5.4-blue)
![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20Linux-green)
![License](https://img.shields.io/badge/License-MIT-yellow)

## 游戏简介

扮演森林深处诞生的生物母巢核心，通过吞噬植物、动物和变异生物获得生物质，不断进化自身结构。逐步扩张母巢网络，占领森林生态位，最终成为覆盖整片大陆的超级生命体。

### 核心特色

- **RTS + FPS 混合玩法**: 红色警戒式的宏观指挥 + 第一人称微观战斗
- **套装进化系统**: 收集装备部件激活套装奖励，取代复杂的基因树
- **动态生态系统**: 真实的森林食物链，生物会觅食、逃跑、繁殖
- **UE5 顶级画质**: Nanite虚拟几何体 + Lumen全局光照
- **大规模单位控制**: MassEntity系统支持数百个单位同时战斗

---

## 快速开始

### 前置要求

1. **Unreal Engine 5.4** 
   - 从 [Epic Games Launcher](https://www.unrealengine.com/zh-CN/download) 安装
   - 需要至少 100GB 磁盘空间（SSD推荐）

2. **Visual Studio 2022** (Windows) 或 **Xcode** (Mac)
   - 安装 "Game Development with C++" workload

3. **Git** (可选，用于版本控制)

### 安装步骤

#### Windows

1. **克隆或下载项目**
   ```bash
   # 如果从Git仓库获取
   git clone <repository-url>
   cd BioHiveEvolution
   ```

2. **运行启动脚本**
   ```bash
   LaunchProject.bat
   ```
   选择选项 `4` (Build and launch)

3. **首次打开编辑器**
   - UE5会自动编译C++代码（约5-10分钟）
   - 编译完成后编辑器自动打开

#### Linux/Mac

```bash
chmod +x LaunchProject.sh
./LaunchProject.sh
# 选择选项 4
```

### 手动设置（如果脚本失败）

1. **生成项目文件**
   ```bash
   "C:\Program Files\Epic Games\UE_5.4\Engine\Build\BatchFiles\GenerateProjectFiles.bat" BioHiveEvolution.uproject
   ```

2. **用Visual Studio打开**
   - 打开 `BioHiveEvolution.sln`
   - 构建配置选择 "Development Editor"
   - 按 `Ctrl+Shift+B` 编译

3. **启动编辑器**
   - 在VS中设置 `BioHiveEvolution` 为启动项目
   - 按 `F5` 调试启动
   - 或直接双击 `.uproject` 文件

---

## 项目结构

```
BioHiveEvolution/
├── BioHiveEvolution.uproject      # UE5项目文件
├── LaunchProject.bat              # Windows启动脚本
├── LaunchProject.sh               # Linux/Mac启动脚本
├── README.md                      # 本文档
│
├── Config/
│   ├── DefaultEngine.ini          # 引擎配置（Nanite/Lumen/输入）
│   └── DefaultGame.ini            # 游戏配置
│
├── Source/
│   └── BioHive/                   # C++源代码
│       ├── BioHive.Build.cs       # 模块依赖配置
│       ├── BioHive.h/.cpp         # 模块入口
│       ├── BioHiveGameMode.h/.cpp # 游戏模式
│       ├── BioHivePlayerController.h/.cpp # RTS控制器
│       │
│       ├── Units/                 # 单位系统
│       │   ├── BioUnit.h/.cpp             # 单位基类
│       │   └── BioUnitSelectionManager.h/.cpp # 选择管理
│       │
│       ├── Buildings/             # 建筑系统
│       │   └── BioBuilding.h/.cpp         # 建筑基类
│       │
│       ├── Systems/               # 资源系统
│       │   ├── BioResourceTypes.h         # 资源类型
│       │   ├── BioResourceManager.h/.cpp  # 资源管理器
│       │   └── BioResourceNode.h/.cpp     # 资源采集点
│       │
│       ├── Items/                 # 套装系统
│       │   └── BioItemSystem.h/.cpp       # 装备和套装
│       │
│       ├── Camera/                # 相机系统
│       │   └── BioRTSCamera.h/.cpp        # RTS相机
│       │
│       └── AI/                    # AI系统
│           └── BioAIController.h/.cpp     # AI控制器
│
├── Content/
│   ├── Blueprints/                # 蓝图资产
│   │   ├── Units/                 # 单位蓝图（需在编辑器中创建）
│   │   ├── Buildings/             # 建筑蓝图
│   │   ├── UI/                    # UMG界面
│   │   └── AI/                    # 行为树
│   │
│   ├── Maps/                      # 关卡
│   │   └── L_TestForest.umap      # 测试关卡（需创建）
│   │
│   ├── Input/                     # 输入映射
│   │   └── IMC_BioHiveRTS.uasset  # RTS控制方案
│   │
│   ├── Models/                    # 3D模型（从Quixel/Sketchfab导入）
│   ├── Materials/                 # 材质
│   └── Sounds/                    # 音效
│
└── DIR.txt                        # 文件清单
```

---

## 核心系统说明

### 1. 资源管理系统

**三种资源类型**:
- **生物质 (Biomass)**: 主要资源，用于生产单位和建造建筑
- **基因片段 (Gene Fragments)**: 稀有资源，解锁科技和套装
- **能量 (Energy)**: 限制高级单位数量

**获取方式**:
- 工蜂采集资源节点
- 单位吞噬敌人
- 自动收入（来自采集器建筑）

### 2. 建筑系统

**建筑类型**:
- **母巢核心**: 主基地，初始建筑，升级提升人口上限
- **孵化池**: 生产战斗单位
- **进化腔**: 研究科技
- **防御塔**: 自动攻击敌人

**放置流程**:
1. 按 `B` 打开建造菜单
2. 选择建筑类型
3. 地面显示绿色（可放置）或红色（不可放置）
4. 左键确认，右键取消
5. `Q/E` 旋转建筑

### 3. 单位系统

**单位类型**:
- **工蜂 (Drone)**: 采集资源，造价50生物质
- **战士 (Warrior)**: 近战攻击，造价100生物质
- **喷射兽 (Hydralisk)**: 远程酸液，造价150生物质

**RTS命令**:
- 左键框选单位
- 右键移动/攻击/采集
- `A` 攻击移动
- `H` 坚守位置
- `Tab` 切换到第一人称控制选中单位

### 4. 套装系统

**套装类型**:
- **捕食者套装** (红色): 提升攻击力、攻速、暴击
- **防御者套装** (蓝色): 提升护甲、减伤、反伤
- **辅助者套装** (绿色): 毒雾、群体控制、感知增强

**装备方式**:
- 击败Boss或完成任务获得套装部件
- 在单位信息界面拖拽装备
- 集齐2/3/4件激活套装奖励

### 5. 相机系统

**RTS视角**:
- `WASD`: 移动相机
- 鼠标移到屏幕边缘: 边缘滚动
- 滚轮: 缩放
- `空格`: 回到主基地
- `F`: 聚焦到选中单位

**第一人称视角**:
- `Tab`: 切换视角
- 标准FPS控制
- 再次按`Tab`返回RTS视角

---

## 操作指南

### 基础操作

| 按键 | 功能 |
|------|------|
| 左键 | 选择单位 / 确认放置 |
| 右键 | 移动 / 攻击 / 采集 |
| WASD | 移动相机 |
| 滚轮 | 缩放相机 |
| Tab | 切换RTS/FPS视角 |
| B | 打开/关闭建造菜单 |
| 空格 | 相机回到主基地 |
| F | 相机聚焦到选中单位 |

### 单位控制

| 按键 | 功能 |
|------|------|
| Ctrl + 左键 | 添加/移除单位选择 |
| 双击左键 | 选择所有同类型单位 |
| Ctrl + A | 选择所有单位 |
| A | 攻击移动 |
| H | 坚守位置 |
| S | 停止行动 |
| Ctrl + 数字键 | 创建编队 |
| 数字键 | 选择编队 |

### 建造快捷键

| 按键 | 功能 |
|------|------|
| Q | 快速建造工蜂 |
| W | 快速建造战士 |
| E | 快速建造防御塔 |
| B | 打开建造菜单 |

---

## 开发指南

### 添加新单位

1. **在C++中** (如果需要新功能):
   ```cpp
   // 继承 ABioUnit
   UCLASS()
   class AMyNewUnit : public ABioUnit
   {
       // 自定义逻辑
   };
   ```

2. **在编辑器中**:
   - 右键 Content Browser → Blueprint Class
   - 父类选择 `BioUnit`
   - 命名 `BP_MyNewUnit`
   - 设置默认属性（生命值、攻击力等）

3. **添加到生产队列**:
   - 打开 `BP_Hatchery` 蓝图
   - 在生产列表中添加新单位

### 添加新建筑

1. **创建蓝图**:
   - 父类 `BioBuilding`
   - 设置网格体和碰撞

2. **配置属性**:
   - 建造时间
   - 建造成本
   - 生命值

3. **添加到建造菜单**:
   - 编辑 `WBP_RTS_HUD`
   - 添加新的建造按钮

### 创建新套装

1. **创建DataAsset**:
   - 右键 Content Browser → Data Asset
   - 选择 `ItemSet` 类
   - 命名 `IS_PredatorSet`

2. **配置套装**:
   - 添加套装部件引用
   - 设置2/3/4件套奖励

3. **掉落配置**:
   - 在Boss或任务中添加到奖励表

---

## 性能优化

### 已实施的优化

- **Nanite**: 树木和岩石使用虚拟几何体，无LOD开销
- **Lumen**: 实时全局光照，无需烘焙
- **MassEntity**: 大规模单位AI并行处理
- **世界分区**: 关卡流送，只加载可见区域
- **HLOD**: 远处建筑合并渲染

### 建议设置

**低配机器**:
- 关闭光线追踪
- 降低阴影质量
- 减少视野距离
- 关闭体积云

**高配机器**:
- 启用光线追踪阴影
- 开启硬件光线追踪
- 提高反射质量
- 启用虚拟阴影贴图

---

## 常见问题

### Q: 编译时出现 "Module not found" 错误
**A**: 确保已安装UE5.4，并且插件依赖正确。检查 `BioHive.Build.cs` 中的模块名称。

### Q: 编辑器崩溃
**A**: 
1. 删除 `Intermediate` 和 `DerivedDataCache` 文件夹
2. 重新生成项目文件
3. 重新编译

### Q: 帧率过低
**A**:
1. 按 `~` 打开控制台，输入 `stat fps` 查看性能
2. 降低画质设置
3. 减少同屏单位数量
4. 关闭不必要的特效

### Q: 单位无法移动
**A**: 检查导航网格是否正确生成。在编辑器中按 `P` 显示NavMesh。

### Q: 资源不显示
**A**: 确保已安装Quixel Bridge插件并下载了森林资产包。

---

## 路线图

### MVP (当前阶段)
- [x] 核心C++框架
- [x] 资源管理系统
- [x] 建筑放置系统
- [x] 单位控制和选择
- [x] RTS相机
- [ ] 蓝图类实现
- [ ] 测试关卡搭建
- [ ] UI界面完成

### Alpha
- [ ] 完整gameplay循环
- [ ] 3种单位 + 3种建筑
- [ ] 基础AI
- [ ] 套装系统实装
- [ ] 第一个Boss

### Beta
- [ ] 内容完善（更多单位、建筑、套装）
- [ ] 平衡性调整
- [ ] 音效和音乐
- [ ] 教程关卡
- [ ] 性能优化

### Release
- [ ] Bug修复
- [ ] 多语言支持
- [ ] Steam集成
- [ ] 成就系统
- [ ] 发布

---

## Credits

### 使用的开源资源

- **Unreal Engine 5** - Epic Games
- **Quixel Bridge** - Quixel AB (Microsoft)
- **Mixamo** - Adobe
- **Sketchfab** - Sketchfab SAS

### 开发团队

本项目由个人开发者创建，欢迎贡献者！

---

## License

MIT License - 详见 LICENSE 文件

---

## 联系方式

- **Issues**: GitHub Issues
- **Discussions**: GitHub Discussions
- **Email**: (待添加)

---

**祝游戏愉快！** 🎮
