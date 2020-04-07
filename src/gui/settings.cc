// Copyright (c) 2015, Joseph Mirabel
// Authors: Joseph Mirabel (joseph.mirabel@laas.fr)
//
// This file is part of gepetto-viewer.
// gepetto-viewer is free software: you can redistribute it
// and/or modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation, either version
// 3 of the License, or (at your option) any later version.
//
// gepetto-viewer is distributed in the hope that it will be
// useful, but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Lesser Public License for more details.  You should have
// received a copy of the GNU Lesser General Public License along with
// gepetto-viewer. If not, see <http://www.gnu.org/licenses/>.

#include <gepetto/gui/settings.hh>

#include <QSettings>
#include <QDir>
#include <QtDebug>
#include <QStyleFactory>

#include <osg/DisplaySettings>
#include <osg/ArgumentParser>

#include <gepetto/gui/dialog/dialogloadrobot.hh>
#include <gepetto/gui/dialog/dialogloadenvironment.hh>
#include <gepetto/gui/mainwindow.hh>

#if GEPETTO_GUI_HAS_PYTHONQT
# include <gepetto/gui/pythonwidget.hh>
#endif

namespace gepetto {
  namespace gui {
    Settings::Settings (const char* installDir)
      : configurationFile ("settings")
      , predifinedRobotConf ("robots")
      , predifinedEnvConf ("environments")
      , stateConf (".state")
      , verbose (false)
      , noPlugin (false)
      , useNameService (false)
      , refreshRate (30)
      , captureDirectory ()
      , captureFilename ("screenshot")
      , captureExtension ("png")
      , avconv (AVCONV)
      , installDirectory (installDir)
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
      , appStyle ("cleanlooks")
#else
      , appStyle ("fusion")
#endif

      , mw (0)
    {
      QDir user (QDir::home());
      const char path[] = "Pictures/gepetto-gui";
      user.mkpath (path);
      user.cd (path);
      captureDirectory = user.absolutePath().toStdString();

      avConvInputOptions
            << "-r" << "25";
      avConvOutputOptions
            << "-vf" << "scale=trunc(iw/2)*2:trunc(ih/2)*2"
            << "-r" << "25"
            << "-vcodec" << "libx264";
    }

    void Settings::setupPaths () const
    {
      QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
      if (env.contains ("GEPETTO_GUI_SETTINGS_DIR")) {
        QSettings::setPath(QSettings::IniFormat,
            QSettings::SystemScope, env.value("GEPETTO_GUI_SETTINGS_DIR"));
        QSettings::setPath(QSettings::NativeFormat,
            QSettings::SystemScope, env.value("GEPETTO_GUI_SETTINGS_DIR"));
      } else {
        QSettings::setPath(QSettings::IniFormat,
            QSettings::SystemScope, installDirectory + "/etc");
        QSettings::setPath(QSettings::NativeFormat,
            QSettings::SystemScope, installDirectory + "/etc");
      }

      PluginManager::addPluginDir (CMAKE_INSTALL_PREFIX "/lib/gepetto-gui-plugins");
      if (env.contains ("GEPETTO_GUI_PLUGIN_DIRS")) {
        foreach (QString p, env.value("GEPETTO_GUI_PLUGIN_DIRS").split(':')) {
          PluginManager::addPluginDir (p + "/gepetto-gui-plugins");
        }
      }
      foreach (QString p, env.value("LD_LIBRARY_PATH").split(':')) {
        PluginManager::addPluginDir (p + "/gepetto-gui-plugins");
      }
    }

