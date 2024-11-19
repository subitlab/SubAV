SubIT Audio and Video (SubAV) SDK

In game industry and its relatives we often want a av format to store our CGs in high-speed, high-quality, low-size and also controlable. 
However, formats such as MP4, AVI, MOV are closed-source and OGG, MKV are not suit for games. 'Bink' is a good choice, but as I have mentioned,
it's closed-source and could be expensive. 

So I managed to make this 'SubAV-SDK' which contains a standalone image format (sbsi) that can be used for textures inside our games, a binary interchange
yuv (sbrfs) as a replacement of YUV4MPEG, a small audio format (sbsw -- still in develop) and a combination of them all (sbav -- still in develop). All these
stuff can be found inside "AVCore" folder.

There is also a tool that can easily generate or playback these formats, it's called 'sbavtool' and sources are included in "AVTool" folder. Sources inside "AVTool"
can also be samples to show you how to intergrate this SDK into your own game project.

You don't need any extra dependencies to build "sbavcore" which is the core part of the SDK.
However, you need "VulkanSDK" and "FFmpeg" installed on your computer and "glfw" cloned into the project folder to build "sbavtool".

If you need to intergrate this SDK into your own project, just copy sources and "NOT USE CMAKE", which I think could be very heavy and
I also didn't managed to adapt it.

HenryDu 2024.11.9