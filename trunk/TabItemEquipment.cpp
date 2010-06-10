#include "TabItemEquipment.h"

TabItemEquipment::TabItemEquipment(QWidget *parent) : QWidget(parent)
{
    model = new QSqlQueryModel();

    btn_add = new QPushButton("Ajouter");
    btn_del = new QPushButton("Supprimer");
    btn_del->setDisabled(true);

    QHBoxLayout *layout_btn = new QHBoxLayout();
    layout_btn->addWidget(btn_add);
    layout_btn->addWidget(btn_del);
    layout_btn->addStretch();

    view = new QTableView();
    view->setEditTriggers(QAbstractItemView::CurrentChanged);
    view->setDragEnabled(true);
    view->verticalHeader()->hide();
    view->setSelectionBehavior(QAbstractItemView::SelectRows);
    view->setSelectionMode(QAbstractItemView::SingleSelection);
    //view->setAlternatingRowColors(true);
    view->setSortingEnabled (true);
    view->setContextMenuPolicy(Qt::CustomContextMenu);

    proxyModel = new QSortFilterProxyModel();

    refreshList();

    QVBoxLayout *layout_main = new QVBoxLayout();
    layout_main->addLayout(layout_btn);
    layout_main->addWidget(view);

    setLayout(layout_main);

    connect(view, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(createMenu(QPoint)));
    connect(view, SIGNAL(clicked(QModelIndex)), this, SLOT(refreshButtonState(QModelIndex)));
    connect(btn_add, SIGNAL(clicked()), this, SLOT(addItem()));
    connect(btn_del, SIGNAL(clicked()), this, SLOT(deleteItem()));

}

void TabItemEquipment::refreshList(){

    model->setQuery("SELECT * FROM equipment ORDER BY equip_name");
    model->setHeaderData(1, Qt::Horizontal, "D�signation");

    proxyModel->setSourceModel(model);

    view->setModel(proxyModel);
    view->hideColumn(0); // id

}

void TabItemEquipment::createMenu(QPoint pos){
    QMenu *menu = new QMenu(this);
    QAction *action_add = new QAction("&Nouveau", this);
    menu->addAction(action_add);

    QModelIndex index = view->selectionModel()->currentIndex();
    index = index.sibling(index.row(), 0); // ici je force le n� de colonne � 0, pour etre sur le premier champ
    if (index.row() != -1){

        QAction *action_del = new QAction("&Supprimer", this);
        menu->addAction(action_del);

        connect(action_del, SIGNAL(triggered()), this, SLOT(deleteItem()));
    }
    connect(action_add, SIGNAL(triggered()), this, SLOT(addItem()));
    menu->exec(view->mapToGlobal(pos));
}

void TabItemEquipment::deleteItem(){
    QModelIndex index = view->selectionModel()->currentIndex();
    index = index.sibling(index.row(), 0); // ici je force le n� de colonne � 0, pour etre sur le premier champ
    if (index.row() != -1){
        int id_equip = proxyModel->data(index).toInt();
        QSqlQuery *req = new QSqlQuery();
        req->prepare("DELETE FROM equipment WHERE equip_id = :id_equip");
        req->bindValue("id_equip", id_equip);

        int rep = QMessageBox::question(this, "Confirmation", "Etes-vous s�r de vouloir supprimer cet �quipement ?", QMessageBox::Yes | QMessageBox::No);
        if(rep == QMessageBox::Yes){
            if (req->exec()){
                refreshList();
                QMessageBox::information(this, "Requ�te ex�cut�e avec succ�s !", "L'�quipement a �t� supprim� de la base de donn�es !");
            }
            else
                QMessageBox::warning(this, "Erreur !", "La requ�te n'a pas pu �tre ex�cut�e !");
        }
    }
}

void TabItemEquipment::refreshButtonState(QModelIndex index){
    if(index.isValid())
    {
        btn_del->setDisabled(false);
    }
    else
    {
        btn_del->setDisabled(true);
    }
}

void TabItemEquipment::addItem(){
    TabItemEquipmentActions *equipAdd = new TabItemEquipmentActions(this);
    connect(equipAdd, SIGNAL(notifyRefreshList()), this, SLOT(refreshList()));
    equipAdd->exec();
}
