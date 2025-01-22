# SubIT Audio and Video (SubAV) SDK

We came up with this idea because we wanted a fast and simple library that can deal with pretty much all kinds of media import and export in our **VRFZ** game project.

---

## The Xiao (OVC)

**JPEG** is quite good as an image format and there are indeed some free/open source implementations like **libjpeg**. However, We're not so satisfied with it because it is so 'compressed' that it's hard to implement it fast enough. So, we developed this image format based on **JPEG** and used some techniques to improve performance, which are ***Rotor SIMD DCT*** and ***IKP MaxFOG Coding***. The image format has its own name *"Xiao"*, which means *"Owl"* in Chinese. We use this name because owls have nice vision during the night and they are ferocious hunters. If you are an English speaker, you may prefer calling it "OVC(/ˈɑːvɪk/)", which means "Owl Vision Core(Container)".

We've also designed a "token-like" logo for it, which can also be regarded as a feature.

<img src = "owl_vision_poster.jpg"></img>

*You may not use this logo in case it might cause confusion.*

## The Fu (DAC)
It will be our audio format but we're still working on it.

## The Mi (MMC)
It will be our audio&video format but we're still working on it.

## How to use
It's easy to use this library.

If you look at *CMakeLists.txt* you will find out that the library is divided into two parts: "sbavcore" and "sbavtool".

"sbavcore" can be compiled into both static and dynamic (you have to modify CMakeLists.txt yourself) libraries and only depends on the C++ standard library.

"sbavtool" is used to generate "ovc", "dac" and "mmc" files. It's not strictly necessary, but it will make your life easier and can also be used as an example or something. It requires "FFmpeg" installed (and in your path) to run (It has a straightforward command-line interface), but the program itself only depends on the C++ standard library.

Both of the parts require **C++20** to build.

By the way, since we used some platform specific techniques, this library only supports **AMD64(Intel 64)** CPUs and **Windows, Linux** platforms currently. (We don't have **ARM** support yet, neither do we support **Android** or **MacOS**. It might work on Intel-based Macs but it's not tested and its performance is not gueranteed).

## Claims
I think that's already enough, but there are still some things I want to mention. 

Firstly, this project is licensed under the **MIT license**.

Secondly, I want to give my huge thanks to ***Steve Wang*** who developed the "IKP MaxFOG coding" in "The Xiao", which is a fascinating algorithm that combines **JIT** and a modified version of **Golomb coding**.

I also want to thank ***Xincheng Hao*** for his contribution to the artworks in **VRFZ**, which is our main motivation, and he is also the one who created the logo of the Xiao (but it's designed by me).

This project still needs help, so if you want to be a contributor, feel free to contact us.

# The End

***Henry Du 2024.12.28***

*Modified by Steve Wang on Jan 22, 2025*
