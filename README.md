# ruiji

## Description
Command that uploads a supported image format to http://iqdb.org,
parses the subsequent html for similar image results 
and prompt the user which one to download.

![Alt text](ruiji_screenshot.png?raw=true "ruiji")

## Dependencies
 - GNU Make
 - c library supporting: gnu99
 - libcurl (preferably version >=7.51)
   - Must be built against either >=GnuTLS-3.3.24 or >=OpenSSL-1.1.0c
     - could work with other libraries (such as nss), but have not been tested

## Installation
First, download the source using either HTTP or SSH: 
```
$ git clone https://github.com/Kamiyaa/ruiji.git
$ git clone git@github.com:Kamiyaa/ruiji.git
```
Then install:
```
$ cd ruiji/src
$ make
# make install
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
  -v --verbose		 Show verbose output
  -V --version		 Show version number and quit
  -y --noprompt		 Skips user interactions and downloads
			 the most similar image
```

## Features
Currently supports:
 - http://danbooru.donmai.us/
 - http://e-shuushuu.net/
 - https://gelbooru.com/
 - https://konachan.com/
 - https://chan.sankakucomplex.com/
 - https://yande.re/
 - http://www.zerochan.net/ **

_\*\* currently broken as zerochan redirects to registry page._
