# mandarin-vm
This is the mandarin virtual machine. The main purpose of creating this virtual machine was a replace client-side stack of HTML-CSS-Javascript in order to provide better performance, security and capabilities of web applications. The core design is own binary bytecode for RISC-like virtual machine, designed through evolution to be fast and secure.
  
  
### Why not LLVM? 
LLVM is very good. But it doesn't provide an easy solution for a secure execution. Also theoretically there could be the cases when simple applications like text-view pages (biggest part of current Web), will require more time (and energy) for translating IR assembly into native binary and subsequent execution than a just virtual execution of mandarin bytecode. Moreover, I just wanted to invent my own instruction set :)

### Design features: 
1. Risc-like virtual machine.
2. API for calling external functions with "standard" list, like OpenGL, JPEG/PNG decompressors, x264 video decoder and such things to provide standard web browser functional.

### Current status:
VM itself is almost complete, need polish and work on couple of instructions.
API in very pre-alfa state, only having jpeg decompression and some tools for 2D painting. Require alot of work.
Project is on conservation until next inspiration wave.
