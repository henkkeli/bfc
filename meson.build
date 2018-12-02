project('bfc', 'c',
  version: '0',
  default_options: [
    'c_std=gnu99',
  ],
  license: 'GPLv3',
)

inc = include_directories(
  'src',
)

executable('bfc',
  'src/bfc.c',
  'src/compile.c',
  include_directories: inc
)
