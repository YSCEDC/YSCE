# YSFlight Community Edition (YSCE)

This repository is home to the YSFlight Community Edition fork of YSFlight. Here I have added compiled downloads to help those who couldn't compile it!

# DISCLAIMER

This repository, nor the YSCEDC (YSFlight Community Edition Development Committee) is not supported, funded, endorsed or otherwise approved by Soji Yamakawa. YSCE Intends solely to be a modernization and revision of YSFlight whilst retaining its' ease of access (in regards to low end hardware) and ease of use (in regards to knowledge needed to play). YSCEDC is not administrated, maintained or otherwise under the authority of any community website, group or other organization. YSCE is intended to be a greater community project to revise the platform for the benefit of all members of the YSFlight community regardless of nationality or other status.

YSCEDC claims no responsibility or liability for use of this code and compiled software by the end user and is provided AS-IS WITH NO WARRANTY WHATSOEVER.

YSCEDC encourages all end users to report any bugs or issues to the YSCE Repository Issues section.

Please do not issue bug reports relating to YSCE to the main YSFlight repository unless you can replicate issues with the stock YSFlight version.

Current YSCEDC Contributors (Code, Design, Advisory):
* Violet 
* Decaff42
* Nodoka Hanamura
* Krux
* Ace Lord
* HawkbitAlpha
* Kubson
* Vertex
* Doomsday
* Indy
* Hornet
* Skipper
* pasutisu (Mofumofu)
* JOBBIN

If you prefer compiling YSCE yourself (the downloads are much easier), here are the steps:

## Compile Instruction
Prequisites are:
* CMake
* Any CMake-compatible compiler
* The YSCE Public Libraries (Fork of YSFLIGHT Public Libraries)

Open your preferred terminal client and input the following commands in your build area (This is for Linux only):

```
git clone https://github.com/YSCEDC/ysce_public.git
git clone https://github.com/YSCEDC/YSCE.git
cd YSCE
mkdir build
cd build
cmake ../src
cmake --build . --config Release --parallel
```
