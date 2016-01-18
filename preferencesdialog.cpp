#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"

PreferencesDialog::PreferencesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PreferencesDialog)
{
    ui->setupUi(this);
    connect(ui->buttonBox, SIGNAL(accepted()),
            this, SLOT(loadSettingsToFile()));
    connect(ui->buttonBox, SIGNAL(rejected()),
            this, SLOT(loadSettingsToFile()));

    sheetSizeSignalMapper = new QSignalMapper(this);

    connect(ui->A4RadioButton, SIGNAL(clicked()),
            sheetSizeSignalMapper, SLOT(map()));
    connect(ui->A5RadioButton, SIGNAL(clicked()),
            sheetSizeSignalMapper, SLOT(map()));
    connect(ui->sheetHeightSpinBox, SIGNAL(valueChanged(int)),
            sheetSizeSignalMapper, SLOT(map()));
    connect(ui->sheetWidthSpinBox, SIGNAL(valueChanged(int)),
            sheetSizeSignalMapper, SLOT(map()));
    connect(ui->VRadioButton, SIGNAL(toggled(bool)),
            this, SLOT(setVertical(bool)));

    sheetSizeSignalMapper->setMapping(ui->A4RadioButton, (int)SheetSize::A4);
    sheetSizeSignalMapper->setMapping(ui->A5RadioButton, (int)SheetSize::A5);
    sheetSizeSignalMapper->setMapping(ui->sheetHeightSpinBox, (int)SheetSize::Custom);
    sheetSizeSignalMapper->setMapping(ui->sheetWidthSpinBox, (int)SheetSize::Custom);

    connect (sheetSizeSignalMapper, SIGNAL(mapped(int)),
             this, SLOT(setSheetSize(int)));
}

PreferencesDialog::~PreferencesDialog()
{
    delete ui;
    delete sheetSizeSignalMapper;
}

void PreferencesDialog::loadSettingsToFile()
{
    QSettings settings("Settings.ini", QSettings::IniFormat);
    settings.beginGroup("Settings");
    settings.setValue("dpi", QVariant(ui->dpiSpinBox->value()));
    settings.setValue("letter-spacing", QVariant(ui->letterSpacingSpinBox->value()));
    settings.setValue("font-size", QVariant(ui->fontSizeSpinBox->value()));
    settings.setValue("sheet-width",QVariant(ui->sheetWidthSpinBox->value()));
    settings.setValue("sheet-height",QVariant(ui->sheetHeightSpinBox->value()));
    settings.setValue("right-margin",QVariant(ui->rightMarginsSpinBox->value()));
    settings.setValue("left-margin",QVariant(ui->leftMarginsSpinBox->value()));
    settings.setValue("top-margin",QVariant(ui->topMarginsSpinBox->value()));
    settings.setValue("bottom-margin",QVariant(ui->bottomMarginsSpinBox->value()));
    settings.setValue("is-sheet-orientation-vertical",QVariant(ui->VRadioButton->isChecked()));
    settings.endGroup();

    emit settingsChanged();
}

void PreferencesDialog::loadSettingsFromFile()
{
    QSettings settings("Settings.ini", QSettings::IniFormat);
    settings.beginGroup("Settings");
    ui->dpiSpinBox->setValue(settings.value("dpi", 300).toInt());
    ui->letterSpacingSpinBox->setValue(settings.value("letter-spacing", -10.0).toDouble());
    ui->fontSizeSpinBox->setValue(settings.value("font-size", 6.0).toDouble());
    ui->sheetWidthSpinBox->setValue(settings.value("sheet-width", 148).toInt());
    ui->sheetHeightSpinBox->setValue(settings.value("sheet-height", 210).toInt());
    ui->rightMarginsSpinBox->setValue(settings.value("right-margin", 20).toInt());
    ui->leftMarginsSpinBox->setValue(settings.value("left-margin", 10).toInt());
    ui->topMarginsSpinBox->setValue(settings.value("top-margin", 10).toInt());
    ui->bottomMarginsSpinBox->setValue(settings.value("bottom-margin", 5).toInt());
    ui->VRadioButton->setChecked(settings.value("is-sheet-orientation-vertical", true).toBool());
    settings.endGroup();
    setSheetSize((int)SheetSize::Custom);
}

//TODO: rewrite this terrible function clearer
void PreferencesDialog::setSheetSize(int size)
{
    SheetSize s = SheetSize(size);

    QSettings settings("Settings.ini", QSettings::IniFormat);
    settings.beginGroup("Settings");
    bool isVertical = settings.value("is-sheet-orientation-vertical", true).toBool();
    settings.endGroup();

    switch (s)
    {
        case SheetSize::A5:
        {
            changedByProgram = true;
            if (isVertical)
            {
                ui->sheetHeightSpinBox->setValue(210);
                ui->sheetWidthSpinBox->setValue(148);
            }
            else
            {
                ui->sheetHeightSpinBox->setValue(148);
                ui->sheetWidthSpinBox->setValue(210);
            }

            changedByProgram = false;
            break;
        }
        case SheetSize::A4:
        {
            changedByProgram = true;
            if (isVertical)
            {
                ui->sheetHeightSpinBox->setValue(297);
                ui->sheetWidthSpinBox->setValue(210);
            }
            else
            {
                ui->sheetHeightSpinBox->setValue(210);
                ui->sheetWidthSpinBox->setValue(297);
            }
            changedByProgram = false;
            break;
        }
        case SheetSize::Custom:
        {
            if (changedByProgram)
                return;
            int height = ui->sheetHeightSpinBox->value();
            int width = ui->sheetWidthSpinBox->value();
            if ((height == 148 && width == 210) ||
                (height == 210 && width == 148))
            {
                ui->A5RadioButton->setChecked(true);
                return;
            }
            if ((height == 210 && width == 297) ||
                (height == 297 && width == 210))
            {
                ui->A4RadioButton->setChecked(true);
                return;
            }

            ui->CustomRadioButton->setChecked(true);
            break;
        }
    }
}

void PreferencesDialog::setVertical(bool isVertical)
{
    changedByProgram = true;
    int temp = ui->sheetHeightSpinBox->value();
    ui->sheetHeightSpinBox->setValue(ui->sheetWidthSpinBox->value());
    ui->sheetWidthSpinBox->setValue(temp);
    changedByProgram = false;
}