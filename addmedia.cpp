#include "addmedia.h"

AddMedia::AddMedia(QWidget *parent)
    : QWidget(parent)
{
	ui.setupUi(this);

	// format
	ui.formatcomboBox->addItem(tr("Detect ( default )"),"");
	ui.formatcomboBox->addItem(tr("Raw disk image format to avoid interpreting an untrusted format header( raw )"),"raw");
	ui.formatcomboBox->addItem(tr("Host device format ( host_device )"),"host_device");
	ui.formatcomboBox->addItem(tr("QEMU image format ( qcow2 )"),"qcow2");
	ui.formatcomboBox->addItem(tr("Old QEMU image format ( qcow )"),"qcow");
	ui.formatcomboBox->addItem(tr("User Mode Linux Copy On Write image format ( cow )"),"cow");
	ui.formatcomboBox->addItem(tr("VirtualBox 1.1 compatible image format ( vdi )"),"vdi");
	ui.formatcomboBox->addItem(tr("VMware 3 and 4 compatible image format ( vmdk )"),"vmdk");
	ui.formatcomboBox->addItem(tr("VirtualPC compatible image format VHD ( vpc )"),"vpc");
	ui.formatcomboBox->addItem(tr("Linux Compressed Loop image ( cloop )"),"cloop");

	// media
	ui.mediacomboBox->addItem(tr("Disk,SD,Flash,Floppy ( Default )"),"disk");
	ui.mediacomboBox->addItem(tr("CD-Rom"),"cdrom");
	this->mediaChanged(0);

	// interface
	ui.interfacecomboBox->addItem(tr("IDE Bus"),"ide");
	ui.interfacecomboBox->addItem(tr("SCSI Bus"),"scsi");
	ui.interfacecomboBox->addItem(tr("SD Slot"),"sd");
	ui.interfacecomboBox->addItem(tr("MTD"),"mtd");
	ui.interfacecomboBox->addItem(tr("Floppy"),"floppy");
	ui.interfacecomboBox->addItem(tr("Parallel Flash"),"pflash");
	ui.interfacecomboBox->addItem(tr("VirtIO"),"virtio");
	this->interfaceChanged(0);

	// index
	ui.indexcomboBox->addItem(tr("C: / hda ( A: / fd0 for Floppy )"),"0");
	ui.indexcomboBox->addItem(tr("D: / hdb ( B: / fd1 for Floppy )"),"1");
	ui.indexcomboBox->addItem(tr("E: / hdc"),"2");
	ui.indexcomboBox->addItem(tr("F: / hdd"),"3");
	ui.indexcomboBox->addItem(tr("G: / hde"),"4");
	ui.indexcomboBox->addItem(tr("H: / hdf"),"5");
	ui.indexcomboBox->addItem(tr("I: / hdg"),"6");
	ui.indexcomboBox->addItem(tr("J: / hdh"),"7");

	// cache
	ui.cachecomboBox->addItem(tr("None ( Default )"),"none");
	ui.cachecomboBox->addItem(tr("Writeback"),"writeback");
	ui.cachecomboBox->addItem(tr("Writethrough"),"writethrough");

	// aio
	ui.aiocomboBox->addItem(tr("pThread based disk I/O"),"threads");
	ui.aiocomboBox->addItem(tr("Native Linux AIO"),"native");

	connect( ui.serialcheckBox, SIGNAL(	stateChanged(int) ), this, SLOT(serialChanged(int)) );
	connect( ui.geometrycheckBox, SIGNAL( stateChanged(int) ), this, SLOT(geometryChanged(int)) );

	connect( ui.mediacomboBox, SIGNAL( activated(int) ), this, SLOT(mediaChanged(int)) );
	connect( ui.interfacecomboBox, SIGNAL( activated(int) ), this, SLOT(interfaceChanged(int)) );

	connect( ui.filepushButton, SIGNAL( clicked() ), this, SLOT( selectFile() ) );

	connect( ui.donepushButton, SIGNAL( clicked() ), this, SLOT( doneButton() ) );
	connect( ui.cancelpushButton, SIGNAL( clicked() ), this, SLOT( cancelButton() ) );

	startNew();
}

