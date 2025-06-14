# CoTigraphy

![CoTigraphy example](samples/CoTigraphy.webp "CoTigraphy example")

---

## 📋 목차

- [실행 방법](#-실행-방법)
- [명령줄 인자](#-명령줄-인자-사용법)
- [GitHub Access Token 생성](#-github-access-token-생성-방법)
- [직접 빌드하기](#-직접-빌드하기)
- [라이선스](#-라이선스)
- [기여 및 지원](#-기여-및-지원)

---

## 🚀 실행 방법

### 미리 빌드된 파일 다운로드 (권장)

바로 사용할 수 있는 실행 파일을 다운로드하세요:

**👉 [최신 버전 다운로드](https://github.com/ohsungsik/CoTigraphy/releases/latest)**

### 시스템 요구사항

- **운영체제**: Windows 10 이상
- **아키텍처**: x64, x86, ARM64 지원

### 설치 단계

1. 위 링크에서 본인의 시스템에 맞는 파일 다운로드
   - `CoTigraphy.x64.Release.exe` (64비트 Windows, **권장**)
   - `CoTigraphy.x86.Release.exe` (32비트 Windows)
   - `CoTigraphy.ARM64.Release.exe` (ARM64 Windows)

2. 원하는 폴더에 실행 파일 저장

3. 명령 프롬프트 또는 PowerShell에서 실행

---

## 🧾 명령줄 인자 사용법

CoTigraphy는 다양한 명령줄 인자를 통해 자동화 및 사용자 설정을 지원합니다.

### 기본 사용법

```bash
CoTigraphy.x64.Release.exe [options]
```

### 지원되는 옵션

| 옵션            | 단축형  | 값 필요 | 설명                              |
| ------------- | ---- | ----- | ------------------------------- |
| `--help`      | `-h` | ❌     | 도움말 메시지를 출력하고 종료                |
| `--version`   | `-v` | ❌     | 프로그램 버전 출력                      |
| `--token`     | `-t` | ✅     | GitHub Personal Access Token 입력 |
| `--user_name` | `-n` | ✅     | GitHub 사용자 이름 입력                |
| `--output`    | `-o` | ✅     | 결과물을 저장할 출력 경로 지정               |

### 사용 예시

```bash
# 기본 사용법
CoTigraphy.x64.Release.exe -t ghp_abc123 -n ohsungsik -o CoTigraphy.webp

# 도움말 확인
CoTigraphy.x64.Release.exe --help

# 버전 정보 확인
CoTigraphy.x64.Release.exe --version
```

---

## 🔐 GitHub Access Token 생성 방법

Personal Access Token (PAT)이 필요합니다.

> 💡 **참고**: 토큰은 `--token` 또는 `-t` 옵션으로 전달할 수 있습니다.

### 생성 절차

1. **GitHub 설정 페이지로 이동**  
   👉 [https://github.com/settings/personal-access-tokens](https://github.com/settings/personal-access-tokens)

2. **토큰 유형 선택**  
   - **Fine-grained token** 선택

3. **토큰 설정**

   | 항목                    | 설정값                                    |
   | --------------------- | -------------------------------------- |
   | **Token name**        | `CoTigraphyToken` 등 식별 가능한 이름         |
   | **Expiration**        | 사용 목적에 따른 적절한 만료 기간                  |
   | **Resource owner**    | 본인의 계정 선택                             |
   | **Repository access** | `Public repositories` 선택      |
   | **Permissions**       | 기본 설정으로 충분                           |

4. **토큰 생성 및 저장**  
   - **Generate token** 버튼 클릭
   - ⚠️ **중요**: 토큰이 생성되면 즉시 복사하여 안전한 곳에 저장하세요. 다시 확인할 수 없습니다.

### 토큰 사용 예시

```bash
CoTigraphy.x64.Release.exe -t ghp_xxxxxxxxxxxxxxxxxxx -n your_github_id -o CoTigraphy.webp
```

### 보안 주의사항

- ❌ 토큰을 공개 저장소에 업로드하지 마세요
- ❌ 소스코드에 하드코딩하지 마세요  
- ❌ 스크립트 파일에 직접 포함하지 마세요
- ✅ 환경 변수나 별도 설정 파일 사용을 권장합니다
- ⚠️ 토큰이 유출된 경우 즉시 GitHub에서 **Revoke** 하세요

---

## 🔨 직접 빌드하기

개발자이거나 소스코드를 수정하고 싶다면 직접 빌드할 수 있습니다.

### 빌드 환경 요구사항

- **운영체제**: Windows 10 이상
- **개발도구**: Visual Studio 2022 이상
- **C++ 컴파일러**: MSVC (Microsoft Visual C++)

### 빌드 방법

#### 방법 1: 자동 빌드 스크립트

```cmd
# 프로젝트 루트 디렉토리에서 실행
build.bat
```

빌드 완료 후 `out\bin\` 폴더에서 실행 파일을 확인할 수 있습니다.

#### 방법 2: Visual Studio 사용

1. `CoTigraphy.sln` 파일을 Visual Studio로 열기
2. 상단에서 플랫폼 선택 (x64/x86/ARM64)
3. 구성 선택 (Debug/Release)
4. **빌드** → **솔루션 빌드** (`Ctrl+Shift+B` 또는 `F7`)

### 빌드 출력

성공적으로 빌드되면 다음 위치에 실행 파일이 생성됩니다:
- **Release**: `out\bin\Release\`
- **Debug**: `out\bin\Debug\`

---

## 📄 라이선스

이 프로젝트는 **MIT 라이선스** 하에 배포됩니다.

```
SPDX-License-Identifier: MIT
```

자세한 내용은 [LICENSE](LICENSE) 파일을 참조하세요.

---

## 🤝 기여 및 지원

### 🐛 버그 신고 및 💡 기능 제안

[GitHub Issues](https://github.com/ohsungsik/CoTigraphy/issues)를 통해 버그 신고나 새로운 기능을 제안해 주세요:

- **버그 신고**: 문제 상황과 재현 방법을 상세히 작성
- **기능 제안**: 필요한 기능과 사용 사례 설명  
- **문서 개선**: 이해하기 어려운 부분이나 누락된 내용 지적

### 💻 개발 참여

Pull Request를 통한 코드 기여를 환영합니다:

1. 프로젝트를 Fork합니다
2. 기능 브랜치를 생성합니다 (`git checkout -b feature/amazing-feature`)
3. 변경사항을 커밋합니다 (`git commit -m 'Add some amazing feature'`)
4. 브랜치에 Push합니다 (`git push origin feature/amazing-feature`)
5. Pull Request를 생성합니다

### 📬 문의

궁금한 점이 있으시면 언제든 연락주세요:

- **이메일**: [ohsungsik@outlook.com](mailto:ohsungsik@outlook.com)
- **GitHub**: [@ohsungsik](https://github.com/ohsungsik)

---

<div align="center">
**⭐ 이 프로젝트가 유용하다면 별표를 눌러주세요! ⭐**
Made with ❤️ by [ohsungsik](https://github.com/ohsungsik)
</div>