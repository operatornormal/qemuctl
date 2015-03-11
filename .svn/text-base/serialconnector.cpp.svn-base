#include "serialconnector.h"

SerialConnector::SerialConnector(QWidget *parent)
    : QWidget(parent)
{
	ui.setupUi(this);
	connect( ui.serialTypeComboBox, SIGNAL( activated(int) ), this, SLOT( typeChanged(int) ) );
	connect( ui.tcpServerCheckBox, SIGNAL( stateChanged(int) ), this, SLOT( tcpServerCheckBox(int) ) );
	connect( ui.donePushButton, SIGNAL( clicked() ), this, SLOT( done() ) );
	connect( ui.cancelPushButton, SIGNAL( clicked() ), this, SLOT( cancel() ) );

	connect( ui.pipePushButton, SIGNAL( clicked() ), this, SLOT( pipeChoose() ) );
	connect( ui.filePushButton, SIGNAL( clicked() ), this, SLOT( fileChoose() ) );
	connect( ui.unixPushButton, SIGNAL( clicked() ), this, SLOT( unixChoose() ) );

	this->typeChanged(ui.serialTypeComboBox->currentIndex());
	this->tcpServerCheckBox(ui.tcpServerCheckBox->isChecked());
}

SerialConnector::~SerialConnector()
{
}

void SerialConnector::done(){
	this->command = "";
	if( ui.monitorCheckBox->isChecked() ){
		this->command.prepend("mon:");
	}

	switch( ui.serialTypeComboBox->currentIndex() ){
		case 0:
			if( ui.vcHeightSpinBox->value() > 0 && ui.vcWidthSpinBox-> value() > 0){
				this->command.append("vc:");
				if( ui.vcCheckBox->isChecked() ){
					this->command.append(QString().sprintf("%dCx%dC",ui.vcWidthSpinBox->value(),ui.vcHeightSpinBox->value()));
				} else {
					this->command.append(QString().sprintf("%dx%d",ui.vcWidthSpinBox->value(),ui.vcHeightSpinBox->value()));
				}
			} else {
				this->command.append("vc");
			}
			break;
		case 1:
			this->command.append("pty");
			break;
		case 2:
			this->command.append("none");
			break;
		case 3:
			this->command.append("null");
			break;
		case 4:
			this->command.append(ui.ttyComboBox->currentText());
			break;
		case 5:
			this->command.append(ui.parportComboBox->currentText());
			break;
		case 6:
			this->command.append("file:");
			this->command.append(ui.fileLineEdit->text());
			break;
		case 7:
			this->command.append("stdio");
			break;
		case 8:
			this->command.append("pipe:");
			this->command.append(ui.pipeLineEdit->text());
			break;
		case 9:
			this->command.append("udp:");
			this->command.append( ui.udpDestinationLineEdit->text() );
			this->command.append( ":" );
			this->command.append( QString().sprintf("%d",ui.udpDestinationSpinBox->value()) );
			this->command.append( "@" );
			this->command.append( ui.udpSourceComboBox->itemData( ui.udpSourceComboBox->currentIndex(), Qt::UserRole ).toString());
			this->command.append( ":" );
			this->command.append( QString().sprintf("%d",ui.udpSourceSpinBox->value()) );
			break;
		case 10:
			if( ui.tcpTelnetCheckBox->isChecked() ){
				this->command.append("telnet:");
			} else {
				this->command.append("tcp:");
			}
			if( ui.tcpServerCheckBox->isChecked() ){
				this->command.append( ui.tcpComboBox->itemData( ui.tcpComboBox->currentIndex(), Qt::UserRole ).toString());
				this->command.append(":");
				this->command.append( QString().sprintf("%d",ui.tcpSpinBox->value()) );
				this->command.append(",server");
			} else {
				this->command.append( ui.tcpLineEdit->text() );
				this->command.append(":");
				this->command.append( QString().sprintf("%d",ui.tcpSpinBox->value()) );
			}
			if( ui.tcpNowaitCheckBox->isChecked() ){
				this->command.append(",nowait");
			}
			if( ui.tcpNodelayCheckBox->isChecked() ){
				this->command.append(",nodelay");
			}
			break;
		case 11:
			this->command.append("unix:");
			this->command.append(ui.unixLineEdit->text());
			if( ui.unixServerCheckBox->isChecked() ){
				this->command.append(",server");
			}
			if( ui.unixNowaitCheckBox->isChecked() ){
				this->command.append(",nowait");
			}
			break;
		case 12:
			this->command.append("braile");
			break;
		case 13:
			this->command.append("msmouse");
			break;
		default:
			break;
	}
	this->hide();
	emit newCommand(command);
}

void SerialConnector::cancel(){
	this->command = "";
	this->hide();
}