AddMedia::~AddMedia()
{

}

void AddMedia::doneButton(){
	command = "";

	if( ! ui.filelineEdit->text().isEmpty() ){
		command.append( "file=" );
		command.append( ui.filelineEdit->text() );
	}

	if( ui.formatcomboBox->currentIndex() > 0 ){
		command.append( ",format=" );
		command.append( ui.formatcomboBox->itemData( ui.formatcomboBox->currentIndex(), Qt::UserRole ).toString() );
	}

	if( ui.serialcheckBox->isChecked() && ( ! ui.seriallineEdit->text().isEmpty() ) ){
		command.append( ",serial=" );
		command.append( ui.seriallineEdit->text() );
	}

	command.append( ",media=" );
	command.append( ui.mediacomboBox->itemData( ui.mediacomboBox->currentIndex(), Qt::UserRole ).toString() );

	if( ui.geometrycheckBox->isChecked() && ui.mediacomboBox->currentIndex() == 0 ){
		command.append( ",cyls=" );
		command.append( QString().sprintf( "%d", ui.geo1spinBox->value() ) );
		command.append( ",heads=" );
		command.append( QString().sprintf( "%d", ui.geo2spinBox->value() ) );
		command.append( ",secs=" );
		command.append( QString().sprintf( "%d", ui.geo3spinBox->value() ) );
		if( ui.geo4spinBox->value() > 0 ){
			command.append( ",trans=" );
			command.append( QString().sprintf( "%d", ui.geo4spinBox->value() ) );
		}
	}

	command.append( ",cache=" );
	command.append( ui.cachecomboBox->itemData( ui.cachecomboBox->currentIndex(), Qt::UserRole ).toString() );

	command.append( ",aio=" );
	command.append( ui.aiocomboBox->itemData( ui.aiocomboBox->currentIndex(), Qt::UserRole ).toString() );

	command.append( ",if=" );
	command.append( ui.interfacecomboBox->itemData( ui.interfacecomboBox->currentIndex(), Qt::UserRole ).toString() );

	if( ui.snapshotcheckBox->isChecked() ){
		command.append( ",snapshot=on" );
	} else {
		command.append( ",snapshot=off" );
	}

	switch( ui.interfacecomboBox->currentIndex() ){
		case 0: // ide
		case 4: // floppy
			command.append( ",index=" );
			command.append( ui.indexcomboBox->itemData( ui.indexcomboBox->currentIndex(), Qt::UserRole ).toString() );
			break;
		case 1: // scsi
			command.append( ",bus=" );
			command.append( QString().sprintf( "%d", ui.busspinBox->value() ) );
			command.append( ",unit=" );
			command.append( QString().sprintf( "%d", ui.unitspinBox->value() ) );
			break;
		case 6: // virtio
			command.append( ",addr=" );
			command.append( ui.addresscomboBox->itemData( ui.addresscomboBox->currentIndex(), Qt::UserRole ).toString() );
			break;
	}
	emit newCommand( command );
	this->hide();
	qDebug( "AddMedia::doneButton(): %s",command.toAscii().data() );
}

void AddMedia::cancelButton(){
	command = "";
	this->hide();
}

QString AddMedia::getCommand() const
{
    return command;
}

void AddMedia::startNew(){
	ui.filelineEdit->setText("");
	this->mediaChanged(0);
	this->interfaceChanged(0);
	this->serialChanged(0);
	this->geometryChanged(0);
}

void AddMedia::selectFile(){
	QString string = QFileDialog::getOpenFileName( this, tr("QEMUCTL - Select Disc Image File"), "", tr("Disc Files ( *.bin *.img *.raw *.qcow2 *.qcow *.cow *.vdi *.vmdk *.vpc *.cloop );;All Files(*)"));
	if( string.size() > 0 ){
		ui.filelineEdit->setText(string);
	}
}

