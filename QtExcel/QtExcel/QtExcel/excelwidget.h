#ifndef EXCELWIDGET_H
#define EXCELWIDGET_H

#include <QWidget>
#include <QString>
#include <QAxObject>

namespace Ui {
class ExcelWidget;
}

class ExcelWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ExcelWidget(QWidget *parent = 0);
    ~ExcelWidget();

private slots:
    void on_openButton_clicked();

private:
    Ui::ExcelWidget *ui;
    bool file_csv(const QString &path);
    bool file_excel_xlsx(const QString &path);
    bool firstFlag;
};

#endif // EXCELWIDGET_H
