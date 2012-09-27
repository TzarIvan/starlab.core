#pragma once
#include <QApplication>
class GUIApplication : public QApplication{
public:
    GUIApplication(int& argc, char* argv[]) : QApplication(argc,argv){
        QLocale::setDefault(QLocale::C); // Use "C"urrent locale
        setOrganizationName("Free Software Foundation");
        setApplicationName("Starlab");
        setApplicationVersion("1.0.1");
        setQuitOnLastWindowClosed(true);
    }
    
    /// @todo why is this necessary?
    bool notify( QObject * rec, QEvent * ev ){
        try{
            return QApplication::notify(rec,ev);
        } catch (StarlabException& e){        
            QString title = "Unmanaged StarLab Exception";
            QString message = e.message();
            int retval = QMessageBox::critical(NULL, title, message, QMessageBox::Abort|QMessageBox::Ignore);
            if( retval == QMessageBox::Abort ){
                qCritical() << "Terminated because aborted exception";
                // this->closeAllWindows();
                this->exit(-1);
            }
            /// Restore pointer if it was changed
            qApp->restoreOverrideCursor();
        } catch (...) {
            qDebug() << "BAD EXCEPTION..";
        }
        // the event will be propagated to the receiver's parent and 
        // so on up to the top-level object if the receiver is not 
        // interested in the event (i.e., it returns false).
        return false;
    }
};
