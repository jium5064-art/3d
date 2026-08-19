# C++ 3D Physics Engine

간단한 C++ 3D 물리 엔진 테스트 프로젝트입니다.

## 기능

- WASD 이동
- 마우스 시점 조작
- 중력
- 점프 (`SPACE`)
- 바닥 충돌
- 아레나 경계 충돌
- 3D 테스트 큐브와 플레이어 구체
- CMake 빌드

## 요구 사항

- C++17 이상
- CMake 3.20+
- raylib 5.0+

## 빌드

```bash
cmake -S . -B build
cmake --build build
```

실행 파일을 실행한 뒤 `WASD`로 이동하고 `SPACE`로 점프할 수 있습니다.