    int Settings::initSettings (int argc, char * argv[])
    {
      // 1. Declare the supported options.
      osg::ArgumentParser arguments(&argc, argv);
      osg::ApplicationUsage* au (arguments.getApplicationUsage());
      au->setApplicationName(arguments.getApplicationName());
      au->setCommandLineUsage(arguments.getApplicationName()+" [options]");

      au->addCommandLineOption("-v or --verbose",  "Activate verbose output");
      au->addCommandLineOption("-g or --generate-config-files", "generate configuration files in " + installDirectory.toStdString() + "/etc and quit");
      au->addCommandLineOption("-c or --config-file", "set the configuration file (do not include .conf)", configurationFile);
      au->addCommandLineOption("--predefined-robots", "set the predefined robots configuration file (do not include .conf)", predifinedRobotConf);
      au->addCommandLineOption("--predefined-environments", "set the predefined environments configuration file (do not include .conf)", predifinedEnvConf);
      au->addCommandLineOption("--add-robot", "Add a robot (a list of comma sperated string)");
      au->addCommandLineOption("--add-env", "Add an environment (a list of comma sperated string)");
      au->addCommandLineOption("-p or --load-plugin", "load the plugin");
#if GEPETTO_GUI_HAS_PYTHONQT
      au->addCommandLineOption("-q or --load-pyplugin", "load the PythonQt module as a plugin");
      au->addCommandLineOption("-x or --run-pyscript", "run a script into the PythonQt console");
#endif
      au->addCommandLineOption("-P or --no-plugin", "do not load any plugin");
      au->addCommandLineOption("-w or --auto-write-settings", "write the settings in the configuration file");
      au->addCommandLineOption("--no-viewer-server", "do not start the Gepetto Viewer server");
      au->addCommandLineOption("--use-nameservice" , "The server will be registered to the Omni NameService");

      // 2. Read configuration files
      if (arguments.read("-c", configurationFile) || arguments.read("--config-file", configurationFile)) {}
      if (arguments.read("--predefined-robots",       predifinedRobotConf)) {}
      if (arguments.read("--predefined-environments", predifinedEnvConf)) {}
      fromFiles ();

      // 3. Read other command line arguments
      bool genAndQuit = false;
      int retVal = 0;

      // Declare the supported options.
      osg::ApplicationUsage::Type help (arguments.readHelpType());

      osg::DisplaySettings* ds = osg::DisplaySettings::instance().get();
      ds->readCommandLine (arguments); // populate the help message.

      verbose =                 (arguments.read("-v") || arguments.read("--verbose"));
      genAndQuit =              (arguments.read("-g") || arguments.read("--generate-config-files"));
      noPlugin =                (arguments.read("-P") || arguments.read("--no-plugin"));
      autoWriteSettings =       (arguments.read("-w") || arguments.read("--auto-write-settings"));
      bool startViewerServer = (!arguments.read("--no-viewer-server"));
      while (arguments.read ("--use-nameservice", useNameService)) {}

      std::string opt;
      while (arguments.read ("--add-robot", opt))
        addRobotFromString (opt);
      while (arguments.read ("--add-env", opt))
        addEnvFromString (opt);
      if (startViewerServer) {
        // TODO omniorbserver.so should be the first plugin
        // because of ORB::init...
        // addPlugin ("omniorbserver.so", true);
        pluginsToInit_.prepend ("omniorbserver.so");
        pluginManager_.declarePlugin ("omniorbserver.so");
      }
      while (arguments.read ("-p", opt) || arguments.read ("--load-plugin", opt))
        addPlugin (QString::fromStdString(opt), !noPlugin);
      while (arguments.read ("-q", opt) || arguments.read ("--load-pyplugin", opt))
        addPyPlugin (QString::fromStdString(opt), !noPlugin);
      while (arguments.read ("-x", opt) || arguments.read ("--run-pyscript", opt))
        addPyScript (QString::fromStdString(opt));

      for (int i = 0; i < arguments.argc()-1; ) {
        if (strncmp (arguments.argv()[i], "-ORB", 4) == 0) {
          addOmniORB (arguments.argv()[i], arguments.argv()[i+1]);
          arguments.remove (i, 2);
        } else
          ++i;
      }

      arguments.reportRemainingOptionsAsUnrecognized(osg::ArgumentParser::BENIGN);
      if (arguments.errors(osg::ArgumentParser::CRITICAL)) {
        arguments.writeErrorMessages(std::cout);
        retVal = 2;
      } else if (arguments.errors(osg::ArgumentParser::BENIGN)) {
        arguments.writeErrorMessages(std::cout);
      }

      if (!omniORBargv_.contains("-ORBendPoint"))
        addOmniORB ("-ORBendPoint", ":::12321");

      if (genAndQuit && retVal < 1) retVal = 1;

      if (help != osg::ApplicationUsage::NO_HELP) {
        arguments.getApplicationUsage()->write(std::cout, help, 80, true);
        if (retVal < 1) retVal = 1;
      }
      if (verbose) print (std::cout) << std::endl;
      if (genAndQuit) writeSettings ();

      return retVal;
    }

