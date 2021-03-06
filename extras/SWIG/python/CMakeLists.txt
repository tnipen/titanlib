find_package(PythonLibs REQUIRED)
include_directories(${PYTHON_INCLUDE_PATH})

set(CMAKE_SWIG_FLAGS "")
set_source_files_properties(../${PROJECT_NAME}.i PROPERTIES CPLUSPLUS ON)
include_directories(../../include)
#set_property(SOURCE foo.i PROPERTY SWIG_MODULE_NAME pytitanlib)


# Add swig module
#set(CMAKE_SWIG_FLAGS -interface _pytitanlib)
swig_add_library(titanlib TYPE SHARED LANGUAGE python SOURCES ../${PROJECT_NAME}.i)
swig_link_libraries(titanlib ${PROJECT_NAME} ${PYTHON_LIBRARIES})
if(APPLE)
    set_property(TARGET _titanlib PROPERTY SWIG_USE_TARGET_INCLUDE_DIRECTORIES ON)
    set_target_properties(_titanlib PROPERTIES
       SUFFIX ".so"
       INSTALL_RPATH"@loader_path;@loader_path/../../${PROJECT_NAME}/.libs"
       )
endif()
#set_property(TARGET _pytitanlib PROPERTY OUTPUT_NAME titanlib)

# Files to install with Python
set(PYTHON_INSTALL_FILES
        ${CMAKE_CURRENT_BINARY_DIR}/titanlib.py
        ${CMAKE_CURRENT_BINARY_DIR}/_titanlib.so)

# Configure setup.py and copy to output directory
set(SETUP_PY_IN ${CMAKE_CURRENT_SOURCE_DIR}/setup.in.py)
set(SETUP_PY_OUT ${CMAKE_CURRENT_BINARY_DIR}/setup.py)
configure_file(${SETUP_PY_IN} ${SETUP_PY_OUT})

#set(INIT_PY_IN ${CMAKE_CURRENT_SOURCE_DIR}/version.in.py)
#set(INIT_PY_OUT ${CMAKE_CURRENT_BINARY_DIR}/titanlib/__init__.py)
#configure_file(${INIT_PY_IN} ${INIT_PY_OUT})

# Install target to call setup.py
add_custom_target(install-python
        DEPENDS _titanlib
        COMMAND python3 ${SETUP_PY_OUT} install)

add_custom_target(install-python-user
        DEPENDS _titanlib
        COMMAND python3 ${SETUP_PY_OUT} install --user)

add_custom_target(build-python
        DEPENDS _titanlib
        COMMAND python3 ${SETUP_PY_OUT} build)

add_custom_target(develop-python
        DEPENDS _titanlib
        COMMAND python3 ${SETUP_PY_OUT} develop)

add_custom_target(develop-python-user
        DEPENDS _titanlib
        COMMAND python3 ${SETUP_PY_OUT} develop --user)

