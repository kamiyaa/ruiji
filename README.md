# ruiji

## Description
Command that uploads a supported image to http://iqdb.org,
parses the html after uploading and get a list of the images similar to it.

![Alt text](ruiji_screenshot.png?raw=true "ruiji")

## Dependencies
 - c library supporting: gnu99
 - libcurl (preferably version >=7.51)

## Installation
First, download the source: 
```
# for HTTP
git clone https://github.com/Kamiyaa/ruiji.git
# for SSH
git clone git@github.com:Kamiyaa/ruiji.git
```
Then install:
```
$ cd ruiji/src
$ make
# make install
```

## Usage
```
$ ruiji <source image>
```

## Features
Currently supports:
 - http://danbooru.donmai.us/
 - https://chan.sankakucomplex.com/

### Todo
Add support for:
 - https://yande.re/
 - http://www.zerochan.net/
 - http://konachan.com/
 - http://e-shuushuu.net/

 - Add a better interface (?)
 - Preview support (?)

