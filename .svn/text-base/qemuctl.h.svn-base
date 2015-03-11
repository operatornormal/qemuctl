#ifndef QEMUCTL_H
#define QEMUCTL_H

#include <QMainWindow>
#include <QX11EmbedContainer>
#include <QProcess>
#include <QStringList>
#include <QDir>
#include <QCloseEvent>
#include <QSignalMapper>
#include <QFileDialog>
#include <QMessageBox>
#include <QMessageBox>
#include <QTimer>
#include <QFileInfo>
#include <QInputDialog>
#include "ui_qemuctl.h"
#include "QemuComunication.h"
#include "debugmonitor.h"
#include "ConsoleInput.h"
#include "addmedia.h"
#include "serialconnector.h"
#include "QHexSpinbox.h"

#include <X11/Xlib.h>
#include <X11/Xatom.h>

#define MAX_PROPERTY_VALUE_LEN 4096
//static bool envir_utf8 = true;

namespace QEMUCTL{
	enum e {
	    Save_Screenshot,
	    Quit,
	    Save_VM,
	    Save_Sound,
	    Stop_Sound,
	    Singlestep,
	    SinglestepOff,
	    Pause,
	    Continue,
	    Power_Off,
	    Reset,
	    Commit,
	    Quit_2,
	    Makros_2,
	    Strg_Alt_Delete,
	    Strg_Alt_Backspace,
	    Strg_Alt_F1,
	    Strg_Alt_F2,
	    Strg_Alt_F3,
	    Strg_Alt_F4,
	    Strg_Alt_F5,
	    Strg_Alt_F6,
	    Strg_Alt_F7,
	    Strg_Alt_F8,
	    Strg_Alt_F9,
	    Strg_Alt_F10,
	    Strg_Alt_F11,
	    Strg_Alt_F12,
	    Monitor_Shell,
	    HelpMonitor,
	    HelpQemu,
	    HelpQemuctl,
	    About_Qemuctl,
	    About_Qt,
	    About_Qemu
	};
}

class qemuctl : public QMainWindow
{
    Q_OBJECT

public:
    qemuctl( QStringList * argv, QWidget *parent = 0);
    ~qemuctl();
    void closeEvent(QCloseEvent *event);
public slots:
    void embedError(QX11EmbedContainer::Error error);
    void embedClosed( int exitCode, QProcess::ExitStatus exitStatus  );
    void actionTriggered(const int & action );
    void sendProc( QByteArray command );
    void newAnswer( QStringList answer, bool ok );
    void qemutrigger();
    void sendCommand( const QString & cmd );
    void gotAction( const QString & action );
    void usbConSerial( const QString & action );
private:
    Ui::qemuctlClass ui;
    QProcess * qemuProcess;
    QX11EmbedContainer * qemuContainer;
    qint16 qemuPort;
    QString command;
    QString suspendfile;
    QString suspenddir;
    bool nowakeup;
    bool usecin;
    QStringList parameter;
    Display *disp;
    qint64 winid;
    void getXWinID();
    char * get_property (Display *disp, Window win, Atom xa_prop_type, char *prop_name, unsigned long *size);
    char * get_window_title (Display *disp, Window win);
    int list_windows (Display *disp);
    Window * get_client_list (Display *disp, unsigned long *size);
    //char * get_output_str (char *str, bool is_utf8);
    char * get_window_class (Display *disp, Window win);
    QemuComunication * com;
    QSignalMapper * mapper;
    QSignalMapper * dynmapper;
    QTimer timer;
    ConsoleInput * conin;
    DebugMonitor * monitor;
    AddMedia * addmedia;
    SerialConnector * usbserialconnector;
    QLabel * usbserialvendoridlabel;
    QHexSpinbox * usbserialvendorid;
    QLabel * usbserialproductidlabel;
    QHexSpinbox * usbserialproductid;
    QString usbdir;
    QAction * savevm;
    QString lastguicommand;
signals:
    void send( QString cmd );
};

#endif // QEMUCTL_H
