#!/bin/bash
# ECDSA 빌드 및 실행 스크립트

echo "=========================================="
echo "ECDSA Implementation - Build and Run"
echo "=========================================="
echo ""

# 색상 정의
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# 디렉토리 확인
if [ ! -f "Makefile" ]; then
    echo -e "${YELLOW}Makefile을 찾을 수 없습니다. ECDSA 디렉토리에서 실행하세요.${NC}"
    exit 1
fi

echo -e "${BLUE}[1] 빌드 중...${NC}"
make clean
make

if [ -f "ecdsa_demo" ]; then
    echo -e "${GREEN}✓ 빌드 완료${NC}"
    echo ""
    echo -e "${BLUE}[2] 데모 프로그램 실행 중...${NC}"
    echo "=========================================="
    ./ecdsa_demo
    echo "=========================================="
    echo ""
    echo -e "${GREEN}✓ 모든 작업 완료${NC}"
else
    echo -e "${YELLOW}✗ 빌드 실패${NC}"
    exit 1
fi
