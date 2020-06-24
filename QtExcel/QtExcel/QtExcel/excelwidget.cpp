#include "excelwidget.h"
#include "ui_excelwidget.h"
#include <QFileDialog>
#include <QDebug>
#include <QDateTime>

ExcelWidget::ExcelWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ExcelWidget)
{
    ui->setupUi(this);
    this->setWindowTitle("EXCEL数据分析");
    // 在后台线程中使用QAxObject必须先初始化
    //CoInitializeEx(NULL, COINIT_MULTITHREADED);
}

ExcelWidget::~ExcelWidget()
{
    delete ui;
}

void ExcelWidget::on_openButton_clicked()
{
    //操作打开文件
    QString curPath = QDir::currentPath();
    QString dlgTitle = "打开一个文件";
    QString filter = "excel(*.xlsx *.csv);;所有文件(*.*)";
    QString aFileName = QFileDialog::getOpenFileName(this,dlgTitle,curPath,filter);
    if(aFileName.isEmpty())
    {
        qDebug() << "打开文件失败";
        return;
    }

    //QString aFileNewName = QFileDialog::getSaveFileName(this,"保存文件"，curPath,filter);
//    QString aFileNewName = QFileDialog::getSaveFileName(this,"保存文件",curPath,filter);
//    qDebug() << aFileNewName;
    ui->label->setText(aFileName);

    //检查文件类型
    if(aFileName.right(4).contains("csv"))
    {
        if(!file_csv(aFileName))
            ui->textEdit->setText("打开文件失败");
    }
    else if(aFileName.right(4).contains("xls"))
    {
        if(!file_excel_xlsx(aFileName))
            ui->textEdit->setText("打开文件失败");
    }
    else
    {
        qDebug() << "只操作xlsx、csv文件";
    }
      return;

}

bool ExcelWidget::file_csv(const QString &path)
{
    QFile file(path);
    QList<QStringList> data;
    ui->textEdit->clear();
    if(file.open(QIODevice::ReadOnly))
    {
        QTextStream stream(&file);
        forever{
            QString line = stream.readLine();
            if(line.isEmpty())
                break;
            QStringList row;
            for(const QString& cell: line.split(","))
                row.append(cell.trimmed());
            data.append(row);
        }
        file.close();

    }
    else
        return false;

    QStringList str = data.at(1);
    QDateTime dateTime;
    int TypeDate = 0;
    int second = 0;
    QString savePath;

    //("2020/6/17 23:58:59", "001108", "0", "001108225", "40", "31.563", "123.170", "停止中", "追日表日期错误、定日表日期错误、追日预备表日期错误", "12", "5")
    //"2020/6/17 23:58:59" "001108225"
    qDebug() << data.at(1);
    //通过解析数据判断 为何种格式 保存文件格式为2020_6_17_1108225.csv
    if(dateTime.fromString(str.at(0),"yyyy/MM/dd hh:mm:ss").toTime_t() != -1)
    {
        //PC断导出数据格式
        TypeDate = 1;
        savePath = QDir::currentPath() + "/" + dateTime.fromTime_t(second).toString("yyyy_MM_dd_") + str.at(3) + ".csv";
        qDebug() << savePath;

    }else if(dateTime.fromString(str.at(1),"yyyy-MM-dd hh:mm:ss.zzz").toTime_t() != -1)
    {
        //数据库导出数据格式
        TypeDate = 2;
        savePath = QDir::currentPath() + "/" + dateTime.fromTime_t(second).toString("yyyy_MM_dd_") + str.at(4) + ".csv";
        qDebug() << savePath;
    }else
    {
        ui->textEdit->setText("解析数据格式不对");
        return false;
    }

    QFile saveFile(savePath);
    if(!saveFile.open(QIODevice::ReadWrite))
    {
        qDebug() << "打开保存文件失败";
        return false;
    }
    QTextStream saveStrem(&saveFile);
    QString formate = "行数,时间,行数,时间,差值：秒,马达控制器编码,\n";
    saveStrem << formate.toUtf8();

    //读取数据比较2个数值 的时间差值
    QString strTextEdit;
    int second1 = 0,second2 = 0,diffValue = 0;
    for(int i=1; i<data.size()-1;++i)
    {
        QStringList s1=data.at(i);
        QStringList s2=data.at(i+1);
        //QString str=s1.join(" ")+"!";
        second1 = 0;
        second2 = 0;
        diffValue = 0;
        switch(TypeDate){
        case 1://PC短导出的数据分析
            second1 = dateTime.fromString(s1.at(0),"yyyy/MM/dd hh:mm:ss").toTime_t();
            second2 = dateTime.fromString(s2.at(0),"yyyy/MM/dd hh:mm:ss").toTime_t();
            //qDebug() << "第" << i << "行" << s1.at(0) << "," << second1 << ",第" << i+1 << "行"<< s2.at(0) <<","<<second2;
            //qDebug() <<s1;
            diffValue = qAbs(second1-second2);
            if(diffValue > ui->lineEdit->text().toInt()){

                qDebug() << "第" << i << "行" << s1.at(0) << "," << second1 << ",第" << i+1 << "行"<< s2.at(0) <<","<<second2 <<" 马达控制器编码" <<s1.at(3);
                qDebug() << "差值为："<<diffValue;
                strTextEdit = "第" + QString::number(i) + "行 " + s1.at(0) + "," + QString::number(second1) +
                    ",第" + QString::number(i+1) + "行 " + s2.at(0) + "," + QString::number(second2) + " 马达控制器编码" + s1.at(3);
                ui->textEdit->append(strTextEdit);
                strTextEdit = "差值为：" + QString::number(diffValue);
                ui->textEdit->append(strTextEdit);

                //保存数据
                saveStrem << QString::number(i) + "," << s1.at(0) + "," << QString::number(i+1) + "," << s2.at(0) + "," << QString::number(diffValue) + "," << s1.at(3) + "," << "\n";
            }
            break;
        case 2://数据库导出的数据分析
            second1 = dateTime.fromString(s1.at(1),"yyyy-MM-dd hh:mm:ss.zzz").toTime_t();
            second2 = dateTime.fromString(s2.at(1),"yyyy-MM-dd hh:mm:ss.zzz").toTime_t();
            //qDebug() << "第" << i << "行" << s1.at(1) << "," << second1 << ",第" << i+1 << "行"<< s2.at(1) <<","<<second2;
            //qDebug() <<s1;
            diffValue = qAbs(second1-second2);
            qDebug() << ui->lineEdit->text().toUInt();
            if(diffValue >= ui->lineEdit->text().toInt()){

                qDebug() << "第" << i << "行" << s1.at(1) << "," << second1 << ",第" << i+1 << "行"<< s2.at(1) <<","<<second2 <<" 马达控制器编码" <<s1.at(4);
                qDebug() << "差值为："<<diffValue;
                strTextEdit = "第" + QString::number(i) + "行 " + s1.at(1) + "," + QString::number(second1) +
                    ",第" + QString::number(i+1) + "行 " + s2.at(1) + "," + QString::number(second2) + " 马达控制器编码" + s1.at(4);
                ui->textEdit->append(strTextEdit);
                strTextEdit = "差值为：" + QString::number(diffValue);
                ui->textEdit->append(strTextEdit);

                //保存数据
                saveStrem << QString::number(i) + "," << s1.at(1) + "," << QString::number(i+1) + "," << s2.at(1) + ","
                        << QString::number(diffValue) + "," << s1.at(4) + "," << "\n";

            }
            break;
        }
    }
    file.close();
    saveFile.close();
    return true;
}

