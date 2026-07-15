"""
BioHive Evolution - UE5 自动化设置脚本
在UE5编辑器中运行此脚本以自动创建所有蓝图和内容

使用方法:
1. 打开UE5编辑器
2. 打开 Output Log (Window -> Developer Tools -> Output Log)
3. 切换到 Python 标签
4. 复制并粘贴此脚本内容
5. 按 Enter 执行
"""

import unreal
import os

def create_directories():
    """创建必要的文件夹结构"""
    print("=== 创建文件夹结构 ===")
    
    directories = [
        "/Game/Blueprints/Units",
        "/Game/Blueprints/Buildings",
        "/Game/Blueprints/UI",
        "/Game/Blueprints/AI",
        "/Game/Blueprints/GameModes",
        "/Game/Maps",
        "/Game/Input",
        "/Game/DataAssets/Sets",
        "/Game/DataAssets/Resources",
    ]
    
    for dir_path in directories:
        unreal.EditorAssetLibrary.make_directory(dir_path)
        print(f"  创建: {dir_path}")
    
    print("文件夹创建完成!\n")

def create_input_mapping_context():
    """创建输入映射上下文"""
    print("=== 创建输入映射上下文 ===")
    
    # 创建 Input Mapping Context
    imc_path = "/Game/Input/IMC_BioHiveRTS"
    imc_factory = unreal.InputMappingContextFactory()
    
    # 注意: 实际需要通过编辑器UI创建，这里提供指导
    print(f"请在编辑器中手动创建 Input Mapping Context:")
    print(f"  路径: {imc_path}")
    print(f"  或运行控制台命令: CreateInputMappingContext {imc_path}")
    print()

def create_blueprint_unit(unit_name, display_name, unit_type, health, damage, speed, cost):
    """创建单位蓝图"""
    print(f"创建单位蓝图: {display_name}")
    
    # 查找父类
    parent_class = unreal.load_object(None, "/Script/BioHive.BioUnit")
    if not parent_class:
        print(f"  错误: 找不到 BioUnit 类，请确保C++模块已编译")
        return None
    
    # 创建蓝图
    blueprint_path = f"/Game/Blueprints/Units/BP_{unit_name}"
    factory = unreal.BlueprintFactory()
    factory.set_editor_property("parent_class", parent_class)
    
    try:
        asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
        blueprint = asset_tools.create_asset(
            asset_name=f"BP_{unit_name}",
            package_path="/Game/Blueprints/Units",
            factory=factory
        )
        
        if blueprint:
            # 设置默认属性
            blueprint_generated_class = blueprint.generated_class
            CDO = blueprint_generated_class.get_default_object()
            
            # 设置单位属性
            CDO.set_editor_property("UnitName", unreal.Text(display_name))
            CDO.set_editor_property("MaxHealth", health)
            CDO.set_editor_property("BaseAttackDamage", damage)
            CDO.set_editor_property("BaseMovementSpeed", speed)
            
            print(f"  ✓ 成功创建 {display_name}")
            return blueprint
    except Exception as e:
        print(f"  ✗ 创建失败: {str(e)}")
        return None

def create_all_units():
    """创建所有单位蓝图"""
    print("\n=== 创建单位蓝图 ===")
    
    units = [
        ("Drone", "工蜂", "Drone", 80.0, 5.0, 550.0, 50),
        ("Warrior", "战士", "Warrior", 150.0, 25.0, 650.0, 100),
        ("Hydralisk", "喷射兽", "Hydralisk", 120.0, 30.0, 500.0, 150),
    ]
    
    blueprints = []
    for unit_data in units:
        bp = create_blueprint_unit(*unit_data)
        if bp:
            blueprints.append(bp)
    
    print(f"共创建 {len(blueprints)} 个单位蓝图\n")
    return blueprints

def create_blueprint_building(building_name, display_name, building_type, health, cost, construction_time):
    """创建建筑蓝图"""
    print(f"创建建筑蓝图: {display_name}")
    
    parent_class = unreal.load_object(None, "/Script/BioHive.BioBuilding")
    if not parent_class:
        print(f"  错误: 找不到 BioBuilding 类")
        return None
    
    blueprint_path = f"/Game/Blueprints/Buildings/BP_{building_name}"
    factory = unreal.BlueprintFactory()
    factory.set_editor_property("parent_class", parent_class)
    
    try:
        asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
        blueprint = asset_tools.create_asset(
            asset_name=f"BP_{building_name}",
            package_path="/Game/Blueprints/Buildings",
            factory=factory
        )
        
        if blueprint:
            CDO = blueprint.generated_class.get_default_object()
            CDO.set_editor_property("BuildingName", unreal.Text(display_name))
            CDO.set_editor_property("MaxHealth", health)
            CDO.set_editor_property("ConstructionTime", construction_time)
            
            print(f"  ✓ 成功创建 {display_name}")
            return blueprint
    except Exception as e:
        print(f"  ✗ 创建失败: {str(e)}")
        return None

