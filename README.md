# SFML-Basketball

SFML Basketball was one of my unversity Box2D project. 
The goal of this project is to show what kind of possible games to make with Physics Engine. 

Design and gameplay was not changed, but the code is updated to be more friendly than previous version. ".
Assets are mostly internet sourced, and they were used as "educational purposes". Please beware of copyright issues. 

Setup
---
1. Have SFML v2.5.1 Visual C++15 (2017) 64-bit Downloaded and place under C:/SFML-2.5.1-64-bit/. Make sure the include and bin folder path matches. 
- For SFML, you can download it here : https://www.sfml-dev.org/. 32-bit doesn't work with Box2D. Dll are in repo for easier linkage.
- Your path location should looks like this : 
  - SFML include location : C:/SFML-2.5.1-64-bit/include
  - SFML lib location : C:/SFML-2.5.1-64-bit/lib

2. Have Box2D downloaded : https://github.com/erincatto/Box2D and place it under C:/Box2D-master/. 
- I have included a Box2D.lib built along with the project. If you wish to rebuild, follow the Box2D github instruction and relink the lib in Solution Properties. 