bool ExcelWidget::file_excel_xlsx(const QString &path)
{
    QAxObject *excel = NULL;    //本例中，excel设定为Excel文件的操作对象
    QAxObject *workbooks = NULL;
    QAxObject *workbook = NULL;  //Excel操作对象
    ui->textEdit->clear();
    excel = new QAxObject("Excel.Application");
    excel->dynamicCall("SetVisible(bool)", true); //true 表示操作文件时可见，false表示为不可见
    workbooks = excel->querySubObject("WorkBooks");

    //————————————————按文件路径打开文件————————————————————
    workbook = workbooks->querySubObject("Open(QString&)", path);
    // 获取打开的excel文件中所有的工作sheet
    QAxObject * worksheets = workbook->querySubObject("WorkSheets");

    //—————————————————Excel文件中表的个数:——————————————————
    int iWorkSheet = worksheets->property("Count").toInt();
    qDebug() << QString("Excel文件中表的个数: %1").arg(QString::number(iWorkSheet));

    // ————————————————获取第n个工作表 querySubObject("Item(int)", n);——————————
    QAxObject * worksheet = worksheets->querySubObject("Item(int)", 1);//本例获取第一个，最后参数填1

    //—————————获取该sheet的数据范围（可以理解为有数据的矩形区域）————
    QAxObject * usedrange = worksheet->querySubObject("UsedRange");

    //———————————————————获取行数———————————————
    QAxObject * rows = usedrange->querySubObject("Rows");
    int iRows = rows->property("Count").toInt();
    qDebug() << QString("行数为: %1").arg(QString::number(iRows));

    //————————————获取列数—————————
    QAxObject * columns = usedrange->querySubObject("Columns");
    int iColumns = columns->property("Count").toInt();
    qDebug() << QString("列数为: %1").arg(QString::number(iColumns));

    //————————数据的起始行———
    int iStartRow = rows->property("Row").toInt();
    qDebug() << QString("起始行为: %1").arg(QString::number(iStartRow));

    //————————数据的起始列————————————
    int iColumn = columns->property("Column").toInt();
    qDebug() << QString("起始列为: %1").arg(QString::number(iColumn));

    QAxObject *range1;
    QAxObject *range2;
    QString strRowColo="",strRowColo1 = "",strRowColo2 = "",strTextEdit;
    QDateTime dateTime;

    //保存文件
    QAxObject *range = worksheet->querySubObject("Cells(int,int)", 1,1);
    strRowColo = range->property("Value").toString();
    int second = dateTime.fromString(strRowColo,"yyyy-MM-ddThh:mm:ss").toTime_t();

    //2020_6_17_1108225.csv
    QString savePath = QDir::currentPath() + "/" + dateTime.fromTime_t(second).toString("yyyy_MM_dd_")
            + worksheet->querySubObject("Cells(int,int)", 1,4)->property("Value").toString()+ ".csv";
    qDebug() << savePath;
    QFile saveFile(savePath);
    if(!saveFile.open(QIODevice::ReadWrite))
    {
        qDebug() << "打开保存文件失败";
        return false;
    }
    QTextStream saveStrem(&saveFile);
    QString formate = "行数,时间,行数,时间,差值：秒,马达控制器编码,\n";
    saveStrem << formate.toUtf8();

    //读取数据比较2个数值 的时间差值
    for(int i = iStartRow; i< iRows;i++){
        range1 = worksheet->querySubObject("Cells(int,int)", i,1);
        range2= worksheet->querySubObject("Cells(int,int)", i+1,1);
        strRowColo1 = range1->property("Value").toString();
        strRowColo2 = range2->property("Value").toString();
        //qDebug() << "第" << i << "行：" << strRowColo1 << "第" << i+1 << "行：" << strRowColo2;
        int second1 = dateTime.fromString(strRowColo1,"yyyy-MM-ddThh:mm:ss").toTime_t();
        int second2 = dateTime.fromString(strRowColo2,"yyyy-MM-ddThh:mm:ss").toTime_t();

        int diffValue = qAbs(second1 - second2);

        if(diffValue > ui->lineEdit->text().toInt()){
            qDebug() << "第" << i << "行" << dateTime.fromTime_t(second1).toString("yyyy/MM/dd hh:mm:ss") << "," << second1
                     << ",第" << i+1 << "行"<< dateTime.fromTime_t(second2).toString("yyyy/MM/dd hh:mm:ss") <<","<<second2
                     << " 马达控制器编码" <<worksheet->querySubObject("Cells(int,int)", i,4)->property("Value").toString();
            qDebug() << "差值为："<<diffValue;
            strTextEdit = "第" + QString::number(i) + "行 " + dateTime.fromTime_t(second1).toString("yyyy/MM/dd hh:mm:ss") + "," + QString::number(second1)
                    + ",第" + QString::number(i+1) + "行 " + dateTime.fromTime_t(second2).toString("yyyy/MM/dd hh:mm:ss") + "," + QString::number(second2)
                    + " 马达控制器编码" + worksheet->querySubObject("Cells(int,int)", i,4)->property("Value").toString();
            ui->textEdit->append(strTextEdit);
            strTextEdit = "差值为：" + QString::number(diffValue);
            ui->textEdit->append(strTextEdit);

            //保存数据
            saveStrem << QString::number(i) + "," << dateTime.fromTime_t(second1).toString("yyyy/MM/dd hh:mm:ss") + ","
                      << QString::number(i+1) + "," << dateTime.fromTime_t(second2).toString("yyyy/MM/dd hh:mm:ss") + ","
                      << QString::number(diffValue) + "," << worksheet->querySubObject("Cells(int,int)", i,4)->property("Value").toString() + "," << "\n";
        }

    }
        //待添加一个转换函数，即第6行，第6列，66转为F6
    #if 0
        //—————————————写入数据—————————————
        //获取F6的位置
        QAxObject *range2 = worksheet->querySubObject("Range(QString)", "F6:F6");
        //写入数据, 第6行，第6列
        range2->setProperty("Value", "中共十九大");
        QString newStr = "";
        newStr = range2->property("Value").toString();
        qDebug() << "写入数据后，第6行，第6列的数据为：" + newStr;
    #endif
        //!!!!!!!一定要记得close，不然系统进程里会出现n个EXCEL.EXE进程
     workbook->dynamicCall("Save()");
     workbook->dynamicCall("Close()");
     excel->dynamicCall("Quit()");
     if (excel)
     {
        delete excel;
        excel = NULL;
     }
     saveFile.close();
     return true;

}
