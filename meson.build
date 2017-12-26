project(
	'ruiji',
	'c',
	license: 'LGPL3',
	meson_version: '>0.40.0',
	version: '0.6',
	default_options: ['c_std=c99', 'warning_level=2'],
)

if get_option('enable_color')
        add_project_arguments('-DCOLOR', language: 'c')
endif

subdir('src')
