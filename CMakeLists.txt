cmake_minimum_required(VERSION 3.21)

# ==============================================================
# 프로젝트 메타 (Windows 배포 전용)
# ==============================================================
project(AccessControl LANGUAGES CXX VERSION 1.0.0)
set(APP_NAME "AccessControl")

# ==============================================================
# C++ 표준
# ==============================================================
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt 자동 처리 옵션 (MOC, RCC, UIC)
set(CMAKE_AUTOMOC ON)

# ==============================================================
# Qt 설정
# ==============================================================
find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Network)
qt_standard_project_setup()

# ==============================================================
# OpenCV 설정 (카메라/전처리/인코딩 최소 구성)
# ==============================================================
find_package(OpenCV REQUIRED COMPONENTS core videoio imgproc imgcodecs)

# ==============================================================
# 소스 파일 구성
# ==============================================================
file(GLOB SRC_FILES CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/src/*.cpp)
file(GLOB HDR_FILES CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/include/*.h)

qt_add_executable(${PROJECT_NAME}
    WIN32 MACOSX_BUNDLE
    ${SRC_FILES}
    ${HDR_FILES}
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
)

# ==============================================================
# QSS 리소스 임베드
#  - 런타임 접근 경로: ":/qss/app.qss"
# ==============================================================
qt_add_resources(${PROJECT_NAME} app_qss
    PREFIX "/"
    FILES
        qss/app.qss
)

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
# 빌드별 로그 출력 제어 (Release에서 qDebug/qInfo 제거)
# ==============================================================
target_compile_definitions(${PROJECT_NAME} PRIVATE
    $<$<CONFIG:Release>:QT_NO_DEBUG_OUTPUT>
    $<$<CONFIG:Release>:QT_NO_INFO_OUTPUT>
)

# ==============================================================
# 설치 및 배포 설정 (Windows 전용 튜닝)
# ==============================================================
include(GNUInstallDirs)

# 실행 파일을 설치 루트(예: dist/AccessControl)의 최상위에 배치
# DLL도 windeployqt가 같은 폴더에 풀어놓을 예정
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION .
    BUNDLE  DESTINATION .
)

# (선택) 외부 리소스 디렉터리 동봉: 있으면 설치 루트 하위로 복사
if(EXISTS "${CMAKE_SOURCE_DIR}/assets")
    install(DIRECTORY ${CMAKE_SOURCE_DIR}/assets/   DESTINATION assets)
endif()
if(EXISTS "${CMAKE_SOURCE_DIR}/models")
    install(DIRECTORY ${CMAKE_SOURCE_DIR}/models/   DESTINATION models)
endif()
if(EXISTS "${CMAKE_SOURCE_DIR}/cascades")
    install(DIRECTORY ${CMAKE_SOURCE_DIR}/cascades/ DESTINATION cascades)
endif()

# Qt 런타임 의존성 자동 배포 스크립트 생성
# - Windows에서는 windeployqt를 설치 단계에서 실행해 DLL/플러그인 자동 복사
qt_generate_deploy_app_script(
    TARGET ${PROJECT_NAME}
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})

# 마무리
qt_finalize_executable(${PROJECT_NAME})
