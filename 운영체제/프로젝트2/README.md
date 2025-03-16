# 운영체제 2024 가을 프로젝트 2

## 수학과 202021218 김재영
## 프로젝트 개요
이 프로젝트는 **동기화**(**Synchronization**)의 중요성을 이해하기 위해 **Producer-Consumer 문제**와 **Reader-Writer 문제**를 구현하고, 동기화 적용 전, 후의 차이를 비교한다. 두 문제 모두 공유 자원에 대한 여러 스레드의 동시 접근을 조율하여 동기화 기법의 필요성을 잘 보여준다.

---

## 1. Producer-Consumer 문제

### 문제 설명
- **Producer**는 네트워크 패킷을 생산하여 버퍼에 추가하고, **Consumer**는 버퍼에서 네트워크 패킷을 소비한다.
- 생산자와 소비자는 동시에 실행되며, 서로 충돌하지 않도록 해야 합니다.
- **제약 조건**:
  - 버퍼가 가득 차면 Producer는 대기.
  - 버퍼가 비어 있으면 Consumer는 대기.

### 구현 방식
- **동기화 전**:
  - 생산자와 소비자는 공유 변수(`counter`, `in`, `out`)를 직접 조작.
  - 충돌 방지 장치가 없어 **Race Condition**이 발생할 가능성이 높다.
- **동기화 후**:
  - 세마포어(`empty`, `full`)와 뮤텍스(`mutex`)를 활용하여 공유 자원을 보호하고, 대기를 구현한다.
  - **동기화 전**과 달리 mutex lock을 걸어서 **Race Condition** 발생 가능성이 낮다.

---

## 2. Reader-Writer 문제

### 문제 설명
- 여러 **Reader**는 동시에 데이터를 읽을 수 있지만, **Writer**는 다른 Reader 또는 Writer가 데이터에 접근 중일 때 대기해야 한다.

### 구현 방식
- **동기화 전**:
  - Reader와 Writer는 공유 변수(`read_count`)에 대해 동기화 없이 접근한다.
  - **Race Condition**으로 인해 데이터 무결성이 훼손될 수 있다.
- **동기화 후**:
  - Reader와 Writer의 접근을 조율하기 위해 뮤텍스와 세마포어를 사용.

---

## 3. 구현 환경

- **언어**: C
- **운영 체제**:
  - Linux: POSIX Threads 및 세마포어 사용
- **컴파일러**:
  - Linux: `gcc`

---

## 4. 실행 방법

### Linux
1. **Producer-Consumer 문제**:
    ```bash
    gcc PC_no_sync.c -o PC_no_sync
    ./PC_no_sync
    ```

    ```bash
    gcc PC_sync.c -o PC_sync
    ./PC_sync
    ```
2. **Reader-Writer 문제**:
    ```bash
    gcc RW_no_sync.c -o RW_no_sync
    ./RW_no_sync
    ```
    ```bash
    gcc RW_sync.c -o RW_sync
    ./RW_sync
    ```

## 5. 결과 확인

1. **Producer-Consumer 문제**
    첨부된 text파일 확인 (`PC_no_sync.txt` & `PC_sync.txt`)
    PC_no_sync.c 파일의 실행은 종료되지 않으므로 강제 종료 명령(`ctrl+c`)를 입력해야 한다.
2. **Reader-Writer 문제**
    첨부된 text파일 확인 (`RW_no_sync.txt` & `RW_sync.txt`)