void SerialConnector::fileChoose(){
	QString file = QFileDialog::getSaveFileName(this,tr("Output file"));
	ui.fileLineEdit->setText(file);
}

void SerialConnector::pipeChoose(){
	QString file = QFileDialog::getSaveFileName(this,tr("Named pipe file"));
	ui.pipeLineEdit->setText(file);
}

void SerialConnector::unixChoose(){
	QString file = QFileDialog::getSaveFileName(this,tr("Unix domain socket file"));
	ui.unixLineEdit->setText(file);
}

void SerialConnector::tcpServerCheckBox( int state ){
	if( state != 0 ){
		ui.tcpLineEdit->hide();
		ui.tcpComboBox->show();
	} else {
		ui.tcpLineEdit->show();
		ui.tcpComboBox->hide();
	}
}

void SerialConnector::fillHostAdresses(){
	QList<QNetworkInterface> networks = QNetworkInterface::allInterfaces();
	QList<QNetworkAddressEntry> addresses;
	ui.tcpComboBox->clear();
	ui.tcpComboBox->addItem( tr("All Interfaces"), "0.0.0.0" );
	ui.udpSourceComboBox->clear();
	ui.udpSourceComboBox->addItem( tr("All Interfaces"), "0.0.0.0" );
	for( int i = 0; i < networks.size(); i++ ){
		qDebug("net: %s",networks.at(i).humanReadableName().toAscii().data());
		addresses = networks.at(i).addressEntries();
		for( int j = 0; j < addresses.size(); j++ ){
			if( addresses.at(j).ip().protocol() == QAbstractSocket::IPv4Protocol ){
				qDebug("	address: %s",addresses.at(j).ip().toString().toAscii().data());
				QString item = "";
				item.append(addresses.at(j).ip().toString());
				item.append(" - ( ");
				item.append(networks.at(i).humanReadableName());
				item.append(" ");
				item.append(networks.at(i).hardwareAddress());
				item.append(" )");
				ui.tcpComboBox->addItem( item, addresses.at(j).ip().toString() );
				ui.udpSourceComboBox->addItem( item, addresses.at(j).ip().toString() );
			}
		}
	}
}

void SerialConnector::fillParport(){
	QDir dir = QDir("/dev");
	dir.setFilter( QDir::System | QDir::Files );
    dir.setSorting( QDir::Name );
    QStringList filters;
    filters << "parport*";
    dir.setNameFilters(filters);
    QFileInfoList list = dir.entryInfoList();
    ui.parportComboBox->clear();
    for( int i = 0; i < list.size(); i++ ){
    	ui.parportComboBox->addItem(list.at(i).absoluteFilePath());
    }
}

void SerialConnector::fillTTY(){
	QDir dir = QDir("/dev");
	dir.setFilter( QDir::System | QDir::Files );
    dir.setSorting( QDir::Name );
    QStringList filters;
    filters << "ttyS*" << "ttyU*" << "ttyA*";
    dir.setNameFilters(filters);
    QFileInfoList list = dir.entryInfoList();
    ui.ttyComboBox->clear();
    for( int i = 0; i < list.size(); i++ ){
    	ui.ttyComboBox->addItem(list.at(i).absoluteFilePath());
    }
}

void SerialConnector::typeChanged( int type ){
	ui.fileGroupBox->hide();
	ui.vcGroupBox->hide();
	ui.pipeGroupBox->hide();
	ui.unixGroupBox->hide();
	ui.ttyGroupBox->hide();
	ui.parportGroupBox->hide();
	ui.tcpGroupBox->hide();
	ui.udpGroupBox->hide();
	QList<QNetworkInterface> networks = QNetworkInterface::allInterfaces();
	QList<QNetworkAddressEntry> adresses;
	switch( type ){
		case 0:
			ui.vcGroupBox->show();
			break;
		case 1:
			break;
		case 2:
			break;
		case 3:
			break;
		case 4:
			fillTTY();
			ui.ttyGroupBox->show();
			break;
		case 5:
			fillParport();
			ui.parportGroupBox->show();
			break;
		case 6:
			ui.fileGroupBox->show();
			break;
		case 7:
			break;
		case 8:
			ui.pipeGroupBox->show();
			break;
		case 9:
			fillHostAdresses();
			ui.udpGroupBox->show();
			break;
		case 10:
			fillHostAdresses();
			ui.tcpGroupBox->show();
			break;
		case 11:
			ui.unixGroupBox->show();
			break;
		case 12:
			break;
		case 13:
			break;
		default:
			break;
	}
	this->adjustSize();
}

const QString SerialConnector::getCommand()
{
    return command;
}

QVBoxLayout * SerialConnector::getMainLayout(){
	return ui.mainVerticalLayout;
}