    void Settings::fromFiles ()
    {
      readRobotFile();
      readEnvFile();
      readSettingFile();
    }

    void Settings::writeSettings()
    {
      writeRobotFile ();
      writeEnvFile ();
      writeSettingFile ();
    }

    void Settings::initPlugins()
    {
      foreach (QString name, pluginsToInit_) {
        pluginManager_.loadPlugin (name);
        pluginManager_.initPlugin (name);
      }
      foreach (QString name, pyplugins_) {
        pluginManager_.loadPyPlugin (name);
      }
#if GEPETTO_GUI_HAS_PYTHONQT
      PythonWidget* pw = mw->pythonWidget();
      // TODO Wouldn't it be better to do this later ?
      foreach (QString fileName, pyscripts_) {
        pw->runScript(fileName);
      }
#else
      foreach (QString fileName, pyscripts_) {
        logError ("gepetto-viewer was compiled without GEPETTO_GUI_HAS_"
            "PYTHONQT flag. Cannot not load Python script " + fileName);
      }
#endif
    }

    void Settings::restoreState () const
    {
      QSettings settings (QSettings::SystemScope,
          QCoreApplication::organizationName (),
          getQSettingsFileName (stateConf));
      if (settings.status() != QSettings::NoError) {
        qDebug () << "Could not restore the window state from" << settings.fileName();
      } else {
        settings.beginGroup("mainWindow");
        mw->restoreGeometry (settings.value("geometry").toByteArray());
        mw->restoreState (settings.value("state").toByteArray());
        mw->centralWidget()->setVisible (settings.value("centralWidgetVisibility", true).toBool());
        settings.endGroup();
#if GEPETTO_GUI_HAS_PYTHONQT
        mw->pythonWidget()->restoreHistory(settings);
#endif
      }
    }

    void Settings::restoreDockWidgetsState () const
    {
      QSettings settings (QSettings::SystemScope,
          QCoreApplication::organizationName (),
          getQSettingsFileName (stateConf));
      if (settings.status() != QSettings::NoError) {
        qDebug () << "Could not restore the dock widget state from" << settings.fileName();
      } else {
        settings.beginGroup("mainWindow");
        mw->restoreState (settings.value("state").toByteArray());
        settings.endGroup();
      }
    }

    void Settings::saveState () const
    {
      QSettings settings (QSettings::SystemScope,
          QCoreApplication::organizationName (),
          getQSettingsFileName (stateConf));
      if (settings.status() != QSettings::NoError) {
        qDebug () << "Could not save the window state to" << settings.fileName();
      } else {
        settings.beginGroup("mainWindow");
        settings.setValue("geometry", mw->saveGeometry());
        settings.setValue("state"   , mw->saveState());
        settings.setValue("centralWidgetVisibility", mw->centralWidget()->isVisible ());
        settings.endGroup();
#if GEPETTO_GUI_HAS_PYTHONQT
        mw->pythonWidget()->saveHistory(settings);
#endif
      }
    }

    void Settings::setMainWindow(gepetto::gui::MainWindow *main)
    {
      mw = main;
    }

