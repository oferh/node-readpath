# Node readpath

[![Build Status](https://travis-ci.org/oferh/node-readpath.svg?branch=master)](https://travis-ci.org/oferh/node-readpath)

fs.readdir alternative that calls a callback per entry instead of returning an array with all entries.

readpath uses POSIX readdir to read the contents of the directory instead of using scandir like fs.readdir is doing.

## Installation

```
$ npm install readpath
```

## Read directory Asych


```node
var readpath = require('../');

readpath.read('my-path',
  function(entry) {
    console.log(entry);
  },
  function(err) {
    console.log("Error: " + err);
  }
);

```
