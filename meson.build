project('bfc', 'c',
  version: '0',
  default_options: [
    'c_std=gnu99',
  ],
  license: 'GPLv3',
)

incdir = include_directories('src')

cflags = [
  '-Wall',
  '-Wextra',
  '-Wshadow',
  '-Werror=implicit-function-declaration',
  '-Wstrict-prototypes'
]
add_project_arguments(cflags, language: 'c')

executable('bfc',
  'src/arch.c',
  'src/bfc.c',
  'src/compile.c',
  'src/file.c',
  'src/str.c',
  'src/arch/amd64.c',
  'src/arch/armv6l.c',
  include_directories: incdir
)

check = dependency('check', required: false)

if check.found()
  test_exe = executable('check',
    'test/check.c',
    'src/arch.c',
    'src/compile.c',
    'src/file.c',
    'src/str.c',
    'src/arch/amd64.c',
    'src/arch/armv6l.c',
    dependencies: check,
    include_directories: incdir
  )

  test('check', test_exe)
else
  message('Install check for unit tests')
endif
