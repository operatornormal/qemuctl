/*
    Copyright (C) 2010  Peter Rustler

    This file is part of qemuctl.

    qemuctl is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    qemuctl is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with qemuctl.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "qemuctl.h"

#include <unistd.h>

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <list>

using namespace std;

class WindowsMatchingPid
{
public:
    WindowsMatchingPid(Display *display, Window wRoot, unsigned long pid)
    {
        _display = display;
        _pid = pid;
        _atomPID = XInternAtom(display, "_NET_WM_PID", True);
        if(_atomPID != None) search(wRoot);
    }

    const list<Window> &result() const { return _result; }

private:
    unsigned long  _pid;
    Atom           _atomPID;
    Display       *_display;
    list<Window>   _result;

    void search(Window w)
    {
    // Get the PID for the current Window.
        Atom           type;
        int            format;
        unsigned long  nItems;
        unsigned long  bytesAfter;
        unsigned char *propPID = 0;
        if(Success == XGetWindowProperty(_display, w, _atomPID, 0, 1, False, XA_CARDINAL,
                                         &type, &format, &nItems, &bytesAfter, &propPID))
        {
            if(propPID != 0)
            {
            // If the PID matches, add this window to the result set.
                if(_pid == *((unsigned long *)propPID))
                    _result.push_back(w);

                XFree(propPID);
            }
        }

    // Recurse into child windows.
        Window    wRoot;
        Window    wParent;
        Window   *wChild;
        unsigned  int nChildren;
        if(XQueryTree(_display, w, &wRoot, &wParent, &wChild, &nChildren))
        {
            for(unsigned int i = 0; i < nChildren; i++)
                search(wChild[i]);
        }
    }
};


qemuctl::qemuctl( QStringList * argv, QWidget *parent)
    : QMainWindow(parent)
{
	QDiscFileDialog * dfdialog = new QDiscFileDialog();
	dfdialog->show();
	winid = 0;
	bool name = false;
	ui.setupUi(this);
	usbdir = "/dev/bus/usb";
	lastguicommand = "";
	savevm = new QAction(tr("New Slot"),this);
	qemuContainer = new QX11EmbedContainer();
	this->setCentralWidget(qemuContainer);
	connect(qemuContainer, SIGNAL(error(QX11EmbedContainer::Error)), this, SLOT(embedError(QX11EmbedContainer::Error)));

	// parsing commandline
	usecin = false;
    command = "qemu";
    suspendfile = "suspend";
    suspenddir = QDir::currentPath();
    nowakeup = false;
    qemuPort = 6453;
	parameter = *argv;
	parameter.removeAt(0);
	this->setWindowTitle("QEMUCTL");
	for( int i = 0; i < parameter.size(); i++ ){
		// do it for qemu -monitor stdio
		if( parameter[i].compare( QString("-monitor") ) == 0 && parameter[i+1].compare( QString("stdio") ) == 0 ){
			usecin = true;
		} else
		// change name to find qemu/kvm window
		if( parameter[i].compare( QString("-name") ) == 0 ){
			i++;
			if( parameter.size() > i ){
				this->setWindowTitle(QString("QEMUCTL (").append(parameter[i]).append(")"));
				parameter[i] = QString().sprintf( "QEMUCTL %lld", QApplication::applicationPid() );
				name = true;
			}
		} else
		// change qemu/kvm executable ( default: /usr/bin/qemu )
		if( parameter[i].compare( QString("-qemuctl-cmd") ) == 0 ){
			if( parameter.size() > i ){
				parameter.removeAt(i);
				command = parameter[i];
				parameter.removeAt(i);
				i--;
			}
		} else
		// change qemu/kvm suspend file ( default: suspend )
		if( parameter[i].compare( QString("-qemuctl-suspend-file") ) == 0 ){
			if( parameter.size() > i ){
				parameter.removeAt(i);
				suspendfile = parameter[i];
				parameter.removeAt(i);
				i--;
			}
		} else
		// change qemu/kvm suspend dir ( default: current dir )
		if( parameter[i].compare( QString("-qemuctl-suspend-dir") ) == 0 ){
			if( parameter.size() > i ){
				parameter.removeAt(i);
				suspenddir = parameter[i];
				parameter.removeAt(i);
				i--;
			}
		} else
		// do not wakeup using default suspend file
		if( parameter[i].compare( QString("-qemuctl-nowakeup") ) == 0 ){
			if( parameter.size() > i ){
				nowakeup = true;
				parameter.removeAt(i);
				i--;
			}
		} else
		// change qemu/kvm monitor port ( default: 6453 )
		if( parameter[i].compare( QString("-qemuctl-port") ) == 0 ){
			if( parameter.size() > i ){
				parameter.removeAt(i);
				qemuPort = parameter[i].toUShort();
				parameter.removeAt(i);
				i--;
			}
		}
	}
	// name set ?
	if( name == false ){
		parameter.push_back( QString("-name") );
		parameter.push_back( QString().sprintf( "QEMUCTL %lld", QApplication::applicationPid() ) );
	}
	// setting monitor port
	parameter.push_back( QString("-monitor") );
	parameter.push_back( QString().sprintf( "tcp:127.0.0.1:%d,server,nowait", qemuPort ) );
	// start qemu
	qemuProcess = new QProcess( qemuContainer );
	connect(qemuProcess, SIGNAL(finished( int, QProcess::ExitStatus )), this, SLOT(embedClosed( int , QProcess::ExitStatus )));
	qemuProcess->setProcessChannelMode(QProcess::ForwardedChannels);
	qemuProcess->setReadChannelMode(QProcess::ForwardedChannels);
	qemuProcess->start(command,parameter);

	// start cin input
	if( usecin == true ){
		conin = new ConsoleInput();
		connect( conin, SIGNAL(gotCin(QByteArray)), this, SLOT(sendProc(QByteArray)) );
		conin->start();
	}

	// monitor
	monitor = new DebugMonitor();

	// new device dialog
	addmedia = new AddMedia();

	// usb serial connector
	usbserialconnector = new SerialConnector();
	QHBoxLayout * usbseriallayout = new QHBoxLayout();
	usbserialvendoridlabel = new QLabel( tr("Vendor ID") );
	usbserialvendorid = new QHexSpinbox();
	usbserialvendorid->setValue(0x0403);
	usbserialproductidlabel = new QLabel( tr("Product ID") );
	usbserialproductid = new QHexSpinbox();
	usbserialproductid->setValue(0x6001);
	usbseriallayout->addWidget(usbserialvendoridlabel);
	usbseriallayout->addWidget(usbserialvendorid);
	usbseriallayout->addWidget(usbserialproductidlabel);
	usbseriallayout->addWidget(usbserialproductid);
	usbserialconnector->getMainLayout()->insertLayout( 0, usbseriallayout );
	connect( ui.actionUSBSerial, SIGNAL(triggered()), usbserialconnector, SLOT(show()));
	connect( usbserialconnector, SIGNAL(newCommand( const QString & )), this, SLOT(usbConSerial( const QString & )));

	// start connection to qemu server
	com = new QemuComunication();
	com->setServer("127.0.0.1",qemuPort);
	connect( this, SIGNAL(send(QString)), com, SLOT(send(QString)), Qt::QueuedConnection );
	connect( monitor, SIGNAL(send(QString)), com, SLOT(send(QString)), Qt::QueuedConnection );
	connect( com, SIGNAL(newAnswer( QStringList, bool )), this, SLOT(newAnswer( QStringList, bool )), Qt::QueuedConnection );
	connect( com, SIGNAL(newAnswer( QStringList, bool )), monitor, SLOT(newAnswer( QStringList, bool )), Qt::QueuedConnection );
	com->start();

	// search winid for 10s
	int timeout = 1000;
	while( ( this->winid == 0 ) && ( (timeout--) > 0 ) ){
		QApplication::processEvents();
		usleep(10);
		getXWinID();
	}

	// embed xwindow
	if( this->winid != 0 ){
		qemuContainer->embedClient(winid);
	} else {
		qDebug("Cant find QEMU/KVM Window");
	}

	// start timer
	connect( &timer, SIGNAL(timeout()), this, SLOT(qemutrigger()) );
	timer.start(3000);

	dynmapper = new QSignalMapper(this);
	connect( dynmapper, SIGNAL(mapped( const QString & )), this, SLOT(gotAction( const QString & )) );

	mapper = new QSignalMapper(this);

	// connecting 2way commands
	connect( ui.menuMedia, SIGNAL(aboutToShow()), mapper, SLOT(map()) );
	connect( ui.menuStop_Record_Sound, SIGNAL(aboutToShow()), mapper, SLOT(map()) );
	connect( ui.menuDelete_VM_State, SIGNAL(aboutToShow()), mapper, SLOT(map()) );
	connect( ui.menuSave_VM_State, SIGNAL(aboutToShow()), mapper, SLOT(map()) );
	connect( ui.menuLoad_VM_State, SIGNAL(aboutToShow()), mapper, SLOT(map()) );
	connect( ui.menuInfo, SIGNAL(aboutToShow()), mapper, SLOT(map()) );
	connect( ui.menuUSBHost, SIGNAL(aboutToShow()), mapper, SLOT(map()) );
	connect( ui.menuUSBDisconnect, SIGNAL(aboutToShow()), mapper, SLOT(map()) );

	connect( ui.actionUSBMouse, SIGNAL(triggered()), mapper, SLOT(map()) );
	connect( ui.actionUSBKeyboard, SIGNAL(triggered()), mapper, SLOT(map()) );
	connect( ui.actionUSBBraille, SIGNAL(triggered()), mapper, SLOT(map()) );
	connect( ui.actionUSBTablet, SIGNAL(triggered()), mapper, SLOT(map()) );
	connect( ui.actionUSBWacom, SIGNAL(triggered()), mapper, SLOT(map()) );

	connect( ui.actionWatchdogReset, SIGNAL(triggered()), mapper, SLOT(map()) );
	connect( ui.actionWatchdogShutdown, SIGNAL(triggered()), mapper, SLOT(map()) );
	connect( ui.actionWatchdogPoweroff, SIGNAL(triggered()), mapper, SLOT(map()) );
	connect( ui.actionWatchdogPause, SIGNAL(triggered()), mapper, SLOT(map()) );
	connect( ui.actionWatchdogDebug, SIGNAL(triggered()), mapper, SLOT(map()) );
	connect( ui.actionWatchdogNone, SIGNAL(triggered()), mapper, SLOT(map()) );

	mapper->setMapping( ui.actionWatchdogReset, "watchdog_action reset" );
	mapper->setMapping( ui.actionWatchdogShutdown, "watchdog_action shutdown" );
	mapper->setMapping( ui.actionWatchdogPoweroff, "watchdog_action poweroff" );
	mapper->setMapping( ui.actionWatchdogPause, "watchdog_action pause" );
	mapper->setMapping( ui.actionWatchdogDebug, "watchdog_action debug" );
	mapper->setMapping( ui.actionWatchdogNone, "watchdog_action none" );

	connect( ui.actionBoot_A, SIGNAL(triggered()), mapper, SLOT(map()) );
	connect( ui.actionBoot_B, SIGNAL(triggered()), mapper, SLOT(map()) );
	connect( ui.actionBoot_C, SIGNAL(triggered()), mapper, SLOT(map()) );
	connect( ui.actionBoot_D, SIGNAL(triggered()), mapper, SLOT(map()) );
	connect( ui.actionBoot_E, SIGNAL(triggered()), mapper, SLOT(map()) );
	connect( ui.actionBoot_F, SIGNAL(triggered()), mapper, SLOT(map()) );
	connect( ui.actionBoot_G, SIGNAL(triggered()), mapper, SLOT(map()) );
	connect( ui.actionBoot_H, SIGNAL(triggered()), mapper, SLOT(map()) );
	connect( ui.actionBoot_I, SIGNAL(triggered()), mapper, SLOT(map()) );
	connect( ui.actionBoot_J, SIGNAL(triggered()), mapper, SLOT(map()) );

	mapper->setMapping( ui.actionBoot_A, "boot_set a" );
	mapper->setMapping( ui.actionBoot_B, "boot_set b" );
	mapper->setMapping( ui.actionBoot_C, "boot_set c" );
	mapper->setMapping( ui.actionBoot_D, "boot_set d" );
	mapper->setMapping( ui.actionBoot_E, "boot_set e" );
	mapper->setMapping( ui.actionBoot_F, "boot_set f" );
	mapper->setMapping( ui.actionBoot_G, "boot_set g" );
	mapper->setMapping( ui.actionBoot_H, "boot_set h" );
	mapper->setMapping( ui.actionBoot_I, "boot_set i" );
	mapper->setMapping( ui.actionBoot_J, "boot_set j" );

	mapper->setMapping( ui.menuMedia, "info block" );
	mapper->setMapping( ui.menuStop_Record_Sound, "info capture" );
	mapper->setMapping( ui.menuDelete_VM_State, "info snapshots" );
	mapper->setMapping( ui.menuSave_VM_State, "info snapshots" );
	mapper->setMapping( ui.menuLoad_VM_State, "info snapshots" );
	mapper->setMapping( ui.menuInfo, "info" );
	mapper->setMapping( ui.menuUSBHost, "info usbhost" );
	mapper->setMapping( ui.menuUSBDisconnect, "info usb" );

	mapper->setMapping( ui.actionUSBMouse, "usb_add mouse" );
	mapper->setMapping( ui.actionUSBKeyboard, "usb_add keyboard" );
	mapper->setMapping( ui.actionUSBBraille, "usb_add braille" );
	mapper->setMapping( ui.actionUSBTablet, "usb_add tablet" );
	mapper->setMapping( ui.actionUSBWacom, "usb_add wacom-tablet" );

	// connecting 1 way commands
    connect( ui.actionSave_Screenshot, SIGNAL(triggered()), mapper, SLOT(map()) );
    connect( ui.actionQuit, SIGNAL(triggered()), mapper, SLOT(map()) );
    connect( savevm, SIGNAL(triggered()), mapper, SLOT(map()) );
    connect( ui.actionSave_Sound, SIGNAL(triggered()), mapper, SLOT(map()) );
    connect( ui.actionSinglestep, SIGNAL(triggered()), mapper, SLOT(map()) );
    connect( ui.actionSinglestepOff, SIGNAL(triggered()), mapper, SLOT(map()) );
    connect( ui.actionPause, SIGNAL(triggered()), mapper, SLOT(map()) );
    connect( ui.actionContinue, SIGNAL(triggered()), mapper, SLOT(map()) );
    connect( ui.actionPower_Off, SIGNAL(triggered()), mapper, SLOT(map()) );
    connect( ui.actionReset, SIGNAL(triggered()), mapper, SLOT(map()) );
    connect( ui.actionCommit, SIGNAL(triggered()), mapper, SLOT(map()) );
    connect( ui.actionQuit_2, SIGNAL(triggered()), mapper, SLOT(map()) );
    connect( ui.actionStrg_Alt_Delete, SIGNAL(triggered()), mapper, SLOT(map()) );
    connect( ui.actionStrg_Alt_Backspace, SIGNAL(triggered()), mapper, SLOT(map()) );
    connect( ui.actionStrg_Alt_F1, SIGNAL(triggered()), mapper, SLOT(map()) );
    connect( ui.actionStrg_Alt_F2, SIGNAL(triggered()), mapper, SLOT(map()) );
    connect( ui.actionStrg_Alt_F3, SIGNAL(triggered()), mapper, SLOT(map()) );
    connect( ui.actionStrg_Alt_F4, SIGNAL(triggered()), mapper, SLOT(map()) );
    connect( ui.actionStrg_Alt_F5, SIGNAL(triggered()), mapper, SLOT(map()) );
    connect( ui.actionStrg_Alt_F6, SIGNAL(triggered()), mapper, SLOT(map()) );
    connect( ui.actionStrg_Alt_F7, SIGNAL(triggered()), mapper, SLOT(map()) );
    connect( ui.actionStrg_Alt_F8, SIGNAL(triggered()), mapper, SLOT(map()) );
    connect( ui.actionStrg_Alt_F9, SIGNAL(triggered()), mapper, SLOT(map()) );
    connect( ui.actionStrg_Alt_F10, SIGNAL(triggered()), mapper, SLOT(map()) );
    connect( ui.actionStrg_Alt_F11, SIGNAL(triggered()), mapper, SLOT(map()) );
    connect( ui.actionStrg_Alt_F12, SIGNAL(triggered()), mapper, SLOT(map()) );
    connect( ui.actionMonitor_Shell, SIGNAL(triggered()), mapper, SLOT(map()) );
    connect( ui.actionHelpMonitor, SIGNAL(triggered()), mapper, SLOT(map()) );
    connect( ui.actionHelpQemu, SIGNAL(triggered()), mapper, SLOT(map()) );
    connect( ui.actionHelpQemuctl, SIGNAL(triggered()), mapper, SLOT(map()) );
    connect( ui.actionAbout_Qt, SIGNAL(triggered()), mapper, SLOT(map()) );
    connect( ui.actionAbout_Qemu, SIGNAL(triggered()), mapper, SLOT(map()) );
    connect( ui.actionAbout_Qemuctl, SIGNAL(triggered()), mapper, SLOT(map()) );

    mapper->setMapping( ui.actionSave_Screenshot, QEMUCTL::Save_Screenshot );
    mapper->setMapping( ui.actionQuit, QEMUCTL::Quit );
    mapper->setMapping( savevm, QEMUCTL::Save_VM );
    mapper->setMapping( ui.actionSave_Sound, QEMUCTL::Save_Sound );
    mapper->setMapping( ui.actionSinglestep, QEMUCTL::Singlestep );
    mapper->setMapping( ui.actionSinglestepOff, QEMUCTL::SinglestepOff );
    mapper->setMapping( ui.actionPause, QEMUCTL::Pause );
    mapper->setMapping( ui.actionContinue, QEMUCTL::Continue );
    mapper->setMapping( ui.actionPower_Off, QEMUCTL::Power_Off );
    mapper->setMapping( ui.actionReset, QEMUCTL::Reset );
    mapper->setMapping( ui.actionCommit, QEMUCTL::Commit );
    mapper->setMapping( ui.actionQuit_2, QEMUCTL::Quit_2 );
    mapper->setMapping( ui.actionStrg_Alt_Delete, QEMUCTL::Strg_Alt_Delete );
    mapper->setMapping( ui.actionStrg_Alt_Backspace, QEMUCTL::Strg_Alt_Backspace );
    mapper->setMapping( ui.actionStrg_Alt_F1, QEMUCTL::Strg_Alt_F1 );
    mapper->setMapping( ui.actionStrg_Alt_F2, QEMUCTL::Strg_Alt_F2 );
    mapper->setMapping( ui.actionStrg_Alt_F3, QEMUCTL::Strg_Alt_F3 );
    mapper->setMapping( ui.actionStrg_Alt_F4, QEMUCTL::Strg_Alt_F4 );
    mapper->setMapping( ui.actionStrg_Alt_F5, QEMUCTL::Strg_Alt_F5 );
    mapper->setMapping( ui.actionStrg_Alt_F6, QEMUCTL::Strg_Alt_F6 );
    mapper->setMapping( ui.actionStrg_Alt_F7, QEMUCTL::Strg_Alt_F7 );
    mapper->setMapping( ui.actionStrg_Alt_F8, QEMUCTL::Strg_Alt_F8 );
    mapper->setMapping( ui.actionStrg_Alt_F9, QEMUCTL::Strg_Alt_F9 );
    mapper->setMapping( ui.actionStrg_Alt_F10, QEMUCTL::Strg_Alt_F10 );
    mapper->setMapping( ui.actionStrg_Alt_F11, QEMUCTL::Strg_Alt_F11 );
    mapper->setMapping( ui.actionStrg_Alt_F12, QEMUCTL::Strg_Alt_F12 );
    mapper->setMapping( ui.actionMonitor_Shell, QEMUCTL::Monitor_Shell );
    mapper->setMapping( ui.actionHelpQemuctl, QEMUCTL::HelpQemuctl );
    mapper->setMapping( ui.actionHelpQemu, QEMUCTL::HelpQemu );
    mapper->setMapping( ui.actionHelpMonitor, QEMUCTL::HelpMonitor );
    mapper->setMapping( ui.actionAbout_Qt, QEMUCTL::About_Qt);
    mapper->setMapping( ui.actionAbout_Qemu, QEMUCTL::About_Qemu);
    mapper->setMapping( ui.actionAbout_Qemuctl, QEMUCTL::About_Qemuctl);

	connect( mapper, SIGNAL(mapped( const int & )), this, SLOT(actionTriggered( const int & )) );
	connect( mapper, SIGNAL(mapped( const QString & )), this, SLOT(sendCommand( const QString & )) );
}

qemuctl::~qemuctl()
{
	delete( mapper );
	delete( com );
	delete( qemuContainer );
	delete( qemuProcess );
	delete( conin );
}

void qemuctl::usbConSerial( const QString & action ){
	QString ret = "usb_add serial:vendorid=";
	ret.append( usbserialvendorid->text() );
	ret.append( ",productid=" );
	ret.append( usbserialproductid->text() );
	ret.append(":");
	ret.append(action);
	emit send(ret);
}

void qemuctl::sendProc( QByteArray command ){
	qemuProcess->write(command);
}

void qemuctl::actionTriggered(const int & action ){
	qDebug("actionTriggered");
	QString tempfile;
	switch( action ){
		case QEMUCTL::Save_Screenshot:
			tempfile = QFileDialog::getSaveFileName( this, tr("QEMUCTL - Select PPM Image File"), "", tr("PPM Files (*.ppm)"));
			if( tempfile.isEmpty() == false ){
				emit send(QString().sprintf("screendump \"%s\"",tempfile.toAscii().data()));
			}
			break;
		case QEMUCTL::Save_VM:
			bool ok;
			tempfile = QInputDialog::getText(NULL,tr("Save VM State"),tr("Input name for saved state"),QLineEdit::Normal,"",&ok);
			if( ok == true ){
				emit send(QString().sprintf("savevm \"%s\"",tempfile.toAscii().data()));
			}
			break;
		case QEMUCTL::Quit:
			emit send("quit");
			break;
		case QEMUCTL::Save_Sound:
			tempfile = QFileDialog::getSaveFileName( this, tr("QEMUCTL - Select Sound Capture File"), "", tr("WAV Files (*.wav)"));
			if( tempfile.isEmpty() == false ){
				emit send(QString().sprintf("wavcapture \"%s\"",tempfile.toAscii().data()));
			}
			break;
		case QEMUCTL::Stop_Sound:
			emit send("info capture");
			break;
		case QEMUCTL::Singlestep:
			emit send("singlestep");
			break;
		case QEMUCTL::SinglestepOff:
			emit send("singlestep off");
			break;
		case QEMUCTL::Pause:
			emit send("stop");
			emit send("info status");
			break;
		case QEMUCTL::Continue:
			emit send("cont");
			emit send("info status");
			break;
		case QEMUCTL::Power_Off:
			emit send("system_powerdown");
			break;
		case QEMUCTL::Reset:
			emit send("system_reset");
			break;
		case QEMUCTL::Commit:
			emit send("commit all");
			break;
		case QEMUCTL::Quit_2:
			break;
		case QEMUCTL::Makros_2:
			break;
		case QEMUCTL::Strg_Alt_Delete:
			emit send("sendkey ctrl-alt-delete");
			break;
		case QEMUCTL::Strg_Alt_Backspace:
			emit send("sendkey ctrl-alt-backspace");
			break;
		case QEMUCTL::Strg_Alt_F1:
			emit send("sendkey ctrl-alt-f1");
			break;
		case QEMUCTL::Strg_Alt_F2:
			emit send("sendkey ctrl-alt-f2");
			break;
		case QEMUCTL::Strg_Alt_F3:
			emit send("sendkey ctrl-alt-f3");
			break;
		case QEMUCTL::Strg_Alt_F4:
			emit send("sendkey ctrl-alt-f4");
			break;
		case QEMUCTL::Strg_Alt_F5:
			emit send("sendkey ctrl-alt-f5");
			break;
		case QEMUCTL::Strg_Alt_F6:
			emit send("sendkey ctrl-alt-f6");
			break;
		case QEMUCTL::Strg_Alt_F7:
			emit send("sendkey ctrl-alt-f7");
			break;
		case QEMUCTL::Strg_Alt_F8:
			emit send("sendkey ctrl-alt-f8");
			break;
		case QEMUCTL::Strg_Alt_F9:
			emit send("sendkey ctrl-alt-f9");
			break;
		case QEMUCTL::Strg_Alt_F10:
			emit send("sendkey ctrl-alt-f10");
			break;
		case QEMUCTL::Strg_Alt_F11:
			emit send("sendkey ctrl-alt-f11");
			break;
		case QEMUCTL::Strg_Alt_F12:
			emit send("sendkey ctrl-alt-f12");
			break;
		case QEMUCTL::Monitor_Shell:
			monitor->show();
			break;
		case QEMUCTL::HelpMonitor:
			lastguicommand = "help";
			emit send("help");
			break;
		case QEMUCTL::HelpQemu:
			//QProcess::execute("man qemu");
			break;
		case QEMUCTL::HelpQemuctl:
			//QProcess::execute("man qemuctl");
			break;
		case QEMUCTL::About_Qemuctl:
			QMessageBox::about(NULL,tr("QEMUCTL - About QEMUCTL"),tr(""
					"<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN" "http://www.w3.org/TR/REC-html40/strict.dtd\">"
							"<html><head><meta name=\"qrichtext\" content=\"1\" /></head><body align=\"center\">"
							"<h3>About Qemuctl</h3>"
							"<a href=http://qemuctl.sourceforge.net>http://qemuctl.sourceforge.net</a><br><br>"
							"(c) by <a href=mailto:chefpro@users.sourceforge.net>Peter Rustler</a>"
							"</body></html>"));
			break;
		case QEMUCTL::About_Qt:
			QMessageBox::aboutQt(NULL,tr("QEMUCTL - About QT"));
			break;
		case QEMUCTL::About_Qemu:
			QMessageBox::about(NULL,tr("QEMUCTL - About QEMU"),tr(""
					"<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN" "http://www.w3.org/TR/REC-html40/strict.dtd\">"
							"<html><head><meta name=\"qrichtext\" content=\"1\" /></head><body align=\"center\">"
							"<h3>About Qemu</h3>"
							"<a href=http://www.qemu.org>http://www.qemu.org</a><br><br>"
							"(c) by Fabrice Bellard"
							"</body></html>"));
			break;
	}
}

void qemuctl::sendCommand( const QString & cmd ){
	emit send(cmd);
}

void qemuctl::embedError(QX11EmbedContainer::Error error)
{
    switch (error) {
    case QX11EmbedContainer::InvalidWindowID:
        qDebug("Invalid window ID");
        break;
    default:
    	qDebug("Unknown error");
        break;
    }
}

void qemuctl::qemutrigger(){
	//emit send("info status");
	//this->send("info block");
}

void qemuctl::gotAction( const QString & action ){
	qDebug( "qemuctl::gotAction: %s", action.toAscii().data() );
	QString temp = action;

	// capturestop
	if( temp.indexOf("capturestop ") == 0 ){
		int pos = temp.indexOf("]");
		temp.remove( pos, 1000 );
		pos = temp.indexOf("[");
		temp.remove( 0, (pos+1) );
		emit send(QString("stopcapture ").append(temp));
	} else

	// usb host add
	if( temp.indexOf("usbhostadd ") == 0 ){
		int pos;
		pos = temp.indexOf(".");
		if( pos > 11 ){
			int bus,device;
			bus = temp.mid(11,(pos-11)).toInt();
			device = temp.mid(pos+1,2).toInt();
			//qDebug("qemuctl::gotAction: usbhostadd %d %d",bus,device);
			QString devicedir = usbdir;
			devicedir.append("/%3.3d/%3.3d");
			devicedir = QString().sprintf(devicedir.toAscii().data(),bus,device);
			qDebug("qemuctl::gotAction: devicedir %s",devicedir.toAscii().data());
			QString cmd = QString().sprintf("usb_add host:%d.%d",bus,device);
#if QT_VERSION >= 0x040600
			QFileInfo info(devicedir);
			if( ! ( info.isReadable() && info.isWritable() ) ){
				QString message = tr("You have no rights to access the usb bus.\nI am trying to become the owner of the usbdevice.\nRoot passwort ?");
				QString user = QProcessEnvironment::systemEnvironment().value("USER");
				QString command = QString().sprintf("gksu --message \"%s\" \"chown -c %s %s\"",message.toAscii().data(),user.toAscii().data(),devicedir.toAscii().data());
				qDebug("qemuctl::gotAction: command %s",command.toAscii().data());
				int ret = QProcess::execute(command);
				if( ret == 0 ){
					emit send(cmd);
				}
			} else {
				emit send(cmd);
			}
#else
			emit send(cmd);
#endif

		}
	} else

	// removemedia
	if( temp.indexOf("removemedia ") == 0 ){
		temp.remove( temp.indexOf(":"), 1000 );
		temp.remove( 0, 12 );
		temp.push_front("eject -f ");
		emit send(temp);
	} else

	// commitmedia
	if( temp.indexOf("commitmedia ") == 0 ){
		temp.remove( temp.indexOf(":"), 1000 );
		temp.remove( 0, 12 );
		temp.push_front("commit ");
		emit send(temp);
	} else

	// insertmedia
	if( temp.indexOf("insertmedia ") == 0 ){
		QString cmd = temp;
		QString string;
		cmd.remove( temp.indexOf(":"), 1000 );
		cmd.remove( 0, 12 );
		cmd.push_front("change ");
		cmd.push_back(" \"");
		if ( temp.indexOf("type=cdrom") >= 0 ) {
			string = QFileDialog::getOpenFileName( this, tr("QEMUCTL - Select CD Image File"), "", tr("Disc Files ( *.bin *.img *.raw *.iso );;All Files(*)"));
		} else {
			string = QFileDialog::getOpenFileName( this, tr("QEMUCTL - Select Disc Image File"), "", tr("Disc Files ( *.bin *.img *.raw *.qcow2 *.qcow *.cow *.vdi *.vmdk *.vpc *.cloop );;All Files(*)"));
		}
		if( string.size() > 0 ){
			cmd.append(string);
			cmd.push_back("\"");
			emit send(cmd);
		}
	} else

	// info menu
	if( temp.indexOf("info ") == 0 ){
		int pos = temp.indexOf("  -- ");
		temp.remove( pos, 1000 );
		lastguicommand = temp;
		emit send(temp);
	}

}

void qemuctl::newAnswer( QStringList answer, bool ok ){

	// was gui command ?
	if(	lastguicommand.compare(answer[0]) == 0 ){
		lastguicommand = "";
		QMessageBox::about(NULL,answer[0],answer.join("\n"));
	}

	// info snapshots
	if( ( ok == true ) && ( answer.size() > 0 ) && ( answer[0].compare("info snapshots") == 0 ) ){
		ui.menuSave_VM_State->clear();
		ui.menuSave_VM_State->addAction(savevm);
		ui.menuLoad_VM_State->clear();
		ui.menuDelete_VM_State->clear();
		int pos;
		QString vmid;
		for( int i = 4; i < answer.size(); i++ ){
			pos = answer[i].indexOf(" ");
			vmid = answer[i].mid(0,pos);
			QAction * ac1 = new QAction( answer[i], ui.menuSave_VM_State );
			connect( ac1, SIGNAL(triggered()), mapper, SLOT(map()) );
			mapper->setMapping( ac1, QString("savevm ").append( vmid ) );
			ui.menuSave_VM_State->addAction(ac1);

			QAction * ac2 = new QAction( answer[i], ui.menuLoad_VM_State );
			connect( ac2, SIGNAL(triggered()), mapper, SLOT(map()) );
			mapper->setMapping( ac2, QString("loadvm ").append( vmid ) );
			ui.menuLoad_VM_State->addAction(ac2);

			QAction * ac3 = new QAction( answer[i], ui.menuDelete_VM_State );
			connect( ac3, SIGNAL(triggered()), mapper, SLOT(map()) );
			mapper->setMapping( ac3, QString("delvm ").append( vmid ) );
			ui.menuDelete_VM_State->addAction(ac3);
		}
	} else

	// info usbhost
	if( ( ok == true ) && ( answer.size() > 0 ) && ( answer[0].compare("info usbhost") == 0 ) ){
		ui.menuUSBHost->clear();
		int pos,end;
		QString device, venor, menu;
		for( int i = 1; i < answer.size(); i+=2 ){
			if( answer[i].indexOf("husb: using sys file-system with ") == 0 ){
				usbdir = answer[i].mid(33,-1);
				qDebug("usbdir = %s",usbdir.toAscii().data());
			}
			if( ( pos = ( answer[i].indexOf("Device ") + 7 ) ) >= 7 ){
				if( ( (i+1) < answer.size() ) && ( answer[i+1].indexOf("Hub:") < 0 ) ){
					end = answer[i].indexOf(",");
					device = answer[i].mid(pos,(end-pos));
					menu = QString(device).append(answer[i+1]);
					QAction * ac = new QAction( menu, ui.menuUSBHost );
					connect( ac, SIGNAL(triggered()), dynmapper, SLOT(map()) );
					dynmapper->setMapping( ac, QString("usbhostadd ").append( device ) );
					ui.menuUSBHost->addAction(ac);
				}
			} else {
				i--;
			}
		}
	} else

	// info usb
	if( ( ok == true ) && ( answer.size() > 0 ) && ( answer[0].compare("info usb") == 0 ) ){
		ui.menuUSBDisconnect->clear();
		int pos,end;
		QString device;
		for( int i = 1; i < answer.size(); i++ ){
			if( ( pos = ( answer[i].indexOf("Device ") + 7 ) ) >= 7 ){
				end = answer[i].indexOf(",");
				device = answer[i].mid(pos,(end-pos));
				QAction * ac = new QAction( answer[i], ui.menuUSBDisconnect );
				connect( ac, SIGNAL(triggered()), mapper, SLOT(map()) );
				mapper->setMapping( ac, QString("usb_del ").append( device ) );
				ui.menuUSBDisconnect->addAction( ac );
			}
		}
	} else

	// info status
	if( ( ok == true ) && ( answer.size() > 1 ) && ( answer[0].compare("info status") == 0 ) ){
		if( answer[1].indexOf("VM status:") == 0 ){
			ui.statusbar->showMessage( answer[1] );
		}
	} else

	// info capture
	if( ( ok == true ) && ( answer.size() > 0 ) && ( answer[0].compare("info capture") == 0 ) ){
		ui.menuStop_Record_Sound->clear();
		for( int i = 1; i < answer.size(); i++ ){
			QAction * ac = new QAction( answer[i], ui.menuStop_Record_Sound );
			connect( ac, SIGNAL(triggered()), dynmapper, SLOT(map()) );
			dynmapper->setMapping( ac, QString("capturestop ").append( answer[i]) );
			ui.menuStop_Record_Sound->addAction(ac);
		}
	} else

	// info block
	if( ( ok == true ) && ( answer.size() > 0 ) && ( answer[0].compare("info block") == 0 ) ){
		ui.menuMedia->clear();
		for( int i = 1; i < answer.size(); i++ ){
			int pos = answer[i].indexOf(": ");
			QString caption = answer[i];
			caption.remove(pos,1000);
			QString tooltip = answer[i];
			tooltip.remove( 0, ( pos + 2 ) );

			QMenu * menu = new QMenu(caption,ui.menuMedia);
			//menu->setToolTip( tooltip );

			QString iconstr = answer[i];
			int iconpos = iconstr.indexOf("type=") + 5;
			int iconlen = iconstr.indexOf(" ",iconpos) - iconpos;
			iconstr = iconstr.mid(iconpos,iconlen);
			iconstr.prepend(":icons/");
			iconstr.push_back(".png");
			QIcon type  = QIcon(iconstr);
			QString acstr = answer[i];
			acstr.remove(iconpos-5,iconlen+6);
			acstr.remove(0,acstr.indexOf(":")+2);
			QAction * ac = new QAction( type, acstr, menu );
			ac->setIconVisibleInMenu(true);
			menu->addAction(ac);
			menu->addSeparator();

			QAction * ac1 = new QAction( tr("Eject Media"), menu );
			ac1->setToolTip( tooltip );
			connect( ac1, SIGNAL(triggered()), dynmapper, SLOT(map()) );
			dynmapper->setMapping( ac1, QString("removemedia ").append(answer[i]) );

			QAction * ac2 = new QAction( tr("Commit"), menu );
			ac2->setToolTip( tooltip );
			connect( ac2, SIGNAL(triggered()), dynmapper, SLOT(map()) );
			dynmapper->setMapping( ac2, QString("commitmedia ").append(answer[i]) );

			QAction * ac3 = new QAction( tr("Insert Media"), menu );
			ac3->setToolTip( tooltip );
			connect( ac3, SIGNAL(triggered()), dynmapper, SLOT(map()) );
			dynmapper->setMapping( ac3, QString("insertmedia ").append(answer[i]) );

			if( answer[i].indexOf("locked=1") >=0 ){
				ac1->setText( ac1->text().append( tr(" ( force )") ) );
				ac3->setText( ac3->text().append( tr(" ( force )") ) );
			}

			if( answer[i].indexOf("removable=1") >=0 ){
				menu->addAction(ac3);
				if( answer[i].indexOf("[not inserted]") < 0 ){
					menu->addAction(ac1);
				}
			}

			if( answer[i].indexOf("[not inserted]") < 0 && answer[i].indexOf("backing_file=") >= 0 ){
				menu->addAction(ac2);
			}
			ui.menuMedia->addMenu( menu );
		}
		static QAction * am = new QAction( tr("Add media"), this );
		ui.menuMedia->addAction( am );
		connect( am, SIGNAL(triggered()), addmedia, SLOT(show()));

	} else

	// info
	if( ( ok == true ) && ( answer.size() > 0 ) && ( answer[0].compare("info") == 0 ) ){
		ui.menuInfo->clear();
		for( int i = 1; i < answer.size(); i++ ){
			QAction * action = new QAction( answer[i], ui.menuInfo );
			connect( action, SIGNAL(triggered()), dynmapper, SLOT(map()) );
			dynmapper->setMapping( action, answer[i] );
			ui.menuInfo->addAction( action );
		}
	}

	for( int i = 0; i < answer.size(); i++){
		qDebug( "qemuctl::newAnswer \"%s\"", answer[i].toAscii().data() );
	}
}

void qemuctl::embedClosed( int exitCode, QProcess::ExitStatus exitStatus  ){
	if( exitStatus == QProcess::CrashExit ){
		QProcess::ProcessError error = qemuProcess->error();
		switch( error ){
			case QProcess::FailedToStart:
				qDebug("Failed To Start");
				break;
			case QProcess::Crashed:
				qDebug("Crashed");
				break;
			case QProcess::Timedout:
				qDebug("Timedout");
				break;
			case QProcess::WriteError:
				qDebug("WriteError");
				break;
			case QProcess::ReadError:
				qDebug("ReadError");
				break;
			case QProcess::UnknownError:
			default:
				qDebug("UnknownError");
				break;
		}
	}
	exit( exitCode );
}

void qemuctl::closeEvent(QCloseEvent *event){
	qemuProcess->terminate();
	//event->accept();
	event->ignore();
}

void qemuctl::getXWinID(){
	if (! (disp = XOpenDisplay(NULL))) {
        qDebug("Cannot open display.");
        exit(1);
    }
	list_windows(disp);
    XCloseDisplay(disp);
}

int qemuctl::list_windows (Display *disp) {/*{{{*/
    Window *client_list;
    unsigned long client_list_size;
    unsigned int i;
    int max_client_machine_len = 0;

    unsigned long pid = qemuProcess->pid();
    Window rootw = XDefaultRootWindow(disp);
    WindowsMatchingPid match(disp, rootw, pid);
        const list<Window> &result = match.result();

    list<Window>::const_iterator it;
    for(list<Window>::const_iterator it = result.begin(); it != result.end(); it++)
    {
        QString qtitle;
        qtitle = get_window_title(disp, *it);
        qDebug("Window %s", get_window_title(disp, *it));
        if( qtitle.indexOf( QString().sprintf( "QEMUCTL %lld", QApplication::applicationPid() ) ) >= 0 )
        {
            winid = *it;
            return EXIT_SUCCESS;
        }
    }
    return EXIT_FAILURE;


