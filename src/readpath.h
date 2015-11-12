/*********************************************************************
 *
 ********************************************************************/
 #ifndef _READPATH_H_
 #define _READPATH_H_

#include <string>

using namespace std;

template<class Handler>
class Readpath {
  public:
    Readpath(string &path, Handler *handler) :
      path_(path), handler_(handler) {}
    ~Readpath() {};

    int read() {
      DIR *dir;
      int error = 0;
      struct dirent entry;
      struct dirent *result = NULL;

      dir = opendir(path_.c_str());
      if (!dir) {
        return errno;
      }

       for (;;) {
        char *name;

        error = readdir_r(dir, &entry, &result);
        if (error != 0 || result == NULL) {
          break;
        }

        name = result->d_name;
        if ((strcmp(name, ".") == 0) || (strcmp(name, "..") == 0)) {
            continue;
        }

        handler_->onEntry(string(name));

      }
      return 0;
    }

  private:
    string path_;
    Handler *handler_;
};

#endif  // _READPATH_H_