def create_all_buildings():
    """创建所有建筑蓝图"""
    print("\n=== 创建建筑蓝图 ===")
    
    buildings = [
        ("HiveCore", "母巢核心", "HiveCore", 1000.0, [], 0.0),
        ("Hatchery", "孵化池", "Hatchery", 600.0, [("Biomass", 200)], 15.0),
        ("DefenseTower", "防御塔", "DefenseTower", 400.0, [("Biomass", 150)], 12.0),
    ]
    
    blueprints = []
    for building_data in buildings:
        bp = create_blueprint_building(*building_data)
        if bp:
            blueprints.append(bp)
    
    print(f"共创建 {len(blueprints)} 个建筑蓝图\n")
    return blueprints

def create_widget_ui(widget_name, widget_class):
    """创建UMG Widget蓝图"""
    print(f"创建UI蓝图: {widget_name}")
    
    try:
        asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
        factory = unreal.WidgetBlueprintFactory()
        
        widget = asset_tools.create_asset(
            asset_name=widget_name,
            package_path="/Game/Blueprints/UI",
            factory=factory
        )
        
        if widget:
            print(f"  ✓ 成功创建 {widget_name}")
            return widget
    except Exception as e:
        print(f"  ✗ 创建失败: {str(e)}")
        return None

def create_all_ui():
    """创建所有UI蓝图"""
    print("\n=== 创建UI蓝图 ===")
    
    widgets = [
        ("WBP_RTS_HUD", "UserWidget"),
        ("WBP_UnitCard", "UserWidget"),
        ("WBP_BuildButton", "UserWidget"),
        ("WBP_Minimap", "UserWidget"),
    ]
    
    ui_blueprints = []
    for widget_data in widgets:
        bp = create_widget_ui(*widget_data)
        if bp:
            ui_blueprints.append(bp)
    
    print(f"共创建 {len(ui_blueprints)} 个UI蓝图\n")
    return ui_blueprints

def create_test_level():
    """创建测试关卡"""
    print("\n=== 创建测试关卡 ===")
    
    try:
        # 创建新关卡
        level_path = "/Game/Maps/L_TestForest"
        unreal.EditorLevelLibrary.new_level(level_path)
        
        # 添加 PlayerStart
        player_start_class = unreal.load_object(None, "/Script/Engine.PlayerStart")
        if player_start_class:
            player_start = unreal.EditorLevelLibrary.spawn_actor_from_class(
                player_start_class,
                unreal.Vector(0, 0, 0),
                unreal.Rotator(0, 0, 0)
            )
            print("  ✓ 添加 PlayerStart")
        
        # 添加 DirectionalLight (太阳)
        light_class = unreal.load_object(None, "/Script/Engine.DirectionalLight")
        if light_class:
            light = unreal.EditorLevelLibrary.spawn_actor_from_class(
                light_class,
                unreal.Vector(0, 0, 500),
                unreal.Rotator(-45, 0, 0)
            )
            print("  ✓ 添加 DirectionalLight")
        
        # 添加 SkyLight
        skylight_class = unreal.load_object(None, "/Script/Engine.SkyLight")
        if skylight_class:
            skylight = unreal.EditorLevelLibrary.spawn_actor_from_class(
                skylight_class,
                unreal.Vector(0, 0, 0),
                unreal.Rotator(0, 0, 0)
            )
            print("  ✓ 添加 SkyLight")
        
        # 保存关卡
        unreal.EditorAssetLibrary.save_loaded_asset(level_path)
        print(f"  ✓ 关卡已保存: {level_path}")
        
    except Exception as e:
        print(f"  ✗ 创建关卡失败: {str(e)}")

def create_data_assets():
    """创建DataAsset配置"""
    print("\n=== 创建数据资产 ===")
    
    # 这里需要在编辑器中手动创建 ItemSet DataAssets
    print("请在编辑器中创建以下 DataAsset:")
    print("  /Game/DataAssets/Sets/IS_PredatorSet (捕食者套装)")
    print("  /Game/DataAssets/Sets/IS_DefenderSet (防御者套装)")
    print("  /Game/DataAssets/Sets/IS_SupportSet (辅助者套装)")
    print()

def configure_project_settings():
    """配置项目设置"""
    print("\n=== 配置项目设置 ===")
    
    try:
        # 设置默认 GameMode
        settings = unreal.GameProjectSettings()
        settings.set_editor_property(
            "default_game_mode",
            "/Script/BioHive.BioHiveGameMode"
        )
        settings.save_config()
        print("  ✓ 设置默认 GameMode")
        
    except Exception as e:
        print(f"  ✗ 配置失败: {str(e)}")

def main():
    """主函数 - 执行所有自动化任务"""
    print("=" * 60)
    print("BioHive Evolution - UE5 自动化设置")
    print("=" * 60)
    print()
    
    # 1. 创建文件夹
    create_directories()
    
    # 2. 创建单位蓝图
    create_all_units()
    
    # 3. 创建建筑蓝图
    create_all_buildings()
    
    # 4. 创建UI蓝图
    create_all_ui()
    
    # 5. 创建测试关卡
    create_test_level()
    
    # 6. 创建数据资产说明
    create_data_assets()
    
    # 7. 配置项目设置
    configure_project_settings()
    
    print("\n" + "=" * 60)
    print("自动化设置完成!")
    print("=" * 60)
    print("\n下一步:")
    print("1. 从 Quixel Bridge 导入森林资产")
    print("2. 在蓝图中分配网格体和材质")
    print("3. 配置 UI 界面布局")
    print("4. 测试游戏 (Play)")
    print()

# 执行主函数
if __name__ == "__main__":
    main()
