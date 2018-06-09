# ruiji

## Description
Command that uploads a supported image format to http://iqdb.org,
parses the subsequent html for similar image results 
and prompt the user which one to download.

![Alt text](ruiji_screenshot.png?raw=true "ruiji")

## Dependencies
 - GNU Make
 - c library supporting c99
 - libcurl (preferably version >=7.51)
   - Must be built against either >=GnuTLS-3.3.24 or >=OpenSSL-1.1.0c
     - could work with other libraries (such as nss), but have not been tested

## Installation
First, download the source using either HTTP or SSH: 
```
$ git clone https://gitlab.com/Kamiyaa/ruiji.git
$ git clone git@gitlab.com:Kamiyaa/ruiji.git
```
Then install using **make**:
```
$ cd ruiji/src
$ make
# make install
```
Alternatively, you can install using **meson**:
```
$ meson build
$ ninja -C build/
# ninja -C build/ install
```

## Usage
```
$ ruiji --help

Usage: ruiji [options] [file]

  -h --help		 Show this help message and quit
  -q --quiet		 Suppress verbose output
  -t --threshold <0-100> Only process and show images above given
			 similarity percentage
  -T --tags		 Shows the tags associated with the image downloaded
  -v --version		 Show version number and quit
  -y --noprompt		 Skips user interactions and downloads
			 the most similar image
```

## Features
Currently supports:
 - http://www.theanimegallery.com/ 
 - http://danbooru.donmai.us/
 - http://e-shuushuu.net/
 - https://gelbooru.com/
 - https://konachan.com/
 - https://chan.sankakucomplex.com/
 - https://yande.re/
 - http://www.zerochan.net/

## Tips
This is how I usually use it when I have tons of pictures that I also would like to manually check
```
#!/bin/sh

for file
do
	ruiji -T -t 90 "$file" && gio trash "$file" && echo "removed $file"
	sleep 5		# enough time to rename/organize the newly downloaded file
done
```
