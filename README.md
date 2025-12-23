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

## 🎥 Demo
- Gameplay Video: [(Youtube 링크)](https://youtu.be/luwh2BIV59g)
- Build File:  [(Google Drive / OneDrive 링크)](https://drive.google.com/drive/u/0/folders/1LQ9EOn7oTka26-jDLLP2s-R_oeZCWhSR)

본 프로젝트는 학습 및 포트폴리오 목적의 개인 프로젝트입니다.
