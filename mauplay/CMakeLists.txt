include(MDEBase)
include(MDEPkgConfig)
setup_project(mauplay ${CMAKE_INSTALL_BINDIR})

pkg_config_add(mauplay sqlite3)
pkg_config_add(mauplay sndfile)
pkg_config_add(mauplay samplerate)

target_link_libraries(
	mauplay
	PRIVATE
	MDEAudio
)
