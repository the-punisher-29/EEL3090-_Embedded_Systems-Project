# Enhanced FreeRTOS for Embedded Machine Learning

This repository explores the enhancements required in FreeRTOS to support modern Embedded Machine Learning (ML) workloads effectively. The work spans theoretical concepts, practical modifications, and experimental validation using STM32 and TensorFlow Lite Micro.

---

## 📘 Contents

### 1. Background
- **1.1 The Current Embedded ML Landscape**  
  Overview of trends and current tools in embedded ML.
- **1.2 FreeRTOS Architecture and Limitations**
  - 1.2.1 Memory Management Limitations  
  - 1.2.2 Task Scheduling Constraints  
  - 1.2.3 Limited Resource Management  
  - 1.2.4 API Integration Challenges
- **1.3 Challenges of ML Workloads on Embedded Devices**  
  Discusses performance, memory, and latency issues in constrained environments.
- **1.4 Example Implementation Challenges**  
  Real-world issues encountered while prototyping ML tasks.
- **1.5 The Need for Enhanced FreeRTOS**  
  Motivation for system-level changes.

---

### 2. Understanding the Challenge
Summary of the core design problems and embedded ML bottlenecks.

---

### 3. Theoretical Implementation Framework
- **3.1 Memory Management Enhancements**
  - 3.1.1 Memory Pooling Implementation  
  - 3.1.2 Enhanced Heap_5 Implementation  
- **3.2 Task Scheduling Optimizations**
  - 3.2.1 Implementing an ML-aware EDF Scheduler  
  - 3.2.2 Mixed-Task Handler for ML Workloads  
- **3.3 API Specifications for ML Framework Integration**  
  Interfaces to support frameworks like TFLite Micro.

---

### 4. Practical Implementation
- **4.1 Setting Up the Development Environment**
  - 4.1.1 STM32 Environment Setup  
- **4.2 FreeRTOS Modifications for ML Workloads**  
- **4.3 TensorFlow Lite Micro Integration**  
- **4.4 Temperature Classification Implementation**  
  Simple ML use-case (e.g., classifying temperature ranges).
- **4.5 System Configuration and Setup**  
- **4.6 Results and Output**  
- **4.7 Challenges and Solutions**  
- **4.8 Methodologies That Could Be Used for Performance Comparison**  

---

## 🚀 Getting Started

1. Clone the repository
   ```bash
   git clone https://github.com/your-username/EEL3090-_Embedded_Systems-Project.git
   cd EEL3090-_Embedded_Systems-Project

## ℹ️ Additional Information

For detailed insights into the project, including theoretical background, implementation steps, and experimental results:

📄 **Open the report:** `EEL3090_Report.pdf`  
I have made the necessary changes in FreeRTOS to support ML workloads and successfully ran a machine learning task within the enhanced FreeRTOS environment.

