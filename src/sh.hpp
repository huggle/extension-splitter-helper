//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.

#ifndef THANKS_H
#define THANKS_H

#define HUGGLE_EXTENSION
#include <iextension.hpp>
#include <QAction>
#include <QDeclarativeItem>

namespace Huggle
{
    class WikiPage;
    class WikiEdit;
    class WikiSite;
}

class huggle_splitter : public QDeclarativeItem, public Huggle::iExtension
{
    Q_OBJECT
#if QT_VERSION >= 0x050000
        Q_PLUGIN_METADATA(IID "org.huggle.extension.qt" FILE "splitter.json")
#endif
    Q_INTERFACES(Huggle::iExtension)
    public:
        static bool WikiCk(Huggle::WikiSite *site);

        huggle_splitter();
        ~huggle_splitter();
        bool Register();
        bool IsWorking();
        QString GetExtensionName() { return "Splitter"; }
        QString GetExtensionAuthor() { return "Petr Bena"; }
        QString GetExtensionDescription() { return "Splits a diff into new text and old text"; }
        QString GetExtensionVersion() { return "1.0.0"; }
        bool RequestCore() { return true; }
        bool RequestNetwork() { return true; }
        bool RequestConfiguration() { return true; }
    private:
};

QML_DECLARE_TYPE(huggle_splitter)

#endif // THANKS_H
