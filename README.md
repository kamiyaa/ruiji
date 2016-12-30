# ruiji

## Description
Command that uploads a supported image format to http://iqdb.org,
parses the html after uploading and get a list of the images similar to it.

![Alt text](ruiji_screenshot.png?raw=true "ruiji")

## Dependencies
 - GNU Make
 - c library supporting: gnu99
 - libcurl
   - preferably version >=7.51
   - Must be built against either >=GnuTLS-3.4.17 or >=OpenSSL-1.1.0c

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
$ ruiji </path/to/image>
```

## Features
Currently supports:
 - http://danbooru.donmai.us/
 - http://e-shuushuu.net/
 - https://konachan.com/
 - https://chan.sankakucomplex.com/
 - http://www.zerochan.net/ **

### Todo
 - support for: https://yande.re/
 - Clean code
  - excess code reuse
  - hardcoded values
 - Config file support*
 - Add a better interface*
 - Preview support*

_\* possibly_

_\*\* currently broken as zerochan redirects to registry page._
