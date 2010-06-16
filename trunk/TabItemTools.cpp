#include "TabItemTools.h"

TabItemTools::TabItemTools(QWidget *parent) : QWidget(parent)
{
    model = new QSqlQueryModel();

    te_request = new QTextEdit();
    te_request->setFixedHeight(150);

    btn_execute = new QPushButton("Executer la requ�te");
    btn_execute->setFixedWidth(150);

    view = new QTableView();
    view->setDragEnabled(true);

    view->setSortingEnabled (true);
    view->setEditTriggers(QAbstractItemView::DoubleClicked);

    proxyModel = new QSortFilterProxyModel();

    QVBoxLayout *layout_main = new QVBoxLayout();
    layout_main->addWidget(te_request);
    layout_main->addWidget(btn_execute);
    layout_main->addWidget(view);

    setLayout(layout_main);

    connect(btn_execute, SIGNAL(clicked()), this, SLOT(refreshList()));
}

void TabItemTools::refreshList(){
    if(te_request->toPlainText() != "")
    {
        QSqlQuery *req = new QSqlQuery();
        req->prepare(te_request->toPlainText());
        if(req->exec())
        {
            model->setQuery(*req);
            proxyModel->setSourceModel(model);
            view->setModel(proxyModel);

            QMessageBox::information(this, "Requ�te ex�cut�e avec succ�s !", "Requ�te bien execut�e.\n" + req->lastInsertId().toString());
        }
        else
        {
            QMessageBox::warning(this, "Erreur !", "La requ�te n'a pas pu �tre ex�cut�e !");
        }
    }
}
