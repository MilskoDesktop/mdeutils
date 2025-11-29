include(MDEBase)
include(MDEOpenGL)
setup_project(mclock ${CMAKE_INSTALL_BINDIR})

opengl_add(mclock)
math_add(mclock)
