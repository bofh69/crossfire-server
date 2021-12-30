#include <Qt>
#include <QApplication>
#include <QCoreApplication>
#include <QTemporaryDir>

#include <CREMainWindow.h>
#include "CRESettings.h"

#include "CREPixmap.h"

#include "CREFilterDefinition.h"
#include "CREFilterDefinitionManager.h"
#include "CREReportDefinition.h"
#include "CREReportDefinitionManager.h"

#ifdef WIN32
extern "C" {
#include "global.h"
}
#endif

int main(int argc, char **argv) {
    QCoreApplication::setOrganizationName("The Legendary Team of Ailesse");
    QCoreApplication::setApplicationName("CRE");
    QApplication app(argc, argv);
    app.setWindowIcon(QIcon(":resources/app_icon.png"));

    qRegisterMetaTypeStreamOperators<CREFilterDefinition>("CREFilterDefinition");
    qRegisterMetaTypeStreamOperators<CREFilterDefinitionManager>("CREFilterDefinitionManager");
    qRegisterMetaTypeStreamOperators<CREReportDefinition>("CREReportDefinition");
    qRegisterMetaTypeStreamOperators<CREReportDefinitionManager>("CREReportDefinitionManager");

    qRegisterMetaType<const Face *>("const Face*");
    qRegisterMetaType<const Face *>("const treasurelist*");
    qRegisterMetaType<const Face *>("const archetype*");

    // QHelpEngine apparently needs write access to files, so move'em to a writable location
    QTemporaryDir forHelp;
    QFile::copy(app.applicationDirPath() + "/cre.qch", forHelp.path() + "/cre.qch");
    QFile::copy(app.applicationDirPath() + "/cre.qhc", forHelp.path() + "/cre.qhc");

#ifdef WIN32
    // Application compiled with "--prefix=", somehow the root is then c:\, which is bad
    // So force directories to point to the exe's path and something else.
    std::string confdir = app.applicationDirPath().toStdString() + "/etc";
    settings.confdir = confdir.c_str();
    std::string localdir = app.applicationDirPath().toStdString() + "/var";
    settings.localdir = localdir.c_str();
    std::string datadir = app.applicationDirPath().toStdString() + "/share";
    settings.datadir = datadir.c_str();
    std::string temp = QDir::tempPath().toStdString();
    settings.tmpdir = temp.c_str();
    // Force log level to be debug to see all output
    settings.debug = llevDebug;
    // Set a log file
    std::string log = (QDir::tempPath() + "\\cre.log").toStdString();
    settings.logfilename = log.c_str();
#endif

    CRESettings settings;
    if (!settings.ensureOptions())
        return -1;

    CREMainWindow win(forHelp.path());

    CREPixmap::init();

    win.show();

    return app.exec();
}
