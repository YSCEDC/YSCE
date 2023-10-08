# YSFlight Community Edition (YSCE)

This repository is home to the YSFlight Community Edition fork of YSFlight. If you are interested in joining the project, please email Current Repository maintainer Nodoka Hanamura at nodo-at-kahanamura.com (temporary) or contact any of the current YSCEDC contributors on YSFlight Headquarters (https://www.ysfhq.com/).

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

To compile YSCE at this time, please follow the instructions below as prepared for the original YSFlight repository by Soji Yamakawa.

## Compile Instruction
Prerequisite is a compiler and cmake.

YSFLIGHT requires my public libraries to build.  Follow the steps below:

```
(In your working directory)
git clone https://github.com/YSCEDC/ysce_public.git
git clone https://github.com/YSCEDC/YSCE.git
cd YSCE
mkdir build
cd build
cmake ../src
cmake --build . --config Release --parallel
```
