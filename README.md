# Node readpath: Read the contents of a directory using callbacks

fs.readdir alternative that calls a callback per entry instead of returning an array with all entries.

readpath uses POSIX readdir to read the contents of the directory instead of using scandir like fs.readdir is doing.

## Installation

Clone the repository to a local folder and run

```
$ npm install
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
