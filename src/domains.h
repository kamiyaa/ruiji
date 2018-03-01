#ifndef DOMAINS_H
#define DOMAINS_H

#include "animegallery.h"
#include "animepictures.h"
#include "danbooru.h"
#include "eshuushuu.h"
#include "gelbooru.h"
#include "konachan.h"
#include "mangadrawing.h"
#include "sankakucomplex.h"
#include "yandere.h"
#include "zerochan.h"

#define ANIMEGALLERY_DOMAIN	"www.theanimegallery.com"
#define ANIMEPICTURES_DOMAIN	"anime-pictures.net"
#define DANBOORU_DOMAIN		"danbooru.donmai.us"
#define ESHUUSHUU_DOMAIN	"e-shuushuu.net"
#define GELBOORU_DOMAIN		"gelbooru.com"
#define KONACHAN_DOMAIN		"konachan.com"
#define MANGADRAWING_DOMAIN	"mangadrawing.net"
#define SANKAKUCOMPLEX_DOMAIN	"chan.sankakucomplex.com"
#define YANDERE_DOMAIN		"yande.re"
#define ZEROCHAN_DOMAIN		"www.zerochan.net"

/* enum used to identify domains */
enum domain_t {
	danbooru,
	yandere,
	sankakucomplex,
	konachan,
	gelbooru,
	eshuushuu,
	zerochan,
	animepictures,
	animegallery,
	mangadrawing
};

#endif
