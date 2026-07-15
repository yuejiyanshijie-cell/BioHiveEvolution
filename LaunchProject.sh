#!/bin/bash
# BioHiveEvolution - Linux/Mac 启动脚本

echo "========================================"
echo "BioHive Evolution - Project Launcher"
echo "========================================"
echo ""

# UE5路径（根据系统调整）
if [[ "$OSTYPE" == "darwin"* ]]; then
    # macOS
    UE5_PATH="/Users/Shared/Epic Games/UE_5.4"
else
    # Linux
    UE5_PATH="$HOME/UnrealEngine/5.4"
fi

if [ ! -d "$UE5_PATH" ]; then
    echo "[ERROR] UE5 not found at $UE5_PATH"
    echo "Please install UE5.4 from Epic Games Launcher"
    exit 1
fi

echo "[INFO] Found UE5 at: $UE5_PATH"
echo ""

PROJECT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_FILE="$PROJECT_DIR/BioHiveEvolution.uproject"

echo "[INFO] Project directory: $PROJECT_DIR"
echo ""

echo "Select action:"
echo "1. Generate project files"
echo "2. Build project (Development Editor)"
echo "3. Launch editor"
echo "4. Build and launch"
echo "5. Package game"
echo "6. Clean intermediate files"
echo ""
read -p "Enter choice (1-6): " ACTION

case $ACTION in
    1)
        echo ""
        echo "[INFO] Generating project files..."
        "$UE5_PATH/Engine/Build/BatchFiles/GenerateProjectFiles.sh" "$PROJECT_FILE"
        ;;
    2)
        echo ""
        echo "[INFO] Building project..."
        "$UE5_PATH/Engine/Build/BatchFiles/Build.sh" BioHiveEvolutionEditor Linux Development "$PROJECT_FILE"
        ;;
    3)
        echo ""
        echo "[INFO] Launching UE5 Editor..."
        "$UE5_PATH/Engine/Binaries/Linux/UnrealEditor" "$PROJECT_FILE" &
        ;;
    4)
        bash "$0" <<< "2"
        bash "$0" <<< "3"
        ;;
    5)
        echo ""
        echo "[INFO] Packaging game..."
        "$UE5_PATH/Engine/Build/BatchFiles/RunUAT.sh" BuildCookRun \
            -project="$PROJECT_FILE" \
            -noP4 \
            -platform=Linux \
            -clientconfig=Shipping \
            -cook \
            -allmaps \
            -stage \
            -pak \
            -archive \
            -archivedirectory="$PROJECT_DIR/Builds"
        ;;
    6)
        echo ""
        echo "[INFO] Cleaning intermediate files..."
        rm -rf "$PROJECT_DIR/Intermediate"
        rm -rf "$PROJECT_DIR/DerivedDataCache"
        rm -rf "$PROJECT_DIR/.vs"
        rm -rf "$PROJECT_DIR/Binaries"
        echo "[SUCCESS] Cleaned!"
        ;;
    *)
        echo "Invalid choice"
        exit 1
        ;;
esac

echo ""
echo "Done."
