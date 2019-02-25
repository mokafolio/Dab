helloTriangle = executable('HelloTriangle', 'HelloTriangle.cpp', 
    dependencies: [dabDep, cc.find_library('Luke', required: true), dependency('sdl2')], 
    cpp_args : ['-fsanitize=address'],
    link_args : '-fsanitize=address')