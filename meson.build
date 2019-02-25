project('Dab', ['cpp', 'c'], default_options : ['cpp_std=c++14'])

if meson.is_subproject() == false
    # disable this bogus warning (should be deprecated in newer clang versions)
    add_global_arguments('-Wno-missing-braces', '-fno-exceptions', language: 'cpp')
else
    message('Building Dab as subproject.')
endif

install_headers('Dab/Dab.hpp', subdir: 'Pic')
install_headers('Dab/OpenGL/GLDab.hpp', subdir: 'Dab/OpenGL')
install_headers('Dab/Libs/GL/gl3w.h', subdir: 'Dab/Libs/GL')

dabSrc = [
    'Dab/Dab.cpp',
    'Dab/OpenGL/GLDab.cpp',
    'Dab/Libs/GL/gl3w.c'
]

cc = meson.get_compiler('cpp')

incDirs = include_directories('.', 'Dab/Libs')

dabDeps = [cc.find_library('Stick', required: true), dependency('GL')]

# if host_machine.system() == 'darwin'
#     extraCppArgs += '-DGL_SILENCE_DEPRECATION'
# endif

dab = library('Dab', dabSrc, dependencies: dabDeps, include_directories : incDirs, install: true)

dabDep = declare_dependency(link_with : dab, 
    dependencies: dabDeps, 
    include_directories: incDirs)

subdir('Examples')
