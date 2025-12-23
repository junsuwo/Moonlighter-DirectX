# Moonlighter (DirectX 9 Action RPG)

## 📌 Repository Purpose
본 저장소는 **DirectX 9 기반 게임 엔진 구조와 전투 시스템 구현 방식**을  
코드 중심으로 정리한 포트폴리오용 저장소입니다.

상용 에셋 및 리소스는 제외하고,  
**플레이어 전투 / 몬스터 FSM / 보스 패턴 / 충돌 판정 / 렌더링 흐름** 등  
핵심 로직과 설계 의도를 확인할 수 있도록 구성되었습니다.

---

## 🎮 Project Overview
- Engine: DirectX 9
- Language: C++
- Genre: Top-Down Action RPG
- Platform: Windows (PC)
- Focus: Combat System / FSM / Low-Level Rendering

---

## 🧑‍💻 My Role
- DirectX 9 기반 렌더링 및 스프라이트 처리
- 플레이어 이동 / 구르기(무적) / 콤보 전투 시스템 구현
- 몬스터 FSM (Idle / Chase / Attack) 구조 설계
- 보스 전용 FSM 및 공격 패턴 분기
- 충돌 판정 및 디버깅 구조 설계
- 사운드 매니저 구조 설계 (FMOD / 기본 사운드 시스템)

---

## 🗂 Repository Structure
```text
FrameWork/
 ├ Core        # 게임 루프 / 씬 관리
 ├ Graphics    # DirectX 9 렌더링 / 스프라이트 처리
 ├ Input       # 키 입력 처리
 ├ Player      # 플레이어 이동 / 전투 / 상태 관리
 ├ Monster     # 몬스터 FSM 구조
 ├ Boss        # 보스 FSM / 패턴 로직
 ├ Collision   # 충돌 판정 및 디버깅
 ├ Sound       # 사운드 매니저 (FMOD 등)
 └ Util        # 공용 유틸리티
```

⭐ Key Features
🧍 Player Combat System

4방향 이동 및 방향 기반 애니메이션 전환

입력 타이밍 기반 최대 3단 콤보 공격

구르기(Roll) 시 무적 프레임 적용

공격 / 이동 / 회피 상태 전환 관리

🤖 Monster & Boss AI (FSM)

FSM 기반 몬스터 AI (Idle / Chase / Attack)

시야 범위 기반 플레이어 반응

보스 전용 FSM 및 패턴 분기 구조

방향에 따른 공격 범위 및 애니메이션 처리

💥 Collision & Feedback

플레이어 / 몬스터 / 보스 간 충돌 판정

공격 범위 및 충돌 영역 시각화(Debug)

데미지 처리 및 전투 피드백 구조

🔊 Sound System

공격 / 피격 / 환경 사운드 처리

중복 재생 방지를 위한 채널 관리 구조

🛠 Troubleshooting Highlights

애니메이션 프레임 타이밍 관리 복잡도 해결

공격 콤보 및 무적 처리 타이밍 분리

사운드 중복 재생 문제 해결

FSM 상속 구조 복잡도 → 보스 전용 FSM 분리

충돌 판정 디버깅을 위한 시각화 도입

🚀 What I Learned

DirectX 9 기반 렌더링 파이프라인 이해

상태 머신(FSM)을 활용한 전투 AI 설계 경험

입력 / 상태 / 애니메이션 분리 설계의 중요성

UI 중심 개발을 넘어 엔진 레벨 구조 이해 확장

시스템 관점에서 전투 흐름을 설계하는 경험

## 🎥 Demo
- Gameplay Video: [(Youtube 링크)](https://youtu.be/luwh2BIV59g)
- Build File:  [(Google Drive / OneDrive 링크)](https://drive.google.com/drive/u/0/folders/1LQ9EOn7oTka26-jDLLP2s-R_oeZCWhSR)

본 프로젝트는 학습 및 포트폴리오 목적의 개인 프로젝트입니다.
