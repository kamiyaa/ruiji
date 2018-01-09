project(
	'ruiji',
	'c',
	license: 'LGPL3',
	meson_version: '>=0.41.2',
	version: '0.7.0',
	default_options: ['c_std=c99', 'warning_level=2'],
)

if get_option('enable_color')
        add_project_arguments('-DCOLOR', language: 'c')
endif

subdir('src')
