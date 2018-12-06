project('bfc', 'c',
  version: '0',
  default_options: [
    'c_std=gnu99',
  ],
  license: 'GPLv3',
)

cflags = [
  '-Wall',
  '-Wextra',
  '-Wshadow',
  '-Werror=implicit-function-declaration',
  '-Wstrict-prototypes'
]
add_project_arguments(cflags, language: 'c')

executable('bfc',
  'src/bfc.c',
  'src/compile.c',
  'src/file.c',
  'src/str.c',
)
