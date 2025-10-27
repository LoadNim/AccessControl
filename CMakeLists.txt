cmake_minimum_required(VERSION 3.21)
project(AccessControl LANGUAGES CXX)

# ==============================================================
# 프로젝트 설정
# ==============================================================

# C++ 표준 설정
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt 자동 처리 옵션 (MOC, RCC, UIC)
set(CMAKE_AUTOMOC ON)

# ==============================================================
# Qt 설정
# ==============================================================
# Core, Widgets, Network 모듈 사용
find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Network)

# 표준 Qt 프로젝트 세팅(경고/생성기 등 플랫폼s별 설정 포함)
qt_standard_project_setup()

# ==============================================================
# OpenCV 설정
# ==============================================================
# 카메라 + 전처리 + 인코딩(메모리/파일) 용 최소 구성
find_package(OpenCV REQUIRED COMPONENTS core videoio imgproc imgcodecs)

# ==============================================================
# 소스 파일 구성
# ==============================================================
# src / include / ui 디렉터리에서 파일 자동 탐색
file(GLOB SRC_FILES CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/src/*.cpp)
file(GLOB HDR_FILES CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/include/*.h)

# 실행 파일 생성
qt_add_executable(${PROJECT_NAME}
    WIN32 MACOSX_BUNDLE
    ${SRC_FILES}
    ${HDR_FILES}
)

# 헤더 검색 경로 추가 (include/의 .h를 찾기 위함)
target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
)

# ==============================================================
# QSS 리소스 임베드 (권장)
# ==============================================================
# qss/app.qss를 실행파일에 내장한다. 런타임 접근 경로는 ":/qss/app.qss".
qt_add_resources(${PROJECT_NAME} app_qss
    PREFIX "/"
    FILES
        qss/app.qss
        # qss/dark.qss  # ← 다크 테마 등 추가 시 여기에 나열
)

# (선택) IDE 트리에서 qss 파일을 보여주고 싶다면 다음 라인으로도 등록 가능
# target_sources(${PROJECT_NAME} PRIVATE qss/app.qss)

# ==============================================================
# 링크 설정
# ==============================================================
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        Qt6::Core
        Qt6::Widgets
        Qt6::Network
        ${OpenCV_LIBS}
)

# ==============================================================
# 설치 및 배포 설정
# ==============================================================
include(GNUInstallDirs)

# 실행 파일 설치 경로 지정
install(TARGETS ${PROJECT_NAME}
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# Qt 런타임 의존성 자동 배포 스크립트 생성
qt_generate_deploy_app_script(
    TARGET ${PROJECT_NAME}
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})

# ==============================================================
# 빌드 완료 후 실행파일 마무리 설정
# ==============================================================
qt_finalize_executable(${PROJECT_NAME})
