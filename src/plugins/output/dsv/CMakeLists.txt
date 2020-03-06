# Create a linkable module
add_library(dsv-output MODULE
        src/dsv.cpp
        src/Config.cpp
        src/Config.hpp
        src/Storage.cpp
        src/Storage.hpp
        src/Printer.cpp
        src/Printer.hpp
        src/File.cpp
        src/File.hpp
        src/Server.cpp
        src/Server.hpp
        src/Sender.cpp
        src/Sender.hpp
        )

find_package(ZLIB REQUIRED)
include_directories(${ZLIB_INCLUDE_DIRS})
target_link_libraries(dsv-output ${ZLIB_LIBRARIES})

install(
        TARGETS dsv-output
        LIBRARY DESTINATION "${INSTALL_DIR_LIB}/ipfixcol2/"
)

if (ENABLE_DOC_MANPAGE)
    # Build a manual page
    set(SRC_FILE "${CMAKE_CURRENT_SOURCE_DIR}/doc/ipfixcol2-dsv-output.7.rst")
    set(DST_FILE "${CMAKE_CURRENT_BINARY_DIR}/ipfixcol2-dsv-output.7")

    add_custom_command(TARGET dsv-output PRE_BUILD
            COMMAND ${RST2MAN_EXECUTABLE} --syntax-highlight=none ${SRC_FILE} ${DST_FILE}
            DEPENDS ${SRC_FILE}
            VERBATIM
            )

    install(
            FILES "${DST_FILE}"
            DESTINATION "${INSTALL_DIR_MAN}/man7"
    )
endif ()