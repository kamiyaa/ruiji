# update config.h
run_command('cp',
	['-u', 'config.def.h', 'config.h']
	)

curl_dep = dependency('libcurl', version: '>=7.51.0')

lib_domains = static_library(
	'domains',
	['animegallery.c', 'danbooru.c' , 'eshuushuu.c',
	'gelbooru.c', 'konachan.c', 'mangadrawing.c',
	'sankakucomplex.c', 'yandere.c' , 'zerochan.c']
	)
lib_helpers = static_library(
	'helpers',
	['interface.c', 'parser.c', 'udload.c']
	)

executable('ruiji',
	'ruiji.c',
	dependencies : curl_dep,
	link_with : [lib_domains, lib_helpers],
	install: true
	)