void AddMedia::mediaChanged( int index ){
	switch( index ){
		case 0:
			ui.geo1spinBox->setVisible(1);
			ui.geo2spinBox->setVisible(1);
			ui.geo3spinBox->setVisible(1);
			ui.geo4spinBox->setVisible(1);
			ui.geometrycheckBox->setVisible(1);
			ui.geometrylabel->setVisible(1);
			break;
		case 1:
			ui.geo1spinBox->setVisible(0);
			ui.geo2spinBox->setVisible(0);
			ui.geo3spinBox->setVisible(0);
			ui.geo4spinBox->setVisible(0);
			ui.geometrycheckBox->setVisible(0);
			ui.geometrylabel->setVisible(0);
			break;
		case 2:
			break;
	}
	this->adjustSize();
}

void AddMedia::interfaceChanged( int index ){
	switch( index ){
		case 0: // ide
			ui.addresscomboBox->setVisible(0);
			ui.addresslabel->setVisible(0);

			ui.buslabel->setVisible(0);
			ui.busspinBox->setVisible(0);
			ui.unitspinBox->setVisible(0);

			ui.indexcomboBox->setVisible(1);
			ui.indexlabel->setVisible(1);

			ui.addresscomboBox->setVisible(0);
			ui.addresslabel->setVisible(0);
			break;
		case 1: // scsi
			ui.addresscomboBox->setVisible(0);
			ui.addresslabel->setVisible(0);

			ui.buslabel->setVisible(1);
			ui.busspinBox->setVisible(1);
			ui.unitspinBox->setVisible(1);

			ui.indexcomboBox->setVisible(0);
			ui.indexlabel->setVisible(0);

			ui.addresscomboBox->setVisible(0);
			ui.addresslabel->setVisible(0);
			break;
		case 2: // sd
			ui.addresscomboBox->setVisible(0);
			ui.addresslabel->setVisible(0);

			ui.buslabel->setVisible(0);
			ui.busspinBox->setVisible(0);
			ui.unitspinBox->setVisible(0);

			ui.indexcomboBox->setVisible(0);
			ui.indexlabel->setVisible(0);

			ui.addresscomboBox->setVisible(0);
			ui.addresslabel->setVisible(0);
			break;
		case 3: // mtd
			ui.addresscomboBox->setVisible(0);
			ui.addresslabel->setVisible(0);

			ui.buslabel->setVisible(0);
			ui.busspinBox->setVisible(0);
			ui.unitspinBox->setVisible(0);

			ui.indexcomboBox->setVisible(0);
			ui.indexlabel->setVisible(0);

			ui.addresscomboBox->setVisible(0);
			ui.addresslabel->setVisible(0);
			break;
		case 4: // floppy
			ui.addresscomboBox->setVisible(0);
			ui.addresslabel->setVisible(0);

			ui.buslabel->setVisible(0);
			ui.busspinBox->setVisible(0);
			ui.unitspinBox->setVisible(0);

			ui.indexcomboBox->setVisible(1);
			ui.indexlabel->setVisible(1);

			ui.addresscomboBox->setVisible(0);
			ui.addresslabel->setVisible(0);
			break;
		case 5: // pflash
			ui.addresscomboBox->setVisible(0);
			ui.addresslabel->setVisible(0);

			ui.buslabel->setVisible(0);
			ui.busspinBox->setVisible(0);
			ui.unitspinBox->setVisible(0);

			ui.indexcomboBox->setVisible(0);
			ui.indexlabel->setVisible(0);

			ui.addresscomboBox->setVisible(0);
			ui.addresslabel->setVisible(0);
			break;
		case 6: // virtio
			ui.addresscomboBox->setVisible(0);
			ui.addresslabel->setVisible(0);

			ui.buslabel->setVisible(0);
			ui.busspinBox->setVisible(0);
			ui.unitspinBox->setVisible(0);

			ui.indexcomboBox->setVisible(0);
			ui.indexlabel->setVisible(0);

			ui.addresscomboBox->setVisible(1);
			ui.addresslabel->setVisible(1);
			break;
	}
	this->adjustSize();
}

void AddMedia::serialChanged( int index ){
	ui.seriallineEdit->setEnabled(index);
}

void AddMedia::geometryChanged( int index ){
	ui.geo1spinBox->setEnabled(index);
	ui.geo2spinBox->setEnabled(index);
	ui.geo3spinBox->setEnabled(index);
	ui.geo4spinBox->setEnabled(index);
}
