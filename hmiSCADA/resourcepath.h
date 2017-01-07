#ifndef RESOURCEPATH_H
#define RESOURCEPATH_H

#include <QString>

class ResourcePath
{
public:

    static void set(const QString newPath) {
        path(newPath + "/");
    }

    static QString get() {
        return path();
    }

private:

    static QString path(const QString resourcepath = "") {
        static ResourcePath rp;
        if(!resourcepath.isEmpty())
            rp.m_resource_path = resourcepath;

        return rp.m_resource_path;
    }

    QString m_resource_path;
};

#endif // RESOURCEPATH_H
