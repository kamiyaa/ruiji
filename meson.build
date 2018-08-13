project(
	'ruiji',
	'c',
	license: 'LGPL3',
	meson_version: '>=0.41.2',
	version: '0.7.0',
	default_options: ['warning_level=2'],
)

version = get_option('version')

add_project_arguments('-DVERSION="@0@"'.format(version), language: 'c')
if get_option('color')
        add_project_arguments('-DCOLOR', language: 'c')
endif

subdir('src')
subdir('man')
