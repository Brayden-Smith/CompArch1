# Pipelined Processor Simulator

This project is a cycle-by-cycle simulator for a simplified pipelined RISC-V processor, created as part of my coursework in computer architecture. The simulator steps through a series of RISC-V instructions, simulating how they would move through a processor pipeline, handling aspects like hazards, branch instructions, and register updates along the way.

## About the Project

The simulator demonstrates my understanding of a classic pipelined architecture and includes several core features:

- **Instruction Fetch & Decode**: Fetches up to two instructions per cycle, managing stalls when branches or queue limits are encountered.
- **Issue & Execution**: Implements a basic scoreboard to track register and memory dependencies, issuing instructions when operands are ready and no hazards are present.
- **Memory Access & Write-Back**: Supports memory load/store and updates registers once instructions are complete, simulating the flow of data through a processor pipeline.

## How It Works

This simulator is written in C++ and outputs a detailed trace of each cycle, showing the states of the pipeline’s registers, memory, and queues. I worked to make it as close to cycle-accurate as possible for a single-threaded simulation.

The pipeline has these main stages:
- **IF** (Instruction Fetch/Decode)
- **Issue** (Handles operand dependencies and issues ready instructions)
- **ALU1/ALU2** (Separate paths for memory and non-memory instructions)
- **MEM** (Executes load and store instructions)
- **WB** (Write-Back, finalizing data updates)

This setup allowed me to dive deep into how processors handle multiple instructions at once, manage resource conflicts, and maintain efficiency despite limitations.

## Highlights

- **Hazard Detection**: The simulator detects and handles RAW, WAR, and WAW hazards to ensure accurate results.
- **Branch Prediction**: Accounts for branch instructions (`jal`, `beq`, `bne`, `blt`) to keep instruction flow smooth.
- **Cycle-by-Cycle Trace**: Prints a detailed log after each cycle to visualize how instructions progress through the pipeline.

## Why I Built This

This project was a great hands-on experience to reinforce my learning in computer architecture. It required thinking through the inner workings of a processor, including parallelism, resource management, and timing challenges. Working on this helped deepen my understanding of how real-world processors balance efficiency with complexity in a pipelined design.
