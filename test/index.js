var readpath = require('../'),
  assert = require('assert'),
  rmdir = require( 'rmdir' ),
  async = require('async'),
  fs = require('fs');

var TEST_PATH = './.readpath-test';

describe('readpath', function(done) {
  var testFilesNum = Math.floor(Math.random() * 1000) + 1;

  before('create test folders and files', function(cb) {
    async.series([
      function(callback){
        rmdir(TEST_PATH, function(err) {
          var ret = err;
          if (err && err.code === 'ENOENT'){
            ret = null;
          }
          callback(ret);
        });
      },
      function(callback){
        fs.mkdir(TEST_PATH,callback);
      },
      function(callback) {
        //Create files in the test dir
        for (var fileNum = testFilesNum; fileNum>0; fileNum--) {
          fs.openSync(TEST_PATH + '/' + fileNum, 'w');
        }
        callback();
      }],
      function(err){
        cb(err);
      }
    );
  });

  it('should find all files', function(done) {
    var files = {},
      filesNum = 0;
    readpath.read(TEST_PATH,
      function(file) {
        assert.equal(files.hasOwnProperty(file), false, 'file was already found');
        files[file] = 1;
        filesNum++;
      },
      function(err) {
        assert.equal(filesNum, testFilesNum, err);
        done();
    });

  });

  it('should return error when the path is not found', function(done) {
    readpath.read('.no-such-path',
      function(file) {
        assert(false, 'found files for not existing folder');
      },
      function(err) {
        assert.equal(err,'Error: No such file or directory');
        done();
    });
  });


  after(function(cb) {
    rmdir(TEST_PATH, cb);    
  });
});
