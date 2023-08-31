
install(FILES
    errno.h
    syscall.h
    unistd.h
DESTINATION ${CMAKE_INSTALL_DATADIR}/mosnix/include)

add_subdirectory(bits)
add_subdirectory(mosnix)
add_subdirectory(sys)