#if 0
    if ((client_list = get_client_list(disp, &client_list_size)) == NULL) {
        return EXIT_FAILURE;
    }

    /* find the longest client_machine name */
    for (i = 0; i < client_list_size / sizeof(Window); i++) {
        char *client_machine;
        if ((client_machine = get_property(disp, client_list[i],
                XA_STRING, (char*)("WM_CLIENT_MACHINE"), NULL))) {
            max_client_machine_len = strlen(client_machine);
        }
        //free(client_machine);
    }

    /* print the list */
    for (i = 0; i < client_list_size / sizeof(Window); i++) {
        char *title_utf8 = get_window_title(disp, client_list[i]); /* UTF8 */
        //char *title_out = get_output_str(title_utf8, TRUE);
        char *title_out = title_utf8;
        char *client_machine;
        //char *class_out = get_window_class(disp, client_list[i]); /* UTF8 */
        unsigned long *pid;
        unsigned long *desktop;
        int x, y, junkx, junky;
        unsigned int wwidth, wheight, bw, depth;
        Window junkroot;

        /* desktop ID */
        if ((desktop = (unsigned long *)get_property(disp, client_list[i],
                XA_CARDINAL, (char*)("_NET_WM_DESKTOP"), NULL)) == NULL) {
            desktop = (unsigned long *)get_property(disp, client_list[i],
                    XA_CARDINAL, (char*)("_WIN_WORKSPACE"), NULL);
        }

        /* client machine */
        client_machine = get_property(disp, client_list[i],
                XA_STRING, (char*)("WM_CLIENT_MACHINE"), NULL);

        /* pid */
        pid = (unsigned long *)get_property(disp, client_list[i],
                XA_CARDINAL, (char*)("_NET_WM_PID"), NULL);

	    /* geometry */
        XGetGeometry (disp, client_list[i], &junkroot, &junkx, &junky,
                          &wwidth, &wheight, &bw, &depth);
        XTranslateCoordinates (disp, client_list[i], junkroot, junkx, junky,
                               &x, &y, &junkroot);

        /* special desktop ID -1 means "all desktops", so we
           have to convert the desktop value to signed long */
        //printf("0x%.8lx %2ld", client_list[i], desktop ? (signed long)*desktop : 0);
        //if (options.show_pid) {
           //printf(" %-6lu", pid ? *pid : 0);
        //}
        //if (options.show_geometry) {
           //printf(" %-4d %-4d %-4d %-4d", x, y, wwidth, wheight);
        //}
		//if (options.show_class) {
		   //printf(" %-20s ", class_out ? class_out : "N/A");
		//}

        //printf(" %*s %s\n", max_client_machine_len, client_machine ? client_machine : "N/A", title_out ? title_out : "N/A" );

        QString qtitle(title_out);
        if( qtitle.indexOf( QString().sprintf( "QEMUCTL %lld", QApplication::applicationPid() ) ) >= 0 ){
        	winid = client_list[i];
        	//this->centralWidget()->setMinimumSize(wwidth,wheight);
        	//this->centralWidget()->setMaximumSize(wwidth,wheight);
        }

        ////free(title_utf8);
        ////free(title_out);
        ////free(desktop);
        ////free(client_machine);
        ////free(class_out);
        ////free(pid);
    }
    ////free(client_list);

    return EXIT_SUCCESS;
