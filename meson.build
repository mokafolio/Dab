project('Dab', ['cpp', 'c'], default_options : ['cpp_std=c++14'])

if meson.is_subproject() == false
    # disable this bogus warning (should be deprecated in newer clang versions)
    add_global_arguments('-Wno-missing-braces', '-fno-exceptions', language: 'cpp')
else
    message('Building Dab as subproject.')
endif

# to silence gl deprecation warnings on osx
if host_machine.system() == 'darwin'
    add_project_arguments('-DGL_SILENCE_DEPRECATION', language: ['cpp', 'c'])
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

cc = meson.get_compiler('cpp')
dabDeps = []
dabExampleDeps = []
if get_option('buildSubmodules') == false
    dabDeps += [cc.find_library('Stick'), dependency('threads')]
    dabExampleDeps += [cc.find_library('Luke', required: true), dependency('sdl2')]
else
    stickProj = subproject('Stick')
    dabDeps += stickProj.get_variable('stickDep')
    lukeProj = subproject('Luke')
    dabExampleDeps += lukeProj.get_variable('lukeDep')
endif

dabDeps += dependency('GL')

dab = library('Dab', dabSrc, dependencies: dabDeps, include_directories : incDirs, install: true)

dabDep = declare_dependency(link_with : dab, 
    dependencies: dabDeps, 
    include_directories: incDirs)

if get_option('buildExamples') == true
    subdir('Examples')
endif
