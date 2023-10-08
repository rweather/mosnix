
install(FILES
    dirent.h
    errno.h
    fcntl.h
    getopt.h
    syscall.h
    unistd.h
DESTINATION ${CMAKE_INSTALL_DATADIR}/mosnix/include)

add_subdirectory(bits)
add_subdirectory(sys)