#endif
}/*}}}*/

char * qemuctl::get_window_title (Display *disp, Window win) {/*{{{*/
    char *title_utf8;
    char *wm_name;
    char *net_wm_name;

    wm_name = get_property(disp, win, XA_STRING, (char*)("WM_NAME"), NULL);
    net_wm_name = get_property(disp, win,
            XInternAtom(disp, (char*)("UTF8_STRING"), False), (char*)("_NET_WM_NAME"), NULL);

    if (net_wm_name) {
        //title_utf8 = g_strdup(net_wm_name);
    	title_utf8 = net_wm_name;
    }
    else {
        if (wm_name) {
            //title_utf8 = g_locale_to_utf8(wm_name, -1, NULL, NULL, NULL);
        	title_utf8 = wm_name;
        }
        else {
            title_utf8 = NULL;
        }
    }

    //free(wm_name);
    //free(net_wm_name);

    return title_utf8;
}/*}}}*/

char * qemuctl::get_property (Display *disp, Window win, /*{{{*/
        Atom xa_prop_type, char *prop_name, unsigned long *size) {
    Atom xa_prop_name;
    Atom xa_ret_type;
    int ret_format;
    unsigned long ret_nitems;
    unsigned long ret_bytes_after;
    unsigned long tmp_size;
    unsigned char *ret_prop;
    char *ret;

    xa_prop_name = XInternAtom(disp, prop_name, False);

    /* MAX_PROPERTY_VALUE_LEN / 4 explanation (XGetWindowProperty manpage):
     *
     * long_length = Specifies the length in 32-bit multiples of the
     *               data to be retrieved.
     */
    if (XGetWindowProperty(disp, win, xa_prop_name, 0, MAX_PROPERTY_VALUE_LEN / 4, False,
            xa_prop_type, &xa_ret_type, &ret_format,
            &ret_nitems, &ret_bytes_after, &ret_prop) != Success) {
        //p_verbose("Cannot get %s property.\n", prop_name);
        return NULL;
    }

    if (xa_ret_type != xa_prop_type) {
        //p_verbose("Invalid type of %s property.\n", prop_name);
        XFree(ret_prop);
        return NULL;
    }

    /* null terminate the result to make string handling easier */
    tmp_size = (ret_format / 8) * ret_nitems;
    //ret = malloc(tmp_size + 1);
    ret = new char[tmp_size + 1];
    memcpy(ret, ret_prop, tmp_size);
    ret[tmp_size] = '\0';

    if (size) {
        *size = tmp_size;
    }

    XFree(ret_prop);
    return ret;
}/*}}}*/

