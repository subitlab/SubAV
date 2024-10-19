SubIT Audio and Video format (SubAV)

In game industry and its relatives, an awesome CG can often catches players' eyes. However, even though there are a variety of famous container formats we can often achieve on our devices such as avi, mp4, mov for us to use, they have some problems we can not overcome. The first one is they are not open-source (yes mp4 is not); and the second one is they are not suitable for game industry and its relatives (videos used in these areas requires high performance).There are indeed some open-source formats like ogg-video and matroska(mkv) but ogg is a little bit old (the last official release was at 2005) and matroska's decoding can be very complicated (I don't actually have interests to its documentation).In the past, most big companies use "Bink-video" as there CG format, Bink is a fast, small and high quality format which is just designed for games, but, bink is not open-source and could be expensive for commercial usage.

So, after crticizing all formats above, I decided to design our very own audio-video format which is "SubAV", this format views Bink as its goal but faster and simpler. The most importantly, I don't like technique privacy so this format is also open-sourced under the "MIT-License".

This repositroy now contains a very simple interchange format -- SbYUV and its SbYUVTool to do convertions. For simplicity, just include "SbYUV.hpp" and "SbYUV.cpp" into your own projects, then you can start quickly following the comments or samples.

This project is still in-developed, so more and more features will be added in the future, I hope it can make some difference in (at least our own in SubIT) game development.

HenryDu 2024.10.19