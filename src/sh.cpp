//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.

#include "sh.hpp"
#include <core.hpp>
#include <wikiedit.hpp>
#include <wikisite.hpp>
#include <wikipage.hpp>
#include <syslog.hpp>
#include <QtXml>

using namespace Huggle;

long long splitter_hits = 0;
long long splitter_misses = 0;

huggle_splitter::huggle_splitter()
{

}

huggle_splitter::~huggle_splitter()
{

}

bool huggle_splitter::Register()
{
    Huggle::Core::HuggleCore = (Huggle::Core*) this->HuggleCore;
    //Huggle::QueryPool::HugglePool = Huggle::Core::HuggleCore->HGQP;
    //Huggle::Localizations::HuggleLocalizations = (Huggle::Localizations*) this->Localization;
    Huggle::Syslog::HuggleLogs = Huggle::Core::HuggleCore->HuggleSyslog;
    //Huggle::GC::gc = Huggle::Core::HuggleCore->gc;
    //Huggle::Query::NetworkManager = this->Networking;
    //Huggle::Configuration::HuggleConfiguration = (Huggle::Configuration*) this->Configuration;
    return true;
}

bool huggle_splitter::IsWorking()
{
    return true;
}

static bool process(QString *new_text, QString *old_text, QDomElement e)
{
    QString rc = e.attribute("class");
    if (rc.isEmpty())
    {
        return false;
    }
    if (rc == "diff-context" || rc == "diff-marker" || rc == "diff-lineno" || rc == "diff-empty")
    {
        // this can be safely ignored it's not needed for us
        return true;
    } else if (rc == "diff-deletedline")
    {
        // line was deleted, so it's present only in old text
        *old_text += e.text() + "\n";
        return true;
    } else if (rc == "diff-addedline")
    {
        *new_text += e.text() + "\n";
        return true;
    }
    return false;
}

bool huggle_splitter::Hook_EditBeforeScore(void *edit)
{
    // this function takes the horrid html diff as provided by mediawiki and split it into new and old text
    WikiEdit *e = (WikiEdit*)edit;
    if (e->DiffText_IsSplit)
        return true;
    QDomDocument page;
    QString xml = "<diff>" + e->DiffText + "</diff>";
    page.setContent(xml);
    QDomNodeList text = page.elementsByTagName("tr");
    int n = 0;
    QString text_old = "";
    QString text_new = "";
    bool success = false;
    while (n < text.count())
    {
        QDomElement element = text.at(n++).toElement();
        // there should be exactly 2 or 4 columns in every row, first one contains old text
        // second one new
        QDomNodeList rows = element.elementsByTagName("td");
        if (rows.count() == 2)
            continue;
        if (rows.count() < 3 || rows.count() > 4)
        {
            Huggle::Syslog::HuggleLogs->DebugLog("Invalid diff (too many cols): " + e->DiffText);
            splitter_misses++;
            return true;
        }
        int t = 0;
        while (t < rows.count())
        {
            if (process(&text_new, &text_old, rows.at(t++).toElement()))
            {
                success = true;
            } else
            {
                Huggle::Syslog::HuggleLogs->DebugLog("Invalid diff (wrong class): " + e->DiffText);
                splitter_misses++;
                return true;
            }
        }
    }
    if (!success)
    {
        Syslog::HuggleLogs->DebugLog("Unable to split diff: " + e->DiffText, 2);
        splitter_misses++;
        return true;
    }
    e->DiffText_Old = text_old;
    e->DiffText_IsSplit = true;
    e->DiffText_New = text_new;
    splitter_hits++;
    return true;
}

void huggle_splitter::Hook_Shutdown()
{
    Syslog::HuggleLogs->DebugLog("Splitter hits/misses: " + QString::number(splitter_hits) + "/" + QString::number(splitter_misses));
}

#if QT_VERSION < 0x050000
    Q_EXPORT_PLUGIN2("org.huggle.extension.qt", huggle_splitter)
#endif


