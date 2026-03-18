#!/bin/bash
# GUI Tests Quick Reference Guide
# This script demonstrates how to run the GUI tests

# Colors for output
GREEN='\033[0;32m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}=== cppforge GUI Tests Quick Reference ===${NC}\n"

# Navigate to build directory
BUILD_DIR="/home/iluha/cppforge/build"

if [ ! -d "$BUILD_DIR" ]; then
    echo "Build directory not found. Please run cmake .. and make first."
    exit 1
fi

cd "$BUILD_DIR"

# Show menu
echo "Available commands:"
echo ""
echo "1. Run all tests with summary:"
echo -e "   ${GREEN}ctest${NC}"
echo ""
echo "2. Run all tests with verbose output:"
echo -e "   ${GREEN}ctest -V${NC}"
echo ""
echo "3. Run tests with output on failure:"
echo -e "   ${GREEN}ctest --output-on-failure${NC}"
echo ""
echo "4. Run specific test suite:"
echo -e "   ${GREEN}ctest -R test_MainWindow${NC}"
echo ""
echo "5. Run individual test manually (with offscreen rendering):"
echo -e "   ${GREEN}QT_QPA_PLATFORM=offscreen ./tests/test_CustomTitleBar${NC}"
echo ""
echo "6. List all available tests:"
echo -e "   ${GREEN}ctest --print-labels${NC}"
echo ""
echo "Tests Available:"
echo "  - test_CustomTitleBar:   9 tests for CustomTitleBar widget"
echo "  - test_MainWindow:       12 tests for MainWindow widget"
echo "  - test_TaskWindow:       14 tests for TaskWindow widget"
echo "  - test_TaskManager:      18 tests for TaskManager singleton"
echo "  - test_AuthWindow:       2 tests for AuthWindow (10 skipped)"
echo "  - test_SignUpWindow:     2 tests for SignUpWindow (10 skipped)"
echo ""
echo "Current location: $BUILD_DIR"
echo ""
echo "To run a test, execute one of the commands above."
