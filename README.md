# 🚀 Embedded Systems & Mono-Stage Amplifiers (UTN-FRBA)

Welcome to this repository dedicated to the design, simulation, and low-level control of electronic systems. This project integrates **Assembly (ASM) software development**.

## 🛠️ Required Hardware

To run and test the programs in this repository, you will need the following development platforms:

* **Primary Board:** [STM32F446RET6 (WeAct Studio)](https://github.com/WeActStudio/WeActStudio.STM32F446RET6) – Based on ARM Cortex-M4. It is used for the majority of processing routines and optimization algorithms.
* **Specific Board:** [STM32F103C8T6 (Blue Pill)](https://www.st.com/en/microcontrollers-microprocessors/stm32f103c8.html) – Used for legacy or specific modules identified in the code headers.

---

## 📂 Repository Content

### 💾 Assembly Software (ARM Cortex-M)
Highly optimized routines following the **AAPCS** (Procedure Call Standard), focusing on cycle efficiency and memory management:
* **Vector Arithmetic:** 32-bit vector summation using post-indexed addressing modes.
* **Lightweight Cryptography:** *Xorshift* algorithm for pseudo-random number generation.
* **Bit Manipulation:** Efficient rotations (ROR/LSL) and logic operations utilizing the `S` suffix for flow control via **NZCV flags**.
* **Standardized Documentation:** All files include detailed comments on register usage (R0-R15) and status flag states (EQ, NE, etc.).

---

## 🚀 Quick Start Guide

1.  [cite_start]**Analog Simulation:** Before hardware assembly, verify the Quiescent point ($Q$), gain, and maximum symmetric excursion in LTspice[cite: 33, 34, 37, 40].
2.  **Software Deployment:**
    * Connect your **STM32F446RET6** using an ST-Link programmer.
    * Compile using the `arm-none-eabi-gcc` toolchain.
    * For **STM32F103** specific modules, check the individual file headers for compatibility notes.