Window * qemuctl::get_client_list (Display *disp, unsigned long *size) {/*{{{*/
    Window *client_list;

    if ((client_list = (Window *)get_property(disp, DefaultRootWindow(disp),
                    XA_WINDOW, (char*)("_NET_CLIENT_LIST"), size)) == NULL) {
        if ((client_list = (Window *)get_property(disp, DefaultRootWindow(disp),
                        XA_CARDINAL, (char*)("_WIN_CLIENT_LIST"), size)) == NULL) {
            fputs("Cannot get client list properties. \n"
                  "(_NET_CLIENT_LIST or _WIN_CLIENT_LIST)"
                  "\n", stderr);
            return NULL;
        }
    }

    return client_list;
}/*}}}*/

//char * qemuctl::get_output_str (char *str, bool is_utf8) {/*{{{*/
//    return str;
//}/*}}}*/

char * qemuctl::get_window_class (Display *disp, Window win) {/*{{{*/
    char *class_utf8;
    char *wm_class;
    unsigned long size;

    wm_class = get_property(disp, win, XA_STRING, (char*)("WM_CLASS"), &size);
    if (wm_class) {
        char *p_0 = strchr(wm_class, '\0');
        if (wm_class + size - 1 > p_0) {
            *(p_0) = '.';
        }
        //class_utf8 = g_locale_to_utf8(wm_class, -1, NULL, NULL, NULL);
        class_utf8 = wm_class;
    }
    else {
        class_utf8 = NULL;
    }

    //free(wm_class);

    return class_utf8;
}/*}}}*/