    std::ostream& Settings::print (std::ostream& os)
    {
      const char tab = '\t';
      const char nl = '\n';
      os
        << nl <<     "Configuration:"
        << nl << tab << "Configuration file:     " << tab << configurationFile
        << nl << tab << "Predefined robots:      " << tab << predifinedRobotConf
        << nl << tab << "Predefined environments:" << tab << predifinedEnvConf

        << nl << nl << "Options:"
        << nl << tab << "Verbose:                " << tab << verbose
        << nl << tab << "No plugin:              " << tab << noPlugin
        << nl << tab << "Use omni name service:  " << tab << useNameService
        << nl << tab << "Refresh rate:           " << tab << refreshRate

        << nl << nl << "Screen capture options:"
        << nl << tab << "Directory:              " << tab << captureDirectory
        << nl << tab << "Filename:               " << tab << captureFilename
        << nl << tab << "Extension:              " << tab << captureExtension
        << nl << tab << "Avconv command:         " << tab << avconv.toStdString()
        << nl << tab << "Avconv input options:   " << tab << avConvInputOptions .join(" ").toStdString()
        << nl << tab << "Avconv output options:  " << tab << avConvOutputOptions.join(" ").toStdString()

        << nl
        << nl << "omniORB configuration" ;
      for (int i = 1; i < omniORBargv_.size(); i+=2)
        os << nl << tab << omniORBargv_[i-1].toStdString()
                        << " = " << omniORBargv_[i].toStdString();
      return os;
    }

    QString Settings::getQSettingsFileName (const std::string& settingsName) const
    {
      QString name (QString::fromStdString (settingsName));
      QString ext (".conf");
      // Remove extension
      if (name.endsWith (ext))
        name = name.left (name.size() - ext.size());
      return name;
    }

    void Settings::readRobotFile ()
    {
      QSettings robot (QSettings::SystemScope,
          QCoreApplication::organizationName (),
          getQSettingsFileName (predifinedRobotConf));
      if (robot.status() != QSettings::NoError) {
        logError(QString ("Unable to open configuration file ")
                 + robot.fileName());
      } else {
        foreach (QString name, robot.childGroups()) {
          robot.beginGroup(name);
          QString robotName = robot.value("RobotName", name).toString();
          DialogLoadRobot::addRobotDefinition(
              name,
              robotName,
              robot.value("RootJointType", "freeflyer").toString(),
              robot.value("ModelName", robotName).toString(),
              robot.value("Package", "").toString(),
              robot.value("URDFSuffix", "").toString(),
              robot.value("SRDFSuffix", "").toString()
              );
          robot.endGroup();
        }
        log(QString ("Read configuration file ")
            + robot.fileName());
      }
    }

