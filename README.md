# Digital Techniques II - Embedded Systems Lab 🚀

Welcome to the **Digital Techniques II** repository. This project serves as a comprehensive collection of firmware developments, ranging from low-level hardware abstraction to advanced Real-Time Operating Systems (RTOS) integration.

The repository is designed to demonstrate proficiency in ARM Cortex-M architecture, focusing on modularity, efficiency, and industrial-standard programming patterns.

---

## 🛠️ Hardware Requirements

To run the programs in this repository, you will primarily need the following development boards:

### 1. Primary Board: **STM32F446RET6 (WeAct Studio)**
Most of the advanced projects, high-speed processing, and RTOS implementations are targeted for this powerful **Cortex-M4** core.
* **Core:** ARM® 32-bit Cortex®-M4 CPU with FPU.
* **Clock:** Up to 180 MHz.

### 2. Secondary Board: **STM32F103C8T6 (Blue Pill)**
Specific legacy modules or low-power consumption exercises utilize the classic **Cortex-M3** "Blue Pill".
* *Note: Please check the individual source headers to confirm if a project is targeted for the F103 series.*

---

## 🧠 Core Methodologies

The firmware architecture revolves around three fundamental pillars:

### ⚙️ Finite State Machines (FSM)
We implement robust **Mealy and Moore FSMs** to handle complex system logic. This ensures deterministic behavior and simplifies the debugging of asynchronous events in embedded environments.

### ⏱️ FreeRTOS Integration
For multi-tasking applications, we leverage **FreeRTOS**. 
- Preemptive scheduling.
- Task synchronization using Semaphores, Mutexes, and Queues.
- Memory management and efficient resource allocation.

### 🏗️ Assembly (ASM)
To understand the underlying architecture and optimize critical code paths, several modules are written in **ARM Assembly**.
- Inline Assembly and standalone `.s` files.
- Optimization of mathematical operations.
- Deep dive into the NVIC and Exception handling.