include(MDEBase)
setup_project(mauplay ${CMAKE_INSTALL_BINDIR})

target_link_libraries(
	mauplay
	PRIVATE
	MDEAudio
)

find_package(PkgConfig)