    void Settings::readEnvFile ()
    {
      QSettings env (QSettings::SystemScope,
          QCoreApplication::organizationName (),
          getQSettingsFileName (predifinedEnvConf));
      if (env.status() != QSettings::NoError) {
        logError(QString ("Unable to open configuration file ") + env.fileName());
      } else {
        foreach (QString name, env.childGroups()) {
          env.beginGroup(name);
          QString envName = env.value("EnvironmentName", name).toString();
          DialogLoadEnvironment::addEnvironmentDefinition(
              name,
              envName,
              env.value("Package", "").toString(),
              env.value("URDFFilename").toString(),
	      env.value("URDFSuffix").toString(),
	      env.value("SRDFSuffix").toString()
              );
          env.endGroup();
        }
        log (QString ("Read configuration file ") + env.fileName());
      }
    }

#define GET_PARAM(what, type, variantMethod)                                   \
    {                                                                          \
      QVariant param (env.value (#what, what));                                \
      if (param.canConvert<type>()) what = param.variantMethod ();             \
      else logError ("Setting viewer/" #what " should be an " #type ".");      \
    }

    void Settings::readSettingFile ()
    {
      QSettings env (QSettings::SystemScope,
          QCoreApplication::organizationName (),
          getQSettingsFileName (configurationFile));
      if (env.status() != QSettings::NoError) {
        logError(QString ("Unable to open configuration file ") + env.fileName());
      } else {
        osg::DisplaySettings* ds = osg::DisplaySettings::instance().get();
        env.beginGroup("viewer");
        GET_PARAM(refreshRate, int, toInt);
        int nbMultiSamples = 4;
        GET_PARAM(nbMultiSamples, int, toInt);
        ds->setNumMultiSamples(nbMultiSamples);

        QString appStyle;
        GET_PARAM(appStyle, QString, toString);
        if (!appStyle.isNull()) {
          if (QStyleFactory::keys().contains(appStyle, Qt::CaseInsensitive))
            this->appStyle = appStyle;
          else {
            logError ("Available application styles are " + QStyleFactory::keys().join(", "));
            logError ("Requested value is " + appStyle);
            logError ("Current value is " + this->appStyle);
          }
        }

        GET_PARAM(useNameService, bool, toBool);
        env.endGroup ();

        env.beginGroup("plugins");
        foreach (QString name, env.childKeys()) {
            addPlugin (name, (noPlugin)?false:env.value(name, true).toBool());
        }
        env.endGroup ();

        env.beginGroup("pyplugins");
        foreach (QString name, env.childKeys()) {
            addPyPlugin (name, (noPlugin)?false:env.value(name, true).toBool());
        }
        env.endGroup ();

        env.beginGroup("omniORB");
        foreach (QString name, env.childKeys()) {
            addOmniORB ("-ORB" + name, env.value(name).toString());
        }
        env.endGroup ();

        env.beginGroup("avconv");
        avconv = env.value ("command", avconv).toString();
        avConvInputOptions  = env.value ("input_options" ,
            avConvInputOptions).toStringList();
        avConvOutputOptions = env.value ("output_options",
            avConvOutputOptions).toStringList();
        env.endGroup ();
        log (QString ("Read configuration file ") + env.fileName());
      }
    }

    void Settings::writeRobotFile ()
    {
      QSettings robot (QSettings::SystemScope,
          QCoreApplication::organizationName (),
          getQSettingsFileName (predifinedRobotConf));
      if (!robot.isWritable()) {
          log (QString("Configuration file ") + robot.fileName() + QString(" is not writable."));
          return;
        }
      foreach (DialogLoadRobot::RobotDefinition rd, DialogLoadRobot::getRobotDefinitions()) {
          if (rd.name_.isEmpty()) continue;
          robot.beginGroup(rd.name_);
          robot.setValue("RobotName", rd.robotName_);
          robot.setValue("ModelName", rd.modelName_);
          robot.setValue("RootJointType", rd.rootJointType_);
          robot.setValue("Package", rd.package_);
          robot.setValue("URDFSuffix", rd.urdfSuf_);
          robot.setValue("SRDFSuffix", rd.srdfSuf_);
          robot.endGroup();
        }
      log (QString("Wrote configuration file ") + robot.fileName());
    }

    void Settings::writeEnvFile ()
    {
      QSettings env (QSettings::SystemScope,
          QCoreApplication::organizationName (),
          getQSettingsFileName (predifinedEnvConf));
      if (!env.isWritable()) {
          logError (QString ("Configuration file") + env.fileName() + QString("is not writable."));
          return;
        }
      foreach (DialogLoadEnvironment::EnvironmentDefinition ed, DialogLoadEnvironment::getEnvironmentDefinitions()) {
          if (ed.name_.isEmpty()) continue;
          env.beginGroup(ed.name_);
          env.setValue("RobotName", ed.envName_);
          env.setValue("Package", ed.package_);
          env.setValue("URDFFilename", ed.urdfFilename_);
          env.endGroup();
        }
      log (QString ("Wrote configuration file ") + env.fileName());
    }

    void Settings::writeSettingFile ()
    {
      QSettings env (QSettings::SystemScope,
          QCoreApplication::organizationName (),
          getQSettingsFileName (configurationFile));
      if (!env.isWritable()) {
        logError (QString ("Configuration file") + env.fileName() + QString("is not writable."));
        return;
      }

      osg::DisplaySettings* ds = osg::DisplaySettings::instance().get();
      env.beginGroup("viewer");
      env.setValue ("refreshRate", refreshRate);
      env.setValue ("nbMultiSamples", ds->getNumMultiSamples());
      env.setValue ("useNameService", useNameService);
      env.setValue ("appStyle", appStyle);
      env.endGroup ();

      env.beginGroup("plugins");
      for (PluginManager::Map::const_iterator p = pluginManager_.plugins ().constBegin();
          p != pluginManager_.plugins().constEnd(); p++) {
        env.setValue(p.key(), (noPlugin)?false:p.value()->isLoaded());
      }
      env.endGroup ();

      env.beginGroup("pyplugins");
      for (PluginManager::PyMap::const_iterator p = pluginManager_.pyplugins ().constBegin();
          p != pluginManager_.pyplugins().constEnd(); p++) {
        env.setValue(p.key(), (noPlugin)?false:pluginManager_.isPyPluginLoaded (p.key()));
      }
      env.endGroup ();

      env.beginGroup("omniORB");
      for (int i = 1; i < omniORBargv_.size(); i+=2)
        env.setValue (omniORBargv_[i-1].mid(4), omniORBargv_[i]);
      env.endGroup ();

      env.beginGroup("avconv");
      env.setValue ("command", avconv);
      env.setValue ("input_options", avConvInputOptions);
      env.setValue ("output_options", avConvInputOptions);
      env.endGroup ();
      log (QString ("Wrote configuration file ") + env.fileName());
    }

    const char** Settings::makeOmniORBargs (int& argc)
    {
      argc = omniORBargv_.size();
      const char ** argv = new const char* [argc];
      for (int i = 0; i < argc; ++i) {
        const QString& v = omniORBargv_[i];
        argv[i] = strdup (v.toLocal8Bit().constData());
      }
      return argv;
    }

    QVariant Settings::getSetting (const QString & key,
        const QVariant & defaultValue)
    {
      QSettings env (QSettings::SystemScope,
          QCoreApplication::organizationName (),
          getQSettingsFileName (configurationFile));
      if (env.status() == QSettings::NoError) {
        return env.value (key, defaultValue);
      }
      return defaultValue;
    }

    void Settings::addRobotFromString (const std::string& rbtStr)
    {
      QString rbt = QString::fromStdString(rbtStr);
      QStringList split = rbt.split(",", QString::KeepEmptyParts);
      if (split.size() != 7) {
          logError ("Robot string is not of length 7");
          logError (rbt);
          return;
        }
      DialogLoadRobot::addRobotDefinition(split[0], split[1],
          split[2].toLower(), split[3], split[4],
          split[5], split[6]);
    }

    void Settings::addEnvFromString (const std::string& envStr)
    {
      QString env = QString::fromStdString(envStr);
      QStringList split = env.split(",", QString::KeepEmptyParts);
      if (split.size() != 6) {
          logError ("Environment string is not of length 6");
          logError (env);
          return;
        }
      DialogLoadEnvironment::addEnvironmentDefinition(split[0],
	  split[1], split[2], split[3], split[4], split[5]);
    }

    void Settings::addPlugin (const QString& plg, bool init)
    {
      if (init) pluginsToInit_.append (plg);
      pluginManager_.declarePlugin (plg);
    }

    void Settings::addPyPlugin (const QString& plg, bool init)
    {
      if (init) pyplugins_.append (plg);
      pluginManager_.declarePyPlugin (plg);
    }

    void Settings::addPyScript (const QString& fileName)
    {
      pyscripts_.append (fileName);
    }

    void Settings::addOmniORB (const QString& arg, const QString& value)
    {
      int i = omniORBargv_.indexOf (arg);
      if (i == -1)
        omniORBargv_ << arg << value;
      else
        omniORBargv_[i+1] = value;
    }

    void Settings::log(const QString &t)
    {
      if (!verbose) return;
      if (mw) mw->log(t);
      else    std::cout << t.toLocal8Bit().constData() << std::endl;
    }

    void Settings::logError(const QString &t)
    {
      if (mw) mw->logError(t);
      else    qWarning() << t;
    }
  } // namespace gui
} // namespace gepetto
