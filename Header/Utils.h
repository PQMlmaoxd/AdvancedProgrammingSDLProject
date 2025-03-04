#ifndef UTILS_H
#define UTILS_H

#ifdef _WIN32
    #include <direct.h>  // Windows: _mkdir
#else
    #include <sys/stat.h>  // Linux/macOS: mkdir
    #include <sys/types.h>
#endif

inline void createSaveDirectory() {
    #ifdef _WIN32
        _mkdir("Save");
    #else
        mkdir("Save", 0777);
    #endif
}

#endif // UTILS_H
