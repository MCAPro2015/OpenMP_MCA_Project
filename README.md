#
OpenMP_MCA_Project

Led by Sunita Chandrasekaran (University of Delaware) and Barbara Chapman (University of Houston/Stony Brook University)

Ph.D. Students: Peng Sun, Cheng Wang and
Master's Student: Suyang Zhu (Graudated in 2015 and currently with Microsoft) 

Contact: schandra@udel.edu for any questions

Multicore embedded systems are being widely used in telecommunication systems, robotics, medical applications and more. While such systems offer a high-performance with low-power solution, programming in an efficient way is still a challenge. In order to exploit the hardware potential of multicore embedded systems, software developers are still expected to handle many of the low-level details of programming including utilizing DMA, ensuring cache coherency, and inserting synchronization primitives explicitly; these issues are not only challenging to tackle but also time consuming and error-prone. Software portability is yet another issue. The state-of-the-art is towards using vendor-specific software toolchains that tightly couple the software to its designed hardware hence the portability is lost.

In this project we focus on developing a runtime system to explore mapping a high-level programming model, OpenMP, on to the multicore embedded systems. A key feature of our scheme is that unlike the existing approaches that usually rely on POSIX threads, our runtime leverages the Multicore Association (MCA) APIs as an OpenMP translation layer. The MCA API is a set of low-level API that handle resource management, inter-process communications and task scheduling. 

By deploying the MCA API, our runtime is able to capture the characteristics of multicore embedded systems more efficiently than pthreads. Furthermore, the MCA layer enables our runtime implementation to be portable cross various architectures; devices of more than one type. As a result, programmers only need to write to OpenMP code once but will be able to reuse the code for more than platform, thus improving productivity. 

We would like to acknowledge the Multicore Association, Freescale Semiconductor, Siemens and OpenMP for their continued support. 

The MCA-2.0.3 contains the referenced MCA implementation provided by the Multicore Association. 

The MTAPI folder contains the MTAPI implementation that follows the MTAPI specification 1.0.

The OpenMP-MTAPI RTL folder includes mapping of OpenMP to MTAPI RTL developing the OpenMP-MTAPI Runtime Library.

The libopenmp-OpenUH consists of mapping OpenMP runtime library onto MCA APIs, within the OpenUH compiler.

Current list of publications on this work is available in the 'Papers' folder. 
