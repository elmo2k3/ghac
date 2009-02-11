project(ghac)

set(APPLICATION_NAME "ghac")
set(APPLICATION_VERSION "0.1")

cmake_minimum_required(VERSION 2.6)

include_directories(../libhac/)
include_directories(/usr/i586-mingw32msvc/include/gtk-2.0/)
include_directories(/usr/i586-mingw32msvc/include/glib-2.0/)
include_directories(/usr/i586-mingw32msvc/lib/glib-2.0/include/)
include_directories(/usr/i586-mingw32msvc/lib/gtk-2.0/include/)
include_directories(/usr/i586-mingw32msvc/include/cairo/)
include_directories(/usr/i586-mingw32msvc/include/pango-1.0/)
include_directories(/usr/i586-mingw32msvc/include/atk-1.0/)
link_directories(../libhac/build/win32/)
add_executable(ghac.exe ghac.c graph_view.c libhagraph.c data.c config.c)
target_link_libraries(libhac)
